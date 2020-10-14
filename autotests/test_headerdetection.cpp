/*
 *  SPDX-FileCopyrightText: 2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "test_headerdetection.h"
#include "../licenseregistry.h"
#include "../directoryparser.h"
#include <QTest>
#include <QDebug>
#include <QDir>
#include <QDirIterator>

void TestHeaderDetection::detectForIdentifier(const QString &spdxMarker)
{
    const QString testdataDir { ":/testdata/" + spdxMarker };

    // read comparison headers
    LicenseRegistry registry;

    QVERIFY(registry.expressions().contains(spdxMarker));
    QVERIFY(registry.headerTexts(spdxMarker).size() > 0);

    // load test data
    QVERIFY(QDir().exists(testdataDir));
    QVERIFY(QDir(testdataDir).count() > 0);
    QDirIterator testdataIter(testdataDir);
    while (testdataIter.hasNext()) {
        QFile file(testdataIter.next());
        file.open(QIODevice::ReadOnly);
        const QString fileContents { file.readAll() };
        QVERIFY(!fileContents.isEmpty());
        bool result = false;
        for (auto regexp: registry.headerTextRegExps(spdxMarker)) {
            result = fileContents.contains(regexp);
            if (result) {
                break;
            }
        }

        if (!result) {
            qWarning() << "Could not detect" << spdxMarker << ":" << testdataIter.filePath();
        }
        QVERIFY(result);
    }
}

void TestHeaderDetection::detectLGPL20orlater()
{
    detectForIdentifier("LGPL-2.0-or-later");
}

void TestHeaderDetection::detectLGPL21orlater()
{
    detectForIdentifier("LGPL-2.1-or-later");
}

void TestHeaderDetection::detectLGPL20only()
{
    detectForIdentifier("LGPL-2.0-only");
}

void TestHeaderDetection::detectLGPL21onlyOrLGPL30only()
{
    detectForIdentifier("LGPL-2.1-only_OR_LGPL-3.0-only");
}

void TestHeaderDetection::detectLGPL20onlyWithQtCommercialException()
{
    detectForIdentifier("LGPL-2.0-only_WITH_Qt-Commercial-exception-1.0");
}

void TestHeaderDetection::detectGPL20only()
{
    detectForIdentifier("GPL-2.0-only");
}

void TestHeaderDetection::detectGPL20orlater()
{
    detectForIdentifier("GPL-2.0-or-later");
}

void TestHeaderDetection::detectGPL30orlater()
{
    detectForIdentifier("GPL-3.0-or-later");
}

void TestHeaderDetection::detectGPL2orlaterwithQtCommercialException()
{
    detectForIdentifier("GPL-2.0-or-later_WITH_Qt-Commercial-exception-1.0");
}

void TestHeaderDetection::detectGPL30_or_KDE()
{
    detectForIdentifier("GPL-3.0-only_OR_LicenseRef-KDE-Accepted-GPL");
}

void TestHeaderDetection::detectGPL20_or_GPL30_or_KDE()
{
    detectForIdentifier("GPL-2.0-only_OR_GPL-3.0-only_OR_LicenseRef-KDE-Accepted-GPL");
}

void TestHeaderDetection::detectLGPL21only()
{
    detectForIdentifier("LGPL-2.1-only");
}

void TestHeaderDetection::detectLGPL20_or_LGPL30()
{
    detectForIdentifier("LGPL-2.0-only_OR_LGPL-3.0-only");
}

void TestHeaderDetection::detectLGPL21_or_LGPL30_or_KDE()
{
    detectForIdentifier("LGPL-2.1-only_OR_LGPL-3.0-only_OR_LicenseRef-KDE-Accepted-LGPL");
}

void TestHeaderDetection::detectLGPL30orlater()
{
    detectForIdentifier("LGPL-3.0-or-later");
}

void TestHeaderDetection::detectLGPL30onlyOrGPL20orlater()
{
    detectForIdentifier("LGPL-3.0-only_OR_GPL-2.0-or-later");
}

void TestHeaderDetection::detectBSD2Clause()
{
    detectForIdentifier("BSD-2-Clause");
}

void TestHeaderDetection::detectBSD3Clause()
{
    detectForIdentifier("BSD-3-Clause");
}

void TestHeaderDetection::detectMIT()
{
    detectForIdentifier("MIT");
}

void TestHeaderDetection::detectLGPL21withQtLGPLexception_or_QtCommercial()
{
    detectForIdentifier("LGPL-2.1-only_WITH_Qt-LGPL-exception-1.1_OR_LicenseRef-Qt-Commercial");
}

void TestHeaderDetection::detectLGPL21withQtLGPLexceptionOrLGPL30withQtLGPLexception()
{
    detectForIdentifier("LGPL-2.1-only_WITH_Qt-LGPL-exception-1.1_OR_LGPL-3.0-only_WITH_Qt-LGPL-exception-1.1_OR_LicenseRef-Qt-Commercial");
}

void TestHeaderDetection::detectLGPL30_or_GPL20_or_GPL30_or_GPLKFQF_or_QtCommercial()
{
    detectForIdentifier("LGPL-3.0-only_OR_GPL-2.0-only_OR_GPL-3.0-only_OR_LicenseRef-KFQF-Accepted-GPL_OR_LicenseRef-Qt-Commercial");
}

void TestHeaderDetection::detectSpdxExpressions()
{
    // read comparison headers
    LicenseRegistry registry;
    DirectoryParser parser;

    const QString spdxMarker = "GPL-2.0-only_OR_GPL-3.0-only_OR_LicenseRef-KDE-Accepted-GPL";

    QVERIFY(registry.expressions().contains(spdxMarker));
    QVERIFY(registry.headerTexts(spdxMarker).size() > 0);

    const QString fileContents { "SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL" };
    auto results = parser.detectLicenses(fileContents);

    QCOMPARE(results.count(), 1);
    QCOMPARE(results.first(), "GPL-2.0-only_OR_GPL-3.0-only_OR_LicenseRef-KDE-Accepted-GPL");
}

QTEST_GUILESS_MAIN(TestHeaderDetection);
