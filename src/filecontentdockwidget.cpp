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

// Xinx header
#include "filecontentdockwidget.h"
#include "private/p_filecontentdockwidget.h"
#include "filecontentitemmodel.h"

// Qt header
#include <QVBoxLayout>

/* PrivateContentDockWidget */

PrivateFileContentDockWidget::PrivateFileContentDockWidget( FileContentDockWidget * parent ) : m_model(0), m_sortModel(0), m_parent( parent ) {
	m_contentTreeView = new QTreeView( m_parent );
	m_contentTreeView->setSortingEnabled( true );

	QVBoxLayout * vlayout = new QVBoxLayout();
	vlayout->setSpacing( 0 );
	vlayout->setMargin( 0 );
	vlayout->addWidget( m_contentTreeView );

	QWidget * m_contentWidget = new QWidget( parent );
	m_contentWidget->setLayout( vlayout );

	m_parent->setWidget( m_contentWidget );
	connect( m_contentTreeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(contentTreeViewDblClick(QModelIndex)) );
}

PrivateFileContentDockWidget::~PrivateFileContentDockWidget() {
	
}

void PrivateFileContentDockWidget::contentTreeViewDblClick( QModelIndex index ) {
	QModelIndex mappingIndex = m_sortModel->mapToSource( index );
		
	struct FileContentItemModel::struct_file_content data = m_model->data( mappingIndex, Qt::UserRole ).value<FileContentItemModel::struct_file_content>();
	int line = data.line;
	QString name = data.filename;

	emit m_parent->open( data.filename, line );
}

/* ContentDockWidget */
	
FileContentDockWidget::FileContentDockWidget( const QString & title, QWidget * parent, Qt::WindowFlags flags ) : QDockWidget( title, parent, flags ) {
	d = new PrivateFileContentDockWidget( this );
}

FileContentDockWidget::FileContentDockWidget( QWidget * parent, Qt::WindowFlags flags ) : QDockWidget( parent, flags ) {
	d = new PrivateFileContentDockWidget( this );	
}

FileContentDockWidget::~FileContentDockWidget() {
	delete d;
}

void FileContentDockWidget::updateModel( QAbstractItemModel * model ) {
	if( model == d->m_model ) return;
	if( model ) {
		if( d->m_sortModel ) delete d->m_sortModel;
		d->m_sortModel = new QSortFilterProxyModel( this );
		d->m_sortModel->setSourceModel( model );
		d->m_contentTreeView->setModel( d->m_sortModel );
	} else {
		d->m_contentTreeView->setModel( NULL );
		delete d->m_sortModel;
		d->m_sortModel = NULL;
	}
	d->m_model = model;
}
