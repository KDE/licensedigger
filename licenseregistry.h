/*
 *  SPDX-FileCopyrightText: 2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef LICENSEREGISTRY_H
#define LICENSEREGISTRY_H

#include <QObject>
#include <QVector>
#include <QMap>
#include <QRegularExpression>

class LicenseRegistry : public QObject
{
    Q_OBJECT
public:
    using SpdxIdentifier = QString;
    using SpdxExpression = QString;
    const static QString ToClarifyLicense;
    const static QString UnknownLicense;
    const static QString MissingLicense;
    const static QString AmbigiousLicense;
    const static QString MissingLicenseForGeneratedFile;

    explicit LicenseRegistry(QObject *parent = nullptr);

    /**
     * @brief list of all detectable SPDX expressions
     */
    QVector<SpdxExpression> expressions() const;

    /**
     * @brief list of all known SPDX identifiers
     */
    QVector<SpdxIdentifier> identifiers() const;

    QMap<SpdxIdentifier, QString> licenseFiles() const;

    QVector<QString> headerTexts(const SpdxExpression &identifier) const;

    QRegularExpression headerTextRegExp(const SpdxExpression &identifier) const;

private:
    void loadLicenseHeaders();
    void loadLicenseFiles();
    QMap<SpdxExpression, QVector<QString>> m_registry;
    mutable QMap<SpdxExpression, QRegularExpression> m_regexpCache;
    mutable QMap<SpdxIdentifier, QString> m_licenseFiles;
};

#endif // LICENSEREGISTRY_H
