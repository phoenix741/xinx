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
#include "xsdeditor.h"
#include "borderlayout.h"

// Qt header
#include <QGraphicsTextItem>

/* XsdAnnotationItem */

class XsdAnnotationItem : public QGraphicsTextItem {
public:
	XsdAnnotationItem( const QString & text, QGraphicsItem * parent = 0 );
};

XsdAnnotationItem::XsdAnnotationItem( const QString & text, QGraphicsItem * parent ) : QGraphicsTextItem( text, parent ) {

}

/* XsdAttributeItem */

struct XsdAttributeItem {
	XsdAttributeItem( const QString & name, const QString & value, const QString & use );

	QString m_name, m_value, m_use;
};

XsdAttributeItem::XsdAttributeItem( const QString & name, const QString & value, const QString & use ) : m_name( name ), m_value( value ), m_use( use ) {

}

/* XsdElementItem */

class XsdElementItem : public QGraphicsItem {
public:
	XsdElementItem( const QString & name, const QString & type, int minOccurs = 1, int maxOccurs = 1, QGraphicsItem * parent = 0 );

	QRectF boundingRect () const;
	QRectF elementRect() const;
	virtual void paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * = 0 );

	const QFont & font() const;
	void setFont( const QFont & font );
private:

	QFont m_font;
	QString m_name, m_type;
	int m_minOccurs, m_maxOccurs, m_spacing;
};

XsdElementItem::XsdElementItem( const QString & name, const QString & type, int minOccurs, int maxOccurs, QGraphicsItem * parent ) : QGraphicsItem( parent ), m_name( name ), m_type( type ), m_minOccurs( minOccurs ), m_maxOccurs( maxOccurs ), m_spacing( 5 ) {

}

QRectF XsdElementItem::elementRect () const {
	QFontMetrics metrics( m_font );
	QRect r = metrics.boundingRect( m_name );
	r.moveTo( 0, 0 );
	r.adjust( 0, 0, m_spacing * 2, m_spacing * 2 );
	return r;
}

QRectF XsdElementItem::boundingRect () const {
	QRectF r = elementRect();
	r.setHeight( r.height() + childrenBoundingRect().height() );
	if( r.width() < childrenBoundingRect().width() )
		r.setWidth( childrenBoundingRect().width() );
	return r;
}

void XsdElementItem::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * ) {
	painter->setBrush( Qt::white );
	painter->setPen( Qt::black );
	painter->drawRect( elementRect() );

	painter->drawText( elementRect(), m_name, QTextOption( Qt::AlignCenter ) );
}

/* XsdSequenceItem */

class XsdSequenceItem : public QGraphicsTextItem {
public:
	XsdSequenceItem();
};

XsdSequenceItem::XsdSequenceItem() : QGraphicsTextItem() {

}

/* XsdComplexeType */

class XsdComplexeType : public QGraphicsTextItem {
public:
	XsdComplexeType( const QString & name );
};

XsdComplexeType::XsdComplexeType( const QString & name ) : QGraphicsTextItem() {

}

/* XsdEditor */

XsdEditor::XsdEditor( QWidget * parent ) : AbstractFileEditor( parent ) {
	m_scene = new QGraphicsScene(0, 0, 1024, 768, this);
	m_view  = new QGraphicsView(this);

	m_view->setScene( m_scene );
	//m_view->setDragMode( QGraphicsView::RubberBandDrag );
	//m_view->setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing );

	XsdElementItem * item = new XsdElementItem( "numero", "xs:string" );
	m_scene->addItem( item );
	item->setPos( 15, 15 );

	XsdAnnotationItem * annotation = new XsdAnnotationItem( "Ceci est un petit text explicatif sur le foncitonnement de ...", item );
	annotation->setPos( 0, item->elementRect().height() );
//	m_scene->addItem( annotation );

	item = new XsdElementItem( "edition_special", "xs:string" );
	m_scene->addItem( item );
	item->setPos( 15, 115 );
	item = new XsdElementItem( "level", "xs:string" );
	m_scene->addItem( item );
	item->setPos( 15, 215 );

	m_view->centerOn( m_view->width() / 2, m_view->height() / 2 );

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
