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
#include <core/exceptionmanager.h>
#include <plugins/xinxpluginsloader.h>
#include <application/newversionwizardimpl.h>
#include <editors/editormanager.h>
#include <session/sessionmanager.h>
#include <project/xinxprojectmanager.h>
#include <core/version.h>
#include <translations/translationmanager.h>
#include <core/xinxcore.h>

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
#include <QtPlugin>

// C++ header
#include <iostream>
#include <time.h>

// Import plugins
Q_IMPORT_PLUGIN(coreplugin);

MainformImpl * mainWin = NULL;

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
			const QString filename = QDir::fromNativeSeparators(*it);
			if (QFile(filename).exists())
				EditorManager::self()->openFile(filename);
			it++;
		}
	}
}

class WorkaroundClassToFixBugInMinGW {};

int main(int argc, char *argv[])
{
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
			bool recovering = false;

			// Splash screen must be delete when main is shown
			{
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
				startTranslation();

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

				// Load main window
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
			}

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
					const QString filename = QDir::fromNativeSeparators(*it);
					if (QFile(filename).exists()) app.sendMessage(filename);
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

