/*
 *  SPDX-FileCopyrightText: 2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "directoryparser.h"
#include <QDirIterator>
#include <QTextStream>
#include <QDebug>

QRegularExpression DirectoryParser::copyrightRegExp() const
{
    static auto regexp = QRegularExpression("(SPDX-FileCopyrightText:|Copyright( \\([cC]\\))|Copyright ©|©|Copyright(:)?)"
                                     "[ ]+"
                                     "(?<years>([0-9]+(-[0-9]+|, [0-9]+)*))"
                                     "[ ]+"
                                     "(?<name>(\\w+( \\w+)*))"
                                     "[ ]*"
                                     "(?<contact>.*)"
                                     );
    return regexp;
}

QString DirectoryParser::unifyCopyrightStatements(const QString &originalText) const
{
    QString header = originalText;
    QRegularExpression regExp = copyrightRegExp();
    auto match = regExp.match(header);

    while (match.hasMatch()) {
        QString years = match.captured("years");
        QString name = match.captured("name");
        QString contact = match.captured("contact");

        QString unifiedCopyright = QString("SPDX-FileCopyrightText: %1 %2 %3").arg(years).arg(name).arg(contact);
        header.replace(match.capturedStart(), match.capturedLength(), unifiedCopyright);
        match = regExp.match(header, match.capturedStart() + unifiedCopyright.length());
    }
    return header;
}

QMap<QString, LicenseRegistry::SpdxExpression> DirectoryParser::parseAll(const QString &directory, bool convertMode) const
{
    QVector<LicenseRegistry::SpdxExpression> expressions = m_registry.expressions();
    QMap<QString, LicenseRegistry::SpdxExpression> results;

    if (convertMode) {
        qInfo() << "Running parser in CONVERT mode: every found license will be replaced with SPDX identifiers";
    }

    QStringList missingLicenseHeaderBlacklist;
    {
        QFile file(":/annotations/missing-headers-blacklist.txt");
        file.open(QIODevice::ReadOnly);
        QTextStream in(&file);
        QString line;
        while (in.readLineInto(&line)) {
            missingLicenseHeaderBlacklist.append(line);
        }
    }
    QStringList missingLicenseHeaderGeneratedFileBlacklist;
    {
        QFile file(":/annotations/generated-files.txt");
        file.open(QIODevice::ReadOnly);
        QTextStream in(&file);
        QString line;
        while (in.readLineInto(&line)) {
            missingLicenseHeaderGeneratedFileBlacklist.append(line);
        }
    }

    QDirIterator iterator(directory, QDirIterator::Subdirectories);
    while (iterator.hasNext()) {
        QFile file(iterator.next());
        if (!iterator.fileInfo().isFile()) {
            continue;
        }
        if (!file.fileName().endsWith(".cpp") && !file.fileName().endsWith(".h") && !file.fileName().endsWith(".hpp") && !file.fileName().endsWith(".c") && !file.fileName().endsWith(".qml")) {
            //TODO only support C files for now
            continue;
        }

        file.open(QIODevice::ReadOnly);
        const QString fileContent = file.readAll();
        file.close();

//        qDebug() << "checking:" << iterator.fileInfo();
        for (auto expression : expressions) {
            auto regexp = m_registry.headerTextRegExp(expression);
            if (fileContent.contains(regexp)) {
                if (results.contains(iterator.fileInfo().filePath())) {
                    qCritical() << "UNHANDLED MULTI-LICENSE CASE" << iterator.fileInfo().filePath() << expression << results.value(iterator.fileInfo().filePath());
                    results[iterator.fileInfo().filePath()] = LicenseRegistry::AmbigiousLicense;
                } else {
                    results.insert(iterator.fileInfo().filePath(), expression);
//                    qDebug() << "---> " << iterator.fileInfo().filePath() << identifier;
                }
            }
        }

        // check for blacklisted file because of missing license header
        for (auto backlistPath : missingLicenseHeaderBlacklist) {
            if (iterator.fileInfo().filePath().endsWith(backlistPath)) {
                results.insert(iterator.fileInfo().filePath(), LicenseRegistry::MissingLicense);
                break;
            }
        }
        for (auto backlistPath : missingLicenseHeaderGeneratedFileBlacklist) {
            if (iterator.fileInfo().filePath().endsWith(backlistPath)) {
                results.insert(iterator.fileInfo().filePath(), LicenseRegistry::MissingLicenseForGeneratedFile);
                break;
            }
        }

        // if nothing matches, report error
        if (!results.contains(iterator.fileInfo().filePath())) {
            results.insert(iterator.fileInfo().filePath(), LicenseRegistry::UnknownLicense);
        }

        const QString expression = results.value(iterator.fileInfo().filePath());
        if (convertMode
                && expression != LicenseRegistry::UnknownLicense
                && expression != LicenseRegistry::MissingLicense
                && expression != LicenseRegistry::MissingLicenseForGeneratedFile
                && expression != LicenseRegistry::AmbigiousLicense) {
            auto regexp = m_registry.headerTextRegExp(expression);
            QString outputExpression = expression;
            outputExpression.replace('_', " ");
            QString spdxOutputString = "SPDX-License-Identifier: " + outputExpression;
            QString newContent = fileContent;
            newContent.replace(regexp, spdxOutputString);
            qDebug() << newContent;
            file.open(QIODevice::WriteOnly);
            file.write(newContent.toUtf8());
            const QString fileContent = file.readAll();
            file.close();
        }
    }

    if (convertMode) {
        // compute needed licenses
        QSet<QString> identifiers;
        for (const auto &expression : results.values()) {
            auto expressionSplit = expression.split('_');
            for (const auto &identifier : expressionSplit) {
                // remove SPDX syntax attributes
                if (identifier == "OR" || identifier == "AND" || identifier == "WITH") {
                    continue;
                }
                // remove special placeholders
                if (identifier == LicenseRegistry::UnknownLicense
                        || identifier == LicenseRegistry::MissingLicense
                        || identifier == LicenseRegistry::MissingLicenseForGeneratedFile
                        || identifier == LicenseRegistry::AmbigiousLicense
                        || identifier == "TO-CLARIFY") {
                    continue;
                }
                identifiers.insert(identifier);
            }
        }
        // create licenses directory and put license files therein
        QString licenseDir = directory + "/LICENSES/";
        QDir().mkdir(licenseDir);
        const auto licenseFiles = m_registry.licenseFiles();
        for (const auto &identifier : identifiers) {
            qDebug() << "Deploy license file" << identifier << licenseFiles.value(identifier);
            QFile::copy(licenseFiles.value(identifier), licenseDir + identifier + ".txt");
        }
    }

    return results;
}

void DirectoryParser::convertCopyright(const QString &directory) const
{
    QDirIterator iterator(directory, QDirIterator::Subdirectories);
    while (iterator.hasNext()) {
        QFile file(iterator.next());
        if (!iterator.fileInfo().isFile()) {
            continue;
        }
        if (!file.fileName().endsWith(".cpp") && !file.fileName().endsWith(".h") && !file.fileName().endsWith(".hpp") && !file.fileName().endsWith(".c") && !file.fileName().endsWith(".qml")) {
            //TODO only support C files for now
            continue;
        }

        file.open(QIODevice::ReadOnly);
        const QString fileContent = file.readAll();
        file.close();

        file.open(QIODevice::WriteOnly);
        QString newContent = unifyCopyrightStatements(fileContent);
        file.write(newContent.toUtf8());
        file.close();
    }
}
