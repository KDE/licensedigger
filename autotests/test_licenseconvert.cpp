/*
 *  SPDX-FileCopyrightText: 2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "test_licenseconvert.h"
#include "../licenseregistry.h"
#include "../directoryparser.h"
#include <QTest>
#include <QDebug>
#include <QDir>
#include <QDirIterator>

void TestLicenseConvert::greedyLicenseTextConversion()
{
    // use non-correctly converted licese as test data
    QFile file(":/licenses/LGPL-2.0-or-later/kservice");
    file.open(QIODevice::ReadOnly);
    const QString expression{ "LGPL-2.0-or-later" };
    const QString fileContent = file.readAll();

    LicenseRegistry registry;
    DirectoryParser parser;
    QVERIFY(fileContent.contains(registry.headerTextRegExp(expression)));
    QCOMPARE(parser.replaceHeaderText(fileContent, expression), "SPDX-License-Identifier: LGPL-2.0-or-later\n");
}

void TestLicenseConvert::basicConvertOrExpressions()
{
    // use non-correctly converted licese as test data
    QFile file(":/licenses/LGPL-2.1-only_OR_LGPL-3.0-only/kio_httpserver_p");
    file.open(QIODevice::ReadOnly);
    const QString expression{ "LGPL-2.1-only_OR_LGPL-3.0-only" };
    const QString fileContent = file.readAll();

    LicenseRegistry registry;
    DirectoryParser parser;
    QVERIFY(fileContent.contains(registry.headerTextRegExp(expression)));
    QCOMPARE(parser.replaceHeaderText(fileContent, expression), "SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only\n");
}

QTEST_GUILESS_MAIN(TestLicenseConvert);
