/*
 XINX
 Copyright (C) 2007-2011 by Ulrich Van Den Hekke
 xinx@shadoware.org

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful.
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// Xinx header
#include "core/xinxcore.h"
#include <QString>
#include <QDesktopServices>
#include <QDir>
#include <QApplication>

/* XINXStaticDeleter */

XINXStaticDeleter * XINXStaticDeleter::s_self = 0;

XINXStaticDeleter::XINXStaticDeleter()
{

}

XINXStaticDeleter::~XINXStaticDeleter()
{
	s_self = 0;
	clear();
}

XINXStaticDeleter * XINXStaticDeleter::self()
{
	if (s_self == 0)
	{
		s_self = new XINXStaticDeleter();
	}
	return s_self;
}

void initSearchPath(QApplication * app)
{
	const QString configDirectory    = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
	const QString homeDirectory      = QDir::home().absoluteFilePath(configDirectory);
	const QString i18nDirectory     = QDir(homeDirectory).absoluteFilePath("i18n");
	const QString datasDirectory     = QDir(homeDirectory).absoluteFilePath("datas");
	const QString scriptDirectory    = QDir(homeDirectory).absoluteFilePath("scripts");
	const QString pluginsDirectory   = QDir(homeDirectory).absoluteFilePath("plugins");
	const QString templatesDirectory = QDir(homeDirectory).absoluteFilePath("templates");

	QDir::home().mkpath(datasDirectory);
	QDir::home().mkpath(scriptDirectory);
	QDir::home().mkpath(pluginsDirectory);
	QDir::home().mkpath(templatesDirectory);

	// .. for i18n ...
	QDir::addSearchPath("translations", i18nDirectory);
	QDir::addSearchPath("translations", QDir(QApplication::applicationDirPath()).absoluteFilePath("../i18n"));
	#ifndef Q_WS_WIN
	QDir::addSearchPath("translations", QDir(QApplication::applicationDirPath()).absoluteFilePath("../share/xinx/i18n"));
	QDir::addSearchPath("translations", QDir(QApplication::applicationDirPath()).absoluteFilePath("../lib/xinx/i18n"));
	#endif // Q_WS_WIN

	// .. for datas ...
	QDir::addSearchPath("datas", datasDirectory);
	QDir::addSearchPath("datas", QDir(QApplication::applicationDirPath()).absoluteFilePath("../datas"));
#ifndef Q_WS_WIN
	QDir::addSearchPath("datas", QDir(QApplication::applicationDirPath()).absoluteFilePath("../share/xinx/datas"));
	QDir::addSearchPath("datas", QDir(QApplication::applicationDirPath()).absoluteFilePath("../lib/xinx/datas"));
#endif // Q_WS_WIN

	// ... for scripts ...
	QDir::addSearchPath("scripts", scriptDirectory);
	QDir::addSearchPath("scripts", QDir(QApplication::applicationDirPath()).absoluteFilePath("../scripts"));
#ifndef Q_WS_WIN
	QDir::addSearchPath("scripts", QDir(QApplication::applicationDirPath()).absoluteFilePath("../share/xinx/scripts"));
	QDir::addSearchPath("scripts", QDir(QApplication::applicationDirPath()).absoluteFilePath("../lib/xinx/scripts"));
#endif // Q_WS_WIN

	// ... for plugins ...
	QDir::addSearchPath("plugins", pluginsDirectory);
	QDir::addSearchPath("plugins", QDir(QApplication::applicationDirPath()).absoluteFilePath("../plugins"));
#ifndef Q_WS_WIN
	QDir::addSearchPath("plugins", QDir(QApplication::applicationDirPath()).absoluteFilePath("../share/xinx/plugins"));
	QDir::addSearchPath("plugins", QDir(QApplication::applicationDirPath()).absoluteFilePath("../lib/xinx/plugins"));
#endif // Q_WS_WIN
	foreach(const QString & searchPath, QDir::searchPaths("plugins"))
	{
		app->addLibraryPath(searchPath);
	}

	// ... for tempalte ...
	QDir::addSearchPath("templates", templatesDirectory);
	QDir::addSearchPath("templates", QDir(QApplication::applicationDirPath()).absoluteFilePath("../templates"));
#ifndef Q_WS_WIN
	QDir::addSearchPath("templates", QDir(QApplication::applicationDirPath()).absoluteFilePath("../share/xinx/templates"));
	QDir::addSearchPath("templates", QDir(QApplication::applicationDirPath()).absoluteFilePath("../lib/xinx/templates"));
#endif // Q_WS_WIN
}


