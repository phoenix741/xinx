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
#include "xmlpresentationdockwidget.h"
#include "private/p_xmlpresentationdockwidget.h"
#include "globals.h"
#include "xslproject.h"

// Qt header
#include <QDir>
#include <QFileDialog>

/* PrivateXmlPresentationDockWidget */

PrivateXmlPresentationDockWidget::PrivateXmlPresentationDockWidget( XmlPresentationDockWidget * parent ) : m_parent( parent ) {
	QWidget * contentWidget = new QWidget( m_parent );
	m_xmlPresentationWidget = new Ui::XmlPresentationWidget();
	m_xmlPresentationWidget->setupUi( contentWidget );
	m_parent->setWidget( contentWidget );
	
	initXmlPresentationCombo();
	connect( &global, SIGNAL(projectChanged()), this, SLOT(initXmlPresentationCombo()) );
	connect( m_xmlPresentationWidget->m_presentationComboBox, SIGNAL(activated(int)), this, SLOT(presentationActivated(int)) );
}

PrivateXmlPresentationDockWidget::~PrivateXmlPresentationDockWidget() {
	delete m_xmlPresentationWidget;
}

void PrivateXmlPresentationDockWidget::initXmlPresentationCombo() {
	m_xmlPresentationWidget->m_presentationComboBox->clear();
	
	m_xmlPresentationWidget->m_presentationComboBox->addItem( tr("<No presentation file>") );
	m_xmlPresentationWidget->m_presentationComboBox->addItem( tr("<Choose an XML file ...>") );
	
	if( global.m_project ) {
		m_logPath = global.m_project->logProjectDirectory();
		QDir logDir( m_logPath );
		if( logDir.exists() ) {
			QStringList files = logDir.entryList( QStringList() << "Presentation_*.xml", QDir::Files | QDir::Readable );
			foreach( QString file, files ) {
				m_xmlPresentationWidget->m_presentationComboBox->addItem( file );
			}
		}
	}
}

void PrivateXmlPresentationDockWidget::presentationActivated( int index ) {
	if( index == 0 ) {
		// Clean the tree
	} else if( index == 1 ) {
		// Open a file
		QString name = QFileDialog::getOpenFileName( m_parent, 
			tr("Open a presentation file"),
			m_logPath,
			tr("Presentation XML File (*.xml)") );
		open( name );
	} else {
		// Open the file
		QString name = QDir( m_logPath ).absoluteFilePath( m_xmlPresentationWidget->m_presentationComboBox->itemText( index ) );
		open( name );
	}
}

void PrivateXmlPresentationDockWidget::open( const QString& filename ) {
	
}

/* XmlPresentationDockWidget */

XmlPresentationDockWidget::XmlPresentationDockWidget( const QString & title, QWidget * parent, Qt::WindowFlags flags ) : QDockWidget( title, parent, flags ) {
	d = new PrivateXmlPresentationDockWidget( this );
}

XmlPresentationDockWidget::XmlPresentationDockWidget( QWidget * parent, Qt::WindowFlags flags ) : QDockWidget( parent, flags ) {
	d = new PrivateXmlPresentationDockWidget( this );
}

XmlPresentationDockWidget::~XmlPresentationDockWidget() {
	delete d;
}
