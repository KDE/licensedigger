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
    // convert statements
    void detectCopyright();
    void convertSingleCopyrightStatement();
    void convertFullHeader();
    void skipSourceCodeStrings();

    // convert copyright comments
    void prettyPrintCopyrightComment();

    // do not convert statements in code
    void doNotConvertCopyrightKeyworsInCode();
};
#endif
