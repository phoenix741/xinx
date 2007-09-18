/***************************************************************************
 *   Copyright (C) 2007 by Ulrich Van Den Hekke                            *
 *   ulrich.vdh@free.fr                                                    *
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

#ifndef __XMLPRESENTATIONDOCKWIDGET_H__
#define __XMLPRESENTATIONDOCKWIDGET_H__

#include <QDockWidget>
#include <QString>

class PrivateXmlPresentationDockWidget;

/*!
 * This dock contains a list of XMLPresentationFile founded in the log directory.
 * The log directory can be modified, and the presentation file can be choosen.
 * Some feature as drag & drop must be added in future.
 */
class XmlPresentationDockWidget : public QDockWidget {
	Q_OBJECT
public:
	XmlPresentationDockWidget( const QString & title, QWidget * parent = 0, Qt::WindowFlags flags = 0 );
	XmlPresentationDockWidget( QWidget * parent = 0, Qt::WindowFlags flags = 0 );
	virtual ~XmlPresentationDockWidget();
private:
	PrivateXmlPresentationDockWidget * d;
	friend class PrivateXmlPresentationDockWidget;
};


#endif // __XMLPRESENTATIONDOCKWIDGET_H__
