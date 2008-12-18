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

// Qt header
#include <QTextDocument>

/* XsdGraphicsAnnotationItem */

XsdGraphicsAnnotationItem::XsdGraphicsAnnotationItem( const QString & text, QGraphicsItem * parent ) : QGraphicsTextItem( text, parent ) {
	updatePosition();
}

void XsdGraphicsAnnotationItem::updatePosition() {
	setPos( parentItem()->boundingRect().left(), parentItem()->boundingRect().bottom() );
	setTextWidth( qMin( document()->size().width(), qMax( parentItem()->boundingRect().width(), 150.0 ) ) );
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
		foreach( XsdGraphicsLine * l, m_line ) {
			l->updatePosition();
		}
	}

	return value;
}

/* XsdGraphicsLine */

XsdGraphicsLine::XsdGraphicsLine( XsdNodeItem * startItem, XsdNodeItem * endItem ) : QGraphicsPathItem( dynamic_cast<QGraphicsItem*>( startItem ) ), m_startItem( startItem ), m_endItem( endItem ) {
	m_startItem->m_line.append( this );
	m_endItem->m_line.append( this );
	setZValue( -999 );

	updatePosition();
}

XsdGraphicsLine::~XsdGraphicsLine() {
	m_startItem->m_line.removeOne( this ) ;
	m_endItem->m_line.removeOne( this );
}

QRectF XsdGraphicsLine::boundingRect() const {
	return m_path.boundingRect();
}

void XsdGraphicsLine::updatePosition() {
	m_path = QPainterPath();
	QPointF start = mapFromItem( dynamic_cast<QGraphicsItem*>( m_startItem ), dynamic_cast<QGraphicsItem*>( m_startItem )->boundingRect().right(), 0 );
	QPointF end   = mapFromItem( dynamic_cast<QGraphicsItem*>( m_endItem ), 0, 0 );
	m_path.moveTo( start );
	m_path.lineTo( 30, start.y() );
	m_path.lineTo( 30, end.y() );
	m_path.lineTo( end );

	setPath( m_path );
}

/* XsdElementItem */

XsdGraphicsElementItem::XsdGraphicsElementItem( const QString & name, const QString & type, int minOccurs, int maxOccurs, QGraphicsItem * parent ) : QGraphicsItem( parent ), m_name( name ), m_type( type ), m_minOccurs( minOccurs ), m_maxOccurs( maxOccurs ), m_spacing( 5 ) {
	new XsdGraphicsLine( dynamic_cast<XsdNodeItem*>( parent ), this );
}

QList<XsdAttributeItem> & XsdGraphicsElementItem::attributes() {
	return m_attributes;
}

QRectF XsdGraphicsElementItem::boundingRect () const {
	QFontMetrics metrics( m_font );
	QRectF r = metrics.boundingRect( m_name );
	r.adjust( -m_spacing, -m_spacing, m_spacing, m_spacing );
	r.moveCenter( QPointF( 0, 0 ) );
	return r;
}

void XsdGraphicsElementItem::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * ) {
	QPen borderLine( Qt::black );
	if( m_minOccurs == 0 )
		borderLine.setStyle( Qt::DashLine );
	painter->setPen( borderLine );
	painter->setBrush( Qt::white );
	painter->drawRect( boundingRect() );

	painter->drawText( boundingRect(), m_name, QTextOption( Qt::AlignCenter ) );
}

/* XsdGraphicsSequenceItem */

XsdGraphicsSequenceItem::XsdGraphicsSequenceItem( QGraphicsItem * parent ) : QGraphicsPolygonItem( parent ), m_vspacing( 10 ), m_hspacing( 30 ) {
//	setFlag( QGraphicsItem::ItemIsMovable, true );
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

void XsdGraphicsSequenceItem::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * w ) {
	QGraphicsPolygonItem::paint( painter, option, w );
	painter->drawText( boundingRect(), "seq", QTextOption( Qt::AlignCenter ) );
}

void XsdGraphicsSequenceItem::addItem( QGraphicsItem * item ) {
	item->setParentItem( this );
	updatePosition();
}

void XsdGraphicsSequenceItem::updatePosition() {
	qreal height = 0;
	qreal itemPosX = 0;
	for( int i = 0 ; i < childItems().count(); i++ ) {
		QGraphicsItem * item = childItems().at( i );

		if( dynamic_cast<XsdGraphicsLine*>( item ) ) continue;

		qreal itemHeight = ( item->boundingRect() | item->childrenBoundingRect() ).height();
		height += itemHeight;
		if( i < childItems().count() / 2 ) {
			itemPosX -= itemHeight - m_vspacing;
		}

		if( item != childItems().last() ) height += m_vspacing;
	}

	foreach( QGraphicsItem * i, childItems() ) {
		if( dynamic_cast<XsdGraphicsLine*>( i ) ) continue;

		i->setPos( boundingRect().right() + m_hspacing - i->boundingRect().left(), itemPosX - i->boundingRect().top() );

		itemPosX += ( i->boundingRect() | i->childrenBoundingRect() ).height();
		if( i != childItems().last() ) itemPosX += m_vspacing;
	}
}


/* XsdGraphicsChoiceItem */

XsdGraphicsChoiceItem::XsdGraphicsChoiceItem( QGraphicsItem * parent ) : XsdGraphicsSequenceItem( parent ) {
}

void XsdGraphicsChoiceItem::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * w ) {
	QGraphicsPolygonItem::paint( painter, option, w );
	painter->drawText( boundingRect(), "chx", QTextOption( Qt::AlignCenter ) );
}


/* XsdComplexeType */

XsdGraphicsComplexeType::XsdGraphicsComplexeType( const QString & name ) : QGraphicsRectItem(), m_name( name ), m_spacing( 5 ) {
//	setFlag( QGraphicsItem::ItemIsMovable, true );
	QPen borderPen = QPen( Qt::black );
	borderPen.setStyle( Qt::DashLine );
	setPen( borderPen );
	setBrush( QColor( 255, 252, 158 ) );
	updatePosition();
}

const QString & XsdGraphicsComplexeType::name() const {
	return m_name;
}

void XsdGraphicsComplexeType::addItem( QGraphicsItem * item ) {
	item->setParentItem( this );
	updatePosition();
}

void XsdGraphicsComplexeType::updatePosition() {
	QRectF textRect = QFontMetrics( QFont() ).boundingRect( m_name );
	qreal width = textRect.width(), height = textRect.height();

	foreach( QGraphicsItem * i, childItems() ) {
		QRectF r = i->boundingRect() | i->childrenBoundingRect();
		if( width < r.width() ) width = r.width();
		height += r.height();
	}

	qreal top = - height / 2 + textRect.height();

	foreach( QGraphicsItem * i, childItems() ) {
		QRectF r = i->boundingRect() | i->childrenBoundingRect();

		qreal left = - width / 2;
		i->setPos( left - r.left(), top - r.top() );

		top += i->boundingRect().height();
	}

	QRectF rect( - width / 2, - height / 2, width, height );
	rect.adjust( - m_spacing, - m_spacing, m_spacing, m_spacing );
	setRect( rect );
}

void XsdGraphicsComplexeType::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * w ) {
	QGraphicsRectItem::paint( painter, option, w );
	QRectF textRect = boundingRect();
	textRect.adjust( m_spacing, m_spacing, - m_spacing, - m_spacing );
	painter->drawText( textRect, m_name, QTextOption( Qt::AlignLeft | Qt::AlignTop ) );
}


/* XsdEditor */

XsdEditor::XsdEditor( QWidget * parent ) : AbstractFileEditor( parent ) {
//	m_scene = new QGraphicsScene(0, 0, 1024, 768, this);
	m_scene = new QGraphicsScene(this);
	m_view  = new QGraphicsView(this);

	m_view->setScene( m_scene );
	//m_view->setDragMode( QGraphicsView::RubberBandDrag );
	//m_view->setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing );

	XsdGraphicsComplexeType * complex = new XsdGraphicsComplexeType( "VersionInfo" );
	m_scene->addItem( complex );
	complex->setPos( 500, 500 );

	XsdGraphicsSequenceItem * seq = new XsdGraphicsSequenceItem();
//	m_scene->addItem( seq );
	complex->addItem( seq );

	XsdGraphicsElementItem * item = new XsdGraphicsElementItem( "numero", "xs:string", 1, 1, seq );
	seq->addItem( item );

	new XsdGraphicsAnnotationItem( "Numéro de version", item );

	item = new XsdGraphicsElementItem( "edition_special", "xs:string", 0, 1, seq );
	seq->addItem( item );

	new XsdGraphicsAnnotationItem( "Numéro de l'édition spécial", item );

	item = new XsdGraphicsElementItem( "level", "xs:string", 1, 1, seq );
	seq->addItem( item );
	complex->updatePosition();
	new XsdGraphicsAnnotationItem( "Gestion de la version de eGX. Les valeurs des élements sont mis à jour par l'outil de migration. Toute modification par l'utilisateur entrainera des disfonctionnements de cet outil.", complex );


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
