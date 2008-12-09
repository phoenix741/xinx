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

/* XsdAttributeItem */

class XsdAttributeItem : public QGraphicsTextItem {
public:
	XsdAttributeItem( const QString & name, const QString & value, const QString & use );
};

XsdAttributeItem::XsdAttributeItem( const QString & name, const QString & value, const QString & use ) : QGraphicsTextItem() {

}

/* XsdElementItem */

class XsdElementItem : public QGraphicsTextItem {
public:
	XsdElementItem( const QString & name, const QString & type, int minOccurs, int maxOccurs );
};

XsdElementItem::XsdElementItem( const QString & name, const QString & type, int minOccurs, int maxOccurs ) : QGraphicsTextItem() {

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

/* XsdTabTextItem */

class XsdTabTextItem : public QGraphicsTextItem {
public:
	XsdTabTextItem( const QString & text, XsdTabTextItem * left = 0 );
protected:
	virtual QVariant itemChange( GraphicsItemChange change, const QVariant & value );
private:
	void setLeft( XsdTabTextItem * left );
	XsdTabTextItem * m_left, * m_right;
};

XsdTabTextItem::XsdTabTextItem( const QString & text, XsdTabTextItem * left ) : QGraphicsTextItem(), m_left( 0 ), m_right( 0 ) {
	setHtml( "<table style=\"	padding: 0px 0px 0px 0px;	margin-bottom: 0px;	margin-left: 0px;	margin-right: 0px;	margin-top: 0px;\">"
			 "<tr>"
			 "<td style=\"color : #000000; font-style: normal; text-align: center; font-family: verdana,arial,sans-serif; font-size: 7pt; background-color : #fcfad5; margin: 2px; padding: 2px; border-width:1px; border-color:#54545d; border-style: none solid none none; white-space: nowrap;\">"
			 "<a href=\"#\" style=\"  color: #000000;  text-decoration: none;  font-weight: bold;  font-size: 7pt;\">" +
			 text +
			 "</a>"
			 "</td>"
			 "</tr>"
			 "</table>" );
	setLeft( left );
	setFlags( QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable );
}

void XsdTabTextItem::setLeft( XsdTabTextItem * left ) {
	if( m_left != left ) {
		setPos( left->pos().x() + left->boundingRect().width(), left->pos().y() );
		m_left = left;
		m_left->m_right = this;
	} else {
		setPos( 0, 0 );
	}
}

QVariant XsdTabTextItem::itemChange( GraphicsItemChange change, const QVariant & value ) {
	if( change == QGraphicsItem::ItemPositionChange ) {
		QPointF position = QGraphicsTextItem::itemChange( change, value ).toPointF();
		position.setY( pos().y() );
		return position;
	} else
		return QGraphicsTextItem::itemChange( change, value );
}

/* XsdFormItem */

class XsdFormItem : public QGraphicsItem {
public:
	XsdFormItem();
	virtual ~XsdFormItem();

	virtual QRectF boundingRect() const;
	virtual void paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * = 0 );
};

XsdFormItem::XsdFormItem() : QGraphicsItem( 0 ) {

}

XsdFormItem::~XsdFormItem() {

}

QRectF XsdFormItem::boundingRect() const {
	return QRectF( 0, 0, 1000, 400 );
}

void XsdFormItem::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * ) {
	painter->setBrush( QColor(0xD2, 0xDB, 0xE9) );
	painter->setPen( QColor(0x7e, 0x7e, 0x89) );
	painter->drawRect( boundingRect() );
}

/* XsdEditor */

XsdEditor::XsdEditor( QWidget * parent ) : AbstractFileEditor( parent ) {
	m_scene = new QGraphicsScene(0, 0, 1024, 768, this);
	m_view  = new QGraphicsView(this);

	m_view->setScene( m_scene );
	//m_view->setDragMode( QGraphicsView::RubberBandDrag );
	//m_view->setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing );

	XsdTabTextItem * item = 0;
	m_scene->addItem( item = new XsdTabTextItem( "popup1", item ) );
	m_scene->addItem( item = new XsdTabTextItem( "popup2", item ) );
	m_scene->addItem( item = new XsdTabTextItem( "popup3", item ) );
	m_scene->addItem( item = new XsdTabTextItem( "popup4", item ) );
	m_scene->addItem( item = new XsdTabTextItem( "popup5", item ) );

	XsdFormItem * form = new XsdFormItem();
	form->setPos( 0, item->boundingRect().height() );
	m_scene->addItem( form );

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
