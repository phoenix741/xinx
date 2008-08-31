/***************************************************************************
 *   Copyright (C) 2008 by Ulrich Van Den Hekke   *
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

// Qt header
#include <QApplication>
#include <QLocale>
#include <QTranslator>

// Xinx header
#include "projectwizard.h"

int main( int argc, char *argv[] ) {
	Q_INIT_RESOURCE( xinxprojectwizard );

	QApplication app( argc, argv );

	QString langue = QLocale::system().name();
	QString filename;

	QStringListIterator iterator( app.arguments() );
	iterator.next(); // Zap le nom du programme
	while( iterator.hasNext() ) {
		QString arg  = iterator.next();
		if( arg.startsWith( "-lang" ) ) {
			if( arg.contains("=") ) {
				langue = arg.split( "=" ).at( 1 );
			} else {
				langue = iterator.next();
			}
		} else {
			filename = arg;
		}
	}

	QTranslator translator_xinx, translator_qt;

	translator_qt.load( QString(":/translations/qt_%1").arg( langue ) );
	app.installTranslator(&translator_qt);

	translator_xinx.load( QString(":/translations/xinxprojectwizard_%1").arg( langue ) );
	app.installTranslator(&translator_xinx);

	ProjectWizard wizard( filename );
	wizard.show();

	return app.exec();
}
