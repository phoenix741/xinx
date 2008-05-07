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

// Xinx header
#include "snipetdockwidget.h"
#include "snipetlist.h"
#include "runsnipetdialogimpl.h"
#include "fileeditor.h"
#include "texteditor.h"

// Qt header
#include <QApplication>
#include <QVBoxLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QVariant>

/* SnipetDockWidget */

SnipetDockWidget::SnipetDockWidget( const QString & title, QWidget * parent, Qt::WindowFlags flags ) : QDockWidget( title, parent, flags ) {
	setupUi();
	setEditor( 0 );
}

SnipetDockWidget::SnipetDockWidget( QWidget * parent, Qt::WindowFlags flags ) : QDockWidget( parent, flags ) {
	setupUi();
	setEditor( 0 );
}

SnipetDockWidget::~SnipetDockWidget() {
	
}

void SnipetDockWidget::setupUi() {
	m_snipetsToolBox = new QToolBox( this );

	QVBoxLayout * vlayout = new QVBoxLayout();
	vlayout->setSpacing( 0 );
	vlayout->setMargin( 0 );
	vlayout->addWidget( m_snipetsToolBox );

	QWidget * snipetsWidget = new QWidget( this );
	snipetsWidget->setLayout( vlayout );

	setWidget( snipetsWidget );
	
	updateSnipets();
	connect( SnipetListManager::self(), SIGNAL(listChanged()), this, SLOT(updateSnipets()) );
}

QWidget * SnipetDockWidget::createWidget() {
	QWidget * result = new QWidget( m_snipetsToolBox );
	result->setBackgroundRole( QPalette::Base );
	result->setLayout( new QVBoxLayout );
	result->layout()->setSpacing( 0 );
	return result;
}

QWidget * SnipetDockWidget::createSnipetWidget( const Snipet & s ) {
	QPushButton * p = new QPushButton( QIcon( s.icon() ), s.name() );
	if( s.icon().isEmpty() ) {
		p->setIcon( QIcon( ":/images/colors.png" ) );
	}
	p->setFlat( true );
	p->setStyleSheet( "text-align: left" );
	p->setProperty( "Snipet", QVariant::fromValue( s ) );
	connect( p, SIGNAL(clicked()), this, SLOT(callSnipet()) );
	return p;
}

void SnipetDockWidget::callSnipet() {
	const Snipet & s = sender()->property( "Snipet" ).value<Snipet>();
	RunSnipetDialogImpl dlg( s, this );
	if( qobject_cast<FileEditor*>( m_editor ) && dlg.exec() ) {
		QString text = dlg.getResult();
		qobject_cast<FileEditor*>( m_editor )->textEdit()->insertText( text ); 
	}
}

void SnipetDockWidget::updateSnipets() {
	qDeleteAll( m_pages );
	m_pages.clear();
	
	foreach( const Snipet & s, SnipetListManager::self()->snipets() ) {
		if( m_pages.value( s.category(), 0 ) == 0 ) {
			m_pages[ s.category() ] = createWidget();
			m_snipetsToolBox->addItem( m_pages[ s.category() ], s.category() );
		}
		m_pages[ s.category() ]->layout()->addWidget( createSnipetWidget( s ) );
	}
	foreach( QWidget * w, m_pages ) {
		qobject_cast<QBoxLayout*>( w->layout() )->addStretch();
	}
}

void SnipetDockWidget::setEditor( Editor * ed ) {
	if( (!ed) || (m_editor != ed) ) {
		m_editor = ed;
		bool enabled = false;
		if( qobject_cast<FileEditor*>( ed ) ) 
			enabled = true;
		
		QList<QPushButton*> btnList = m_snipetsToolBox->findChildren<QPushButton*>();
		foreach( QPushButton * btn, btnList ) {
			btn->setEnabled( enabled );
		}
	}
}
