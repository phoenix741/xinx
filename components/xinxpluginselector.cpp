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
#include "p_xinxpluginselector.h"
#include "plugininterfaces.h"

// Qt header
#include <QApplication>
#include <QPainter>
#include <QFont>

/* PrivateXinxPluginSelector */

PrivateXinxPluginSelector::PrivateXinxPluginSelector( XinxPluginSelector * parent ) : m_parent( parent ) {
	m_model = new XinxPluginModel( parent );
	m_delegate = new XinxPluginDelegate( parent );
	parent->setItemDelegate( m_delegate );
}

/* XinxPluginModel */

XinxPluginModel::XinxPluginModel( QWidget * parent ) : QAbstractListModel( parent ) {
	
}

XinxPluginModel::~XinxPluginModel() {
	
}

void XinxPluginModel::addPlugin( IXinxPlugin * plugin ) {
	beginInsertRows( QModelIndex(), m_plugins.count(), m_plugins.count() );
	m_plugins.append( plugin );
	endInsertRows();
}


bool XinxPluginModel::setData( const QModelIndex &index, const QVariant &value, int role ) {
	return false;
}

QVariant XinxPluginModel::data( const QModelIndex &index, int role ) const {
	if( ! index.isValid() ) 
		return QVariant();
	
	int i = index.row();
	if( ( i < 0 ) || ( i >= m_plugins.count() ) ) return QVariant();
	
	QVariant result = m_plugins.at( i )->getPluginAttribute( (IXinxPlugin::PluginAttribute)role );
	if( result.isValid() ) return result;

	switch( role ) {
	case Qt::DisplayRole:
		return m_plugins.at( i )->getPluginAttribute( IXinxPlugin::PLG_NAME );
	default:
		return QVariant();
	}	
}

Qt::ItemFlags XinxPluginModel::flags( const QModelIndex &index ) const {
	if( ! index.isValid() )
        return Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	else
		return QAbstractListModel::flags( index );
}

QModelIndex XinxPluginModel::index( int row, int column, const QModelIndex &parent ) const {
	Q_UNUSED( parent );
	return createIndex( row, column );
}

int XinxPluginModel::rowCount( const QModelIndex &parent ) const {
	Q_UNUSED( parent );
	return m_plugins.count();
}


XinxPluginDelegate::XinxPluginDelegate( QObject * parent ) : QItemDelegate( parent ) {
	m_separatorPixels = 8;
	m_rightMargin = 0;
	m_leftMargin = 0;
}

XinxPluginDelegate::~XinxPluginDelegate() {
	
}

void XinxPluginDelegate::paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const {
	painter->save();
	
	QFont titleFont( painter->font() );
	QFont descFont( painter->font() );
	titleFont.setBold( true );
	
	if( option.state & QStyle::State_Selected ) {
		painter->fillRect( option.rect, option.palette.color( QPalette::Highlight ) );
	} else {
		if( index.row() % 2 != 0 )
			painter->fillRect( option.rect, option.palette.color( QPalette::AlternateBase ) );
		else
			painter->fillRect( option.rect, option.palette.color( QPalette::Base ) );
	}
	
	QString display = index.model()->data( index, IXinxPlugin::PLG_NAME ).toString();
	QString description = index.model()->data( index, IXinxPlugin::PLG_DESCRIPTION ).toString(); 
	
	// Draw about button 
	QStyleOptionButton optAbout;
	{
		optAbout.state |= QStyle::State_Enabled;
		optAbout.icon = QIcon( ":/images/help-about.png" );
		optAbout.iconSize = QSize( 16, 16 );
		optAbout.text = tr("About ...");
		optAbout.fontMetrics = option.fontMetrics;
		optAbout.rect.setTop( option.rect.top() + m_separatorPixels );
		optAbout.rect.setSize( QApplication::style()->sizeFromContents( QStyle::CT_PushButton, &option, QSize( optAbout.fontMetrics.width( optAbout.text ) + optAbout.iconSize.width() + m_separatorPixels * 3, qMax( optAbout.fontMetrics.height(), optAbout.iconSize.height() + m_separatorPixels ) ) ) );
		if( optAbout.direction == Qt::LeftToRight )
			optAbout.rect.setLeft( option.rect.right() - m_rightMargin - m_separatorPixels - optAbout.rect.width() );
		else
			optAbout.rect.setLeft( m_leftMargin + m_separatorPixels );
		optAbout.rect.setSize( QApplication::style()->sizeFromContents( QStyle::CT_PushButton, &option, QSize( optAbout.fontMetrics.width( optAbout.text ) + optAbout.iconSize.width() + m_separatorPixels * 3, qMax( optAbout.fontMetrics.height(), optAbout.iconSize.height() + m_separatorPixels ) ) ) );
		
		QApplication::style()->drawControl( QStyle::CE_PushButton, &optAbout, painter );
	}

	// Draw settings button 
	{
		QStyleOptionButton opt;
		opt.state |= QStyle::State_Enabled;
		opt.icon = QIcon( ":/images/configure.png" );
		opt.iconSize = QSize( 16, 16 );
		opt.text = tr("Configure ...");
		opt.fontMetrics = option.fontMetrics;
		opt.rect.setTop( option.rect.top() + m_separatorPixels );
		opt.rect.setSize( QApplication::style()->sizeFromContents( QStyle::CT_PushButton, &option, QSize( opt.fontMetrics.width( opt.text ) + opt.iconSize.width() + m_separatorPixels * 3, qMax( opt.fontMetrics.height(), opt.iconSize.height() + m_separatorPixels ) ) ) );
		if( opt.direction == Qt::LeftToRight )
			opt.rect.setLeft( option.rect.right() - m_rightMargin - m_separatorPixels - opt.rect.width() - m_separatorPixels - optAbout.rect.width() );
		else
			opt.rect.setLeft( m_leftMargin + m_separatorPixels );
		opt.rect.setSize( QApplication::style()->sizeFromContents( QStyle::CT_PushButton, &option, QSize( opt.fontMetrics.width( opt.text ) + opt.iconSize.width() + m_separatorPixels * 3, qMax( opt.fontMetrics.height(), opt.iconSize.height() + m_separatorPixels ) ) ) );
		
		QApplication::style()->drawControl( QStyle::CE_PushButton, &opt, painter );
	}
	
	painter->setFont( titleFont );
	painter->drawText( m_leftMargin + m_separatorPixels * 2, m_separatorPixels + option.rect.top(), painter->fontMetrics().width( display ), painter->fontMetrics().height(), Qt::AlignLeft, display );
	painter->setFont( descFont );
    painter->drawText( m_leftMargin + m_separatorPixels * 2, option.rect.height() - m_separatorPixels - painter->fontMetrics().height() + option.rect.top(), painter->fontMetrics().width(description), painter->fontMetrics().height(), Qt::AlignLeft, description );

	painter->restore();
}

QSize XinxPluginDelegate::sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const {
	return QSize( 200, 50 );
	//return QItemDelegate::sizeHint( option, index );
}


/* XinxPluginSelector */

XinxPluginSelector::XinxPluginSelector( QWidget *parent ) : QListView( parent ) {
	Q_INIT_RESOURCE( xinxpluginselector );
	
	d = new PrivateXinxPluginSelector( this );
	setModel( d->m_model );
}

XinxPluginSelector::~XinxPluginSelector() {
	delete d;
}
    
void XinxPluginSelector::addPlugin( IXinxPlugin * plugin ) {
	d->m_model->addPlugin( plugin );
}
