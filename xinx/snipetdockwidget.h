/***************************************************************************
 *   Copyright (C) 2008 by Ulrich Van Den Hekke                            *
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

#ifndef __SNIPETDOCKWIDGET_H__
#define __SNIPETDOCKWIDGET_H__

// Xinx header
#include "snipet.h"
#include "editor.h"

// Qt header
#include <QDockWidget>
#include <QString>
#include <QToolBox>
#include <QHash>

/*!
 * This dock represent a Snipet Dock Widget to search and place snipet 
 * in XINX.
 */
class SnipetDockWidget : public QDockWidget {
	Q_OBJECT
public:
	SnipetDockWidget( const QString & title, QWidget * parent = 0, Qt::WindowFlags flags = 0 );
	SnipetDockWidget( QWidget * parent = 0, Qt::WindowFlags flags = 0 );
	virtual ~SnipetDockWidget();

public slots:
	void setEditor( Editor * ed );
private slots:
	void updateSnipets();
	void callSnipet();
private:
	QWidget * createWidget();
	QWidget * createSnipetWidget( const Snipet & s );
	void setupUi();
	
	QToolBox * m_snipetsToolBox;
	QHash<QString,QWidget*> m_pages;
	Editor * m_editor;
};

#endif /*__SNIPETDOCKWIDGET_H__*/