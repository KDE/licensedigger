/*
 *  SPDX-FileCopyrightText: 2020  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef SKIPPARSER_H
#define SKIPPARSER_H

#include <QMap>
#include <QObject>
#include <QRegularExpression>
#include <QVector>
#include <optional>

class SkipParser
{
public:
    std::optional<std::pair<int, int>> findMatch(QString text, QString pattern) const;

    /**
     * @brief obtiain first matching pattern position
     * @param text
     * @param pattern
     * @return position, if found
     */
    std::optional<std::pair<int, int>> findMatch(QString text, QVector<QString> pattern) const;

private:
    /**
     * @brief computeKmpPrefix
     * @param prunedPattern pattern must not contain any skip character, no additional pruning here
     * @return
     */
    std::vector<int> computeKmpPrefix(const QString &prunedPattern) const;

    std::pair<std::vector<QChar>, std::vector<int>> computeTextSkipPrefix(const QString &text) const;

    std::optional<std::pair<int, int>> findMatchKMP(std::vector<QChar> prunedText, std::vector<int> textSkipPrefix, QString pattern) const;
    std::optional<std::pair<int, int>> findMatchNaive(QString text, QString pattern) const;
    static const QRegularExpression sSkipCharDetection;
    mutable QHash<QString, std::vector<int>> mPrefixCache;
};

#endif
