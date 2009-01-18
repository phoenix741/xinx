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
#include "xuieditor.h"
#include "borderlayout.h"

// Qt header
#include <QGraphicsTextItem>

/* XuiTabTextItem */

class XuiTabTextItem : public QGraphicsTextItem {
public:
	XuiTabTextItem( const QString & text, XuiTabTextItem * left = 0 );
protected:
	virtual QVariant itemChange( GraphicsItemChange change, const QVariant & value );
private:
	void setLeft( XuiTabTextItem * left );
	XuiTabTextItem * m_left, * m_right;
};

XuiTabTextItem::XuiTabTextItem( const QString & text, XuiTabTextItem * left ) : QGraphicsTextItem(), m_left( 0 ), m_right( 0 ) {
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

void XuiTabTextItem::setLeft( XuiTabTextItem * left ) {
	if( m_left != left ) {
		setPos( left->pos().x() + left->boundingRect().width(), left->pos().y() );
		m_left = left;
		m_left->m_right = this;
	} else {
		setPos( 0, 0 );
	}
}

QVariant XuiTabTextItem::itemChange( GraphicsItemChange change, const QVariant & value ) {
	if( change == QGraphicsItem::ItemPositionChange ) {
		QPointF position = QGraphicsTextItem::itemChange( change, value ).toPointF();
		position.setY( pos().y() );
		return position;
	} else
		return QGraphicsTextItem::itemChange( change, value );
}

/* XuiFormItem */

class XuiFormItem : public QGraphicsItem {
public:
	XuiFormItem();
	virtual ~XuiFormItem();

	virtual QRectF boundingRect() const;
	virtual void paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * = 0 );
};

XuiFormItem::XuiFormItem() : QGraphicsItem( 0 ) {

}

XuiFormItem::~XuiFormItem() {

}

QRectF XuiFormItem::boundingRect() const {
	return QRectF( 0, 0, 1000, 400 );
}

void XuiFormItem::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * ) {
	painter->setBrush( QColor(0xD2, 0xDB, 0xE9) );
	painter->setPen( QColor(0x7e, 0x7e, 0x89) );
	painter->drawRect( boundingRect() );
}

/* XUIEditor */

XUIEditor::XUIEditor( QWidget * parent ) : AbstractFileEditor( parent ) {
	m_scene = new QGraphicsScene(0, 0, 1024, 768, this);
	m_view  = new QGraphicsView(this);

	m_view->setScene( m_scene );
	//m_view->setDragMode( QGraphicsView::RubberBandDrag );
	//m_view->setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing );

	XuiTabTextItem * item = 0;
	m_scene->addItem( item = new XuiTabTextItem( "popup1", item ) );
	m_scene->addItem( item = new XuiTabTextItem( "popup2", item ) );
	m_scene->addItem( item = new XuiTabTextItem( "popup3", item ) );
	m_scene->addItem( item = new XuiTabTextItem( "popup4", item ) );
	m_scene->addItem( item = new XuiTabTextItem( "popup5", item ) );

	XuiFormItem * form = new XuiFormItem();
	form->setPos( 0, item->boundingRect().height() );
	m_scene->addItem( form );

	m_view->centerOn( m_view->width() / 2, m_view->height() / 2 );

	borderLayout()->addWidget( m_view, BorderLayout::Center );
}

XUIEditor::~XUIEditor() {
}

QString XUIEditor::defaultFileName() const {
	return tr("noname.xui");
}

QIcon XUIEditor::icon() const {
	return QIcon(":/xslgui/images/typexui.png");
}

bool XUIEditor::canCopy() {
	return false;
}

bool XUIEditor::canPaste() {
	return false;
}

bool XUIEditor::canUndo() {
	return false;
}

bool XUIEditor::canRedo() {
	return false;
}

void XUIEditor::loadFromDevice( QIODevice & d ) {
}

void XUIEditor::saveToDevice( QIODevice & d ) {
}

QAbstractItemModel * XUIEditor::model() const {
	return NULL;
}

void XUIEditor::initSearch( SearchOptions & options ) {

}

bool XUIEditor::find( const QString & text, SearchOptions options ) {

}

void XUIEditor::replace( const QString & from, const QString & to, SearchOptions options ) {

}

void XUIEditor::updateModel() {
}

QString XUIEditor::bookmarkAt( int i ) {
	return QString();
}

int XUIEditor::bookmarkCount() {
	return 0;
}

void XUIEditor::undo() {
}

void XUIEditor::redo() {
}

void XUIEditor::cut() {
}

void XUIEditor::copy() {
}

void XUIEditor::paste() {
}

void XUIEditor::toogledBookmark() {
}

void XUIEditor::clearAllBookmark() {
}

bool XUIEditor::previousBookmark() {
	return true;
}

bool XUIEditor::nextBookmark() {
	return false;
}

void XUIEditor::gotoBookmarkAt( int ) {

}
