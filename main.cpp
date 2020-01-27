/*
 *  SPDX-FileCopyrightText: 2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
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
