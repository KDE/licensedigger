/*
 *  Copyright 2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#include "directoryparser.h"
#include <QDirIterator>
#include <QTextStream>
#include <QDebug>

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
