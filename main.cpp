/*
 *  SPDX-FileCopyrightText: 2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "directoryparser.h"
#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <iostream>

int main(int argc, char *argv[])
{
    const std::string hightlightOut{"\033[1;34m"};
    const std::string defaultOut{"\033[0m"};

    QCoreApplication app(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("Digs into licenses and replaces them with SPDX identifiers");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("directory", QCoreApplication::translate("main", "Source file to copy."));

    QCommandLineOption dryOption(QStringList() << "dry",
            "only show detected licenses, do not change any file");
    parser.addOption(dryOption);

    QCommandLineOption forceOption(QStringList() << "f" << "force",
            "convert stated directory right away, do not ask");
    parser.addOption(forceOption);

    QCommandLineOption licenseConvertOption(QStringList() << "l" << "licenses",
            "convert license statements to SPDX-License-Identifier, but nothing else");
    parser.addOption(licenseConvertOption);

    QCommandLineOption copyrightConvertOption(QStringList() << "c" << "copyright",
            "convert copyright statements to SPDX-FileCopyrightText, but nothing else");
    parser.addOption(copyrightConvertOption);

    QCommandLineOption ignorePatternOption(QStringList() << "i" << "ignore",
            "Ignore file path matching the pattern", "ignorePattern", "");
    parser.addOption(ignorePatternOption);

    parser.process(app);

    const QStringList args = parser.positionalArguments();
    if (args.count() == 0) {
        qCritical() << "Required license digging directory is missing";
        return 1;
    }
    const QString directory = args.at(0);
    const QString ignorePattern = parser.value(ignorePatternOption);

    qInfo() << "Digging recursively all files in directory:" << directory;
    DirectoryParser licenseParser;

    // print overview if no parameter is set
    if (!(parser.isSet(licenseConvertOption) || parser.isSet(copyrightConvertOption) || parser.isSet(forceOption))) {
        std::cout << hightlightOut
                  << "==============================" << std::endl
                  << "= LICENSE DETECTION OVERVIEW =" << std::endl
                  << "==============================" << defaultOut
                  << std::endl;
        const auto results = licenseParser.parseAll(directory, false, ignorePattern);
        int undetectedLicenses = 0;
        int detectedLicenses = 0;
        for (auto iter = results.constBegin(); iter != results.constEnd(); iter++) {
            if (iter.value() == LicenseRegistry::UnknownLicense) {
                ++undetectedLicenses;
            } else {
                ++detectedLicenses;
            }
            qInfo() << iter.key() << " --> " << iter.value();
        }
        qInfo().nospace() << "\n" << "Undetected files: " << undetectedLicenses << " (total: " << (undetectedLicenses + detectedLicenses) << ")";
    }

    bool userWantsConversion{ false };
    if (!(parser.isSet(dryOption) || parser.isSet(licenseConvertOption) || parser.isSet(copyrightConvertOption) || parser.isSet(forceOption))) {
        std::string convertAnswer{ "" };
        std::cout << std::endl;
        while (convertAnswer != "n" && convertAnswer != "y") {
            std::cout << "Perform file conversions? [y/n] ";
            std::cin >> convertAnswer;
        }
        if (convertAnswer == "y") {
            userWantsConversion = true;
        }
    }

    // actual conversion steps
    const bool convertLicense = userWantsConversion || parser.isSet(licenseConvertOption) || parser.isSet(forceOption);
    const bool convertCopyright = userWantsConversion || parser.isSet(copyrightConvertOption) || parser.isSet(forceOption);

    if (convertLicense) {
        std::cout << hightlightOut << "Convert license statements: starting..." << defaultOut << std::endl;
        licenseParser.parseAll(directory, true, ignorePattern);
        std::cout << hightlightOut << "Convert license statements: DONE." << defaultOut << std::endl;
    }

    if (convertCopyright) {
        std::cout << hightlightOut << "Convert copyright statements: starting..." << defaultOut << std::endl;
        licenseParser.convertCopyright(directory);
        std::cout << hightlightOut << "Convert copyright statements: DONE." << defaultOut << std::endl;
    }
}
