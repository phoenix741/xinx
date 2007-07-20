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

#include <QApplication>
#include <QSplashScreen>
#include <QStringList>
#include <QFile>
#include <QMessageBox>
#include <QLocale>
#include <QTranslator>
#include <QPlastiqueStyle>
#include "uniqueapplication.h"
#include "xmlvisualstudio.h"
#include "xinxconfig.h"
#include "globals.h"
#include "objectview.h"
#include <csignal>
#include <iostream>

XMLVisualStudio * mainWin = NULL;

void backup_appli() {
	std::signal(SIGSEGV, SIG_DFL);
	std::signal(SIGABRT, SIG_DFL);
	std::signal(SIGINT, SIG_DFL);
	std::signal(SIGTERM, SIG_DFL);
//	std::signal(SIGBREAK, SIG_DFL);
	QMessageBox::critical( NULL, "Error", "Shit ! How can it be happen ? What's the hell Ulrich !\nOk. I try to repair that, and you, send me a detailled report (Where ? When ? Who ? How ? Why ?)." );
	
	if( mainWin )
		mainWin->closeProject( false, true );
	
	exit(1);
}

void backup_appli_signal( int ) {
	backup_appli();
}

int main(int argc, char *argv[]) {
	Q_INIT_RESOURCE(application);

	std::signal(SIGSEGV, backup_appli_signal);
	std::signal(SIGABRT, backup_appli_signal);
	std::signal(SIGINT, backup_appli_signal);
	std::signal(SIGTERM, backup_appli_signal);
//	std::signal(SIGBREAK, backup_appli_signal);
	
	UniqueApplication app(argc, argv);

	global.m_xinxConfig = new XINXConfig();
	global.m_xinxConfig->load();

	QTranslator translator_xinx, translator_qt;
	translator_qt.load(QString("qt_") + global.m_xinxConfig->lang(), app.applicationDirPath());
	app.installTranslator(&translator_qt);
	translator_xinx.load(QString("xinx_") + global.m_xinxConfig->lang(), app.applicationDirPath());
	app.installTranslator(&translator_xinx);
	
/*#ifdef QT_DEBUG
	app.setStyle( new QPlastiqueStyle() );
	QFile qssFile( ":/qss/coffee.qss" );
	qssFile.open( QFile::ReadOnly );
	app.setStyleSheet( qssFile.readAll() );
	qssFile.close();
#endif */

	if( app.isUnique() ) {
		QPixmap pixmap(":/images/splash.png");
		QSplashScreen splash(pixmap);
		splash.show();
  		app.processEvents();

  		splash.showMessage( splash.tr("Load objects file ...") );
  		app.processEvents();
		global.m_javaObjects = new ObjectsView();
		global.m_javaObjects->setPath( global.m_xinxConfig->objectDescriptionPath() );
		global.m_javaObjects->loadFiles();

  		splash.showMessage( splash.tr("Load main window ...") );
  		app.processEvents();
		mainWin = new XMLVisualStudio();
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
				if(QFile(*it).exists()) app.sendSignalOpen(*it);
				it++;
			}
		}
 		return 255;
	}
}

