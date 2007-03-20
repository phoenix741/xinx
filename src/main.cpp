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
#include "uniqueapplication.h"
#include "xmlvisualstudio.h"
#include "xinxconfig.h"

int main(int argc, char *argv[]) {
	Q_INIT_RESOURCE(application);

	UniqueApplication app(argc, argv);

	xinxConfig = new XINXConfig();
	xinxConfig->load();
		
	QTranslator translator_xinx, translator_qt;
	translator_qt.load(QString("qt_") + xinxConfig->lang(), app.applicationDirPath());
	app.installTranslator(&translator_qt);
	translator_xinx.load(QString("xinx_") + xinxConfig->lang(), app.applicationDirPath());
	app.installTranslator(&translator_xinx);
 
	if( app.isUnique() ) {
		QPixmap pixmap(":/splash.png");
		QSplashScreen splash(pixmap);
		splash.show();
		app.processEvents();

		XMLVisualStudio mainWin;
		mainWin.show();
  
		QStringList args = app.arguments();
		if(args.count() > 0) {
			QStringList::iterator it = args.begin();
			it++;
			while (it != args.end()) {
				if(QFile(*it).exists()) mainWin.open(*it);
				it++;
			}
		}

		splash.finish(&mainWin);

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

