/***************************************************************************
 *   Copyright (C) 2006 by Ulrich Van Den Hekke   *
 *   ulrich.vdh@free.fr   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

// Xinx header
#include "uniqueapplication.h"
#include "mainformimpl.h"
#include "xinxconfig.h"
#include "snipetlist.h"
#include "exceptions.h"
#include "xinxpluginsloader.h"

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

// C++ header
#include <csignal>
#include <iostream>

// Import plugins
Q_IMPORT_PLUGIN(webplugin)

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

void xinxMessageHandler( QtMsgType type, const char * msg ) {
	switch( type ) {
	case QtDebugMsg:
		fprintf( stderr, "Debug: %s\n", msg );
		break;
	case QtWarningMsg:
		fprintf( stderr, "Warning: %s\n", msg );
		break;
	case QtCriticalMsg:
	case QtFatalMsg:
		qobject_cast<UniqueApplication*>( qApp )->notifyError( QString( "Fatal : %1" ).arg( msg ), stackTrace[(unsigned long)QThread::currentThreadId()] );
	}
}

int main(int argc, char *argv[]) {
	Q_INIT_RESOURCE(application);
	
	qInstallMsgHandler(xinxMessageHandler);
	std::signal(SIGSEGV, backup_appli_signal);
	std::signal(SIGABRT, backup_appli_signal);
	std::signal(SIGINT, backup_appli_signal);
	std::signal(SIGTERM, backup_appli_signal);
	    
	UniqueApplication app(argc, argv);
	try {
		app.setOrganizationName( "Shadoware" );
		app.setOrganizationDomain( "Shadoware.Org" );
		app.setApplicationName( "XINX" );
		
		if( app.isUnique() ) {
			QPixmap pixmap(":/images/splash.png");
			QSplashScreen splash(pixmap);
			splash.setMask(pixmap.mask());
			splash.show();
	  		app.processEvents();
	
	  		/* Must load to have traductions in plugins */
	  		splash.showMessage( QApplication::translate("SplashScreen", "Load configuration ...") );
			app.processEvents();
			XINXConfig::self()->load();

	  		splash.showMessage( QApplication::translate("SplashScreen", "Load translations ...") );
			app.processEvents();
			QTranslator translator_xinx, translator_qt;
			translator_qt.load( QString(":/translations/qt_%1").arg( XINXConfig::self()->config().language ) );
			app.installTranslator(&translator_qt);
			translator_xinx.load( QString(":/translations/xinx_%1").arg( XINXConfig::self()->config().language ) );
			app.installTranslator(&translator_xinx);
	
			splash.showMessage( QApplication::translate("SplashScreen", "Load plugins ...") );
			app.processEvents();
			XinxPluginsLoader::self()->loadPlugins();

			/* Reload to have options in plugins */
	  		splash.showMessage( QApplication::translate("SplashScreen", "Load configuration ...") );
			app.processEvents();
			XINXConfig::self()->load();

	  		splash.showMessage( QApplication::translate("SplashScreen", "Create completion and snipet object ...") );
			app.processEvents();
			try {
				SnipetList::self()->loadFromFile( "datas:template.xnx" );
			} catch( SnipetListException ) {
				splash.showMessage( QApplication::translate("SplashScreen", "Can't load snipet file.") );
				app.processEvents();
			}
			
	  		splash.showMessage( QApplication::translate("SplashScreen", "Load main window ...") );
	  		app.processEvents();
			mainWin = new MainformImpl();
	  
	  		splash.showMessage( QApplication::translate("SplashScreen", "Load arguments ...") );
	  		app.processEvents();
			QStringList args = app.arguments();
			if(args.count() > 0) {
				QStringList::iterator it = args.begin();
				it++;
				while (it != args.end()) {
					if(QFile(*it).exists()) mainWin->openFile( *it );
					it++;
				}
			}
	
			splash.finish(mainWin);
			
			mainWin->show();
	
			return app.exec();
	 	} else {
			// Send Parameter to open
			QStringList args = app.arguments();
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
		app.notifyError( "In main : " + e.getMessage(), e.getStack() );
		return false;
	} catch( ... ) {
		app.notifyError( "In main : Generic Exception", stackTrace[(unsigned long)QThread::currentThreadId()] );
		return 1;
	}
}
