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

QMap<QString, LicenseRegistry::SpdxIdentifer> DirectoryParser::parseAll(const QString &directory) const
{
    QVector<LicenseRegistry::SpdxIdentifer> identifiers = m_registry.identifiers();
    QMap<QString, LicenseRegistry::SpdxIdentifer> results;

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

    QDirIterator iterator(directory, QDirIterator::Subdirectories);
    while (iterator.hasNext()) {
        QFile file(iterator.next());
        if (!iterator.fileInfo().isFile()) {
            continue;
        }
        if (!file.fileName().endsWith(".cpp") && !file.fileName().endsWith(".h") && !file.fileName().endsWith(".hpp") && !file.fileName().endsWith(".c")) {
            //TODO only support C files for now
            continue;
        }

        file.open(QIODevice::ReadOnly);
        const QString fileContent = file.readAll();

//        qDebug() << "checking:" << iterator.fileInfo();
        for (auto identifier : identifiers) {
            auto regexp = m_registry.headerTextRegExp(identifier);
            if (fileContent.contains(regexp)) {
                if (results.contains(iterator.fileInfo().filePath())) {
                    qCritical() << "NOT SUPPORT FOR MULTI-LICENSED FILES YET";
                }
                results.insert(iterator.fileInfo().filePath(), identifier);
//                qDebug() << "---> " << iterator.fileInfo().filePath() << identifier;
            }
        }

        // check for blacklisted file because of missing license header
        for (auto backlistPath : missingLicenseHeaderBlacklist) {
            if (iterator.fileInfo().filePath().endsWith(backlistPath)) {
                results.insert(iterator.fileInfo().filePath(), LicenseRegistry::MissingLicense);
                break;
            }
        }

        // if nothing matches, report error
        if (!results.contains(iterator.fileInfo().filePath())) {
            results.insert(iterator.fileInfo().filePath(), LicenseRegistry::UnknownLicense);
        }
    }

    return results;
}
