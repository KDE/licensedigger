/**
 * SPDX-FileCopyrightText Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef TEST_LICENSECONVERT_H
#define TEST_LICENSECONVERT_H

#include <QObject>

class TestLicenseConvert : public QObject
{
    Q_OBJECT

public:
    TestLicenseConvert() = default;

private Q_SLOTS:
    /**
     * @brief Test shall ensure correct expression ordering in regex for single license
     *
     * There are license texts for the same license, which differ only in their last character,
     * i.e. a missing "." at the license statement. The license text still is assumed as valid
     * but the regex matcher shall also match the dot. This test ensures that the dot is also
     * matched and replaced.
     */
    void greedyLicenseTextConversion();
};
#endif
