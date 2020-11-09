/*
 *  SPDX-FileCopyrightText: 2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "test_licenseconvert.h"
#include "../licenseregistry.h"
#include "../directoryparser.h"
#include <QTest>
#include <QVector>
#include <QDebug>
#include <QDir>

static const QString licensesRootPath(":/licenses_templates/%1");

void TestLicenseConvert::greedyLicenseTextConversion()
{
    // use non-correctly converted license as test data
    QFile file(licensesRootPath.arg("LGPL-2.0-or-later/kservice"));
    file.open(QIODevice::ReadOnly);
    const QString expression{ "LGPL-2.0-or-later" };
    const QString fileContent = file.readAll();

    LicenseRegistry registry;
    DirectoryParser parser;
    bool result = false;
    for (auto regexp: registry.headerTextRegExps(expression)) {
        result = fileContent.contains(regexp);
        if (result) {
            break;
        }
    }
    QVERIFY(result);
    QCOMPARE(parser.replaceHeaderText(fileContent, expression), "SPDX-License-Identifier: LGPL-2.0-or-later\n");
}

void TestLicenseConvert::basicConvertOrExpressions()
{
    // use non-correctly converted license as test data
    QFile file(licensesRootPath.arg("LGPL-2.1-only_OR_LGPL-3.0-only/kio_httpserver_p"));
    file.open(QIODevice::ReadOnly);
    const QString expression{ "LGPL-2.1-only_OR_LGPL-3.0-only" };
    const QString fileContent = file.readAll();

    LicenseRegistry registry;
    DirectoryParser parser;

    bool result = false;
    for (auto regexp: registry.headerTextRegExps(expression)) {
        result = fileContent.contains(regexp);
        if (result) {
            break;
        }
    }
    QVERIFY(result);
    QCOMPARE(parser.replaceHeaderText(fileContent, expression), "SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only\n");
}

void TestLicenseConvert::doNotModifyFileWithoutDetectedLicense()
{
    QFile fileOrig(":/testdata_conversionexamples/file_without_unrecognized_statement.h.origfile");
    fileOrig.open(QIODevice::ReadOnly);
    const QString fileContentOrig = fileOrig.readAll();

    LicenseRegistry registry;
    DirectoryParser parser;

    QCOMPARE(parser.detectLicenseStatement(fileContentOrig), QString());
    QCOMPARE(parser.replaceHeaderText(fileContentOrig, QString()), fileContentOrig);
}

void TestLicenseConvert::pruneLicenseList()
{
    LicenseRegistry registry;
    DirectoryParser parser;

    // prune multiple detections of same license
    {
        QVector<LicenseRegistry::SpdxExpression> licenses{ "LGPL-2.0-only", "LGPL-2.0-only"};
        auto prunedLicenses = parser.pruneLicenseList(licenses);
        QCOMPARE(prunedLicenses.length(), 1);
        QCOMPARE(prunedLicenses.first(), "LGPL-2.0-only");
    }

    // prune licenses that are detected both "LIC-A" as well as "LIC-A OR LIC-B"
    {
        QVector<LicenseRegistry::SpdxExpression> licenses{ "LGPL-2.1-only", "LGPL-2.1-only_OR_LGPL-3.0-only"};
        auto prunedLicenses = parser.pruneLicenseList(licenses);
        QCOMPARE(prunedLicenses.length(), 1);
        QCOMPARE(prunedLicenses.first(), "LGPL-2.1-only_OR_LGPL-3.0-only");
    }

    // prune licenses that are detected both "LIC-A" as well as "LIC-A OR LIC-B"
    {
        QVector<LicenseRegistry::SpdxExpression> licenses{ "LGPL-2.1-only", "LGPL-2.1-only_WITH_Qt-Commercial-exception-1.0"};
        auto prunedLicenses = parser.pruneLicenseList(licenses);
        QCOMPARE(prunedLicenses.length(), 1);
        QCOMPARE(prunedLicenses.first(), "LGPL-2.1-only_WITH_Qt-Commercial-exception-1.0");
    }
}

void TestLicenseConvert::exampleFileConversion()
{
    QVector<std::pair<QString, QString>> testFiles;

    testFiles.append(std::make_pair("LGPL-2.0-only", ":/testdata_conversionexamples/fake_notifications_server.h"));
    testFiles.append(std::make_pair("GPL-2.0-or-later", ":/testdata_conversionexamples/blur.h"));

    for (auto iter = testFiles.constBegin(); iter != testFiles.constEnd(); ++iter) {
        const QString targetSpdxMarker = iter->first;
        const QString baseFileName = iter->second;

        qDebug() << "Testing:" << baseFileName;

        QFile fileOrig(baseFileName + ".origfile");
        fileOrig.open(QIODevice::ReadOnly);
        const QString fileContentOrig = fileOrig.readAll();

        QFile fileSpdx(baseFileName + ".spdx");
        fileSpdx.open(QIODevice::ReadOnly);
        const QString fileContentSpdx = fileSpdx.readAll();

        LicenseRegistry registry;
        DirectoryParser parser;

        QVERIFY(registry.expressions().contains(targetSpdxMarker));
        QVector<LicenseRegistry::SpdxExpression> licenses = parser.detectLicenses(fileContentOrig);
        QCOMPARE(licenses.count(), 1);
        QCOMPARE(licenses.first(), targetSpdxMarker);
        const QString result = parser.replaceHeaderText(fileContentOrig, targetSpdxMarker);
        QCOMPARE(result, fileContentSpdx);
    }
}

QTEST_GUILESS_MAIN(TestLicenseConvert);
