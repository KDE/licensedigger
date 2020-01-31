/**
 * SPDX-FileCopyrightText Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef TEST_COPYRIGHTCONVERT_H
#define TEST_COPYRIGHTCONVERT_H

#include <QObject>

class TestCopyrightConvert : public QObject
{
    Q_OBJECT

public:
    TestCopyrightConvert() = default;

private Q_SLOTS:
    void detectCopyright();
    void convertSingleCopyrightStatement();
    void convertFullHeader();
};
#endif
