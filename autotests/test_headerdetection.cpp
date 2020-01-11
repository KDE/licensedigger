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

#include "test_headerdetection.h"
#include "../licenseregistry.h"
#include <QTest>
#include <QDebug>
#include <QDir>
#include <QDirIterator>

void TestHeaderDetection::detectForIdentifier(const QString &spdxMarker)
{
    const QString testdataDir { ":/testdata/" + spdxMarker };

    // read comparison headers
    LicenseRegistry registry;

    QVERIFY(registry.identifiers().contains(spdxMarker));
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
        bool result = fileContents.contains(registry.headerTextRegExp(spdxMarker));
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

void TestHeaderDetection::detectLGPL20onlyWithQtGPL10exception()
{
    detectForIdentifier("LGPL-2.0-only_WITH_Qt-GPL-exception-1.0");
}

void TestHeaderDetection::detectLGPL20_or_LGPL30_or_KDE()
{
    detectForIdentifier("LGPL-2.0-only_OR_LGPL-3.0-only_OR_LicenseRef-KDE-Accepted-LGPL");
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

void TestHeaderDetection::detectLGPL21withQtLGPLexception()
{
    detectForIdentifier("LGPL-2.1-only_WITH_Qt-LGPL-exception-1.1");
}

void TestHeaderDetection::detectLGPL21withQtLGPLexceptionOrLGPL30withQtLGPLexception()
{
    detectForIdentifier("LGPL-2.1-only_WITH_Qt-LGPL-exception-1.1_OR_LGPL-3.0-only_WITH_Qt-LGPL-exception-1.1_OR_LicenseRef-QTCOMMERCIAL");
}

QTEST_GUILESS_MAIN(TestHeaderDetection);
