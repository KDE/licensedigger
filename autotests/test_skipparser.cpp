/*
 *  SPDX-FileCopyrightText: 2020 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "test_skipparser.h"
#include "../skipparser.h"
#include "../licenseregistry.h"
#include <QTest>
#include <QVector>
#include <QDebug>
#include <QDir>

void TestSkipParser::basicStringMatcher()
{
    SkipParser parser;

    { // simple substring
        QString pattern{"aaa"};
        QString text{"abcaaabc"};
        auto match = parser.findMatch(text, pattern);
        QVERIFY(match);
        QCOMPARE(match->first, 3);
        QCOMPARE(match->second, 5);
    }
    { // right-hand limit
        QString pattern{"aaa"};
        QString text{"abcaaa"};
        auto match = parser.findMatch(text, pattern);
        QVERIFY(match);
        QCOMPARE(match->first, 3);
        QCOMPARE(match->second, 5);
    }
    { // left-hand limit
        QString pattern{"aaa"};
        QString text{"aaa"};
        auto match = parser.findMatch(text, pattern);
        QVERIFY(match);
        QCOMPARE(match->first, 0);
        QCOMPARE(match->second, 2);
    }
    { // skipping whites in pattern
        QString pattern{"aa a"};
        QString text{"abcaaabc"};
        auto match = parser.findMatch(text, pattern);
        QVERIFY(match);
        QCOMPARE(match->first, 3);
        QCOMPARE(match->second, 5);
    }
    { // skipping whites in text
        QString pattern{"aaa"};
        QString text{"abcaa abc"};
        auto match = parser.findMatch(text, pattern);
        QVERIFY(match);
        QCOMPARE(match->first, 3);
        QCOMPARE(match->second, 6);
    }
    { // pattern and text skips
        QString pattern{"aa a"};
        QString text{"abca a abc"};
        auto match = parser.findMatch(text, pattern);
        QVERIFY(match);
        QCOMPARE(match->first, 3);
        QCOMPARE(match->second, 7);
    }
    { // pattern and text skips
        QString pattern{"aa a"};
        QString text{"abca  a abc"};
        auto match = parser.findMatch(text, pattern);
        QCOMPARE(match->first, 3);
        QCOMPARE(match->second, 8);
    }
    { // not matching and skip at right side
        QString pattern{"aa a"};
        QString text{"abca  a"};
        auto match = parser.findMatch(text, pattern);
        QVERIFY(!match);
    }
}

void TestSkipParser::basicPatternSetMatcher()
{
    SkipParser parser;
    { // skipping whites in pattern
        QVector<QString> patterns = {{"aa a"} , {"aaa"}};
        QString text{"abcaaabc"};
        auto match = parser.findMatch(text, patterns);
        QVERIFY(match);
        QCOMPARE(match->first, 3);
        QCOMPARE(match->second, 5);
    }
}

QTEST_GUILESS_MAIN(TestSkipParser);
