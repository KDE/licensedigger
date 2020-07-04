/*
 *  SPDX-FileCopyrightText: 2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of
 *  the License or (at your option) version 3 or any later version
 *  accepted by the membership of KDE e.V. (or its successor approved
 *  by the membership of KDE e.V.), which shall act as a proxy
 *  defined in Section 14 of version 3 of the license.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "licenseregistry.h"
#include <QDebug>
#include <QDir>
#include <QDirIterator>

const QString LicenseRegistry::ToClarifyLicense("TO-CLARIFY");
const QString LicenseRegistry::UnknownLicense("UNKNOWN-LICENSE");
const QString LicenseRegistry::AmbigiousLicense("AMBIGIOUS");
const QString LicenseRegistry::MissingLicense("MISSING-LICENSE");
const QString LicenseRegistry::MissingLicenseForGeneratedFile("MISSING-LICENSE-GENERATED-FILE");

LicenseRegistry::LicenseRegistry(QObject *parent) : QObject(parent)
{
    loadLicenseHeaders();
    loadLicenseFiles();
}

void LicenseRegistry::loadLicenseHeaders()
{
    if (!m_registry.isEmpty()) {
        m_registry.clear();
    }
    m_registry[LicenseRegistry::UnknownLicense] = QVector<QString>{ "THIS IS A STUB HEADER FOR UNKNOWN LICENSES, IT SHALL NEVER MATCH" };

    QDirIterator spdxIter(":/licenses/");
    while (spdxIter.hasNext()) {
        QString filePath = spdxIter.next();
        if (!spdxIter.fileInfo().isDir()) {
            qWarning() << "A non-directory was found here unexpected:" << spdxIter.fileInfo();
            continue;
        }
        QVector<QString> headerTexts;
        QDirIterator headerIter(filePath);
        while (headerIter.hasNext()) {
            QFile file(headerIter.next());
            file.open(QIODevice::ReadOnly);
            headerTexts.append(file.readAll());
        }
        // sort license texts lexicographically decreasing
        // this is a simple solution for the problem when one license text is a prefix of another license text
        // which is known for license texts with omitted "." at the end
        std::sort(headerTexts.begin(), headerTexts.end(), [](const QString &lhs, const QString &rhs){
            return lhs > rhs;
        });
        m_registry[spdxIter.fileName()] = headerTexts;
    }
}

void LicenseRegistry::loadLicenseFiles()
{
    QDirIterator textIter(":/licensetexts/");
    while (textIter.hasNext()) {
        QString filePath = textIter.next();
        if (textIter.fileInfo().isDir()) {
            qWarning() << "Unexpected directory found:" << textIter.fileInfo();
            continue;
        }
        QString baseName = textIter.fileName().mid(0, textIter.fileName().length() - 4); // remove ".txt"
        m_licenseFiles.insert(baseName, textIter.filePath());
    }
}

QVector<LicenseRegistry::SpdxExpression> LicenseRegistry::expressions() const
{
    return m_registry.keys().toVector();
}

QVector<LicenseRegistry::SpdxIdentifier> LicenseRegistry::identifiers() const
{
    return m_licenseFiles.keys().toVector();
}

QMap<LicenseRegistry::SpdxIdentifier, QString> LicenseRegistry::licenseFiles() const
{
    return m_licenseFiles;
}

QVector<QString> LicenseRegistry::headerTexts(const LicenseRegistry::SpdxExpression &identifier) const
{
    return m_registry.value(identifier);
}

QRegularExpression LicenseRegistry::headerTextRegExp(const SpdxExpression &identifier) const
{
    if (!m_registry.contains(identifier)) {
        qCritical() << "Identifier not found, returning error matcher";
        return QRegularExpression("DOES_NOT_MATCH_ANY_LICENSE_HEADER");
    }
    if (m_regexpCache.contains(identifier)) {
        return m_regexpCache.value(identifier);
    }

    QVector<QString> patterns;
    // additional to all headers also add the SPDX identifier
    if (identifier != LicenseRegistry::UnknownLicense
            && identifier != LicenseRegistry::MissingLicense
            && identifier != LicenseRegistry::MissingLicenseForGeneratedFile
            && identifier != LicenseRegistry::ToClarifyLicense) {
        patterns.append(QString("SPDX-License-Identifier: %1").arg(identifier).replace('_', ' '));
    }

    for (const QString &header : m_registry.value(identifier)) {
        QString pattern(QRegularExpression::escape(header));
        // start detection at first word of license string to make detection easier
        pattern.replace("\\\n", "[\\* ]*\\\n[\\* ]*"); // allow prefixes and suffices of whitespace mixed with stars

        // remove line-break pattern from last line
        pattern = pattern.left(pattern.length() - QString("[\\* ]*\\\n[\\* ]*").length());
        pattern.append("[\\* ]*");

        patterns.append(pattern);
    }

    QVector<QString>::const_iterator iter = patterns.constBegin();
    QString fullPattern = QString("(%1)").arg(*iter);
    while (++iter != patterns.constEnd()) {
        fullPattern.append(QString("|(%1)").arg(*iter));
    }
//    qDebug() << "PATTERN" << fullPattern;
    QRegularExpression detector(fullPattern);
    m_regexpCache[identifier] = detector;

    return detector;
}

bool LicenseRegistry::isFakeLicenseMarker(const QString &expression) const
{
    const QStringList fakeExpressions {
        LicenseRegistry::ToClarifyLicense,
        LicenseRegistry::UnknownLicense,
        LicenseRegistry::MissingLicense,
        LicenseRegistry::AmbigiousLicense,
        LicenseRegistry::MissingLicenseForGeneratedFile
    };
    return fakeExpressions.contains(expression);
}
