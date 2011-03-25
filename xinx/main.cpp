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
#include <core/uniqueapplication.h>
#include <application/mainformimpl.h>
#include <snipets/snipetmanager.h>
#include <snipets/snipetlist.h>
#include <core/xinxconfig.h>
#include <core/exceptions.h>
#include <plugins/xinxpluginsloader.h>
#include <application/newversionwizardimpl.h>
#include <editors/editormanager.h>
#include <session/sessionmanager.h>
#include <project/xinxprojectmanager.h>
#include <core/version.h>
#include <translations/translationmanager.h>

// QCodeEdit header
#include <qlinemarksinfocenter.h>

// Qt header
#include <QApplication>
#include <QSplashScreen>
#include <QStringList>
#include <QFile>
#include <QMessageBox>
#include <QLocale>
#include <QDir>
#include <QThread>
#include <QBitmap>
#include <QStyleFactory>
#include <QDesktopServices>
#include <QtPlugin>

// C++ header
#include <iostream>
#include <time.h>

// Import plugins
Q_IMPORT_PLUGIN(coreplugin);

MainformImpl * mainWin = NULL;

static void initSearchPath(QApplication * app)
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

static void processSnipetArguments(const QStringList & args)
{
	for (int i = 0 ; i < args.count() ; i++)
	{
		if (i + 1 >= args.count()) break;

		const QString & arg = args.at(i);
		const QString & filename = args.at(i + 1);


		if (arg == "--import")
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

void processFilesArguments(const QStringList & args)
{
	if (args.count() > 1)
	{
		QStringList::const_iterator it = args.constBegin();
		it++;
		while (it != args.constEnd())
		{
			if (QFile(*it).exists())
				EditorManager::self()->openFile(*it);
			it++;
		}
	}
}

class WorkaroundClassToFixBugInMinGW {};

int main(int argc, char *argv[])
{
	Q_INIT_RESOURCE(application);
	qsrand(time(NULL));

	// Workaround to fix bug with MinGW
	try
	{
	  throw WorkaroundClassToFixBugInMinGW();
	}
	catch(WorkaroundClassToFixBugInMinGW)
	{
	}

	ExceptionManager::installSignalHandler ();

#if defined(Q_WS_WIN)
	if (QSysInfo::WindowsVersion >= QSysInfo::WV_VISTA && QSysInfo::WindowsVersion < QSysInfo::WV_NT_based)
		QApplication::setStyle(QStyleFactory::create("Explorer"));
#endif // Q_WS_WIN

	UniqueApplication app(argc, argv);
	try
	{
		QStringList args = app.arguments();

		app.setOrganizationName("Shadoware.Org");
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

			// Load the exception manager
			splash.showMessage(QApplication::translate("SplashScreen", "Install exception handler ..."));
			app.processEvents();
			ExceptionManager::installExceptionHandler();

			// Initialize search path for datas ...
			splash.showMessage(QApplication::translate("SplashScreen", "Initialize search path ..."));
			app.processEvents();
			initSearchPath(&app);

			// To have the lang and style loaded earlier in the process, we load configuration of XINX
			// XINX Config doens't have call to another Big instance (has XinxPluginsLoader)
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
			TranslationManager::self()->setLanguage(XINXConfig::self()->config().language);
			TranslationManager::self()->apply();

			bool recovering = false;
			if (XinxSession::Session::sessionsNames().contains(RECOVER_SESSION))
			{
				QMessageBox::StandardButton result = QMessageBox::question(&splash, QApplication::translate("SplashScreen", "Recover"), QApplication::translate("SplashScreen", "There's an existing recover session. Do you wan try to recover ?"), QMessageBox::Yes | QMessageBox::Discard | QMessageBox::Reset);
				switch (result)
				{
					case QMessageBox::Yes:
						recovering = true;
						break;
					case QMessageBox::Discard:
						XinxSession::SessionManager::self()->deleteRecoverSession();
						break;
					case QMessageBox::Reset:
						XINXConfig::self()->setDefault();
						break;
					default:
						qDebug() << "Not managed";
				}
			}

			// Load available marks (for QCodeEdit use)
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

			if (recovering)
			{
				splash.showMessage(QApplication::translate("SplashScreen", "Recovering ..."));
				app.processEvents();
				XinxSession::SessionManager::self()->restoreSession(RECOVER_SESSION);
				XinxSession::SessionManager::self()->deleteRecoverSession();

				processFilesArguments(args);
			}
			// No matter how many parameters are passed we open the last session.
			else if ((XINXConfig::self()->config().project.openTheLastSessionAtStart) && (! XINXConfig::self()->config().project.lastOpenedSession.isEmpty()))
			{
				splash.showMessage(QApplication::translate("SplashScreen", "Load last session ..."));
				app.processEvents();
				XinxSession::SessionManager::self()->restoreSession(XINXConfig::self()->config().project.lastOpenedSession);

				processFilesArguments(args);
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

			if ((!recovering) && !((XINXConfig::self()->config().project.openTheLastSessionAtStart) && (! XINXConfig::self()->config().project.lastOpenedSession.isEmpty())))
			{
				XinxProject::Manager::self()->openWelcomDialog();
				processFilesArguments(args);
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
		return 2;
	}
	catch (...)
	{
		qFatal("In main : Generic Exception");
		return 1;
	}

	return 0;
}

