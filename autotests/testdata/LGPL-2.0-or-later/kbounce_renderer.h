/*
 * Copyright (C) 2006 Dmitry Suzdalev <dimsuz@gmail.com>
 * Copyright (C) 2007 Tomasz Boczkowski <tboczkowski@onet.pl>
 *
 * This file is part of the KDE project "KBounce"
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef RENDERER_H
#define RENDERER_H

#include <QSvgRenderer>
#include <KGameRenderer>

#include <QSize>
#include <QHash>
#include <QPixmap>


/**
 * Class for rendering elements of game SVG to QPixmap
 */

class KBounceRenderer : public KGameRenderer
{
    Q_OBJECT
    public:
        /**
         * Constructor.
         * @param fileName path to SVG containing game graphics
         */
        explicit KBounceRenderer();
        /**
         * Destructor.
         */
        ~KBounceRenderer() override;
        /**
         * Sets Background size and invalidates background cache
         */
        void setBackgroundSize(QSize size);
        /**
         * Renders background to QPixmap of size set by setBachgroundSize
         * Background pixmap is cached (setBackgroundSize() invalidates the cache)
         */
        QPixmap renderBackground();
        /**
         * Set s the path were custom background pictures are located.
         */
        void setCustomBackgroundPath(const QString &path);
        /**
         * Returns a random pixmap from the custom background path.
         * If no picture is located in this path the pixmap is null.
         */
        QPixmap getRandomBackgroundPixmap(const QString& path);
        bool loadNewBackgroundPixmap();

    private:
        QSvgRenderer m_svgRenderer;
        QSize m_backgroundSize;
        QPixmap m_cachedBackground;
        QPixmap m_randomBackground;

        QString m_customBackgroundPath;
        bool m_useRandomBackgrounds;
};

#endif //RENDERER_H

