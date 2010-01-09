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
#include "generixprojectdock.h"
#include "projectproperty/generixproject.h"
#include <editors/editormanager.h>
#include <editors/abstracteditor.h>
#include "configuration/configurationmanager.h"

// Qt header
#include <QFileInfo>
#include <QDir>
#include <QPainter>

/* Constante */

#define SEPARATOR_SIZE 4

/* BusinessViewListDelegate */

BusinessViewListDelegate::BusinessViewListDelegate( QObject * parent ) : QItemDelegate( parent ) {
}

BusinessViewListDelegate::~BusinessViewListDelegate() {
}

void BusinessViewListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
	GenerixProject * gnxProject = static_cast<GenerixProject*>( XINXProjectManager::self()->project() );
	if( ! gnxProject ) return ;

	painter->save();

	QStyleOptionViewItem myOption = option;
	myOption.state |= QStyle::State_Enabled;

	QFontMetrics metrics = painter->fontMetrics();

	QStyleOptionViewItem titleOpt( myOption );
	QFont titleFont( painter->font() );
	titleFont.setBold( true );
	titleOpt.font = titleFont;
	titleOpt.fontMetrics = QFontMetrics( titleFont );

	QStyleOptionViewItem descOpt( myOption );
	QFont descFont( painter->font() );
	descFont.setPointSize( descFont.pointSize() - 1 );
	descOpt.font = descFont;
	descOpt.fontMetrics = QFontMetrics( descFont );

	if( myOption.state & QStyle::State_Selected ) {
		painter->fillRect( myOption.rect, myOption.palette.color( QPalette::Highlight ) );
	} else {
		if( index.row() % 2 != 0 )
			painter->fillRect( myOption.rect, myOption.palette.color( QPalette::AlternateBase ) );
		else
			painter->fillRect( myOption.rect, myOption.palette.color( QPalette::Base ) );
	}

	// Draw text
	int maxTextLength = myOption.rect.width();

	QString filename = index.model()->data( index, Qt::UserRole + 1 ).toString();
	int configurationIndex = index.model()->data( index, Qt::UserRole + 0 ).toInt();

	QString display = QString( "%1 (%2)").arg( index.model()->data( index ).toString() ).arg( index.model()->data( index, Qt::UserRole + 2 ).toString() );
	QString description = (configurationIndex >= 0 ? QString( "%1 - " ).arg( configurationIndex ) : QString() ) + QDir( gnxProject->webModuleLocation() ).relativeFilePath( filename );

	if( myOption.state & QStyle::State_Selected )
		painter->setPen( myOption.palette.color( QPalette::HighlightedText ) );

	painter->setFont( titleFont );
	display = titleOpt.fontMetrics.elidedText( display, Qt::ElideRight, maxTextLength );
	painter->drawText(
			SEPARATOR_SIZE,
			SEPARATOR_SIZE + myOption.rect.top(),
			titleOpt.fontMetrics.width( display ),
			titleOpt.fontMetrics.height(),
			Qt::AlignLeft,
			display );

	painter->setFont( descFont );
	description = descOpt.fontMetrics.elidedText( description, Qt::ElideRight, maxTextLength );
	painter->drawText(
		SEPARATOR_SIZE,
		myOption.rect.height() - SEPARATOR_SIZE - descOpt.fontMetrics.height() + myOption.rect.top(),
		descOpt.fontMetrics.width( description ),
		descOpt.fontMetrics.height(),
		Qt::AlignLeft,
		description );

	painter->restore();
}

QSize BusinessViewListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
	Q_UNUSED( index );

	QFont title( option.font );
	title.setBold( true );

	QFontMetrics titleMetrics( title );
	QFontMetrics currentMetrics( option.font );

	int height = titleMetrics.height() + currentMetrics.height() + SEPARATOR_SIZE;
	height += SEPARATOR_SIZE * 2;

	return QSize( 200, height );
}

/* GenerixProjectDockImpl */

GenerixProjectDockImpl::GenerixProjectDockImpl( QWidget * parent ) : QDockWidget( parent ) {
	setupUi( this );

	m_businessViewList->setItemDelegate( m_delegate = new BusinessViewListDelegate( m_businessViewList ) );

	setProject( 0 );

	connect( dynamic_cast<QObject*>( EditorManager::self() ), SIGNAL(currentChanged(int)), this, SLOT(editorChanged(int)) );
	connect( XINXProjectManager::self(), SIGNAL(changed()), this, SLOT(projectChanged()) );
}

GenerixProjectDockImpl::~GenerixProjectDockImpl() {
}

void GenerixProjectDockImpl::setProject( XinxProject * project ) {
	m_gnxProject = static_cast<GenerixProject*>( project );

	m_prefixCombo->clear();
	if( m_gnxProject && m_gnxProject->prefixes().size() ) {
		m_prefixCombo->addItems( m_gnxProject->prefixes() );

		m_prefixCombo->setCurrentIndex( m_prefixCombo->findText( m_gnxProject->defaultPrefix() ) );
	}
	m_prefixCombo->setVisible( m_gnxProject && ( m_gnxProject->prefixes().size() > 0 ) );
	m_prefixLabel->setVisible( m_gnxProject && ( m_gnxProject->prefixes().size() > 0 ) );
}

void GenerixProjectDockImpl::projectChanged() {
	setProject( XINXProjectManager::self()->project() );
}

void GenerixProjectDockImpl::editorChanged( int index ) {
	if( m_gnxProject && ( index >= 0 ) && ( index < EditorManager::self()->editorsCount() ) ) {
		const QString editorFilename = EditorManager::self()->editor( index )->lastFileName();

		m_filenameLabel->setText( tr("&Business view for file : \n%1").arg( QFileInfo( editorFilename ).fileName() ) );

		const QString filename = QDir( m_gnxProject->webModuleLocation() ).relativeFilePath( editorFilename );

		GceInterface * interface = ConfigurationManager::self()->getInterfaceOfProject( m_gnxProject );
		m_businessViewList->clear();
		foreach( const BusinessViewInformation information, interface->businessView( filename ) ) {
			QListWidgetItem * item = new QListWidgetItem( information.businesViewName(), m_businessViewList );
			item->setData( Qt::UserRole + 0, information.configurationNumber() );
			item->setData( Qt::UserRole + 1, information.configurationFileName() );
			item->setData( Qt::UserRole + 2, information.targetName() );
			m_businessViewList->addItem( item );
		}
	} else {
		m_filenameLabel->setText( QString() );
		m_businessViewList->clear();
	}
}


void GenerixProjectDockImpl::on_m_prefixCombo_activated( QString text ) {
	if( m_gnxProject )
		m_gnxProject->setDefaultPrefix( text );
}