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
    QString unifyCopyrightStatements(const QString &originalText) const;
    QString fixAnyMissingSpaceInCopyrightYearList(const QString &originalYearText) const;
    /**
     * @brief Uses regexp for the SPDX expression and replace matching text
     * @param fileContent The input content
     * @param spdxExpression The SPDX expression that shall be detected
     * @return Converted file content with correct SPDX statement
     */
    QString replaceHeaderText(const QString &fileContent, const QString &spdxExpression) const;

private:
    LicenseRegistry m_registry;
    static const QStringList s_supportedExtensions;
};

#endif // DIRECTORYPARSER_H
