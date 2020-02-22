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
        { "SPDX-FileCopyrightText: 2018 John Doe <mail@example.com>" , "2018", "John Doe", "<mail@example.com>" },
        { "SPDX-FileCopyrightText: 2018 Aurélien Utfät <mail@example.com>" , "2018", "Aurélien Utfät", "<mail@example.com>" },
        { "Copyright: 2018 John Doe <mail@example.com>" , "2018", "John Doe", "<mail@example.com>" },
        { "Copyright 2018-2019 John Doe <mail@example.com>" , "2018-2019", "John Doe", "<mail@example.com>" },
        { "Copyright © 2018-2019 John Doe" , "2018-2019", "John Doe", "" },
        { "Copyright 2018-2019 Singlename <mail@example.com>" , "2018-2019", "Singlename", "<mail@example.com>" },
        { "Copyright 2018-2019 Foo of Baa  <mail@example.com>" , "2018-2019", "Foo of Baa", "<mail@example.com>" },
        { "Copyright 2018, 2019-2020 Foo of Baa  <mail@example.com>" , "2018, 2019-2020", "Foo of Baa", "<mail@example.com>" },
        // KAppTemplate template variables
        { "Copyright (C) %{CURRENT_YEAR} by %{AUTHOR} <%{EMAIL}>" , "%{CURRENT_YEAR}", "%{AUTHOR}", "<%{EMAIL}>" },
    };

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

void TestCopyrightConvert::convertSingleCopyrightStatement()
{
    DirectoryParser parser;
    QRegularExpression exp = parser.copyrightRegExp();

    std::vector<std::pair<QString, QString>> testdata = {
        {"Copyright  2018   John Doe   <mail@example.com>", "SPDX-FileCopyrightText: 2018 John Doe <mail@example.com>"}
    };

    for (const auto &example : testdata) {
        QRegularExpressionMatch match = exp.match(example.first);
        QVERIFY(match.hasMatch());

        QString years = match.captured("years");
        QString name = match.captured("name");
        QString contact = match.captured("contact");

        QString unifiedCopyright = QString("SPDX-FileCopyrightText: %1 %2 %3").arg(years).arg(name).arg(contact);
        QCOMPARE(unifiedCopyright, example.second);
    }
}

void TestCopyrightConvert::convertFullHeader()
{
    const QString originalHeader =
            "/*\n"
            " * Copyright  2018   John Doe   <mail@example.com>\n"
            " * Copyright  2018-2019   Jane Doe <another@example.com>\n"
            " */\n";

    const QString targetHeader =
            "/*\n"
            " * SPDX-FileCopyrightText: 2018 John Doe <mail@example.com>\n"
            " * SPDX-FileCopyrightText: 2018-2019 Jane Doe <another@example.com>\n"
            " */\n";

    DirectoryParser parser;
    QString header = parser.unifyCopyrightStatements(originalHeader);
    QCOMPARE(header, targetHeader);
}

QTEST_GUILESS_MAIN(TestCopyrightConvert);
