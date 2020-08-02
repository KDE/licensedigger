/********************************************************************
 KWin - the KDE window manager
 This file is part of the KDE project.

Copyright (C) 2015 Martin Gräßlin <mgraesslin@kde.org>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*********************************************************************/
#include "screens_hwcomposer.h"
#include "hwcomposer_backend.h"

namespace KWin
{

HwcomposerScreens::HwcomposerScreens(HwcomposerBackend *backend, QObject *parent)
    : OutputScreens(backend, parent)
    , m_backend(backend)
{
    connect(m_backend, &HwcomposerBackend::screensQueried, this, &OutputScreens::updateCount);
    connect(m_backend, &HwcomposerBackend::screensQueried, this, &OutputScreens::changed);
}

}
