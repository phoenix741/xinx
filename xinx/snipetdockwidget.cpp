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
#include "snipetdockwidget.h"
#include <utils/recursivesortfilterproxymodel.h>
#include <snipets/snipetdockitemmodel.h>
#include <snipets/snipetmanager.h>
#include <editors/textfileeditor.h>
#include <editors/xinxcodeedit.h>
#include <editors/editormanager.h>
#include "customdialogimpl.h"

// Qt header
#include <QVBoxLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QVariant>

/* SnipetDockWidget */

SnipetDockWidget::SnipetDockWidget( const QString & title, QWidget * parent, Qt::WindowFlags flags ) : QDockWidget( title, parent, flags ) {
	init();
}

SnipetDockWidget::SnipetDockWidget( QWidget * parent, Qt::WindowFlags flags ) : QDockWidget( parent, flags ) {
	init();
}

SnipetDockWidget::~SnipetDockWidget() {
	delete m_dock;

}

void SnipetDockWidget::init() {
	QWidget * contentWidget = new QWidget( this );
	m_dock = new Ui::SnipetsDockWidget();
	m_dock->setupUi( contentWidget );
	setWidget( contentWidget );

	/* Snipets Tree */
	m_snipetFilterModel = new RecursiveSortFilterProxyModel( m_dock->m_snipetTreeView );
	m_snipetFilterModel->setShowAllChild( true );
	m_snipetFilterModel->setDynamicSortFilter( true );
	m_snipetFilterModel->setFilterCaseSensitivity( Qt::CaseInsensitive );
	//m_snipetFilterModel->setFilterKeyColumn( -1 );

	m_snipetModel = SnipetManager::self()->createSnipetDockItemModel( m_snipetFilterModel );
	m_snipetModel->select();
	m_snipetFilterModel->setSourceModel( m_snipetModel );
	m_snipetFilterModel->setIncludeIndex( QModelIndexList() << m_snipetFilterModel->mapToSource( m_snipetFilterModel->index( 0, 0 ) ) );

	m_dock->m_snipetTreeView->setModel( m_snipetFilterModel );
	m_dock->m_snipetTreeView->setSortingEnabled( true );

	m_dock->m_snipetTreeView->setRootIndex( m_snipetFilterModel->index( 0, 0 ) );
	m_dock->m_snipetTreeView->sortByColumn( 0, Qt::AscendingOrder );
	m_dock->m_snipetTreeView->expandAll();

	connect( m_dock->m_createSnipetBtn, SIGNAL(clicked()), this, SLOT(createSnipet()) );
	connect( m_dock->m_paramSnipetBtn, SIGNAL(clicked()), this, SLOT(customizeSnipet()) );

	connect( m_dock->m_snipetFilter, SIGNAL(textChanged(QString)), this, SLOT(filterChanged(QString)) );
	connect( m_dock->m_snipetTreeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(callSnipet(QModelIndex)) );

	connect( XINXConfig::self(), SIGNAL(changed()), m_snipetModel, SLOT(select()) );

	setEditor( 0 );
}

void SnipetDockWidget::setEditor( AbstractEditor * ed ) {
	if( (!ed) || (m_editor != ed) ) {
		m_editor = ed;
		bool enabled = false;
		if( qobject_cast<TextFileEditor*>( ed ) )
			enabled = true;
	}
}

void SnipetDockWidget::createSnipet() {
	SnipetManager::self()->addSnipet( 1, this );
	m_snipetModel->select();
}

void SnipetDockWidget::customizeSnipet() {
	CustomDialogImpl dlg( this );
	dlg.loadFromConfig( XINXConfig::self() );

	dlg.m_listWidget->setCurrentRow( 7 );
	dlg.m_listWidget->setVisible( false );
	if( dlg.exec() ) {
		dlg.saveToConfig( XINXConfig::self() );
		XINXConfig::self()->save();
	}
}

void SnipetDockWidget::callSnipet( const QModelIndex & index ) {
	if( index.isValid() && EditorManager::self()->currentEditor() ) {
		if( index.data( SnipetDockItemModel::SnipetTypeRole ).toString() == "SNIPET" ) {
			TextFileEditor * ed = qobject_cast<TextFileEditor*>( EditorManager::self()->currentEditor() );
			int id = index.data( SnipetDockItemModel::SnipetIdRole ).toInt();
			QString result;
			if( SnipetManager::self()->callSnipet( id, &result, this ) ) {
				ed->textEdit()->insertText( result );
			}
		}
	}
}

void SnipetDockWidget::filterChanged( const QString & filterText ) {
	m_snipetFilterModel->invalidate();
	m_snipetFilterModel->setFilterRegExp( filterText );
	m_dock->m_snipetTreeView->expandAll();
}
