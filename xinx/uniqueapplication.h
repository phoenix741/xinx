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

#ifndef _UNIQUEAPPLICATION_H_
#define _UNIQUEAPPLICATION_H_

#include <QApplication>

class OrgShadowareXinxInterface;
class MainformImpl;

class UniqueApplication : public QApplication {
	Q_OBJECT
public:
	UniqueApplication ( int & argc, char ** argv );
	UniqueApplication ( int & argc, char ** argv, bool GUIenabled );
	UniqueApplication ( int & argc, char ** argv, Type type );
	virtual ~UniqueApplication ();

	virtual bool notify ( QObject * receiver, QEvent * event );

	bool isUnique();
	void callOpenFile( const QString & fileName );

	void attachMainWindow( MainformImpl * mainform );
public slots:
	void slotErrorTriggered();
private:
	void start();

	bool m_isUnique;
	MainformImpl * m_mainform;
	OrgShadowareXinxInterface * m_interface;
};

#endif
