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
#include <snipets/snipetlist.h>
#include <editors/textfileeditor.h>
#include <editors/xinxcodeedit.h>

// Qt header
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
	// TODO
	/*
	const Snipet & s = sender()->property( "Snipet" ).value<Snipet>();
	RunSnipetDialogImpl dlg( s, this );
	if( qobject_cast<TextFileEditor*>( m_editor ) && dlg.exec() ) {
		QString text = dlg.getResult();
		qobject_cast<TextFileEditor*>( m_editor )->textEdit()->insertText( text );
	}
	*/
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

void SnipetDockWidget::setEditor( AbstractEditor * ed ) {
	if( (!ed) || (m_editor != ed) ) {
		m_editor = ed;
		bool enabled = false;
		if( qobject_cast<TextFileEditor*>( ed ) )
			enabled = true;

		QList<QPushButton*> btnList = m_snipetsToolBox->findChildren<QPushButton*>();
		foreach( QPushButton * btn, btnList ) {
			btn->setEnabled( enabled );
		}
	}
}
