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
#include <QDir>
#include <QDirIterator>

void TestHeaderDetection::detectLGPL20orlater()
{
    const QString spdxMarker { "LGPL-2.0-or-later" };
    const QString testdataDir { ":/testdata/" + spdxMarker };

    // read comparison headers
    LicenseRegistry registry;

    QVERIFY(registry.identifiers().contains(spdxMarker));
    QVERIFY(registry.headerTexts(spdxMarker).size() > 0);

    // load test data
    QVERIFY(QDir().exists(testdataDir));
    QDirIterator testdataIter(testdataDir);
    while (testdataIter.hasNext()) {
        QFile file(testdataIter.next());
        file.open(QIODevice::ReadOnly);
        const QString fileContents { file.readAll() };
        QVERIFY(!fileContents.isEmpty());
        bool result = fileContents.contains(registry.headerTexts(spdxMarker).first()); //TODO all must be tests
        QVERIFY(result);
    }

}

QTEST_GUILESS_MAIN(TestHeaderDetection);
