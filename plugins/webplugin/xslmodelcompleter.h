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

#ifndef __XSLMODELCOMPLETER_H__
#define __XSLMODELCOMPLETER_H__

// Xinx header
#include "filecontentstructure.h"

// Qt header
#include <QAbstractListModel>
#include <QStringList>

class XSLValueCompletionModel : public QAbstractListModel {
	Q_OBJECT
public:
	XSLValueCompletionModel( FileContentElement * data, QObject *parent = 0 );
	virtual ~XSLValueCompletionModel();
	
	void setBaliseName( const QString & name, const QString & attribute );
	
	QVariant data(const QModelIndex &index, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
public slots:
	void refreshList();
	void addElement( FileContentElement*, int );
	void removeElement( FileContentElement* );
private:
	void addElement( FileContentElement* element );
	bool contains( FileContentElement * data );
	void refreshRecursive( FileContentElement * data );

	QList<FileContentElement*> m_objList;
	FileContentElement* rootItem;
	
	QString m_baliseName;
	QString m_attributeName;
	
	QStringList m_files;
};

class XSLParamCompletionModel : public QAbstractListModel {
	Q_OBJECT
public:
	XSLParamCompletionModel( QObject *parent = 0 );
	virtual ~XSLParamCompletionModel();
	
	void setBaliseName( const QString & name );
	
	QVariant data(const QModelIndex &index, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
private:
	QString m_baliseName;
};

class XSLBaliseCompletionModel : public QAbstractListModel {
	Q_OBJECT
public:
	XSLBaliseCompletionModel( QObject *parent = 0, bool onlyHtml = false );
	virtual ~XSLBaliseCompletionModel();
	
	QVariant data(const QModelIndex &index, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
private:
	bool m_onlyHtml;
};

#endif // __XSLMODELCOMPLETER_H__
