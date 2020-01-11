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

#include "test_licensefilesavailable.h"
#include "../licenseregistry.h"
#include <QTest>
#include <QDebug>
#include <QDir>
#include <QDirIterator>


void TestLicenseFilesAvailable::checkAll()
{
    LicenseRegistry registry;
    QVector<QString> expressions = registry.expressions();

    QSet<QString> identifiers;
    for (const auto &expression : expressions) {
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
                    || identifier == "TO-CLARIFY") {
                continue;
            }
            identifiers.insert(identifier);
        }
    }
    qDebug() << "Checking for SPDX license texts:" << identifiers.toList();

    // test missing
    QMap<LicenseRegistry::SpdxIdentifier, QString> licenseFiles = registry.licenseFiles();
    for (const auto &identifier : identifiers) {
        QVERIFY2(licenseFiles.contains(identifier), qPrintable("Searching identifier: " + identifier));
    }
}

QTEST_GUILESS_MAIN(TestLicenseFilesAvailable);
