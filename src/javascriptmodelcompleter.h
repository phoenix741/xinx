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
 
#ifndef __JAVASCRIPTMODELCOMPETER_H__
#define __JAVASCRIPTMODELCOMPETER_H__

#include <QAbstractListModel>

class JavaScriptElement;
class JavaScriptParser;

/* JavascriptModelCompleter */

class JavascriptModelCompleter : public QAbstractListModel {
	Q_OBJECT
public:
	JavascriptModelCompleter( JavaScriptParser * parser, QObject *parent = 0 );
	virtual ~JavascriptModelCompleter();
	
	QVariant data(const QModelIndex &index, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
public slots:
	void setFilter( const QString functionName = QString() );
private:
	void refreshList( JavaScriptElement * element );
	QList<JavaScriptElement*> m_objList;
	
	JavaScriptParser * m_parser;
	QString m_functionFiltre;
};

#endif // __JAVASCRIPTMODELCOMPETER_H__
