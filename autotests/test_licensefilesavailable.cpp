/*
 *  SPDX-FileCopyrightText: 2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
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
                    || identifier == LicenseRegistry::AmbigiousLicense
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
