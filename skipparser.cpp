/*
 *  SPDX-FileCopyrightText: 2020  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "skipparser.h"
#include <QDebug>
#include <functional>
#include <optional>
#include <set>

// for performance reasons, we need to have to lists
// but they must be kept in sync
const QRegularExpression SkipParser::sSkipCharDetection("[ |\\\n|\\\t|/|\\-|\\*|#]");
constexpr bool isSkipChar(const QChar &character)
{
    switch (character.toLatin1()) {
    case ' ':
        return true;
    case '\n':
        return true;
    case '\t':
        return true;
    case '/':
        return true;
    case '-':
        return true;
    case '*':
        return true;
    case '#':
        return true;
    default:
        return false;
    }
    return false;
}

std::optional<std::pair<int, int>> SkipParser::findMatchNaive(QString text, QString pattern) const
{
    if (pattern.length() > text.length()) {
        return {};
    }

    int start = 0;
    while (start <= text.length() - pattern.length()) {
        bool matchFound {true};
        int patternSkipOffset {0};
        int textSkipOffset {0};
        for (int i = 0; i < pattern.length(); ++i) {
            // skip pattern char if contained in skip-list
            // outcondition: valid char that can be matched
            if (isSkipChar(pattern.at(i))) {
                ++patternSkipOffset;
                continue;
            }
            while (isSkipChar(text.at(start + textSkipOffset + i - patternSkipOffset))) {
                if (start + textSkipOffset + i - patternSkipOffset + 1 >= text.length() - 1) {
                    matchFound = false; // we know this, because otherwise we would not be in this loop
                    break;
                }
                ++textSkipOffset;
            }
            if (text.at(start + textSkipOffset + i - patternSkipOffset) != pattern.at(i)) {
                matchFound = false;
                break;
            }
        }
        if (matchFound) {
            return std::optional<std::pair<int, int>> {{start, start + textSkipOffset + pattern.length() - patternSkipOffset - 1}};
        } else {
            ++start;
        }
    }

    return {};
}

std::vector<int> SkipParser::computeKmpPrefix(const QString &pattern) const
{
    const int length = pattern.length();
    std::vector<int> prefix(length);
    prefix[0] = 0;
    int k = 0;
    for (int q = 2; q <= length; ++q) {
        while (k > 0 && pattern.at(k) != pattern.at(q - 1)) {
            k = prefix.at(k - 1);
        }
        if (pattern.at(k) == pattern.at(q - 1)) {
            k = k + 1;
        }
        prefix[q - 1] = k;
    }
    return prefix;
}

std::optional<std::pair<int, int>> SkipParser::findMatchKMP(std::vector<QChar> prunedText, std::vector<int> textSkipPrefix, QString pattern) const
{
    // obtain prefix
    std::vector<int> prefix;
    if (mPrefixCache.contains(pattern)) {
        prefix = mPrefixCache[pattern];
    } else {
        prefix = computeKmpPrefix(pattern);
        mPrefixCache.insert(pattern, prefix);
    }

    // KMP Matcher
    const int textLength = prunedText.size();
    int q = 0;
    for (int i = 1; i <= textLength; ++i) {
        while (q > 0 && pattern.at(q) != prunedText.at(i - 1)) {
            q = prefix.at(q - 1);
        }
        if (pattern.at(q) == prunedText.at(i - 1)) {
            q = q + 1;
        }
        if (q == pattern.length()) {
            return std::optional<std::pair<int, int>> {{i - q + textSkipPrefix.at(i - q), i - 1 + textSkipPrefix.at(i - 1)}};
        }
    }

    return {};
}

std::optional<std::pair<int, int>> SkipParser::findMatch(QString text, QString pattern) const
{
    auto textPreprocessing = computeTextSkipPrefix(text);
    auto match = findMatchKMP(textPreprocessing.first, textPreprocessing.second, pattern.remove(sSkipCharDetection));
    qDebug() << "text   :" << text;
    qDebug() << "pattern:" << pattern;
    //            qDebug() << "start:  " << start;
    //            qDebug() << "skip:   " << textSkipOffset << " / " << patternSkipOffset;
    return match;
}

std::pair<std::vector<QChar>, std::vector<int>> SkipParser::computeTextSkipPrefix(const QString &text) const
{
    // prune text and compute skip prefix
    // skip prefix notes number of skipped chars for each position in "text"
    std::vector<int> textSkipPrefix(text.size());
    std::vector<QChar> prunedText;
    prunedText.reserve(text.size());
    int skipped = 0;
    for (int i = 0; i < text.size(); ++i) {
        if (isSkipChar(text.at(i))) {
            textSkipPrefix.at(i - skipped) = skipped;
            ++skipped;
        } else {
            prunedText.push_back(text.at(i));
            textSkipPrefix.at(i - skipped) = skipped;
        }
    }
    prunedText.shrink_to_fit();
    return {prunedText, textSkipPrefix};
}

std::optional<std::pair<int, int>> SkipParser::findMatch(QString text, QVector<QString> patterns) const
{
    // KMP can work with pruned patterns
    QSet<QString> prunedPatterns;
    // prune all skip chars from pattern
    for (const auto &pattern : patterns) {
        QString tmpPattern = pattern;
        tmpPattern.remove(sSkipCharDetection);
        prunedPatterns.insert(tmpPattern);
    }
    //    qDebug() << "Pruned canonical texts:" << (patterns.count() - prunedPatterns.count());

    auto textPreprocessing = computeTextSkipPrefix(text);
    for (const auto &pattern : prunedPatterns) {
        if (auto match = findMatchKMP(textPreprocessing.first, textPreprocessing.second, pattern)) {
            return match;
        }
    }
    return {};
}
