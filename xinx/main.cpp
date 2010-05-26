/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2009 by Ulrich Van Den Hekke                              *
 * ulrich.vdh@shadoware.org                                                *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 * *********************************************************************** */

// Xinx header
#include "uniqueapplication.h"
#include "mainformimpl.h"
#include <snipets/snipetmanager.h>
#include <snipets/snipetlist.h>
#include <core/xinxconfig.h>
#include <core/exceptions.h>
#include <plugins/xinxpluginsloader.h>
#include "newversionwizardimpl.h"

// QCodeEdit header
#include <qlinemarksinfocenter.h>

// Qt header
#include <QApplication>
#include <QSplashScreen>
#include <QStringList>
#include <QFile>
#include <QMessageBox>
#include <QLocale>
#include <QTranslator>
#include <QDir>
#include <QThread>
#include <QBitmap>
#include <QStyleFactory>
#include <QDesktopServices>

// C++ header
#include <csignal>
#include <iostream>
#include <time.h>

// Import plugins
Q_IMPORT_PLUGIN(coreplugin);

MainformImpl * mainWin = NULL;

class SignalSegFaultException : public XinxException
{
public:
	SignalSegFaultException(int signal) : XinxException(QString("Signal emited : %1").arg(signal))
	{

	}
};

void backup_appli_signal(int signal)
{
	std::signal(SIGSEGV, SIG_DFL);
	std::signal(SIGABRT, SIG_DFL);
	std::signal(SIGINT, SIG_DFL);
	std::signal(SIGTERM, SIG_DFL);
	throw SignalSegFaultException(signal);
}

void initSearchPath(QApplication * app)
{
	const QString configDirectory    = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
	const QString homeDirectory      = QDir::home().absoluteFilePath(configDirectory);
	const QString datasDirectory     = QDir(homeDirectory).absoluteFilePath("datas");
	const QString scriptDirectory    = QDir(homeDirectory).absoluteFilePath("scripts");
	const QString pluginsDirectory   = QDir(homeDirectory).absoluteFilePath("plugins");
	const QString templatesDirectory = QDir(homeDirectory).absoluteFilePath("templates");

	QDir::home().mkpath(datasDirectory);
	QDir::home().mkpath(scriptDirectory);
	QDir::home().mkpath(pluginsDirectory);
	QDir::home().mkpath(templatesDirectory);

	// .. for datas ...
	QDir::addSearchPath("datas", datasDirectory);
	QDir::addSearchPath("datas", QDir(QApplication::applicationDirPath()).absoluteFilePath("../datas"));
#ifndef Q_WS_WIN
	QDir::addSearchPath("datas", QDir(QApplication::applicationDirPath()).absoluteFilePath("../share/xinx/datas"));
#endif /* Q_WS_WIN */

	// ... for scripts ...
	QDir::addSearchPath("scripts", scriptDirectory);
	QDir::addSearchPath("scripts", QDir(QApplication::applicationDirPath()).absoluteFilePath("../scripts"));
#ifndef Q_WS_WIN
	QDir::addSearchPath("scripts", QDir(QApplication::applicationDirPath()).absoluteFilePath("../share/xinx/scripts"));
#endif /* Q_WS_WIN */

	// ... for plugins ...
	QDir::addSearchPath("plugins", pluginsDirectory);
	QDir::addSearchPath("plugins", QDir(QApplication::applicationDirPath()).absoluteFilePath("../plugins"));
#ifndef Q_WS_WIN
	QDir::addSearchPath("plugins", QDir(QApplication::applicationDirPath()).absoluteFilePath("../share/xinx/plugins"));
#endif /* Q_WS_WIN */
	foreach(const QString & searchPath, QDir::searchPaths("plugins"))
	{
		app->addLibraryPath(searchPath);
	}

	// ... for tempalte ...
	QDir::addSearchPath("templates", templatesDirectory);
	QDir::addSearchPath("templates", QDir(QApplication::applicationDirPath()).absoluteFilePath("../templates"));
#ifndef Q_WS_WIN
	QDir::addSearchPath("templates", QDir(QApplication::applicationDirPath()).absoluteFilePath("../share/xinx/templates"));
#endif /* Q_WS_WIN */
}

void processSnipetArguments(const QStringList & args)
{
	for (int i = 0 ; i < args.count() ; i++)
	{
		if (i + 1 >= args.count()) break;

		const QString & arg = args.at(i);
		const QString & filename = args.at(i + 1);


		if (arg == "--init")
		{
			QFileInfo fileInfo("datas:snipets.db");
			if (fileInfo.exists()) return; /* In init mode the file must not exist */
			SnipetList list;
			list.loadFromFile(filename);
			SnipetManager::self()->importSnipetList(list, false);
		}
		else if (arg == "--import")
		{
			SnipetList list;
			list.loadFromFile(filename);
			SnipetManager::self()->importSnipetList(list);
		}
		else if (arg == "--export")
		{
			QList<int> ids = SnipetManager::self()->snipets();

			SnipetList list;
			if (SnipetManager::self()->exportSnipetList(ids, &list))
				list.saveToFile(filename);
		}
	}
}

int main(int argc, char *argv[])
{
	Q_INIT_RESOURCE(application);
	qsrand(time(NULL));

	std::signal(SIGSEGV, backup_appli_signal);
	std::signal(SIGABRT, backup_appli_signal);
	std::signal(SIGINT, backup_appli_signal);
	std::signal(SIGTERM, backup_appli_signal);

#if defined(Q_WS_WIN)
	if (QSysInfo::WindowsVersion >= QSysInfo::WV_VISTA && QSysInfo::WindowsVersion < QSysInfo::WV_NT_based)
		QApplication::setStyle(QStyleFactory::create("Explorer"));
#endif // Q_WS_WIN

	UniqueApplication app(argc, argv);
	try
	{
		QStringList args = app.arguments();

		app.setOrganizationName("Shadoware");
		app.setOrganizationDomain("Shadoware.Org");
		app.setApplicationName("XINX");

		if (args.contains("--snipet"))
		{
			initSearchPath(&app);
			processSnipetArguments(args);
			return 0;
		}

		// .. If application is not started
		if (! app.isRunning())
		{
			// Create the splash screen
			QPixmap pixmap(":/images/splash.png");
			QSplashScreen splash(pixmap);
			splash.setMask(pixmap.mask());
			splash.show();
			app.processEvents();

			/* Load the exception manager */
			splash.showMessage(QApplication::translate("SplashScreen", "Install exception handler ..."));
			app.processEvents();
			ExceptionManager::installExceptionHandler();

			// Initialize search path for datas ...
			splash.showMessage(QApplication::translate("SplashScreen", "Initialize search path ..."));
			app.processEvents();
			initSearchPath(&app);

			/*
			 * To have the lang and style loaded earlier in the process, we load configuration of XINX
			 * XINX Config doens't have call to another Big instance (has XinxPluginsLoader)
			 */
			splash.showMessage(QApplication::translate("SplashScreen", "Load configuration ..."));
			app.processEvents();

			XINXConfig::self()->load();
			if (! XINXConfig::self()->config().style.isEmpty())
			{
				QApplication::setStyle(XINXConfig::self()->config().style);
			}

			// Now we know which lang use, we can load translations. We are not lost in translation ...
			splash.showMessage(QApplication::translate("SplashScreen", "Load translations ..."));
			app.processEvents();

			// ... load qt translation ...
			QTranslator translator_xinx, translator_qt, translator_libxinx, tranlator_components;
			translator_qt.load(QString(":/translations/qt_%1").arg(XINXConfig::self()->config().language));
			app.installTranslator(&translator_qt);
			// ... load xinx translation ...
			translator_xinx.load(QString(":/translations/xinx_%1").arg(XINXConfig::self()->config().language));
			app.installTranslator(&translator_xinx);
			// ... load xinx library translation ...
			translator_libxinx.load(QString(":/translations/libxinx_%1").arg(XINXConfig::self()->config().language));
			app.installTranslator(&translator_libxinx);
			// ... load components translations
			tranlator_components.load(QString(":/translations/xinxcomponents_%1").arg(XINXConfig::self()->config().language));
			app.installTranslator(&tranlator_components);

			/* Load available marks (for QCodeEdit use) */
			splash.showMessage(QApplication::translate("SplashScreen", "Load available marks ..."));
			app.processEvents();
			QLineMarksInfoCenter::instance()->loadMarkTypes(":/qcodeedit/marks.qxm");

			// Loads plugins
			splash.showMessage(QApplication::translate("SplashScreen", "Load plugins ..."));
			app.processEvents();
			XinxPluginsLoader::self()->loadPlugins();

			splash.showMessage(QApplication::translate("SplashScreen", "Load main window ..."));
			app.processEvents();
			app.attachMainWindow(mainWin = new MainformImpl());

			if ((args.count() == 1) && (XINXConfig::self()->config().project.openTheLastProjectAtStart) && (! XINXConfig::self()->config().project.lastOpenedProject.isEmpty()))
			{
				splash.showMessage(QApplication::translate("SplashScreen", "Load last opened project ..."));
				app.processEvents();
				mainWin->openProject(XINXConfig::self()->config().project.lastOpenedProject);
			}

			if (args.count() > 1)
			{
				splash.showMessage(QApplication::translate("SplashScreen", "Load arguments ..."));
				app.processEvents();

				QStringList::iterator it = args.begin();
				it++;
				while (it != args.end())
				{
					if (QFile(*it).exists()) mainWin->openFile(*it);
					it++;
				}
			}

			mainWin->show();
			splash.finish(mainWin);

			if ((XINXConfig::self()->config().version != VERSION_STRING) || (args.contains("--newversion")))
			{
				NewVersionWizardImpl dlg(mainWin);
				dlg.loadFromConfig();
				dlg.exec();
				dlg.saveToConfig();

				XINXConfig::self()->config().version = VERSION_STRING;
			}

			if ((args.count() == 1) && !((XINXConfig::self()->config().project.openTheLastProjectAtStart) && (! XINXConfig::self()->config().project.lastOpenedProject.isEmpty())))
			{
				mainWin->openWelcomDialog();
			}

			int result = app.exec();

			// Free memory
			delete mainWin;
			delete XINXStaticDeleter::self();

			return result;
		}
		else
		{
			// Send Parameter to open
			if (args.count() > 0)
			{
				QStringList::iterator it = args.begin();
				it++;
				while (it != args.end())
				{
					if (QFile(*it).exists()) app.sendMessage(*it);
					it++;
				}
			}
			return 255;
		}
	}
	catch (XinxException e)
	{
		qFatal("In main : %s", qPrintable(e.getMessage()));
		return false;
	}
	catch (...)
	{
		qFatal("In main : Generic Exception");
		return 1;
	}
}

