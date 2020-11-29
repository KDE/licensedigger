/*
 *  SPDX-FileCopyrightText: 2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef TEST_LICENSEFILESAVAILABLE_H
#define TEST_LICENSEFILESAVAILABLE_H

#include <QObject>

class TestLicenseFilesAvailable : public QObject
{
    Q_OBJECT

public:
    TestLicenseFilesAvailable() = default;

private Q_SLOTS:
    void checkAll();
};
#endif
