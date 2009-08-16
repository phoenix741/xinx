/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2009 by Ulrich Van Den Hekke                              *
 * ulrich.vdh@shadoware.org                                                *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 * *********************************************************************** */

// Xinx header
#include "customdialogmodeles.h"
#include <directoryedit.h>

// Qt header
#include <QLayout>
#include <QModelIndex>
#include <QPainter>

/* ToolsModelIndex */

ToolsModelIndex::ToolsModelIndex( QHash<QString,QString> * tools, QObject * parent ) : QAbstractTableModel( parent ) {
	m_tools = tools;
}

ToolsModelIndex::~ToolsModelIndex() {

}

void ToolsModelIndex::setTools( QHash<QString,QString> * tools ) {
	emit layoutAboutToBeChanged();
	m_tools = tools;
	emit layoutChanged();
}

int ToolsModelIndex::rowCount( const QModelIndex & parent ) const {
	Q_UNUSED( parent );

	return m_tools->count();
}

int ToolsModelIndex::columnCount( const QModelIndex & parent ) const {
	Q_UNUSED( parent );

	return 2;
}

bool ToolsModelIndex::setData ( const QModelIndex & index, const QVariant & value, int role ) {
	if( index.isValid() && ( role == Qt::EditRole ) ) {
		m_tools->insert( m_tools->keys().at( index.row() ), value.toString() );
		emit dataChanged( index, index );
		return true;
	}
	return false;
}

QVariant ToolsModelIndex::data( const QModelIndex & index, int role ) const {
	if( ! index.isValid() ) return QVariant();
	if( role == Qt::DisplayRole ) {
		QString key;
		switch( index.column() ) {
		case 0:
			key = m_tools->keys().at( index.row() );
			return key.at(0).toUpper() + key.mid( 1 );
		case 1:
			return QDir::toNativeSeparators( m_tools->values().at( index.row() ) );
		}
	} else
	if( role == Qt::EditRole ) {
		QString key;
		switch( index.column() ) {
		case 0:
			return m_tools->keys().at( index.row() );
		case 1:
			return m_tools->values().at( index.row() );
		}
	}

	return QVariant();
}

QVariant ToolsModelIndex::headerData ( int section, Qt::Orientation orientation, int role ) const {
	if( ( role == Qt::DisplayRole ) && ( orientation == Qt::Horizontal ) )
		switch( section ) {
		case 0:
			return tr( "Tool name" );
		case 1:
			return tr( "Tool path" );
		}
	return QVariant();
}

Qt::ItemFlags ToolsModelIndex::flags( const QModelIndex & index ) const {
	if( index.isValid() && ( index.column() == 1 ) )
		return QAbstractTableModel::flags( index ) | Qt::ItemIsEditable;
	return QAbstractTableModel::flags( index );
}

/* DirectoryEditDelegate */

DirectoryEditDelegate::DirectoryEditDelegate( QObject *parent ) : QItemDelegate( parent ) {
}

QWidget * DirectoryEditDelegate::createEditor( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const {
	Q_UNUSED( option );
	Q_UNUSED( index );
	DirectoryEditWidget * editor = new DirectoryEditWidget( parent );
	editor->setDirectory( false );
	editor->layout()->setSpacing(0);
    return editor;
}

void DirectoryEditDelegate::setEditorData( QWidget *editor, const QModelIndex &index ) const {
    QString value = index.model()->data( index, Qt::EditRole ).toString();
    DirectoryEdit * directoryEdit = qobject_cast<DirectoryEditWidget*>( editor )->lineEdit();
    directoryEdit->setText( QDir::toNativeSeparators( value ) );
    directoryEdit->setFocus();
}

void DirectoryEditDelegate::setModelData( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const {
    DirectoryEdit * directoryEdit = qobject_cast<DirectoryEditWidget*>( editor )->lineEdit();
    QString value = directoryEdit->text();

    model->setData( index, QDir::fromNativeSeparators( value ), Qt::EditRole );
}

void DirectoryEditDelegate::updateEditorGeometry( QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index ) const {
	Q_UNUSED( index );
	editor->setGeometry( option.rect );
}

void DirectoryEditDelegate::paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const {
	if( index.column() == 1 ) {
		QString value = index.model()->data( index, Qt::DisplayRole ).toString();

		painter->save();
		QStyleOptionViewItem o = option;
		if( ! QFile( value ).exists() )
			o.palette.setColor( QPalette::Text, Qt::red );
		drawDisplay( painter, o, option.rect, value );
		painter->restore();
	} else
		QItemDelegate::paint( painter, option, index );
}

/* SpecifiqueModelIndex */

SpecifiqueModelIndex::SpecifiqueModelIndex( QHash<QString,AppSettings::struct_extentions> * extentions, QObject * parent ) : QAbstractTableModel( parent ) {
	foreach( IFileTypePlugin * plugin, XinxPluginsLoader::self()->fileTypes()  ) {
		m_extentions.insert( plugin->description(), extentions->value( plugin->description() ) );
	}
}

SpecifiqueModelIndex::~SpecifiqueModelIndex() {

}

void SpecifiqueModelIndex::setExtentions( QHash<QString,AppSettings::struct_extentions> * extentions ) {
	emit layoutAboutToBeChanged();
	m_extentions.clear();
	foreach( const QString & key, XinxPluginsLoader::self()->managedFilters() ) {
		m_extentions.insert( key, extentions->value( key ) );
	}
	emit layoutChanged();
}

const QHash<QString,AppSettings::struct_extentions> & SpecifiqueModelIndex::extentions() const {
	return m_extentions;
}

int SpecifiqueModelIndex::rowCount( const QModelIndex & parent ) const {
	Q_UNUSED( parent );
	return m_extentions.count();
}

int SpecifiqueModelIndex::columnCount( const QModelIndex & parent ) const {
	Q_UNUSED( parent );
	return 5;
}

QVariant SpecifiqueModelIndex::headerData ( int section, Qt::Orientation orientation, int role ) const {
	if( ( orientation == Qt::Horizontal ) && ( role == Qt::DisplayRole ) )
		switch( section ) {
		case 0 :
			return tr( "File type" );
		case 1 :
			return tr( "Can be save as specifique" );
		case 2 :
			return tr( "Sub-directory" );
		case 3 :
			return tr( "Can be Commited" );
		case 4 :
			return tr( "Is in configuration.xml" );
		}
	return QVariant();
}

bool SpecifiqueModelIndex::setData ( const QModelIndex & index, const QVariant & value, int role ) {
	if( index.isValid() && ( role == Qt::EditRole ) ) {
		QString key = m_extentions.keys().at( index.row() );
		AppSettings::struct_extentions ext = m_extentions.value( key );
		switch( index.column() ) {
		case 1:
			ext.canBeSaveAsSpecifique = value.toBool();
			break;
		case 2:
			ext.specifiqueSubDirectory = value.toString();
			break;
		case 3:
			ext.canBeCommitToRcs = value.toBool();
			break;
		case 4:
			ext.canBeFindInConfiguration = value.toBool();
			break;
		}
		m_extentions.insert( key, ext );
		emit dataChanged( index, index );
		return true;
	}
	return false;
}

QVariant SpecifiqueModelIndex::data ( const QModelIndex & index, int role ) const {
	if( index.isValid() && ( ( role == Qt::DisplayRole ) || ( role == Qt::EditRole ) ) ) {
		switch( index.column() ) {
		case 0 :
			return m_extentions.keys().at( index.row() );
		case 1 :
			return m_extentions.values().at( index.row() ).canBeSaveAsSpecifique;
		case 2 :
			return m_extentions.values().at( index.row() ).specifiqueSubDirectory;
		case 3 :
			return m_extentions.values().at( index.row() ).canBeCommitToRcs;
		case 4 :
			return m_extentions.values().at( index.row() ).canBeFindInConfiguration;
		}
	}

	return QVariant();
}

Qt::ItemFlags SpecifiqueModelIndex::flags ( const QModelIndex & index ) const {
	if( index.isValid() && ( index.column() >= 1 ) )
		return QAbstractTableModel::flags( index ) | Qt::ItemIsEditable;
	return QAbstractTableModel::flags( index );
}

/* ScriptElement */

ScriptElement::ScriptElement( const ScriptValue & script ) : m_script( script ) {
}

bool ScriptElement::isModifiable() const {
	return false;
}

bool ScriptElement::isConfigurable() const {
	return false;
}

QPixmap ScriptElement::pixmap() const {
	return QPixmap( ":/images/ecmascript.png" );
}

QString ScriptElement::name() const {
	return m_script.text().replace( "&", "" );
}

QString ScriptElement::author() const {
	return m_script.value().property( "author" ).toString();
}

QString ScriptElement::version() const {
	return m_script.value().property( "version" ).toString();
}

QString ScriptElement::licence() const {
	return m_script.value().property( "licence" ).toString();
}

QString ScriptElement::description() const {
	return tr("Author : ") + m_script.value().property( "author" ).toString();
}

ScriptValue & ScriptElement::script() {
	return m_script;
}
