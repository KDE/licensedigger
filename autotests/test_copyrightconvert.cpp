/*
 *  SPDX-FileCopyrightText: 2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "test_copyrightconvert.h"
#include "../licenseregistry.h"
#include "../directoryparser.h"
#include <QTest>
#include <QDebug>
#include <QDir>
#include <QDirIterator>

struct CopyrightExample {
    QString text;
    QString years;
    QString name;
    QString contact;
};

void TestCopyrightConvert::detectCopyright()
{

    DirectoryParser parser;
    QRegularExpression exp = parser.copyrightRegExp();

    std::vector<CopyrightExample> testdata = {
        { "SPDX-License-Identifier: 2018 John Doe <mail@example.com>" , "2018", "John Doe", "<mail@example.com>" },
        { "Copyright: 2018 John Doe <mail@example.com>" , "2018", "John Doe", "<mail@example.com>" },
        { "Copyright 2018-2019 John Doe <mail@example.com>" , "2018-2019", "John Doe", "<mail@example.com>" },
        { "Copyright 2018-2019 John Doe" , "2018-2019", "John Doe", "" },
        { "Copyright 2018-2019 Singlename <mail@example.com>" , "2018-2019", "Singlename", "<mail@example.com>" },
        { "Copyright 2018-2019 Foo of Baa  <mail@example.com>" , "2018-2019", "Foo of Baa", "<mail@example.com>" },
        { "Copyright 2018, 2019-2020 Foo of Baa  <mail@example.com>" , "2018, 2019-2020", "Foo of Baa", "<mail@example.com>" }
    };

    //FIXME replacing parser expression with test string
    exp = QRegularExpression("(SPDX-License-Identifier:|Copyright( \\([cC]\\))|Copyright ©|©|Copyright(:)?)"
                             "[ ]+"
                             "(?<years>([0-9]+(-[0-9]+|, [0-9]+)*))"
                             "[ ]+"
                             "(?<name>(\\w+( \\w+)*))"
                             "[ ]*"
                             "(?<contact>.*)"
                             );

    for (const auto &example : testdata) {
        qDebug() << "Run:" << example.text;
        QRegularExpressionMatch match = exp.match(example.text);
        QVERIFY(match.hasMatch());

        QString years = match.captured("years");
        QString name = match.captured("name");
        QString contact = match.captured("contact");
        QCOMPARE(years, example.years);
        QCOMPARE(name, example.name);
        QCOMPARE(contact, example.contact);
    }
}

QTEST_GUILESS_MAIN(TestCopyrightConvert);
