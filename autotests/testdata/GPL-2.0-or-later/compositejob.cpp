/*
 * Copyright (C) 2017 Elvis Angelaccio <elvis.angelaccio@kde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "compositejob.h"
#include "decryptjob.h"
#include "encryptjob.h"
#include "symmydebug.h"

#include <KIO/JobTracker>
#include <KJobTrackerInterface>
#include <KLocalizedString>
#include <KMessageBox>
#include <KNewPasswordDialog>
#include <KPasswordDialog>

#include <QTimer>

namespace Symmy
{

CompositeJob::CompositeJob(const QStringList &filenames, Task task)
    : KCompositeJob {}
    , m_filenames {filenames}
    , m_task {task}
{
    setCapabilities(Killable);
    KIO::getJobTracker()->registerJob(this);
}

CompositeJob::~CompositeJob()
{
}

void CompositeJob::start()
{
    QTimer::singleShot(0, this, &CompositeJob::slotStart);
}

bool CompositeJob::doKill()
{
    if (!hasSubjobs()) {
        return false;
    }

    return subjobs().at(0)->kill();
}

void CompositeJob::slotResult(KJob *job)
{
    if (job->error() == KJob::UserDefinedError) {
        qCDebug(SYMMY) << "Job failed:" << job->errorText();
        if (task() == Task::Encryption) {
            setError(KJob::UserDefinedError);
            KMessageBox::error(nullptr, xi18nc("@info", "Encryption operation failed. Please check whether the <application>gpg-agent</application> process is running."));
            emitResult();
            return;
        }

        auto decryptJob = qobject_cast<Symmy::DecryptJob*>(job);
        if (decryptJob) {
            qCDebug(SYMMY) << "Subjob failed to decrypt" << decryptJob->ciphertextFilename();
            m_failedDecryptions << decryptJob->ciphertextFilename();
        }
    }

    removeSubjob(job);

    if (hasSubjobs()) {
        qCDebug(SYMMY) << "Starting next subjob...";
        startSubjob();
        return;
    }

    qCDebug(SYMMY) << "Composite job finished";

    if (!m_failedDecryptions.isEmpty()) {
        // Nothing was decrypted, mark the composite job as failed.
        if (m_failedDecryptions.size() == filenames().size()) {
            setError(KJob::UserDefinedError);
            KMessageBox::error(nullptr, xi18nc("@info", "Decryption operation failed. Please check whether the decryption key is correct.<nl/>"
                                                        "You should also check whether the <application>gpg-agent</application> process is running."));
        } else {
            KMessageBox::errorList(nullptr, xi18nc("@info", "Could not decrypt the following ciphertexts.<nl/>Please check whether the decryption key is correct."), m_failedDecryptions);
        }
    }

    emitResult();
}

void CompositeJob::slotAccepted()
{
    for (const auto &filename : qAsConst(m_filenames)) {
        Symmy::Job *job = nullptr;
        if (task() == Task::Encryption) {
            job = new Symmy::EncryptJob(qobject_cast<KNewPasswordDialog*>(m_passwordDialog)->password(), filename);
        } else {
            job = new Symmy::DecryptJob(qobject_cast<KPasswordDialog*>(m_passwordDialog)->password(), filename);
        }

        addSubjob(job);
        connect(job, SIGNAL(percent(KJob*,ulong)), this, SLOT(slotPercent(KJob*,ulong)));
    }

    qCDebug(SYMMY) << "Got a passphrase, starting first subjob...";
    startSubjob();
}

void CompositeJob::slotRejected()
{
    qCDebug(SYMMY) << "Passphrase dialog rejected.";
    setError(KilledJobError);
    emitResult();
}

void CompositeJob::slotStart()
{
    if (filenames().isEmpty()) {
        emitResult();
        return;
    }

    qCDebug(SYMMY) << "Starting composite job...";
    emit description(this, i18n("Asking Passphrase"));

    if (task() == Task::Encryption) {
        auto passwordDialog = new KNewPasswordDialog {};
        passwordDialog->setPrompt(i18n("Please supply a password or passphrase to be used as encryption key."));
        passwordDialog->setAllowEmptyPasswords(false);
        m_passwordDialog = passwordDialog;
    } else {
        auto passwordDialog = new KPasswordDialog {};
        passwordDialog->setPrompt(i18n("Please supply a password or passphrase to be used as decryption key."));
        m_passwordDialog = passwordDialog;
    }

    connect(m_passwordDialog, &QDialog::accepted, this, &CompositeJob::slotAccepted);
    connect(m_passwordDialog, &QDialog::rejected, this, &CompositeJob::slotRejected);
    connect(m_passwordDialog, &QDialog::finished, m_passwordDialog, &QObject::deleteLater);

    m_passwordDialog->open();
}

void CompositeJob::slotPercent(KJob *, unsigned long percent)
{
    setPercent(percent);
}

void CompositeJob::startSubjob()
{
    auto job = qobject_cast<Symmy::Job*>(subjobs().at(0));

    if (task() == Task::Encryption) {
        emit description(this, i18nc("description of an encryption job", "Encrypting"),
                         qMakePair(i18nc("File used as input of the encryption algorithm", "Plaintext"), job->plaintextFilename()),
                         qMakePair(i18nc("File created by the encryption algorithm", "Ciphertext"), job->ciphertextFilename()));
    } else {
        emit description(this, i18nc("description of a decryption job", "Decrypting"),
                         qMakePair(i18nc("File used as input of the decryption algorithm", "Ciphertext"), job->ciphertextFilename()),
                         qMakePair(i18nc("File created by the decryption algorithm", "Plaintext"), job->plaintextFilename()));
    }

    job->start();
}

QStringList CompositeJob::filenames() const
{
    return m_filenames;
}

CompositeJob::Task CompositeJob::task() const
{
    return m_task;
}

}
