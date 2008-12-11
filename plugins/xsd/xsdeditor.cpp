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
#include "p_xsdeditor.h"
#include "borderlayout.h"

/* XsdAnnotationItem */

XsdAnnotationItem::XsdAnnotationItem( const QString & text, QGraphicsItem * parent ) : QGraphicsTextItem( text, parent ) {
	setPos( 0, parent->boundingRect().height() );
	setTextWidth( parent->boundingRect().width() * 2 );
}

/* XsdAttributeItem */

XsdAttributeItem::XsdAttributeItem( const QString & name, const QString & value, const QString & use ) : m_name( name ), m_value( value ), m_use( use ) {

}

/* XsdNodeItem */

XsdNodeItem::~XsdNodeItem() {
	qDeleteAll( m_line );
}

QVariant XsdNodeItem::itemChange( QGraphicsItem::GraphicsItemChange change, const QVariant &value ) {
	if( change == QGraphicsItem::ItemPositionHasChanged ) {
		foreach( XsdLine * l, m_line ) {
			l->updatePosition();
		}
	}

	return value;
}

/* XsdLine */

XsdLine::XsdLine( XsdNodeItem * startItem, XsdNodeItem * endItem, QGraphicsItem *parent ) : QGraphicsPathItem( parent ), m_startItem( startItem ), m_endItem( endItem ) {
	m_startItem->m_line.append( this );
	m_endItem->m_line.append( this );
	setZValue( -1 );

	updatePosition();
}

XsdLine::~XsdLine() {
	m_startItem->m_line.removeOne( this ) ;
	m_endItem->m_line.removeOne( this );
}

QRectF XsdLine::boundingRect() const {
	return m_path.boundingRect();
}

void XsdLine::updatePosition() {
	m_path = QPainterPath();
	QPointF start = mapFromItem( dynamic_cast<QGraphicsItem*>( m_startItem ), 0, 0 );
	QPointF end   = mapFromItem( dynamic_cast<QGraphicsItem*>( m_endItem ), 0, 0 );
	m_path.moveTo( start );
	m_path.lineTo( 30, start.y() );
	m_path.lineTo( 30, end.y() );
	m_path.lineTo( end );

	setPath( m_path );
}

/* XsdElementItem */

XsdElementItem::XsdElementItem( const QString & name, const QString & type, int minOccurs, int maxOccurs, QGraphicsItem * parent ) : QGraphicsItem( parent ), m_name( name ), m_type( type ), m_minOccurs( minOccurs ), m_maxOccurs( maxOccurs ), m_spacing( 5 ) {
	scene()->addItem( new XsdLine( dynamic_cast<XsdNodeItem*>( parent ), this ) );
}

QList<XsdAttributeItem> & XsdElementItem::attributes() {
	return m_attributes;
}

QRectF XsdElementItem::boundingRect () const {
	QFontMetrics metrics( m_font );
	QRectF r = metrics.boundingRect( m_name );
	r.adjust( -m_spacing, -m_spacing, m_spacing, m_spacing );
	r.moveCenter( QPointF( 0, 0 ) );
	return r;
}

void XsdElementItem::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * ) {
	painter->setBrush( Qt::white );
	painter->setPen( Qt::black );
	painter->drawRect( boundingRect() );

	painter->drawText( boundingRect(), m_name, QTextOption( Qt::AlignCenter ) );
}

/* XsdSequenceItem */

XsdSequenceItem::XsdSequenceItem( QGraphicsItem * parent ) : QGraphicsPolygonItem( parent ), m_vspacing( 10 ), m_hspacing( 30 ) {
	setBrush( Qt::white );
	setPen( QPen( Qt::black ) );

	QPolygonF polygone;
	polygone << QPointF( 5, 0 );
	polygone << QPointF( 35, 0 );
	polygone << QPointF( 40, 5 );
	polygone << QPointF( 40, 15 );
	polygone << QPointF( 35, 20 );
	polygone << QPointF( 5, 20 );
	polygone << QPointF( 0, 15 );
	polygone << QPointF( 0, 5 );
	polygone << QPointF( 5, 0 );
	polygone.translate( -20, -10 );
	setPolygon( polygone );
}

void XsdSequenceItem::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * w ) {
	QGraphicsPolygonItem::paint( painter, option, w );
	painter->drawText( boundingRect(), "seq", QTextOption( Qt::AlignCenter ) );
}

void XsdSequenceItem::addItem( QGraphicsItem * item ) {
	item->setParentItem( this );
	updatePosition();
}

void XsdSequenceItem::updatePosition() {
	int countOfChild = childItems().count();

	qreal height = 0;
	if( countOfChild > 1 )
		height = ( countOfChild - 1 ) * m_vspacing;
	foreach( QGraphicsItem * i, childItems() ) {
		height += ( i->boundingRect() | i->childrenBoundingRect() ).height();
	}

	qreal itemPosX = - height / 2;
	foreach( QGraphicsItem * i, childItems() ) {
		i->setPos( boundingRect().width() / 2 + m_hspacing + i->boundingRect().width() / 2, itemPosX - i->boundingRect().height() / 2 );

		itemPosX += ( i->boundingRect() | i->childrenBoundingRect() ).height() + m_hspacing;
	}
}


/* XsdChoiceItem */

XsdChoiceItem::XsdChoiceItem( QGraphicsItem * parent ) : XsdSequenceItem( parent ) {
}

void XsdChoiceItem::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * w ) {
	QGraphicsPolygonItem::paint( painter, option, w );
	painter->drawText( boundingRect(), "chx", QTextOption( Qt::AlignCenter ) );
}


/* XsdComplexeType */

XsdComplexeType::XsdComplexeType( const QString & name ) : QGraphicsItem() {

}

/* XsdEditor */

XsdEditor::XsdEditor( QWidget * parent ) : AbstractFileEditor( parent ) {
//	m_scene = new QGraphicsScene(0, 0, 1024, 768, this);
	m_scene = new QGraphicsScene(this);
	m_view  = new QGraphicsView(this);

	m_view->setScene( m_scene );
	//m_view->setDragMode( QGraphicsView::RubberBandDrag );
	//m_view->setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing );

	XsdSequenceItem * seq = new XsdSequenceItem();
	m_scene->addItem( seq );
//	seq->setPos( 500, 500 );

	XsdElementItem * item = new XsdElementItem( "numero", "xs:string", 1, 1, seq );
	seq->addItem( item );

	new XsdAnnotationItem( "Numéro de version", item );

	item = new XsdElementItem( "edition_special", "xs:string", 1, 1, seq );
	seq->addItem( item );

	new XsdAnnotationItem( "Numéro de l'édition spécial", item );

	item = new XsdElementItem( "level", "xs:string", 1, 1, seq );
	seq->addItem( item );

//	m_view->centerOn( 0, 0 );

	borderLayout()->addWidget( m_view, BorderLayout::Center );
}

XsdEditor::~XsdEditor() {
}

QString XsdEditor::defaultFileName() const {
	return tr("noname.xsd");
}

QIcon XsdEditor::icon() const {
	return QIcon(":/xsd/images/typexsd.png");
}

bool XsdEditor::canCopy() {
	return false;
}

bool XsdEditor::canPaste() {
	return false;
}

bool XsdEditor::canUndo() {
	return false;
}

bool XsdEditor::canRedo() {
	return false;
}

void XsdEditor::loadFromDevice( QIODevice & d ) {
}

void XsdEditor::saveToDevice( QIODevice & d ) {
}

QAbstractItemModel * XsdEditor::model() const {
	return 0;
}

void XsdEditor::initSearch( SearchOptions & options ) {

}

bool XsdEditor::find( const QString & text, SearchOptions options ) {

}

void XsdEditor::replace( const QString & from, const QString & to, SearchOptions options ) {

}

void XsdEditor::updateModel() {
}

QString XsdEditor::bookmarkAt( int i ) {
	return QString();
}

int XsdEditor::bookmarkCount() {
	return 0;
}

void XsdEditor::undo() {
}

void XsdEditor::redo() {
}

void XsdEditor::cut() {
}

void XsdEditor::copy() {
}

void XsdEditor::paste() {
}

void XsdEditor::toogledBookmark() {
}

void XsdEditor::clearAllBookmark() {
}
