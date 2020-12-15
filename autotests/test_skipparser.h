/*
 *  SPDX-FileCopyrightText: 2020 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef TEST_SKIPPARSER_H
#define TEST_SKIPPARSER_H

#include <QObject>

class TestSkipParser : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    // simple parser tests
    void basicStringMatcher();
    void basicPatternSetMatcher();
};
#endif
