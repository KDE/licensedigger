/*
 *  Copyright 2006-2009  Parker Coates <coates@kde.org>
 *
 *  This file is part of Killbots.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "mainwindow.h"
#include "killbots_version.h"

#include <KLocalizedString>
#include <Kdelibs4ConfigMigrator>
#include <QApplication>
#include <KAboutData>
#include <KCrash>
#include <QCommandLineParser>
#include <KDBusService>

int main(int argc, char **argv)
{
    // Fixes blurry icons with fractional scaling
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

	QApplication app(argc, argv);

    Kdelibs4ConfigMigrator migrate(QStringLiteral("killbots"));
    migrate.setConfigFiles(QStringList() << QStringLiteral("killbotsrc"));
    migrate.setUiFiles(QStringList() << QStringLiteral("killbotsui.rc"));
    migrate.migrate();

    KLocalizedString::setApplicationDomain("killbots");

    KAboutData about(QStringLiteral("killbots"), i18n("Killbots"), QStringLiteral(KILLBOTS_VERSION_STRING));
    about.setShortDescription(i18n("A KDE game of killer robots and teleportation."));
    about.setLicense(KAboutLicense::GPL_V2);
    about.setCopyrightStatement(i18n("© 2006-2009, Parker Coates"));
    about.addAuthor(i18n("Parker Coates"), i18n("Developer"), QStringLiteral("coates@kde.org"));
    about.addCredit(i18n("Mark Rae"), i18n("Author of Gnome Robots. Invented safe teleports, pushing junkheaps and fast robots."), QLatin1String(""), QStringLiteral("https://wiki.gnome.org/Apps/Robots"));
    about.setHomepage(QStringLiteral("https://www.kde.org/applications/games/killbots/"));

    QCommandLineParser parser;
    KAboutData::setApplicationData(about);
    KCrash::initialize();
    about.setupCommandLine(&parser);
    parser.process(app);
    about.processCommandLine(&parser);

    KDBusService service;

    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("killbots")));

    Killbots::MainWindow *mainWindow = new Killbots::MainWindow;
    mainWindow->show();

    return app.exec();
}
