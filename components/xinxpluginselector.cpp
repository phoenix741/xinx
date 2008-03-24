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
#include <QFont>

/* PrivateXinxPluginSelector */

PrivateXinxPluginSelector::PrivateXinxPluginSelector( XinxPluginSelector * parent ) : m_parent( parent ) {
	m_model = new XinxPluginModel( parent );
	m_delegate = new XinxPluginDelegate( parent );
}

/* XinxPluginModel */

XinxPluginModel::XinxPluginModel( QWidget * parent ) : QAbstractListModel( parent ) {
	
}

XinxPluginModel::~XinxPluginModel() {
	
}

void XinxPluginModel::addPlugin( XinxPluginElement plugin ) {
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
	
	QVariant result = m_plugins.at( i ).plugin->getPluginAttribute( (IXinxPlugin::PluginAttribute)role );
	if( ( (IXinxPlugin::PluginAttribute)role == IXinxPlugin::PLG_NAME ) && m_plugins.at( i ).isStatic )
		result = result.toString() + " (Static loaded)";
	if( result.isValid() ) return result;

	switch( role ) {
	case Qt::DisplayRole:
		return m_plugins.at( i ).plugin->getPluginAttribute( IXinxPlugin::PLG_NAME );
	case Qt::CheckStateRole:
		if( m_plugins.at( i ).isActivated )
			return Qt::Checked;
		else
			return Qt::Unchecked;
	default:
		return QVariant();
	}	
}

Qt::ItemFlags XinxPluginModel::flags( const QModelIndex &index ) const {
	if( ! index.isValid() ) {
		int i = index.row();
		if( ( i < 0 ) || ( i >= m_plugins.count() ) ) 
			return QAbstractListModel::flags( index );

		if( m_plugins.at( i ).isStatic )
			return Qt::ItemIsUserCheckable | Qt::ItemIsSelectable;
		else
			return Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	} else
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

/* XinxPluginDelegate */

XinxPluginDelegate::XinxPluginDelegate( QObject * parent ) : QItemDelegate( parent ) {
	m_separatorPixels = 8;
	m_rightMargin = 0;
	m_leftMargin = 0;
	m_buttonPressed = false;
}

XinxPluginDelegate::~XinxPluginDelegate() {
	
}

bool XinxPluginDelegate::eventFilter( QObject *watched, QEvent *event ) {
	QWidget * viewport = qobject_cast<QWidget*>( watched );
	XinxPluginSelector * listView  = qobject_cast<XinxPluginSelector*>( viewport->parent() );
	if( ! listView )  
		listView = qobject_cast<XinxPluginSelector*>( viewport );
	if( ! ( listView && viewport ) ) return false;
	
	m_cursorPosition = viewport->mapFromGlobal( QCursor::pos() );
	
	QModelIndex currentIndex;
	if( listView )
		currentIndex = listView->indexAt( m_cursorPosition );

	if( ! currentIndex.isValid() ) return false;

	QStyleOptionViewItem option;
	option.fontMetrics = listView->viewOptions().fontMetrics;
	option.direction = listView->layoutDirection();
	option.rect = listView->visualRect( currentIndex );
	
	QStyleOptionButton optAbout, optConfigure;
	optAbout = calculateButtonAbout( option );
	optConfigure = calculateButtonConfigure( option, m_separatorPixels + optAbout.rect.width() );
	
	QRect rectCheck;
	QStyleOptionViewItem optCheck = calculateCheckbox( option, rectCheck );
	
	if( ! ( optAbout.rect.contains( m_cursorPosition ) || optConfigure.rect.contains( m_cursorPosition ) || rectCheck.contains( m_cursorPosition ) ) )
		return false;
	
	if( viewport && ( event->type() == QEvent::MouseMove ) ) {
		viewport->update();
		return true;
	} else if( viewport && ( event->type() == QEvent::MouseButtonPress ) ) {
		m_buttonPressed  = true;
		viewport->update();
		return true;
	} else if( viewport && ( event->type() == QEvent::MouseButtonRelease ) ) {
		m_buttonPressed  = false;
		viewport->update();
		return true;
	}
	return false;
}

void XinxPluginDelegate::paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const {	
	painter->save();
	
	QStyleOptionViewItem titleOpt( option );
	QFontMetrics metrics = painter->fontMetrics();
	QFont titleFont( painter->font() );
	QFont descFont( painter->font() );
	titleFont.setBold( true );
	titleOpt.font = titleFont;
	titleOpt.fontMetrics = QFontMetrics( titleFont );
	
	if( option.state & QStyle::State_Selected ) {
		painter->fillRect( option.rect, option.palette.color( QPalette::Highlight ) );
	} else {
		if( index.row() % 2 != 0 )
			painter->fillRect( option.rect, option.palette.color( QPalette::AlternateBase ) );
		else
			painter->fillRect( option.rect, option.palette.color( QPalette::Base ) );
	}
	
	// Draw checkbox
	QRect checkRect;
	QStyleOptionViewItem checkOpt = calculateCheckbox( option, checkRect );
	if( ! ( index.model()->flags( index ) & Qt::ItemIsEnabled ) )
		checkOpt.state ^= ~ QFlags<QStyle::StateFlag>( QStyle::State_Enabled );	
	drawCheck( painter, checkOpt, checkRect, (Qt::CheckState)index.model()->data( index, Qt::CheckStateRole ).toInt() );
	
    // Draw icon
	QPixmap iconPixmap;
	if( index.model()->data( index, IXinxPlugin::PLG_ICON ).isValid() )
		iconPixmap = index.model()->data( index, IXinxPlugin::PLG_ICON ).value<QPixmap>();
	else
		iconPixmap = QPixmap( ":/images/unknown.png" );
	iconPixmap = iconPixmap.scaled( QSize( 32, 32 ) );
	int pixmapLeftPosition = option.direction == Qt::LeftToRight ? 
								checkRect.right() + m_separatorPixels 
							:   checkRect.left() - m_separatorPixels - iconPixmap.width();
	
    painter->drawPixmap( pixmapLeftPosition, ( option.rect.height() - iconPixmap.height() ) / 2 + option.rect.top(), iconPixmap );

    // Draw buttons 
	QStyleOptionButton optAbout = drawButtonAbout( painter, option );
	QStyleOptionButton optConfigure = drawButtonConfigure( painter, option, m_separatorPixels + optAbout.rect.width() );

	// Draw text 
	int maxTextLength = option.rect.width() - checkRect.width() - iconPixmap.width() - m_leftMargin - m_rightMargin - m_separatorPixels * 7 - optAbout.rect.width() - optConfigure.rect.width();
	
	QString display = index.model()->data( index, IXinxPlugin::PLG_NAME ).toString();
	QString description = index.model()->data( index, IXinxPlugin::PLG_DESCRIPTION ).toString(); 
	
#	define LeftPosition( text, metrics ) option.direction == Qt::LeftToRight ? \
											pixmapLeftPosition + iconPixmap.width() + m_separatorPixels * 2 : \
											pixmapLeftPosition - iconPixmap.width() - m_separatorPixels * 2 - metrics.width( text )
	
	if( option.state & QStyle::State_Selected )
		painter->setPen( option.palette.color( QPalette::HighlightedText ) );
	
	painter->setFont( titleFont );
	display = titleOpt.fontMetrics.elidedText( display, Qt::ElideRight, maxTextLength );
	painter->drawText( 
			LeftPosition( display, titleOpt.fontMetrics ),     
			m_separatorPixels + option.rect.top(), 
			titleOpt.fontMetrics.width( display ), 
			titleOpt.fontMetrics.height(), 
			Qt::AlignLeft, 
			display );

	painter->setFont( descFont );
	description = metrics.elidedText( description, Qt::ElideRight, maxTextLength );
    painter->drawText( 
    		LeftPosition( description, metrics ), 
    		option.rect.height() - m_separatorPixels - metrics.height() + option.rect.top(), 
    		metrics.width( description ), 
    		metrics.height(), 
    		Qt::AlignLeft, 
    		description );

	painter->restore();
}

QStyleOptionButton XinxPluginDelegate::calculateButtonAbout( const QStyleOptionViewItem & option, int decalage ) const {
	return calculateButton( QIcon( ":/images/help-about.png" ), tr("About ..."), option, decalage );
}

QStyleOptionButton XinxPluginDelegate::drawButtonAbout( QPainter * painter, const QStyleOptionViewItem & option, int decalage ) const {
	QStyleOptionButton o = calculateButtonAbout( option, decalage );
	QApplication::style()->drawControl( QStyle::CE_PushButton, &o, painter );
	return o;
}

QStyleOptionButton XinxPluginDelegate::calculateButtonConfigure( const QStyleOptionViewItem & option, int decalage ) const {
	return calculateButton( QIcon( ":/images/configure.png" ), tr("Configure ..."), option, decalage );
}

QStyleOptionButton XinxPluginDelegate::drawButtonConfigure( QPainter * painter, const QStyleOptionViewItem & option, int decalage ) const {
	QStyleOptionButton o = calculateButtonConfigure( option, decalage );
	QApplication::style()->drawControl( QStyle::CE_PushButton, &o, painter );
	return o;
}

QStyleOptionButton XinxPluginDelegate::calculateButton( const QIcon & icon, const QString & caption, const QStyleOptionViewItem & option, int decalage ) const {
	QSize buttonSize;
	QStyleOptionButton o;
	o.state |= QStyle::State_Enabled; 
	
	o.icon = icon;
	o.iconSize = QSize( 16, 16 );
	o.text = caption;
	o.fontMetrics = option.fontMetrics;
	
	buttonSize = QApplication::style()->sizeFromContents( 
					QStyle::CT_PushButton, 
					&option, 
					QSize( o.fontMetrics.width( caption ) + o.iconSize.width() + m_separatorPixels * 3, 
						   qMax( o.fontMetrics.height(), o.iconSize.height() ) + m_separatorPixels  
					) 
				 );
	
	o.rect.setTop( option.rect.top() + m_separatorPixels );
	o.rect.setSize( buttonSize );
	if( option.direction == Qt::LeftToRight )
		o.rect.setLeft( option.rect.right() - m_rightMargin - m_separatorPixels - decalage - o.rect.width() );
	else
		o.rect.setLeft( m_leftMargin + m_separatorPixels + decalage );
	o.rect.setSize( buttonSize );

	if( o.rect.contains( m_cursorPosition ) ) { 
		o.state |= QStyle::State_MouseOver;
		if( m_buttonPressed )
			o.state |= QStyle::State_Sunken;
	}

	return o;
}

QStyleOptionButton XinxPluginDelegate::drawButton( QPainter * painter, const QIcon & icon, const QString & caption, const QStyleOptionViewItem & option, int decalage ) const {
	QStyleOptionButton o = calculateButton( icon, caption, option, decalage );
	QApplication::style()->drawControl( QStyle::CE_PushButton, &o, painter );
	return o;
}

QStyleOptionViewItem XinxPluginDelegate::calculateCheckbox( const QStyleOptionViewItem & option, QRect & rect, int decalage ) const {
	QStyleOptionViewItem checkOpt( option );
	rect = QApplication::style()->subElementRect( QStyle::SE_ViewItemCheckIndicator, &option );
	QSize checkSize = rect.size();
	rect.setTopLeft( QPoint( 
			option.direction == Qt::LeftToRight ? option.rect.left() + m_leftMargin + m_separatorPixels + decalage
											    : option.rect.right() - m_rightMargin -  m_separatorPixels - checkSize.width() - decalage,
			( option.rect.height() - checkSize.height() ) / 2 + option.rect.top()
			));
	rect.setSize( checkSize );
	return checkOpt;
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
	setItemDelegate( d->m_delegate );
    viewport()->installEventFilter( d->m_delegate );
    installEventFilter( d->m_delegate );
    setMouseTracking( true );
}

XinxPluginSelector::~XinxPluginSelector() {
	delete d;
}
    
void XinxPluginSelector::addPlugin( XinxPluginElement plugin ) {
	d->m_model->addPlugin( plugin );
}

QStyleOptionViewItem XinxPluginSelector::viewOptions() const {
    return QListView::viewOptions();
}
