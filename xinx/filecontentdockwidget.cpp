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

// Xinx header
#include "filecontentdockwidget.h"
#include <contentview/contentviewmodel.h>

// Qt header
#include <QVBoxLayout>
#include <QTreeView>
#include <QAbstractItemModel>

#include <modeltest.h>

/* ContentDockWidget */

FileContentDockWidget::FileContentDockWidget( const QString & title, QWidget * parent, Qt::WindowFlags flags ) : QDockWidget( title, parent, flags ), m_model(0) {
	init();
}

FileContentDockWidget::FileContentDockWidget( QWidget * parent, Qt::WindowFlags flags ) : QDockWidget( parent, flags ), m_model(0) {
	init();
}

FileContentDockWidget::~FileContentDockWidget() {

}

void FileContentDockWidget::init() {
	m_contentTreeView = new QTreeView( this );

	QVBoxLayout * vlayout = new QVBoxLayout();
	vlayout->setSpacing( 0 );
	vlayout->setMargin( 0 );
	vlayout->addWidget( m_contentTreeView );

	QWidget * m_contentWidget = new QWidget( this );
	m_contentWidget->setLayout( vlayout );

	setWidget( m_contentWidget );
	connect( m_contentTreeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(contentTreeViewDblClick(QModelIndex)) );
}

void FileContentDockWidget::contentTreeViewDblClick( QModelIndex index ) {
	struct ContentViewModel::struct_file_content data = m_model->data( index, Qt::UserRole ).value<ContentViewModel::struct_file_content>();
	int line = data.line;
	QString name = data.filename;

	emit open( data.filename, line );
}

void FileContentDockWidget::updateModel( QAbstractItemModel * model ) {
	if( model == m_model ) return;
// 	if( model ) {
// 		new ModelTest(model, this);
// 	}
	m_contentTreeView->setModel( model );
	m_model = model;
}

void FileContentDockWidget::positionChanged( const QModelIndex & index ) {
	m_contentTreeView->setCurrentIndex( index );
	/*QItemSelectionModel * selectionModel = m_contentTreeView->selectionModel();
	QItemSelection selection( index, index );
	selectionModel->select( selection, QItemSelectionModel::ToggleCurrent );*/
}

