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

std::optional<std::pair<int, int>> SkipParser::findMatch(QString text, QString pattern) const
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
//            qDebug() << "text   :" << text;
//            qDebug() << "pattern:" << pattern;
//            qDebug() << "start:  " << start;
//            qDebug() << "skip:   " << textSkipOffset << " / " << patternSkipOffset;
            return std::optional<std::pair<int,int>>{ {start, start + textSkipOffset + pattern.length() - patternSkipOffset - 1} };
        } else {
            ++start;
        }
    }

    return {};
}

// TODO: naive and inefficient implementation, but setting a baseline
std::optional<std::pair<int, int>> SkipParser::findMatch(QString text, QVector<QString> patterns) const
{
    for (const auto &pattern : patterns) {
        if (auto match = findMatch(text, pattern)) {
            return match;
        }
    }
    return {};
}
