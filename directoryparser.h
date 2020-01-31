/*
 *  SPDX-FileCopyrightText: 2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef DIRECTORYPARSER_H
#define DIRECTORYPARSER_H

#include "licenseregistry.h"
#include <QRegularExpression>

class DirectoryParser
{
public:
    QMap<QString, LicenseRegistry::SpdxExpression> parseAll(const QString &directory, bool convertMode = false) const;
    void convertCopyright(const QString &directory) const;
    QRegularExpression copyrightRegExp() const;

private:
    LicenseRegistry m_registry;
};

#endif // DIRECTORYPARSER_H
