/***************************************************************************
 *   Copyright (C) 2007 by Ulrich Van Den Hekke                            *
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

#include <QIcon>
#include <QTextEdit>
#include <QStringListModel>
#include <QAbstractItemView>	
#include <QScrollBar>
#include <QKeyEvent>
#include <QTextBlock>
#include <QCompleter>
#include <QDomElement>
#include <assert.h>

#include "globals.h"
#include "xsleditor.h"
#include "editorcompletion.h"
#include "xsllistview.h"

/* XSLEditor */

#define EOW			"~!@$#%^&*()+{}|\"<>?,/;'[]\\="

#define isEditBalise(value) ((value == XSLEditor::cpEditNodeName) || (value == XSLEditor::cpEditParamName) || (value == XSLEditor::cpEditParamValue))

/* XSLValueCompletionModel */

class XSLValueCompletionModel : public QAbstractListModel {
	Q_OBJECT
public:
	XSLValueCompletionModel( XSLModelData * data, QObject *parent = 0 );
	virtual ~XSLValueCompletionModel();
	
	void setBaliseName( const QString & name, const QString & attribute );
	
	QVariant data(const QModelIndex &index, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
public slots:
	void refreshList();
private:
	bool contains( XSLModelData * data );
	void refreshRecursive(XSLModelData * data);

	QList<XSLModelData*> m_objList;
	XSLModelData* rootItem;
	
	QString m_baliseName;
	QString m_attributeName;
};

XSLValueCompletionModel::XSLValueCompletionModel( XSLModelData * data, QObject *parent ) : QAbstractListModel( parent ) {
	rootItem = data;
	refreshList();
	
	connect( rootItem, SIGNAL( childAboutToBeReset() ), this, SIGNAL( modelAboutToBeReset() ) );
	connect( rootItem, SIGNAL( childReseted() ), this, SLOT( refreshList() ) );
}

XSLValueCompletionModel::~XSLValueCompletionModel() {
	disconnect( rootItem, SIGNAL( childAboutToBeReset() ), this, SIGNAL( modelAboutToBeReset() ) );
	disconnect( rootItem, SIGNAL( childReseted() ), this, SLOT( refreshList() ) );
}

bool XSLValueCompletionModel::contains( XSLModelData * data ) {
	for( int i = 0; i < m_objList.count(); i++ ) {
		if( *(m_objList.at( i )) == *data ) 
			return true;
	}
	return false;
}

void XSLValueCompletionModel::refreshRecursive(XSLModelData * data) {
	for( int i = 0; i < data->childCount(); i++ ) {
		if( data->child( i )->type() != XSLModelData::etImport ) {
			if( ! contains( data->child( i ) ) )
			m_objList.append( data->child( i ) );
		} else {
			refreshRecursive( data->child( i ) );
		}
	}
}

bool XSLValueCompletionModelObjListSort( XSLModelData * d1, XSLModelData * d2 ) {
	return d1->name() < d2->name();
}

void XSLValueCompletionModel::refreshList() {
	m_objList.clear();
	refreshRecursive( rootItem );
	qSort( m_objList.begin(), m_objList.end(), XSLValueCompletionModelObjListSort );
	reset();
}

void XSLValueCompletionModel::setBaliseName( const QString & name, const QString & attribute ) { 
	int before = m_objList.count(), after  = m_objList.count();
	
	if( global.m_completionContents && global.m_completionContents->balise( m_baliseName ) && global.m_completionContents->balise( m_baliseName )->attribute( m_attributeName ) )
		before += global.m_completionContents->balise( m_baliseName )->attribute( m_attributeName )->values().count();
		
	if( global.m_completionContents && global.m_completionContents->balise( name ) && global.m_completionContents->balise( name )->attribute( attribute ) )
		after += global.m_completionContents->balise( name )->attribute( attribute )->values().count();
		
	int diff = after - before;

	if( diff > 0 ) 
		beginInsertRows( QModelIndex(), before + 1, after );
	else if( diff < 0 )
		beginRemoveRows( QModelIndex(), after + 1, before );

	m_baliseName = name; 
	m_attributeName = attribute; 

	if( diff > 0 ) {
		emit dataChanged( index( m_objList.count() ), index( before ) );
		endInsertRows();
	} else  if( diff < 0 ) {
		emit dataChanged( index( m_objList.count() ), index( after ) );
		endRemoveRows();
	}
}

	
QVariant XSLValueCompletionModel::data( const QModelIndex &index, int role ) const {
	if (!index.isValid()) return QVariant();

	if( index.row() < m_objList.count() ) {
		XSLModelData * data = m_objList[ index.row() ];
	
		if( role == Qt::DecorationRole ) {
			switch( data->type() ) {
			case XSLModelData::etVariable:
				return QIcon(":/images/variable.png");
				break;
			case XSLModelData::etTemplate:
				return QIcon(":/images/template.png");
				break;
			default:
				return QVariant();
			}
		} 
	
		if ( ( role == Qt::DisplayRole ) && ( index.column() == 0 ) ) 
			return data->name();
		
		if( role == Qt::UserRole ) {
			return (int)data->type();
		}
	} else {
		int value_row = index.row() - m_objList.count(); 
		if( global.m_completionContents && global.m_completionContents->balise( m_baliseName ) && global.m_completionContents->balise( m_baliseName )->attribute( m_attributeName ) ) {
			if( role == Qt::DecorationRole ) 
				return QIcon(":/images/html_value.png");
			if ( ( role == Qt::DisplayRole ) && ( index.column() == 0 ) ) 
				return global.m_completionContents->balise( m_baliseName )->attribute( m_attributeName )->values().at( value_row );
			if( role == Qt::UserRole ) {
				return -1;
			}
		}
	}
	
	return QVariant();
}

Qt::ItemFlags XSLValueCompletionModel::flags(const QModelIndex &index) const {
	if ( index.isValid() )
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	return Qt::ItemIsEnabled;
}

int XSLValueCompletionModel::rowCount(const QModelIndex &parent) const {
	if ( ! parent.isValid() ) {
		int size = m_objList.count();	
		if( global.m_completionContents ) {
			if( global.m_completionContents->balise( m_baliseName ) ) {
				if( global.m_completionContents->balise( m_baliseName )->attribute( m_attributeName ) )
					size += global.m_completionContents->balise( m_baliseName )->attribute( m_attributeName )->values().count();
			}
		}
		return size;
	} else
		return 0;
	}

/* XSLParamCompletionModel */

class XSLParamCompletionModel : public QAbstractListModel {
	Q_OBJECT
public:
	XSLParamCompletionModel( QObject *parent = 0 );
	virtual ~XSLParamCompletionModel();
	
	void setBaliseName( const QString & name );
	
	QVariant data(const QModelIndex &index, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
private:
	QString m_baliseName;
};

XSLParamCompletionModel::XSLParamCompletionModel( QObject *parent ) : QAbstractListModel( parent ) {
}

XSLParamCompletionModel::~XSLParamCompletionModel() {
}

void XSLParamCompletionModel::setBaliseName( const QString & name ) { 
	emit layoutAboutToBeChanged();
	m_baliseName = name; 
	emit layoutChanged();
}

	
QVariant XSLParamCompletionModel::data( const QModelIndex &index, int role ) const {
	if (!index.isValid()) return QVariant();

	int value_row = index.row(); 
	if( global.m_completionContents && global.m_completionContents->balise( m_baliseName ) ) {
		if( role == Qt::DecorationRole ) 
			return QIcon(":/images/noeud.png");
		if ( ( role == Qt::DisplayRole ) && ( index.column() == 0 ) ) 
			return global.m_completionContents->balise( m_baliseName )->attributes().at( value_row )->name();
	}
	
	return QVariant();
}

Qt::ItemFlags XSLParamCompletionModel::flags(const QModelIndex &index) const {
	if ( index.isValid() )
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	return Qt::ItemIsEnabled;
}

int XSLParamCompletionModel::rowCount(const QModelIndex &parent) const {
	if ( ! parent.isValid() ) {
		int size = 0;	
		if( global.m_completionContents && global.m_completionContents->balise( m_baliseName ) ) 
			size += global.m_completionContents->balise( m_baliseName )->attributes().count();
		return size;
	} else
		return 0;
}

/* XSLBaliseCompletionModel */

class XSLBaliseCompletionModel : public QAbstractListModel {
	Q_OBJECT
public:
	XSLBaliseCompletionModel( QObject *parent = 0 );
	virtual ~XSLBaliseCompletionModel();
	
	QVariant data(const QModelIndex &index, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
};

XSLBaliseCompletionModel::XSLBaliseCompletionModel( QObject *parent ) : QAbstractListModel( parent ) {
}

XSLBaliseCompletionModel::~XSLBaliseCompletionModel() {
}

QVariant XSLBaliseCompletionModel::data( const QModelIndex &index, int role ) const {
	if (!index.isValid()) return QVariant();

	int value_row = index.row(); 
	if( global.m_completionContents ) {
		if( role == Qt::DecorationRole ) 
			return QIcon(":/images/balise.png");
		if ( ( role == Qt::DisplayRole ) && ( index.column() == 0 ) ) 
			return global.m_completionContents->xmlBalises().at( value_row )->name();
	}
	
	return QVariant();
}

Qt::ItemFlags XSLBaliseCompletionModel::flags(const QModelIndex &index) const {
	if ( index.isValid() )
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	return Qt::ItemIsEnabled;
}

int XSLBaliseCompletionModel::rowCount(const QModelIndex &parent) const {
	if ( ! parent.isValid() ) {
		int size = 0;	
		if( global.m_completionContents ) 
			size += global.m_completionContents->xmlBalises().count();
		return size;
	} else
		return 0;
}


/* XSLEditor */

XSLEditor::XSLEditor( QWidget * parent ) : XMLEditor( parent ) {
	// Completer
	m_completerValueParamName = "";
	m_completerParamNodeName  = "";
	m_completerValue 		  = 0;

	m_completerNode = new QCompleter( this );
	m_completerNode->setWidget( this );
	m_completerNode->setCompletionMode( QCompleter::PopupCompletion );
	m_completerNode->setCaseSensitivity( Qt::CaseInsensitive );
	m_completerNode->setCompletionRole( Qt::DisplayRole );
	connect( m_completerNode, SIGNAL(activated(const QModelIndex&)), this, SLOT(insertCompletion(const QModelIndex&)) );
   
	m_completerParam = new QCompleter( this );
	m_completerParam->setWidget( this );
	m_completerParam->setCompletionMode( QCompleter::PopupCompletion );
	m_completerParam->setCaseSensitivity( Qt::CaseInsensitive );
	m_completerParam->setCompletionRole( Qt::DisplayRole );
	connect( m_completerParam, SIGNAL(activated(const QModelIndex&)), this, SLOT(insertCompletion(const QModelIndex&)) );

	m_completerValue = new QCompleter( this );
	m_completerValue->setWidget( this );
	m_completerValue->setCompletionMode( QCompleter::PopupCompletion );
	m_completerValue->setCaseSensitivity( Qt::CaseInsensitive );
	m_completerValue->setCompletionRole( Qt::DisplayRole );
	connect( m_completerValue, SIGNAL(activated(const QModelIndex &)), this, SLOT(insertCompletion(const QModelIndex &)) );
	
	m_modelData = new XSLModelData( NULL );
	m_modelData->loadFromContent( toPlainText() );
	
	m_contentModel = new XSLItemModel( m_modelData, this );

	m_completionValueModel = new XSLValueCompletionModel( m_modelData, this );
	m_completerValue->setModel( m_completionValueModel );

	m_completionParamModel = new XSLParamCompletionModel( this );
	m_completerParam->setModel( m_completionParamModel );

	m_completionBaliseModel = new XSLBaliseCompletionModel( this );
	m_completerNode->setModel( m_completionBaliseModel );
	
	connect( m_modelData, SIGNAL(hasError(QString)), this, SIGNAL(hasError(QString)) );
}

XSLEditor::~XSLEditor() {
	
}
	

void XSLEditor::insertCompletionValue( QTextCursor & tc, QString node, QString param ) {
	if( global.m_completionContents && global.m_completionContents->balise( node ) && global.m_completionContents->balise( node )->attribute( param ) && global.m_completionContents->balise( node )->attribute( param )->defaultValue() >= 0 ) {
		int defaultValue = global.m_completionContents->balise( node )->attribute( param )->defaultValue();
		tc.insertText( QString("=\"%1\"").arg( global.m_completionContents->balise( node )->attribute( param )->values().at( defaultValue ) ) );
	} else {
		tc.insertText("=\"\"");
		tc.movePosition( QTextCursor::PreviousCharacter );
	}
}

int XSLEditor::insertCompletionParam( QTextCursor & tc, QString node, bool movePosition ) {
	int position = -1;
	if( global.m_completionContents && global.m_completionContents->balise( node ) ) {
		CompletionXMLBalise* balise = global.m_completionContents->balise( node );
		foreach( CompletionXMLAttribute* attr, balise->attributes() ) {
			if( attr->isDefault() ) {
				int defaultValue = attr->defaultValue();
				QString defaultValueString;
				if( defaultValue >= 0 )
					defaultValueString = attr->values().at( defaultValue );
				tc.insertText( QString(" %1=\"%2\"").arg( attr->name() ).arg( defaultValueString ) );
				if( ( position == -1 ) && defaultValueString.isEmpty() ) {
					tc.movePosition( QTextCursor::PreviousCharacter );
					position = tc.position();
					tc.movePosition( QTextCursor::NextCharacter );
				}
			}
		}	
	}
	if( ( position != -1 ) && ( movePosition ) )
		tc.setPosition( position );
	return position;
}

int XSLEditor::insertCompletionBalises( QTextCursor & tc, QString node ) {
	QString indent = tc.block().text();
	indent = indent.left( indent.indexOf( QRegExp( "\\S" ) ) );

	int position = -1, cnt = 0;
	
	if( global.m_completionContents && global.m_completionContents->balise( node ) ) {
		CompletionXMLBalise* balise = global.m_completionContents->balise( node );
		foreach( CompletionXMLBalise* b, balise->balises() ) {
			if( b->isDefault() ) {
				tc.insertText( "\n" + indent + "\t" );
				tc.insertText( "<" + b->name() );
				int insertPosition = insertCompletionParam( tc, b->name(), false );
				if( position == -1 ) position = insertPosition;
				tc.insertText( ">" );
				insertCompletionBalises( tc, b->name() );
				cnt++;
			}
		}
	}
	
	if( cnt > 0 )
		tc.insertText( "\n" + indent );
	tc.insertText( QString("</%1>").arg( node ) );
	if( position != -1 )
		tc.setPosition( position );
		
	return position;
} 

void XSLEditor::insertCompletionAccolade( QTextCursor & tc, QString node, QString param, QString value, int type ) {
	Q_UNUSED( param );	
	
	QTextCursor tc2( tc );
	tc2.movePosition( QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor, value.length() );
	
	bool insertDollard = true, insertAccolade = true;
	QTextCursor tc3( tc2 );
	tc3.movePosition( QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor );
	if( tc3.selectedText() == "$" ) {
		tc2.movePosition( QTextCursor::PreviousCharacter );
		insertDollard = false;
	}

	QTextCursor accOpen, accClose, debParam;
	accOpen = document()->find( "{", tc2, QTextDocument::FindBackward );
	accClose = document()->find( "}", tc2, QTextDocument::FindBackward );
	debParam = document()->find( "\"", tc2, QTextDocument::FindBackward );
	if( ( accOpen > accClose ) && ( accOpen > debParam ) ) insertAccolade = false;

	if( global.m_completionContents && global.m_completionContents->balise( node ) ) {
		CompletionXMLBalise* balise = global.m_completionContents->balise( node );
		if( insertAccolade && (balise->category() != "stylesheet") && ( ( type == (int)XSLModelData::etVariable ) || ( type == (int)XSLModelData::etTemplate ) )) {
//			QMessageBox::warning(qApp->activeWindow(), "", "");
			if( insertDollard && ( type == (int)XSLModelData::etVariable ) ) 
				tc2.insertText( "{$" );
			else
				tc2.insertText( "{" );
			tc.insertText( "}" );
		} else
		if( insertDollard && ( type == (int)XSLModelData::etVariable ) ) {
			tc2.insertText( "$" );
		}
	}
}

void XSLEditor::insertCompletion( const QModelIndex& index ) {
	QTextCursor tc = textCursor();
	QCompleter * c = currentCompleter( tc );
	QString completion = c->completionModel()->data( index ).toString();
	
	for( int i = 0; i < c->completionPrefix().length(); i++ ) tc.deletePreviousChar();
	tc.insertText( completion );

	XSLEditor::cursorPosition pos = editPosition(tc);
	if( pos == XSLEditor::cpEditParamName ) {
		insertCompletionValue( tc, m_nodeName, completion );
	} else
	if( pos == XSLEditor::cpEditNodeName ) {
		if( ! m_nodeName.isEmpty() )
			insertCompletionParam( tc, completion );
		else
			tc.insertText( ">" );
	} else
	if( pos == XSLEditor::cpEditParamValue ) {
		int type = c->completionModel()->data( index, Qt::UserRole ).toInt();
		insertCompletionAccolade( tc, m_nodeName, m_paramName, completion, type );
	}

	setTextCursor( tc );
}

QCompleter * XSLEditor::currentCompleter( const QTextCursor & cursor ) {
	assert( ! cursor.isNull() );

	XSLEditor::cursorPosition position = editPosition( cursor );
	switch( position ) {
		case XSLEditor::cpEditNodeName:
			return m_completerNode;
		case XSLEditor::cpEditParamName: 
			if(m_nodeName != m_completerParamNodeName) {
				m_completionParamModel->setBaliseName( m_nodeName );
				m_completerParamNodeName = m_nodeName;
				return m_completerParam;
			}
		
			return m_completerParam;
		case XSLEditor::cpEditParamValue: 
			if( ( m_nodeName != m_completerParamNodeName ) || ( m_paramName != m_completerValueParamName ) ) {
				m_completionParamModel->setBaliseName( m_nodeName );
				m_completionValueModel->setBaliseName( m_nodeName, m_paramName );
				m_completerParamNodeName = m_nodeName;
				m_completerValueParamName = m_paramName;
			}
			return m_completerValue;
		default:
			return NULL;	
	}
}

void XSLEditor::complete() {
	QTextCursor cursor = textCursor();
	
	QRect cr = cursorRect();
	QString completionPrefix = textUnderCursor(cursor);

	QCompleter * c = currentCompleter(textCursor());

	if( c ) {
		if( completionPrefix != m_completerNode->completionPrefix() ) {
		    c->setCompletionPrefix( completionPrefix );
			c->popup()->setCurrentIndex( c->completionModel()->index(0, 0) );
		}
		cr.setWidth( c->popup()->sizeHintForColumn(0) + c->popup()->verticalScrollBar()->sizeHint().width() );
		c->complete( cr );
	}
}

void XSLEditor::keyPressEvent( QKeyEvent *e ) {
	QCompleter * c = currentCompleter( textCursor() );
	
	if (c && c->popup()->isVisible()) {
		// The following keys are forwarded by the completer to the widget
		switch (e->key()) {
		case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
			e->ignore();
			return; // let the completer do default behavior
		default:
            break;
        }
     }

	bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
	if (!c || !isShortcut) // dont process the shortcut when we have a completer
		TextEditor::keyPressEvent( e );
		
	if(!e->text().isEmpty()) {
		if(e->text().right(1) == ">") {
			QTextCursor tc( textCursor() );
			tc.movePosition( QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor );
			tc.movePosition( QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor );
		
			QString selected = tc.selectedText();

			tc.movePosition( QTextCursor::NextCharacter );
	
			if( isEditBalise( editPosition( tc ) ) && selected != "/>" ){
				QString name = m_nodeName;
				if( ( ! name.isEmpty() ) && ( name.at(0) != '/' ) ) {
					tc.movePosition( QTextCursor::NextCharacter );
		
					int position = textCursor().position();
	         	
					//textCursor().insertText( QString("</%1>").arg(name) );
					int newPosition = insertCompletionBalises( tc, name );
					if( newPosition == -1 ) 
						tc.setPosition(position);
					setTextCursor( tc );
				}
			}
			try {
				updateModel();
			} catch( XMLParserException e ) {
				emit hasError( tr( "Error %1 at %2" ).arg( e.getMessage() ).arg( e.getLine() ) );
			}
		} else if( e->text().right(1) == "=" ) { 
			QTextCursor tc( textCursor() );
			QTextCursor tc2( textCursor() );
			tc2.movePosition( QTextCursor::PreviousCharacter );
			if( editPosition( tc2 ) == XSLEditor::cpEditParamName ) {
				tc.deletePreviousChar();
				insertCompletionValue( tc, m_nodeName, m_paramName );
				setTextCursor( tc );
			}
		} else if( e->text().right(1) == " " ) { 
			QTextCursor tc( textCursor() );
			QTextCursor tc2( textCursor() );
			tc2.movePosition( QTextCursor::PreviousCharacter );
			if( editPosition( tc2 ) == XSLEditor::cpEditNodeName ) {
				tc.deletePreviousChar();
				if( insertCompletionParam( tc, m_nodeName ) == -1 )
					tc.insertText( " " );
				setTextCursor( tc );
			}
		}
	}

	const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);

	if (!c || (ctrlOrShift && e->text().isEmpty()))
		return;

     static QString eow(EOW); // end of word
     bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
     QString completionPrefix = textUnderCursor( textCursor() );

     if (!isShortcut && (hasModifier || e->text().isEmpty() || completionPrefix.length() < 2 || eow.contains(e->text().right(1)))) {
        c->popup()->hide();
        return;
     }

     if (completionPrefix != c->completionPrefix()) {
         c->setCompletionPrefix(completionPrefix);
         c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
     }

	QRect cr = cursorRect();
	cr.setWidth(c->popup()->sizeHintForColumn(0) + c->popup()->verticalScrollBar()->sizeHint().width());
	c->complete(cr); // popup it up!
}

void XSLEditor::commentSelectedText( bool uncomment ) {
	QTextCursor cursor( textCursor() );
 
	QTextCursor cursorStart( textCursor() );
	cursorStart.setPosition( cursor.selectionStart() );
	bool isStartCommented = editPosition( cursorStart ) == XSLEditor::cpEditComment;

	QTextCursor cursorEnd( textCursor() );
	cursorEnd.setPosition( cursor.selectionEnd() );
	bool isEndCommented =  editPosition( cursorEnd ) == XSLEditor::cpEditComment;

	QString text = cursor.selectedText();
	text = text.replace( "<!--", "" );
	text = text.replace( "-->", "" );
  
	cursor.beginEditBlock();

	cursor.removeSelectedText();
	if(! ( isStartCommented ^ uncomment ) ) {
		// Comment  	
		if(! uncomment)
			cursor.insertText("<!--");  
		else
			cursor.insertText("-->");  
	}
	cursor.insertText(text);  
	if(! ( isEndCommented ^ uncomment )) {
		// End the comment  	
		if(! uncomment)
			cursor.insertText("-->");  
		else
			cursor.insertText("<!--");  
	}

	cursor.endEditBlock();
}

QAbstractItemModel * XSLEditor::model() {
	return m_contentModel;
}

void XSLEditor::updateModel() {
	XSLModelData * datas = NULL;
	try {
		datas = new XSLModelData();
		datas->loadFromContent( toPlainText() );	
	
		emit deleteModel();
	
		m_completerValue->setModel( NULL );
	
		delete m_contentModel; m_contentModel = NULL;
		delete m_completionValueModel; m_completionValueModel = NULL;
		delete m_modelData; m_modelData = NULL;
	
		m_modelData = datas;
		m_contentModel  = new XSLItemModel( m_modelData, this );
		m_completionValueModel = new XSLValueCompletionModel( m_modelData, this );
	
		emit createModel();

		m_completerValue->setModel( m_completionValueModel );
	} catch( XMLParserException e ) {
		delete datas;
		throw e;
	}
}

#include "xsleditor.moc"
