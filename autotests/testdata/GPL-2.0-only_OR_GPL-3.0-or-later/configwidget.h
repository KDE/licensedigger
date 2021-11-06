/************************************************************************
 * KDevelop4 Custom Buildsystem Support                                 *
 *                                                                      *
 * Copyright 2010 Andreas Pakulat <apaku@gmx.de>                        *
 *                                                                      *
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 or version 3 of the License, or    *
 * (at your option) any later version.                                  *
 *                                                                      *
 * This program is distributed in the hope that it will be useful, but  *
 * WITHOUT ANY WARRANTY; without even the implied warranty of           *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU     *
 * General Public License for more details.                             *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with this program; if not, see <http://www.gnu.org/licenses/>. *
 ************************************************************************/

#ifndef CONFIGWIDGET_H
#define CONFIGWIDGET_H

#include <QWidget>

#include "custombuildsystemconfig.h"

namespace Ui
{
class ConfigWidget;
}

namespace KDevelop
{
    class IProject;
}

class ConfigWidget : public QWidget
{
Q_OBJECT
public:
    explicit ConfigWidget( QWidget* parent = nullptr );
    void loadConfig(const CustomBuildSystemConfig& cfg);
    CustomBuildSystemConfig config() const;
    void clear();
Q_SIGNALS:
    void changed();
private Q_SLOTS:
    void changeAction( int );
    void toggleActionEnablement( bool );
    void actionArgumentsEdited( const QString& );
    void actionEnvironmentChanged(const QString&);
    void actionExecutableChanged( const QUrl& );
    void actionExecutableChanged( const QString& );
private:
    template<typename F>
    void applyChange(F toolChanger);

    Ui::ConfigWidget* ui;
    QVector<CustomBuildSystemTool> m_tools;
    void setTool( const CustomBuildSystemTool& tool );
};

#endif
