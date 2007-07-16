/***************************************************************************
 *   Copyright (C) 2006 by Ulrich Van Den Hekke                            *
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

#include "jseditor.h"
#include "jsfileeditor.h"
#include "javascriptparser.h"

#include <QAbstractItemModel>
#include <QApplication>
#include <QMessageBox>

/* JSItemModel */

class JSItemModel : public QAbstractItemModel {
	Q_OBJECT
public:
	JSItemModel( QObject *parent = 0 );
	JSItemModel( JavaScriptParser * data, QObject *parent = 0 );
	virtual ~JSItemModel();
	
	struct user_data {
		int line;
		QString filename;
	};
	
	QVariant data(const QModelIndex &index, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;	
	
	JavaScriptParser* modelData() { return rootItem; };
protected slots:
	void slotReset() { reset(); };
private:
	JavaScriptParser* rootItem;
};

JSItemModel::JSItemModel( QObject * parent ) : QAbstractItemModel( parent ) {
	rootItem = NULL;
}

JSItemModel::JSItemModel( JavaScriptParser * data, QObject * parent ) : QAbstractItemModel( parent ) {
	rootItem = data;
}

JSItemModel::~JSItemModel() {
	delete rootItem;
}

QVariant JSItemModel::data(const QModelIndex &index, int role) const {
	
}

Qt::ItemFlags JSItemModel::flags(const QModelIndex &index) const {
	
}

QVariant JSItemModel::headerData(int section, Qt::Orientation orientation, int role) const {
	
}

QModelIndex JSItemModel::index(int row, int column, const QModelIndex &parent) const {/*
	if( dynamic_cast<JavaScriptFunction*>( parent.internalPointer() ) == 0 ) {
		JavaScriptFunction * function = dynamic_cast<JavaScriptFunction*>( parent.internalPointer() );
		
	} else
	;*/
}

QModelIndex JSItemModel::parent(const QModelIndex &index) const {
	
}

int JSItemModel::rowCount(const QModelIndex &parent) const {
	
}

int JSItemModel::columnCount(const QModelIndex &parent) const {
	
}

/* PrivateJSFileEditor */

class PrivateJSFileEditor {
public:
	PrivateJSFileEditor( JSFileEditor * parent );
	virtual ~PrivateJSFileEditor();
	
	JavaScriptParser * m_parser;
private:
	JSFileEditor * m_parent;
};

PrivateJSFileEditor::PrivateJSFileEditor( JSFileEditor * parent ) {
	m_parent = parent;
	m_parser = new JavaScriptParser();
}

PrivateJSFileEditor::~PrivateJSFileEditor() {
	delete m_parser;
}

/* JSFileEditor */

JSFileEditor::JSFileEditor( QWidget *parent ) : FileEditor( new JSEditor( parent ), parent ) {
	d = new PrivateJSFileEditor( this );
}

JSFileEditor::~JSFileEditor() {
	delete d;
}

QString JSFileEditor::getSuffix() const {
	if( getFileName().isEmpty() ) 
		return "js";
	else
		return FileEditor::getSuffix();
}

QAbstractItemModel * JSFileEditor::model() {
	try {
		d->m_parser->load( textEdit()->toPlainText(), getFileName() );
	} catch( JavaScriptParserException e ) {
		QMessageBox::warning( qApp->activeWindow(), tr("JavaScript error"), tr("Error JS at line %1").arg( e.m_line ) );
	}
	
	return NULL;
}

#include "jsfileeditor.moc"
