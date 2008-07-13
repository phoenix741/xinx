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

// QT header


XUIEditor::XUIEditor( QWidget * parent ) : AbstractFileEditor( parent ) {
	m_scene = new QGraphicsScene(this);
	m_view  = new QGraphicsView(this);

	m_view->setScene( m_scene );
	//m_view->setDragMode( QGraphicsView::RubberBandDrag );
	m_view->setRenderHints( QPainter::Antialiasing | QPainter::TextAntialiasing );

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
