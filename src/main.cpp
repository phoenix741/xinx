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
#include "xmlvisualstudio.h"
#include "mainformimpl.h"
#include "xinxconfig.h"
#include "globals.h"
#include "objectview.h"
#include "editorcompletion.h"
#include "snipetlist.h"

// Qt header
#include <QApplication>
#include <QSplashScreen>
#include <QStringList>
#include <QFile>
#include <QMessageBox>
#include <QLocale>
#include <QTranslator>
#include <QDir>

// C++ header
#include <csignal>
#include <iostream>

MainformImpl * mainWin = NULL;

class SignalSegFaultException {};

void backup_appli_signal( int ) {
	std::signal(SIGSEGV, SIG_DFL);
	std::signal(SIGABRT, SIG_DFL);
	std::signal(SIGINT, SIG_DFL);
	std::signal(SIGTERM, SIG_DFL);
	throw SignalSegFaultException();
}

int main(int argc, char *argv[]) {
	Q_INIT_RESOURCE(application);

	std::signal(SIGSEGV, backup_appli_signal);
	std::signal(SIGABRT, backup_appli_signal);
	std::signal(SIGINT, backup_appli_signal);
	std::signal(SIGTERM, backup_appli_signal);
	
	UniqueApplication app(argc, argv);
	try {
		global.m_xinxConfig = new XINXConfig();
		global.m_xinxConfig->load();

		QTranslator translator_xinx, translator_qt;
		translator_qt.load( QString("../translations/qt_") + global.m_xinxConfig->lang(), app.applicationDirPath());
		app.installTranslator(&translator_qt);
		translator_xinx.load(QString("../translations/xinx_") + global.m_xinxConfig->lang(), app.applicationDirPath());
		app.installTranslator(&translator_xinx);
	
		if( app.isUnique() ) {
			QPixmap pixmap(":/images/splash.png");
			QSplashScreen splash(pixmap);
			splash.show();
	  		app.processEvents();
	
	  		splash.showMessage( splash.tr("Create completion and snipet object ...") );
			global.m_completionContents = new Completion();
			global.m_snipetList = new SnipetList();
			try {
				global.m_completionContents->setPath( QDir( global.m_xinxConfig->completionFilesPath() ).filePath( "completion.xnx" ) );
			} catch( ENotCompletionFile ) {
				splash.showMessage( splash.tr("Can't load completion file.") );
				sleep( 1 );
			}
			try {
				global.m_snipetList->loadFromFile( QDir( global.m_xinxConfig->completionFilesPath() ).filePath( "template.xnx" ) );
			} catch( SnipetListException ) {
				splash.showMessage( splash.tr("Can't load snipet file.") );
				sleep( 1 );
			}
	
	  		splash.showMessage( splash.tr("Load objects file ...") );
	  		app.processEvents();
			global.m_javaObjects = new ObjectsView();
			global.m_javaObjects->setPath( global.m_xinxConfig->objectDescriptionPath() );
			global.m_javaObjects->loadFiles();
	
	  		splash.showMessage( splash.tr("Load main window ...") );
	  		app.processEvents();
	  		//(new MainformImpl() )->show();
			mainWin = new MainformImpl();
			mainWin->show();
	  
	  		splash.showMessage( splash.tr("Load arguments ...") );
	  		app.processEvents();
			QStringList args = app.arguments();
			if(args.count() > 0) {
				QStringList::iterator it = args.begin();
				it++;
				while (it != args.end()) {
					if(QFile(*it).exists()) mainWin->open(*it);
					it++;
				}
			}
	
			splash.finish(mainWin);
	
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
	} catch( ... ) {
		app.notifyError();
	}
}

