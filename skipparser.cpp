/*
 *  SPDX-FileCopyrightText: 2020  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "skipparser.h"
#include <QDebug>
#include <set>
#include <optional>
#include <functional>

// for performance reasons, we need to have to lists
// but they must be kept in sync
const QRegularExpression SkipParser::sSkipCharDetection("[ |\\\n|\\\t|/|\\-|\\*|#]");
constexpr bool isSkipChar(const QChar &character) {
    switch(character.unicode()) {
    case ' ': return true;
    case '\n': return true;
    case '\t': return true;
    case '/': return true;
    case '-': return true;
    case '*': return true;
    case '#': return true;
    default: return false;
    }
    return false;
}

std::optional<std::pair<int, int>> SkipParser::findMatchNaive(QString text, QString pattern) const
{
    static std::set<QChar> patternSkipChars = {' ', '\n', '\t', '/', '-', '*', '#'};
    static std::set<QChar> textSkipChars = patternSkipChars;

    if (pattern.length() > text.length()) {
        return {};
    }

    int start = 0;
    while (start <= text.length() - pattern.length()) {
        bool matchFound{ true };
        int patternSkipOffset{ 0 };
        int textSkipOffset{ 0 };
        for (int i = 0; i < pattern.length(); ++i) {
            // skip pattern char if contained in skip-list
            // outcondition: valid char that can be matched
            if (patternSkipChars.find(pattern.at(i)) != patternSkipChars.end()) {
                ++patternSkipOffset;
                continue;
            }
            while (textSkipChars.find(text.at(start + textSkipOffset + i - patternSkipOffset)) != textSkipChars.end()) {
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
            return std::optional<std::pair<int,int>>{ {start, start + textSkipOffset + pattern.length() - patternSkipOffset - 1} };
        } else {
            ++start;
        }
    }

    return {};
}

std::optional<std::pair<int, int>> SkipParser::findMatchKMP(QString origText, QString origPattern) const
{
    const int origTextLength = origText.size();

    // prune text and compute skip prefix
    // skip prefix notes number of skipped chars for each position in "text"
    QString text = QString();
    std::vector<int> textSkipPrefix(origTextLength);
    int skipped = 0;
    for (int i = 0; i < origTextLength; ++i) {
        if (isSkipChar(origText.at(i))) {
            textSkipPrefix.at(i - skipped) = skipped;
            ++skipped;
        } else {
            text += origText.at(i);
            textSkipPrefix.at(i - skipped) = skipped;
        }
    }

    // prune all skip chars from pattern
    QString pattern = origPattern;
    pattern.remove(sSkipCharDetection);

    //BEGIN compute KMP prefix function
    const int patternLength = pattern.size();
    std::vector<int> prefix(patternLength, -1); // -1 used as not initialized
    prefix[0] = 0;
    int k = 0;
    for (int q = 2; q <= patternLength; ++q) {
        while (k > 0 && pattern.at(k) != pattern.at(q - 1)) {
            Q_ASSERT(prefix.at(k) != -1);
            k = prefix.at(k-1);
        }
        if (pattern.at(k) == pattern.at(q - 1)) {
            k = k + 1;
        }
        prefix[q-1] = k;
    }
    //END

    // KMP Matcher
    const int textLength = text.size();
    int q = 0;
    for (int i = 1; i <= textLength; ++i) {
        while(q > 0 && pattern.at(q) != text.at(i - 1)) {
            q = prefix.at(q - 1);
        }
        if (pattern.at(q) == text.at(i - 1)) {
            q = q + 1;
        }
        if (q == patternLength) {
            return std::optional<std::pair<int,int>>{ {i - q + textSkipPrefix.at(i-q), i - 1 + textSkipPrefix.at(i - 1)} };
        }
    }

    return {};
}

std::optional<std::pair<int, int>> SkipParser::findMatch(QString text, QString pattern) const
{
    auto match = findMatchKMP(text, pattern);
    qDebug() << "text   :" << text;
    qDebug() << "pattern:" << pattern;
    //            qDebug() << "start:  " << start;
    //            qDebug() << "skip:   " << textSkipOffset << " / " << patternSkipOffset;
    return match;
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

    for (const auto &pattern : prunedPatterns) {
        if (auto match = findMatchKMP(text, pattern)) {
            return match;
        }
    }
    return {};
}
