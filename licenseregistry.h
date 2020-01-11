/*
 *  Copyright 2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of
 *  the License or (at your option) version 3 or any later version
 *  accepted by the membership of KDE e.V. (or its successor approved
 *  by the membership of KDE e.V.), which shall act as a proxy
 *  defined in Section 14 of version 3 of the license.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
    const static QString UnknownLicense;
    const static QString MissingLicense;
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
