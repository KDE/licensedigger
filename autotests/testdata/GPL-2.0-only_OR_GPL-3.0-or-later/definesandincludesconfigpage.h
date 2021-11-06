/************************************************************************
 *                                                                      *
 * Copyright 2010 Andreas Pakulat <apaku@gmx.de>                        *
 *                                                                      *
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 or version 3 of the   *
 * License, or (at your option) any later version.                      *
 *                                                                      *
 * This program is distributed in the hope that it will be useful, but  *
 * WITHOUT ANY WARRANTY; without even the implied warranty of           *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU     *
 * General Public License for more details.                             *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with this program; if not, see <http://www.gnu.org/licenses/>. *
 ************************************************************************/

#ifndef KCM_DEFINESANDINCLUDESCONFIGPAGE_H
#define KCM_DEFINESANDINCLUDESCONFIGPAGE_H

#include <project/projectconfigpage.h>

#include "customdefinesandincludes.h"

class DefinesAndIncludesConfigPage : public ProjectConfigPage<CustomDefinesAndIncludes>
{
    Q_OBJECT
public:
    DefinesAndIncludesConfigPage(KDevelop::IPlugin* plugin, const KDevelop::ProjectConfigOptions& options, QWidget* parent);
    ~DefinesAndIncludesConfigPage() override;

    QString name() const override;
    QString fullName() const override;
    QIcon icon() const override;

    void apply() override;
    void reset() override;
private:
    class ProjectPathsWidget* configWidget;
    void loadFrom( KConfig* cfg );
    void saveTo( KConfig* cfg, KDevelop::IProject* );
};

#endif
