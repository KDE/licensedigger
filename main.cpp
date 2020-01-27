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

#include "directoryparser.h"
#include <QGuiApplication>
#include <QCommandLineParser>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("Digs into licenses and replaces them with SPDX identifiers");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("directory", QCoreApplication::translate("main", "Source file to copy."));

    QCommandLineOption convertOption(QStringList() << "c" << "convert",
            "convert project to SPDX identifiers");
    parser.addOption(convertOption);

    QCommandLineOption copyrightOption(QStringList() << "r" << "copyright",
            "convert copyright identifieres");
    parser.addOption(copyrightOption);

    parser.process(app);

    const QStringList args = parser.positionalArguments();
    if (args.count() == 0) {
        qCritical() << "Directory for scanning must be provided";
        return 1;
    }
    const QString directory = args.at(0);

    qInfo() << "Parsing headers in" << directory;
    DirectoryParser licenseParser;
    const auto results = licenseParser.parseAll(directory, parser.isSet(convertOption));
    int undetectedLicenses = 0;
    int detectedLicenses = 0;
    for (auto iter = results.constBegin(); iter != results.constEnd(); iter++) {
        if (iter.value() == LicenseRegistry::UnknownLicense) {
            ++undetectedLicenses;
        } else {
            ++detectedLicenses;
        }
        qDebug() << iter.key() << " --> " << iter.value();
    }

    qInfo() << "Converting copyright statements";
    if (parser.isSet(copyrightOption)) {
        licenseParser.convertCopyright(directory);
    }

    qDebug().nospace() << "\n" << "Undetected files: " << undetectedLicenses << " (total: " << (undetectedLicenses + detectedLicenses) << ")";
}
