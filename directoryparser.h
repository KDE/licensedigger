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
    enum class LicenseParser { SKIP_PARSER, REGEXP_PARSER };

    void setLicenseHeaderParser(LicenseParser parser);
    QMap<QString, LicenseRegistry::SpdxExpression> parseAll(const QString &directory, bool convertMode = false, const QString &ignorePattern = QString()) const;
    void convertCopyright(const QString &directory, const QString &ignorePattern = QString()) const;
    QRegularExpression copyrightRegExp() const;
    QRegularExpression spdxStatementRegExp() const;
    QString unifyCopyrightStatements(const QString &originalText) const;
    QString cleanupSpaceInCopyrightYearList(const QString &originalYearText) const;
    /**
     * @brief Uses regexp for the SPDX expression and replace matching text
     * @param fileContent The input content
     * @param spdxExpression The SPDX expression that shall be detected
     * @return Converted file content with correct SPDX statement
     */
    QString replaceHeaderText(const QString &fileContent, const QString &spdxExpression) const;

    /**
     * @brief Detect licenses by computing all matches
     * @param fileContent the content of the file
     * @return the list of detected license matches
     */
    QVector<LicenseRegistry::SpdxExpression> detectLicenses(const QString &fileContent) const;
    LicenseRegistry::SpdxExpression detectSpdxLicenseStatement(const QString &fileContent) const;

    /**
     * @brief Take license liste and prune statements
     *
     * This is a simple algorithm can handle the following cases:
     * - license containted twice
     * - license detacted singular and detected in OR statement as well
     * - license detacted singular and detected in WITH statement as well
     *
     * @param inputLicenses
     * @return
     */
    QVector<LicenseRegistry::SpdxExpression> pruneLicenseList(const QVector<LicenseRegistry::SpdxExpression> &inputLicenses) const;

private:
    QVector<LicenseRegistry::SpdxExpression> detectLicensesRegexpParser(const QString &fileContent) const;
    QVector<LicenseRegistry::SpdxExpression> detectLicensesSkipParser(const QString &fileContent) const;

    LicenseRegistry m_registry;
    LicenseParser m_parserType {LicenseParser::REGEXP_PARSER};
    static const QStringList s_supportedExtensions;
};

#endif // DIRECTORYPARSER_H
