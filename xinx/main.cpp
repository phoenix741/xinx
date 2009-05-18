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
#include "snipetlist.h"
#include <core/xinxconfig.h>
#include <core/exceptions.h>
#include <plugins/xinxpluginsloader.h>

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

// C++ header
#include <csignal>
#include <iostream>

// Import plugins
Q_IMPORT_PLUGIN(xmlpresplugin);
Q_IMPORT_PLUGIN(webplugin);

MainformImpl * mainWin = NULL;

class SignalSegFaultException : public XinxException {
public:
	SignalSegFaultException( int signal ) : XinxException( QString("Signal emited : %1").arg( signal ) ) {

	}
};

void backup_appli_signal( int signal ) {
	std::signal(SIGSEGV, SIG_DFL);
	std::signal(SIGABRT, SIG_DFL);
	std::signal(SIGINT, SIG_DFL);
	std::signal(SIGTERM, SIG_DFL);
	throw SignalSegFaultException( signal );
}

int main(int argc, char *argv[]) {
	Q_INIT_RESOURCE(application);

	std::signal(SIGSEGV, backup_appli_signal);
	std::signal(SIGABRT, backup_appli_signal);
	std::signal(SIGINT, backup_appli_signal);
	std::signal(SIGTERM, backup_appli_signal);

#if defined(Q_WS_WIN)
	if( QSysInfo::WindowsVersion >= QSysInfo::WV_VISTA && QSysInfo::WindowsVersion < QSysInfo::WV_NT_based )
		QApplication::setStyle(QStyleFactory::create("Explorer"));
#endif // Q_WS_WIN

	UniqueApplication app(argc, argv);
	try {
		QStringList args = app.arguments();

		app.setOrganizationName( "Shadoware" );
		app.setOrganizationDomain( "Shadoware.Org" );
		app.setApplicationName( "XINX" );

		// .. If application is not started
		if( app.isUnique() ) {
			// Create the splash screen
			QPixmap pixmap(":/images/splash.png");
			QSplashScreen splash(pixmap);
			splash.setMask(pixmap.mask());
			splash.show();
	  		app.processEvents();

	  		/* Load the exception manager */
	  		splash.showMessage( QApplication::translate("SplashScreen", "Install exception handler ...") );
			app.processEvents();
	  		ExceptionManager::installExceptionHandler();

			// Initialize search path for datas ...
	  		splash.showMessage( QApplication::translate("SplashScreen", "Initialize search path ...") );
			app.processEvents();
			QDir::setSearchPaths( "datas", QStringList() ); // Modify by XinxConfig

			// ... for scripts ...
			QDir::addSearchPath( "scripts", QDir( QApplication::applicationDirPath() ).absoluteFilePath( "../scripts" ) );
			QDir::addSearchPath( "scripts", QDir( QApplication::applicationDirPath() ).absoluteFilePath( "../share/xinx/scripts" ) );

			// ... for plugins ...
			QDir::addSearchPath( "plugins", QDir( QApplication::applicationDirPath() ).absoluteFilePath( "../plugins" ) );
			QDir::addSearchPath( "plugins", QDir( QApplication::applicationDirPath() ).absoluteFilePath( "../share/xinx/plugins" ) );
			app.addLibraryPath( "plugins:" );

			/*
			 * To have the lang and style loaded earlier in the process, we load configuration of XINX
			 * XINX Config doens't have call to another Big instance (has XinxPluginsLoader)
			 */
	  		splash.showMessage( QApplication::translate("SplashScreen", "Load configuration ...") );
			app.processEvents();

			XINXConfig::self()->load();
			if( ! XINXConfig::self()->config().style.isEmpty() ) {
				QApplication::setStyle( XINXConfig::self()->config().style );
			}

			// Now we know which lang use, we can load translations. We are not lost in translation ...
	  		splash.showMessage( QApplication::translate("SplashScreen", "Load translations ...") );
			app.processEvents();

			// ... load qt translation ...
			QTranslator translator_xinx, translator_qt, translator_libxinx, tranlator_components;
			translator_qt.load( QString(":/translations/qt_%1").arg( XINXConfig::self()->config().language ) );
			app.installTranslator(&translator_qt);
			// ... load xinx translation ...
			translator_xinx.load( QString(":/translations/xinx_%1").arg( XINXConfig::self()->config().language ) );
			app.installTranslator(&translator_xinx);
			// ... load xinx library translation ...
			translator_libxinx.load( QString(":/translations/libxinx_%1").arg( XINXConfig::self()->config().language ) );
			app.installTranslator(&translator_libxinx);
			// ... load components translations
			tranlator_components.load( QString(":/translations/xinxcomponents_%1").arg( XINXConfig::self()->config().language ) );
			app.installTranslator(&tranlator_components);

			/* Load available marks (for QCodeEdit use) */
	  		splash.showMessage( QApplication::translate("SplashScreen", "Load available marks ...") );
			app.processEvents();
			QLineMarksInfoCenter::instance()->loadMarkTypes( ":/qcodeedit/marks.qxm" );

			// Loads plugins
			splash.showMessage( QApplication::translate("SplashScreen", "Load plugins ...") );
			app.processEvents();
			XinxPluginsLoader::self()->loadPlugins();

	  		splash.showMessage( QApplication::translate("SplashScreen", "Load snipets ...") );
			app.processEvents();
			try {
				SnipetListManager::self()->loadFromSnipetFile();
			} catch( SnipetListException ) {
				splash.showMessage( QApplication::translate("SplashScreen", "Can't load snipet file.") );
				app.processEvents();
			}

	  		splash.showMessage( QApplication::translate("SplashScreen", "Load main window ...") );
	  		app.processEvents();
			mainWin = new MainformImpl();

			if( ( args.count() == 1 ) && ( XINXConfig::self()->config().project.openTheLastProjectAtStart ) && (! XINXConfig::self()->config().project.lastOpenedProject.isEmpty()) ) {
				splash.showMessage( QApplication::translate("SplashScreen", "Load last opened project ...") );
				app.processEvents();
				mainWin->openProject( XINXConfig::self()->config().project.lastOpenedProject );
			}

			if( args.count() > 1 ) {
				splash.showMessage( QApplication::translate("SplashScreen", "Load arguments ...") );
				app.processEvents();

				QStringList::iterator it = args.begin();
				it++;
				while (it != args.end()) {
					if(QFile(*it).exists()) mainWin->openFile( *it );
					it++;
				}
			}

			mainWin->show();
			splash.finish(mainWin);

			return app.exec();
	 	} else {
			// Send Parameter to open
			if(args.count() > 0) {
				QStringList::iterator it = args.begin();
				it++;
				while (it != args.end()) {
					if(QFile(*it).exists()) app.callOpenFile(*it);
					it++;
				}
			}
	 		return 255;
		}
	} catch( XinxException e ) {
		qFatal( "In main : %s", qPrintable( e.getMessage() ) );
		return false;
	} catch( ... ) {
		qFatal( "In main : Generic Exception" );
		return 1;
	}
}

