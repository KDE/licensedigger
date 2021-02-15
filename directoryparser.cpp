/*
 *  SPDX-FileCopyrightText: 2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "directoryparser.h"
#include "skipparser.h"
#include <QDebug>
#include <QDirIterator>
#include <QTextStream>
#include <QVector>

const QStringList DirectoryParser::s_supportedExtensions = {".cpp",  ".cc", ".c", ".h",  ".css",  ".hpp", ".qml", ".cmake", "CMakeLists.txt", ".in",  ".py", ".frag", ".vert",
                                                            ".glsl", "php", "sh", ".mm", ".java", ".kt",  ".js",  ".xml",   ".xsd",           ".xsl", ".pl", ".rb",   ".docbook"};

bool shallIgnoreFile(const QDirIterator &iterator, const QRegularExpression &fileToIgnorePattern)
{
    QFileInfo fileInfo(iterator.fileInfo());
    return !fileInfo.isFile() or (!fileToIgnorePattern.pattern().isEmpty() && fileToIgnorePattern.match(fileInfo.filePath()).hasMatch());
}

void DirectoryParser::setLicenseHeaderParser(LicenseParser parser)
{
    m_parserType = parser;
}

QRegularExpression DirectoryParser::spdxStatementRegExp() const
{
    static auto regexp = QRegularExpression("(SPDX-License-Identifier: (?<expression>(.*)))");
    return regexp;
}

QRegularExpression DirectoryParser::copyrightRegExp() const
{
    static auto regexp = QRegularExpression(
        "(?<!\")" // negative lookahead for quotation marks, to skip string statements
        "(SPDX-FileCopyrightText:|[cC]opyright(\\s+(: )?\\([cC]\\))|(?<![cC]opyright )\\([cC]\\)|[cC]opyright\\s+©|(?<![cC]opyright )©|[cC]opyright(:)?)"
        "[, ]+"
        "(?<years>([0-9]+(-[0-9]+| - [0-9]+| to [0-9]+|,[ ]?[0-9]+)*|%{CURRENT_YEAR}))"
        "[, ]+"
        "([bB]y[ ]+)?"
        "(?<name>([\u00C0-\u017Fa-zA-Z\\-\\.]+( [\u00C0-\u017Fa-zA-Z\\-\\.]+)*|%{AUTHOR}))"
        "[, ]*"
        "(?<contact>.*|%{EMAIL})");
    return regexp;
}

QString DirectoryParser::cleanupSpaceInCopyrightYearList(const QString &originalYearText) const
{
    QString cleanedYearText = originalYearText;

    static auto missingWhitespaceAfterCommaRegex = QRegularExpression(QStringLiteral(",(?=[0-9])"));
    static auto unneededWhitespaceAroundRangeRegex = QRegularExpression(QStringLiteral(" - (?=[0-9])"));
    static auto writtenRangeStatementRegex = QRegularExpression(QStringLiteral(" to (?=[0-9])"));

    cleanedYearText.replace(missingWhitespaceAfterCommaRegex, QStringLiteral(", "));
    cleanedYearText.replace(unneededWhitespaceAroundRangeRegex, QStringLiteral("-"));
    cleanedYearText.replace(writtenRangeStatementRegex, QStringLiteral("-"));

    return cleanedYearText;
}

QString DirectoryParser::unifyCopyrightStatements(const QString &originalText) const
{
    QString header = originalText;
    QRegularExpression regExp = copyrightRegExp();
    auto match = regExp.match(header);

    while (match.hasMatch()) {
        QString years = match.captured("years");
        years = cleanupSpaceInCopyrightYearList(years);
        QString name = match.captured("name");
        QString contact = match.captured("contact");

        QString unifiedCopyright = QString("SPDX-FileCopyrightText: %1 %2 %3").arg(years).arg(name).arg(contact).trimmed();
        header.replace(match.capturedStart(), match.capturedLength(), unifiedCopyright);
        match = regExp.match(header, match.capturedStart() + unifiedCopyright.length());
    }
    return header;
}

QString DirectoryParser::unifyCopyrightCommentHeader(const QString &originalText) const
{
    // restrict conversion to top-file comments
    if (!originalText.startsWith("/*")) {
        qWarning() << "File not starting with comment";
        return originalText;
    }
    auto lines = originalText.split("\n");
    for (int i = 0; i < lines.size(); ++i) {
        lines[i].replace(QRegularExpression("/(\\*)+"), "/*"); // initial comment line
        if (lines[i].startsWith("/*")) {
            continue; // do not further modify first line
        }
        lines[i].replace(QRegularExpression("[ ]*(\\*)+/"), "*/"); // final comment line
        if (lines[i].startsWith("*/")) {
            break;
        }
        // invariant: the following line is guaranteed to be a port of multiline comment
        lines[i].replace(QRegularExpression("^[ \\*]+(?!(\\\\))"), "    "); // in-between line
        lines[i].replace(QRegularExpression("[ \\*]+$"), ""); // in-between line
    }
    QString text = lines.join("\n");
    //qDebug() << text;
    return text;
}

QString DirectoryParser::replaceHeaderText(const QString &fileContent, const QString &spdxExpression) const
{
    auto regexps = m_registry.headerTextRegExps(spdxExpression);
    QString outputExpression = spdxExpression;
    outputExpression.replace('_', ' ');
    QString spdxOutputString = "SPDX-License-Identifier: " + outputExpression;
    QString newContent = fileContent;
    for (auto regexp : regexps) {
        newContent.replace(regexp, spdxOutputString);
    }
    return newContent;
}

LicenseRegistry::SpdxExpression DirectoryParser::detectSpdxLicenseStatement(const QString &fileContent) const
{
    QRegularExpression regExp = spdxStatementRegExp();
    auto match = regExp.match(fileContent);
    if (match.hasMatch()) {
        // TODO this very simple solution only works for SPDX expressions in our database
        // should be made more general
        return match.captured("expression").replace(' ', '_');
    }
    return QString();
}

QVector<LicenseRegistry::SpdxExpression> DirectoryParser::pruneLicenseList(const QVector<LicenseRegistry::SpdxExpression> &inputLicenses) const
{
    // TODO
    // - handle AND combinations
    // - handle complex OR combinations
    // - revisit operator preference order in SPDX and implement it here

    if (inputLicenses.length() == 1) {
        return inputLicenses;
    }

    auto licenses = inputLicenses;
    std::sort(licenses.begin(), licenses.end());

    // pruning step: remove duplicates
    licenses.erase(std::unique(licenses.begin(), licenses.end()), licenses.end());

    // pruning step: compute which licenses are supported with SPDX expression (splitting at "OR")
    // TODO this a very simple initial version and only works yet with simple license statements, not with multiple OR combinations
    QMap<LicenseRegistry::SpdxExpression, QVector<LicenseRegistry::SpdxExpression>> licenseClosure;
    for (const auto &license : qAsConst(licenses)) {
        QVector<LicenseRegistry::SpdxExpression> licenseChoice = license.split("_OR_").toVector();

        // remove all "WITH" statements
        for (int i = 0; i < licenseChoice.size(); ++i) {
            licenseChoice[i].remove(QRegularExpression("_WITH.*"));
        }
        licenseClosure[license] = licenseChoice;
    }
    QMutableVectorIterator<LicenseRegistry::SpdxExpression> iter(licenses);
    while (iter.hasNext()) {
        bool licensedContainedInClosure {false};
        LicenseRegistry::SpdxExpression expression = iter.next();
        for (auto iter = licenseClosure.begin(); iter != licenseClosure.end(); ++iter) {
            if (expression != iter.key() && iter.value().contains(expression)) {
                licensedContainedInClosure = true;
            }
        }
        if (licensedContainedInClosure) {
            iter.remove();
        }
    }
    return licenses;
}

QVector<LicenseRegistry::SpdxExpression> DirectoryParser::detectLicenses(const QString &fileContent) const
{
    switch (m_parserType) {
    case DirectoryParser::LicenseParser::REGEXP_PARSER:
        return detectLicensesRegexpParser(fileContent);
    case DirectoryParser::LicenseParser::SKIP_PARSER:
        return detectLicensesSkipParser(fileContent);
    }
    return {};
}

QVector<LicenseRegistry::SpdxExpression> DirectoryParser::detectLicensesSkipParser(const QString &fileContent) const
{
    SkipParser parser;
    QVector<LicenseRegistry::SpdxExpression> testExpressions = m_registry.expressions();
    QVector<LicenseRegistry::SpdxExpression> detectedLicenses;
    for (auto expression : testExpressions) {
        auto match = parser.findMatch(fileContent, m_registry.headerTexts(expression));
        if (match) {
            detectedLicenses << expression;
        }
    }
    LicenseRegistry::SpdxExpression spdxStatement = detectSpdxLicenseStatement(fileContent);
    if (!spdxStatement.isEmpty()) {
        detectedLicenses << spdxStatement;
    }
    return detectedLicenses;
}

QVector<LicenseRegistry::SpdxExpression> DirectoryParser::detectLicensesRegexpParser(const QString &fileContent) const
{
    QVector<LicenseRegistry::SpdxExpression> testExpressions = m_registry.expressions();
    QVector<LicenseRegistry::SpdxExpression> detectedLicenses;
    for (auto expression : testExpressions) {
        auto regexps = m_registry.headerTextRegExps(expression);
        for (auto regexp : regexps) {
            if (fileContent.contains(regexp)) {
                detectedLicenses << expression;
            }
        }
    }
    LicenseRegistry::SpdxExpression spdxStatement = detectSpdxLicenseStatement(fileContent);
    if (!spdxStatement.isEmpty()) {
        detectedLicenses << spdxStatement;
    }
    return detectedLicenses;
}

QMap<QString, LicenseRegistry::SpdxExpression> DirectoryParser::parseAll(const QString &directory, bool convertMode, const QString &ignorePattern) const
{
    QVector<LicenseRegistry::SpdxExpression> expressions = m_registry.expressions();
    QMap<QString, LicenseRegistry::SpdxExpression> results;

    if (convertMode) {
        qInfo() << "Running parser in CONVERT mode: every found license will be replaced with SPDX identifiers";
    }

    QStringList missingLicenseHeaderBlacklist;
    {
        QFile file(":/annotations/missing-headers-blacklist.txt");
        file.open(QIODevice::ReadOnly);
        QTextStream in(&file);
        QString line;
        while (in.readLineInto(&line)) {
            missingLicenseHeaderBlacklist.append(line);
        }
    }
    QStringList missingLicenseHeaderGeneratedFileBlacklist;
    {
        QFile file(":/annotations/generated-files.txt");
        file.open(QIODevice::ReadOnly);
        QTextStream in(&file);
        QString line;
        while (in.readLineInto(&line)) {
            missingLicenseHeaderGeneratedFileBlacklist.append(line);
        }
    }

    QRegularExpression ignoreFile(ignorePattern);

    QDirIterator iterator(directory, QDirIterator::Subdirectories);
    while (iterator.hasNext()) {
        QFile file(iterator.next());
        if (shallIgnoreFile(iterator, ignoreFile)) {
            continue;
        }

        bool skip = true;
        for (const auto &ending : DirectoryParser::s_supportedExtensions) {
            if (file.fileName().endsWith(ending)) {
                skip = false;
                break;
            }
        }
        if (skip) {
            continue;
        }

        file.open(QIODevice::ReadOnly);
        const QString fileContent = file.readAll();
        file.close();

        //        qDebug() << "checking:" << iterator.fileInfo();
        QVector<LicenseRegistry::SpdxExpression> licenses = detectLicenses(fileContent);
        licenses = pruneLicenseList(licenses);

        if (licenses.count() == 1) {
            results.insert(iterator.fileInfo().filePath(), licenses.first());
            //            qDebug() << "---> " << iterator.fileInfo().filePath() << identifier;
        } else if (licenses.count() > 1) {
            qCritical() << "UNHANDLED MULTI-LICENSE CASE" << iterator.fileInfo().filePath() << "-->" << licenses;
            results[iterator.fileInfo().filePath()] = LicenseRegistry::AmbigiousLicense;
        } else {
            // if nothing matches, report error
            results.insert(iterator.fileInfo().filePath(), LicenseRegistry::UnknownLicense);

            // check for blacklisted file because of missing license header only when no license was detected
            for (auto backlistPath : missingLicenseHeaderBlacklist) {
                if (iterator.fileInfo().filePath().endsWith(backlistPath)) {
                    results.insert(iterator.fileInfo().filePath(), LicenseRegistry::MissingLicense);
                    break;
                }
            }
            for (auto backlistPath : missingLicenseHeaderGeneratedFileBlacklist) {
                if (iterator.fileInfo().filePath().endsWith(backlistPath)) {
                    results.insert(iterator.fileInfo().filePath(), LicenseRegistry::MissingLicenseForGeneratedFile);
                    break;
                }
            }
        }

        const QString expression = results.value(iterator.fileInfo().filePath());
        if (convertMode && !m_registry.isFakeLicenseMarker(expression)) {
            QString newContent = replaceHeaderText(fileContent, expression);
            // qDebug() << newContent;
            file.open(QIODevice::WriteOnly);
            file.write(newContent.toUtf8());
            file.close();
        }
    }

    if (convertMode) {
        // compute needed licenses
        QSet<QString> identifiers;
        for (const auto &expression : results.values()) {
            auto expressionSplit = expression.split('_');
            for (const auto &identifier : expressionSplit) {
                // remove SPDX syntax attributes
                if (identifier == "OR" || identifier == "AND" || identifier == "WITH") {
                    continue;
                }
                // remove special placeholders
                if (m_registry.isFakeLicenseMarker(identifier)) {
                    continue;
                }
                identifiers.insert(identifier);
            }
        }
        // create licenses directory and put license files therein
        QString licenseDir = directory + "/LICENSES/";
        QDir().mkdir(licenseDir);
        const auto licenseFiles = m_registry.licenseFiles();
        for (const auto &identifier : identifiers) {
            qDebug() << "Deploy license file" << identifier << licenseFiles.value(identifier);
            QFile::copy(licenseFiles.value(identifier), licenseDir + identifier + ".txt");
        }
    }

    return results;
}

void DirectoryParser::convertCopyright(const QString &directory, ConvertOptions options, const QString &ignorePattern) const
{
    QRegularExpression ignoreFile(ignorePattern);

    QDirIterator iterator(directory, QDirIterator::Subdirectories);
    while (iterator.hasNext()) {
        QFile file(iterator.next());
        if (shallIgnoreFile(iterator, ignoreFile)) {
            continue;
        }
        bool skip = true;
        for (const auto &ending : DirectoryParser::s_supportedExtensions) {
            if (file.fileName().endsWith(ending)) {
                skip = false;
                break;
            }
        }
        if (skip) {
            continue;
        }

        file.open(QIODevice::ReadOnly);
        QString content = file.readAll();
        file.close();
        if (options & ConvertOption::COPYRIGHT_TEXT) {
            content = unifyCopyrightStatements(content);
        }
        if (options & ConvertOption::PRETTY) {
            content = unifyCopyrightCommentHeader(content);
        }
        file.open(QIODevice::WriteOnly);
        file.write(content.toUtf8());
        file.close();
    }
}
