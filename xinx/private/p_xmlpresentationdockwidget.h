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

#ifndef __P_XMLPRESENTATIONDOCKWIDGET_H__
#define __P_XMLPRESENTATIONDOCKWIDGET_H__

// Xinx header
#include "../xmlpresentationdockwidget.h"
#include "ui_xmlpresentationwidget.h"
#include "../xmlpresentationitem.h"
#include "../filewatcher.h"
#include "../xinxthread.h"

// Qt header
#include <QThread>
#include <QSortFilterProxyModel>
#include <QTimer>

class PrivateXmlPresentationDockWidget : public XinxThread {
	Q_OBJECT
public:
	
	class RecursiveFilterProxyModel : public QSortFilterProxyModel {
	public:
		RecursiveFilterProxyModel( QObject * parent = 0 );
	protected:
		virtual bool filterAcceptsRow ( int source_row, const QModelIndex & source_parent ) const;
		bool canBeShow( const QModelIndex & index ) const;
	};
	
	PrivateXmlPresentationDockWidget( XmlPresentationDockWidget * parent );
	~PrivateXmlPresentationDockWidget();
	
	Ui::XmlPresentationWidget * m_xmlPresentationWidget;
	QString m_logPath, m_openingFile, m_filteredText;
	XmlPresentationModel * m_model;
	QSortFilterProxyModel * m_sortFilterModel;
	FileWatcher * m_watcher;
	QTimer m_timerTextChanged;
	
	enum { THREAD_OPENING, THREAD_FILTERED } m_threadAct;
	
	void open( const QString& filename );
	void setComboToolTip( const QString & filename );
public slots:
	void adaptColumns();
	void open();
	void initXmlPresentationCombo();
	void presentationActivated( int index );
	void threadTerminated();
	
	void filterTextChanged( const QString & text );
	void filterTextChangedTimer();
protected:
	virtual void threadrun();
private:
	XmlPresentationDockWidget * m_parent;
};

#endif // __P_XMLPRESENTATIONDOCKWIDGET_H__
