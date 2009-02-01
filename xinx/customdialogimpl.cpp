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
#include <exceptions.h>
#include "private/p_customdialogimpl.h"
#include "snipetdialog.h"
#include <xinxformatscheme.h>
#include <xinxlanguagefactory.h>
#include <xinxcodeedit.h>

// QCodeEdit header
#include <qnfadefinition.h>

// Qt header
#include <QDir>
#include <QFileDialog>
#include <QPainter>
#include <QMessageBox>
#include <QHeaderView>
#include <QTextCodec>
#include <QStyleFactory>
#include <QDomDocument>

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

/* SnipetModelIndex */

SnipetModelIndex::SnipetModelIndex( const SnipetList & list, QObject * parent ) : QAbstractItemModel( parent ) {
	loadSnipetList( list );
}

SnipetModelIndex::~SnipetModelIndex() {

}

void SnipetModelIndex::loadSnipetList( const SnipetList & list ) {
	foreach( const Snipet & s, list )
		m_snipetList[ s.category() ].append( s );
	reset();
}

void SnipetModelIndex::clear() {
	m_snipetList.clear();
	reset();
}

SnipetList SnipetModelIndex::getSnipetList() const {
	SnipetList result;
	foreach( const SnipetList & list, m_snipetList )
		result += list;
	return result;
}

QModelIndex SnipetModelIndex::index( int row, int column, const QModelIndex & parent ) const {
	if( ( column < 0 ) || ( column > 3 ) ) return QModelIndex(); // Test supplémentaire pour ModelTest

	if( parent.isValid() && ( parent.internalId() == -1 ) ) {
		if( ( row < 0 ) || ( row >= m_snipetList.values().at( parent.row() ).size() ) ) return QModelIndex();
		return createIndex( row, column, parent.row() );
	} else if( !parent.isValid() ) {
		if( ( row < 0 ) || ( row >= m_snipetList.keys().size() ) ) return QModelIndex();
		return createIndex( row, column, -1 );
	}
	return QModelIndex();
}

QModelIndex SnipetModelIndex::parent( const QModelIndex & index ) const {
	if( index.isValid() && (index.internalId() >= 0) ) {
		return createIndex( index.internalId(), 0, -1 );
	}
	return QModelIndex();
}

int SnipetModelIndex::rowCount( const QModelIndex & index ) const {
	if( ! index.isValid() ) return m_snipetList.keys().size();
	if( index.column() != 0 ) return 0; // Column don't have children (Test supplémentaire pour ModelTest)

	if( index.internalId() != -1 ) return 0;
	return m_snipetList.values().at( index.row() ).size();
}

int SnipetModelIndex::columnCount( const QModelIndex & index ) const {
	if( index.isValid() && (index.internalId() == -1) ) return 1;
	return 3;
}

Qt::ItemFlags SnipetModelIndex::flags( const QModelIndex & index ) const {
	if( index.isValid() && (index.internalId() == -1) ) {
		return Qt::ItemIsEnabled;
	} else
		return QAbstractItemModel::flags( index );
}

QVariant SnipetModelIndex::headerData( int section, Qt::Orientation orientation, int role ) const {
	if( role != Qt::DisplayRole ) return QVariant();

	if( orientation == Qt::Horizontal ) {
		switch( section ) {
		case 0:
			return tr("Name");
		case 1:
			return tr("Key");
		case 2:
			return tr("Description");
		default:
			return QVariant();
		}
	} else
		return QVariant();
}

QVariant SnipetModelIndex::data( const QModelIndex & index, int role ) const {
	if( ! index.isValid() ) return QVariant();

	if( index.internalId() == -1 ) {
		if( role == Qt::DisplayRole ) {
			switch( index.column() ) {
			case 0:
				return m_snipetList.keys().at( index.row() );
			case 1:
			case 2:
				return QString(""); // Needed for Model tests
			}
		} else if( role == Qt::DecorationRole ) {
			if( index.column() == 0 )
				return QIcon( ":/images/folder.png" );
		} else if( role == Qt::BackgroundRole ) {
			return QColor( 0xFF, 0xFF, 0xCC );
		} else if( role == Qt::FontRole ) {
			QFont currentFont;
			currentFont.setBold( true );
			return currentFont;
		}
	} else {
		int line = index.row();
		int category = index.internalId();
		if( ( line < 0 ) || ( line >= m_snipetList.values().at( category ).size() ) ) return QVariant();

		if( role == Qt::DisplayRole )
			switch( index.column() ) {
			case 0:
				return m_snipetList.values().at( category ).at( line ).name();
			case 1:
				return m_snipetList.values().at( category ).at( line ).key();
			case 2:
				return m_snipetList.values().at( category ).at( line ).description();
		} else if( role == Qt::DecorationRole ) {
			if( index.column() == 0 )
				return QIcon( m_snipetList.values().at( category ).at( line ).icon() );
		} else if( role == Qt::UserRole ) {
			if( index.column() == 0 )
				return QVariant::fromValue( m_snipetList.values().at( category ).at( line ) );
		}
	}
	return QVariant();
}

void SnipetModelIndex::removeSnipet( const QModelIndexList & indexes ) {
	QMultiMap<QString,int> deletedValue;
	foreach( const QModelIndex & index, indexes ) {
		deletedValue.insert( index.parent().data().toString(), index.row() );
	}

 	foreach( const QString & category, deletedValue.uniqueKeys() ) {
		QList<int> lines = deletedValue.values( category );
		qSort( lines.begin(), lines.end(), qGreater<int>() );
		foreach( int line, lines ) {
			beginRemoveRows( index( m_snipetList.keys().indexOf( category ), 0 ), line, line );
			m_snipetList[ category ].removeAt( line );
			endRemoveRows();
		}
	}
}

void SnipetModelIndex::addSnipet( const Snipet & snipet ) {
	QString category = snipet.category();
	int indexOfCategory = m_snipetList.keys().indexOf( category );

	if( indexOfCategory < 0 ) {
		m_snipetList[ category ] = SnipetList();
		reset();
		indexOfCategory = m_snipetList.keys().indexOf( category );
	}

	SnipetList list = m_snipetList.value( category );

	SnipetList::iterator i = qLowerBound( list.begin(), list.end(), snipet );
	beginInsertRows( index( indexOfCategory, 0 ), i - list.begin(), i - list.begin() );
	list.insert( i, snipet );
	m_snipetList[ category ] = list;
	endInsertRows();
}

/* ScriptElement */

ScriptElement::ScriptElement( const ScriptValue & script ) : m_script( script ) {
}

bool ScriptElement::isModifiable() const { return false; }

bool ScriptElement::isConfigurable() const { return false; }

QPixmap ScriptElement::pixmap() const {
	return QPixmap( ":/images/ecmascript.png" );
}

QString ScriptElement::name() const {
	return m_script.text().replace( "&", "" );
}

QString ScriptElement::description() const {
	return CustomDialogImpl::tr("Author : ") + m_script.value().property( "author" ).toString();
}

const QScriptValue & ScriptElement::script() const {
	return m_script.value();
}

/* PrivateCustomDialogImpl */

PrivateCustomDialogImpl::PrivateCustomDialogImpl( CustomDialogImpl * parent ) : m_snipetModel( 0 ), m_parent( parent ) {
}

void PrivateCustomDialogImpl::showConfig() {//m_specifiqueTableView
	// Application description path
	m_parent->m_descriptionPathLineEdit->lineEdit()->setText( QDir::toNativeSeparators( m_config.config().descriptions.datas ) );

	// Language
	m_parent->m_langComboBox->setCurrentIndex( 0 );
	for( int i = 0; i < m_parent->m_langComboBox->count(); i++ ) {
		if( m_parent->m_langComboBox->itemText( i ).contains( QString("(%1)").arg( m_config.config().language ) ) ) {
			m_parent->m_langComboBox->setCurrentIndex( i );
			break;
		}
	}

	// Trace Log File
	m_parent->m_traceLogWidget->lineEdit()->setText( QDir::toNativeSeparators( m_config.config().xinxTrace ) );

	// Style
	int currentStyle = m_parent->m_styleComboBox->findText( m_config.config().style, Qt::MatchFixedString );
	m_parent->m_styleComboBox->setCurrentIndex( currentStyle );

	// Create backup file when saving
	m_parent->m_createBakCheckBox->setChecked( m_config.config().editor.createBackupFile );

	// Popup when file modified
	m_parent->m_popupWhenFileModifiedCheckBox->setChecked( m_config.config().editor.popupWhenFileModified );

	// Pretty print on saving
	m_parent->m_prettyPrintOnSavingCheckBox->setChecked( m_config.config().editor.autoindentOnSaving );

	// Tab close button
	if( m_config.config().editor.closeButtonOnEachTab ) {
		if( m_config.config().editor.hideCloseTab )
			m_parent->m_closeBtnRadioButton->setChecked( true );
		else
			m_parent->m_closeAndCornerBtnRadioButton->setChecked( true );
	} else
		m_parent->m_cornerBtnRadioButton->setChecked( true );

	// Default text encoding.
	m_parent->m_encodingComboBox->setCurrentIndex( m_parent->m_encodingComboBox->findText( m_config.config().editor.defaultTextCodec ) );

	// Text content refresh
	m_parent->m_refreshContentTimeoutSpinBox->setValue( m_config.config().editor.automaticModelRefreshTimeout );

	// Auto highlight text
	m_parent->m_autoHighlightCheckBox->setChecked( m_config.config().editor.autoHighlight );

	// Color of highlighted text
	m_parent->m_highlightTextColorComboBox->setColor( m_config.config().editor.highlightWord );

	// Show tabulation and space in the editor
	m_parent->m_showTabulationCheckBox->setChecked( m_config.config().editor.showTabulationAndSpace );

	// Show current line
	m_parent->m_showCurrentLineCheckBox->setChecked( m_config.config().editor.highlightCurrentLine );

	// Size of tabulation
	m_parent->m_sizeOfTabSpinBox->setValue( m_config.config().editor.tabulationSize );

	// Font size
	m_parent->m_fontSizeSpinBox->setValue( m_config.config().editor.defaultFormat.pointSize() );

	// Font name
	m_parent->m_fontComboBox->setCurrentFont( m_config.config().editor.defaultFormat );

	// Refresh Directory Timeout
	m_parent->m_refreshProjectTimoutSpinBox->setValue( m_config.config().project.automaticProjectDirectoryRefreshTimeout );

	// Open the last project at start
	m_parent->m_openLastProjectCheckBox->setChecked( m_config.config().project.openTheLastProjectAtStart );

	// Save project with session
	m_parent->m_saveSessionCheckBox->setChecked( m_config.config().project.saveWithSessionByDefault );

	// Close the version management log automaticaly.
	m_parent->m_closeLogCheckBox->setChecked( m_config.config().project.closeVersionManagementLog );

	// Default project directory
	m_parent->m_projectPathLineEdit->lineEdit()->setText( QDir::toNativeSeparators( m_config.config().project.defaultPath ) );

	// Default specifique path name
	m_parent->m_lineEditDefaultProjectPathName->setText( QDir::toNativeSeparators( m_config.config().project.defaultProjectPathName ) );

	// Tools
	ToolsModelIndex * toolsModel = new ToolsModelIndex( &(m_config.config().tools), m_parent->m_toolsTable );
	m_parent->m_toolsTable->setModel( toolsModel );
	m_parent->m_toolsTable->setItemDelegate( new DirectoryEditDelegate( m_parent->m_toolsTable ) );
	m_parent->m_toolsTable->horizontalHeader()->setResizeMode( 0, QHeaderView::ResizeToContents );
	m_parent->m_toolsTable->horizontalHeader()->setResizeMode( 1, QHeaderView::Stretch );
	//m_parent->m_toolsTable->resizeColumnsToContents();
	//QObject::connect( toolsModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), m_parent->m_toolsTable, SLOT(resizeColumnsToContents()) );

	// CVS : Create Change Log automatically
	m_parent->m_changeLogCheckBox->setChecked( m_config.config().rcs.createChangelog );

	// Syntax highlighter
	foreach( IFileTypePlugin * t, XinxPluginsLoader::self()->fileTypes() ) {
		if( IFileTextPlugin * textPlugin = dynamic_cast<IFileTextPlugin*>( t ) ) {
			// Format
			QFormatScheme * scheme = textPlugin->createFormatScheme( &m_config );
			if( ! scheme ) {
				m_config.addFormatScheme( textPlugin->highlighterId(), qobject_cast<XinxFormatScheme*>( scheme ) );
				scheme = m_config.languageFactory()->defaultFormatScheme();
			}

			// Language
			QDomDocument doc;
			QLanguageFactory::LangData data;
			doc.setContent( textPlugin->createLanguageDescription() );
			QNFADefinition::load( doc, &data, scheme );
			m_config.languageFactory()->addLanguage( data );
		}
	}

	QStringList languages = m_config.languageFactory()->languages();
	languages.removeAll( "None" );
	languages.sort();

	//	m_previousFormat = QString();
	m_parent->m_highlighterComboBox->clear();
	m_parent->m_highlighterComboBox->addItems( languages );
	m_parent->m_highlighterComboBox->setCurrentIndex( 0 );
	m_parent->on_m_highlighterComboBox_activated( m_parent->m_highlighterComboBox->currentText() );

	// Extentions
	SpecifiqueModelIndex * specifiqueModel = new SpecifiqueModelIndex( &(m_config.config().files) , m_parent->m_specifiqueTableView );
	m_parent->m_specifiqueTableView->setModel( specifiqueModel );
	m_parent->m_specifiqueTableView->resizeColumnsToContents();
	QObject::connect( specifiqueModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), m_parent->m_specifiqueTableView, SLOT(resizeColumnsToContents()) );

	// Xml pres
	m_parent->m_expandPathLineEdit->setText( m_config.config().xmlPres.autoExpandedPath );
	m_parent->m_hideElementList->setValues( m_config.config().xmlPres.hidePath );
	m_parent->m_showSubEltCheckBox->setChecked( m_config.config().xmlPres.showFilteredSubTree );
	m_parent->m_showNameAttributeCheckBox->setChecked( m_config.config().xmlPres.showNameAttributeIfExists );
	m_parent->m_viewColorBox->setColor( m_config.config().xmlPres.viewColor );
	m_parent->m_errorColorBox->setColor( m_config.config().xmlPres.errorColor );
	m_parent->m_screenColorBox->setColor( m_config.config().xmlPres.screenDataColor );

	// Snipet
	m_snipetModel = new SnipetModelIndex( SnipetListManager::self()->snipets(), m_parent->m_snipetTreeView );
	m_parent->m_snipetTreeView->setModel( m_snipetModel );
	m_parent->m_snipetTreeView->header()->setResizeMode( QHeaderView::ResizeToContents );
	m_parent->m_snipetTreeView->header()->setResizeMode( 2, QHeaderView::Stretch );
	m_parent->m_snipetTreeView->expandAll();
	m_parent->m_snipetTreeView_selectionChanged();
	connect( m_parent->m_snipetTreeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), m_parent, SLOT(m_snipetTreeView_selectionChanged()) );

	// Load Script
	m_parent->m_scriptListView->clear();
	foreach( const ScriptValue & s, ScriptManager::self()->objects() ) {
		m_parent->m_scriptListView->addPlugin( new ScriptElement( s ) );
	}

	// Plugins
	m_parent->m_pluginListView->clear();
	foreach( XinxPluginElement * plugin, XinxPluginsLoader::self()->plugins() ) {
		XinxPluginElement * e = new XinxPluginElement( plugin->plugin(), plugin->isStatic() );
		e->setActivated( plugin->isActivated() );
		m_parent->m_pluginListView->addPlugin( e );
	}
}

void PrivateCustomDialogImpl::storeConfig() {
	// Application description path
	m_config.setXinxDataFiles( QDir::fromNativeSeparators( m_parent->m_descriptionPathLineEdit->lineEdit()->text() ) );

	// Language
	QRegExp exp("^\\((.*)\\).*$");
	if( exp.indexIn( m_parent->m_langComboBox->currentText() ) >= 0 )
		m_config.config().language = exp.cap( 1 );

	// Trace Log File
	m_config.config().xinxTrace = QDir::fromNativeSeparators( m_parent->m_traceLogWidget->lineEdit()->text() );

	// Style
	m_config.config().style = m_parent->m_styleComboBox->currentText();

	// Create backup file when saving
	m_config.config().editor.createBackupFile = m_parent->m_createBakCheckBox->isChecked();

	// Popup when file modified
	m_config.config().editor.popupWhenFileModified = m_parent->m_popupWhenFileModifiedCheckBox->isChecked();

	// Pretty print on saving
	m_config.config().editor.autoindentOnSaving = m_parent->m_prettyPrintOnSavingCheckBox->isChecked();

	// Tab close button
	if( m_parent->m_closeBtnRadioButton->isChecked() ) {
		m_config.config().editor.closeButtonOnEachTab = true;
		m_config.config().editor.hideCloseTab = true;
	} else
	if( m_parent->m_cornerBtnRadioButton->isChecked() ) {
		m_config.config().editor.closeButtonOnEachTab = false;
		m_config.config().editor.hideCloseTab = false;
	} else {
		m_config.config().editor.closeButtonOnEachTab = true;
		m_config.config().editor.hideCloseTab = false;
	}

	// Default text encoding.
	m_config.config().editor.defaultTextCodec = m_parent->m_encodingComboBox->itemText( m_parent->m_encodingComboBox->currentIndex() );

	// Text content refresh
	m_config.config().editor.automaticModelRefreshTimeout = m_parent->m_refreshContentTimeoutSpinBox->value();

	// Auto highlight text
	m_config.config().editor.autoHighlight = m_parent->m_autoHighlightCheckBox->isChecked();

	// Color of highlighted text
	m_config.config().editor.highlightWord = m_parent->m_highlightTextColorComboBox->color();

	// Show tabulation and space in the editor
	m_config.config().editor.showTabulationAndSpace = m_parent->m_showTabulationCheckBox->isChecked();

	// Show current line
	m_config.config().editor.highlightCurrentLine = m_parent->m_showCurrentLineCheckBox->isChecked();

	// Size of tabulation
	m_config.config().editor.tabulationSize = m_parent->m_sizeOfTabSpinBox->value();

	// Font name
	m_config.config().editor.defaultFormat = m_parent->m_fontComboBox->currentFont();

	// Font size
	m_config.config().editor.defaultFormat.setPointSize( m_parent->m_fontSizeSpinBox->value() );

	// Refresh Directory Timeout
	m_config.config().project.automaticProjectDirectoryRefreshTimeout = m_parent->m_refreshProjectTimoutSpinBox->value();

	// Open the last project at start
	m_config.config().project.openTheLastProjectAtStart = m_parent->m_openLastProjectCheckBox->isChecked();

	// Save project with session
	m_config.config().project.saveWithSessionByDefault = m_parent->m_saveSessionCheckBox->isChecked();

	// Close the version management log automaticaly.
	m_config.config().project.closeVersionManagementLog = m_parent->m_closeLogCheckBox->isChecked();

	// Default project directory
	m_config.config().project.defaultPath = QDir::fromNativeSeparators( m_parent->m_projectPathLineEdit->lineEdit()->text() );

	// Default specifique path name
	m_config.config().project.defaultProjectPathName = QDir::fromNativeSeparators( m_parent->m_lineEditDefaultProjectPathName->text() );

	// CVS : Create Change Log automatically
	m_config.config().rcs.createChangelog = m_parent->m_changeLogCheckBox->isChecked();

	// Extentions
	SpecifiqueModelIndex * specifiqueModel = qobject_cast<SpecifiqueModelIndex*>( m_parent->m_specifiqueTableView->model() );
	m_config.config().files.clear();
	m_config.config().files = specifiqueModel->extentions();

	// Xml pres
	m_config.config().xmlPres.autoExpandedPath = m_parent->m_expandPathLineEdit->text();
	m_config.config().xmlPres.hidePath = m_parent->m_hideElementList->values();
	m_config.config().xmlPres.showFilteredSubTree = m_parent->m_showSubEltCheckBox->isChecked();
	m_config.config().xmlPres.viewColor = m_parent->m_viewColorBox->color();
	m_config.config().xmlPres.errorColor = m_parent->m_errorColorBox->color();
	m_config.config().xmlPres.screenDataColor = m_parent->m_screenColorBox->color();
	m_config.config().xmlPres.showNameAttributeIfExists = m_parent->m_showNameAttributeCheckBox->isChecked();

	// Syntax highlighter
	m_config.putFormatsSchemeToConfig();

	// Snipet
	SnipetListManager::self()->setSnipets( m_snipetModel->getSnipetList() );

	// Plugins
	foreach( PluginElement * plugin, m_parent->m_pluginListView->plugins() ) {
		QString name = dynamic_cast<XinxPluginElement*>( plugin )->plugin()->metaObject()->className();
		bool isActivated = plugin->isActivated();

		m_config.config().plugins[ name ] = isActivated;
		if( XinxPluginsLoader::self()->plugin( name ) ) {
			XinxPluginsLoader::self()->plugin( name )->setActivated( isActivated );
		}
	}
}

void PrivateCustomDialogImpl::configurePlugin( PluginElement * plugin ) {
	Q_ASSERT( plugin );
	Q_ASSERT( dynamic_cast<XinxPluginElement*>( plugin ) );
	Q_ASSERT( dynamic_cast<IXinxPluginConfiguration*>( dynamic_cast<XinxPluginElement*>( plugin )->plugin() ) );

	XinxPluginElement * xinxPlugin = dynamic_cast<XinxPluginElement*>( plugin );
	IXinxPluginConfiguration * p = dynamic_cast<IXinxPluginConfiguration*>( xinxPlugin->plugin() );

	QDialog configureDialog;
	configureDialog.setWindowFlags( Qt::MSWindowsFixedSizeDialogHint | Qt::Dialog );

	QWidget * settings = p->createSettingsDialog();
	if( ! p->loadSettingsDialog( settings ) )
		QMessageBox::warning( m_parent, tr("Plugin Customization"), tr("Can't load the plugin configuration") );

	QDialogButtonBox * buttonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );

	QVBoxLayout * vbox = new QVBoxLayout;
	vbox->addWidget( settings );
	vbox->addWidget( buttonBox );

	configureDialog.setLayout( vbox );

	connect( buttonBox, SIGNAL(accepted()), &configureDialog, SLOT(accept()) );
	connect( buttonBox, SIGNAL(rejected()), &configureDialog, SLOT(reject()) );

	if( configureDialog.exec() == QDialog::Accepted ) {
		if( ! p->saveSettingsDialog( settings ) )
			QMessageBox::warning( m_parent, tr("Plugin Customization"), tr("Can't save the plugin configuration") );
	}
}

void PrivateCustomDialogImpl::aboutPlugin( PluginElement * plugin ) {
	Q_ASSERT( plugin );
	Q_ASSERT( dynamic_cast<XinxPluginElement*>( plugin ) );

	XinxPluginElement * xinxPlugin = dynamic_cast<XinxPluginElement*>( plugin );

	QDialog informationDialog;
	informationDialog.setWindowFlags( Qt::MSWindowsFixedSizeDialogHint | Qt::Dialog );

	QLabel * informations = new QLabel( &informationDialog );
	informations->setWordWrap( true );
	informations->setOpenExternalLinks( true );
	informations->setText(
			tr( "<table>"
					"<tr>"
						"<td colspan=\"3\"><b>%1</b></td>"
					"</tr>"
					"<tr>"
						"<td colspan=\"3\">%2</td>"
					"</tr>"
					"<tr><td colspan=\"3\"><hr/></td></tr>"
					"<tr>"
						"<td><b>Author</b></td>"
						"<td width=\"0\">:</td>"
						"<td>%3</td>"
					"</tr>"
					"<tr>"
						"<td><b>E-Mail</b></td>"
						"<td width=\"0\">:</td>"
						"<td><a href=\"mailto:%4\">%4</a></td>"
					"</tr>"
					"<tr>"
						"<td><b>Web site</b></td>"
						"<td width=\"0\">:</td>"
						"<td><a href=\"%5\">%5</a></td>"
					"</tr>"
					"<tr>"
						"<td><b>Version</b></td>"
						"<td width=\"0\">:</td>"
						"<td>%6</td>"
					"</tr>"
					"<tr>"
						"<td><b>Licence</b></td>"
						"<td width=\"0\">:</td>"
						"<td>%7</td>"
					"</tr>"
				"</table>" )
	                       .arg( qobject_cast<IXinxPlugin*>( xinxPlugin->plugin() )->getPluginAttribute( IXinxPlugin::PLG_NAME ).toString() )
	                       .arg( qobject_cast<IXinxPlugin*>( xinxPlugin->plugin() )->getPluginAttribute( IXinxPlugin::PLG_DESCRIPTION ).toString() )
	                       .arg( qobject_cast<IXinxPlugin*>( xinxPlugin->plugin() )->getPluginAttribute( IXinxPlugin::PLG_AUTHOR ).toString() )
	                       .arg( qobject_cast<IXinxPlugin*>( xinxPlugin->plugin() )->getPluginAttribute( IXinxPlugin::PLG_EMAIL ).toString() )
	                       .arg( qobject_cast<IXinxPlugin*>( xinxPlugin->plugin() )->getPluginAttribute( IXinxPlugin::PLG_WEBSITE ).toString() )
	                       .arg( qobject_cast<IXinxPlugin*>( xinxPlugin->plugin() )->getPluginAttribute( IXinxPlugin::PLG_VERSION ).toString() )
	                       .arg( qobject_cast<IXinxPlugin*>( xinxPlugin->plugin() )->getPluginAttribute( IXinxPlugin::PLG_LICENCE ).toString() )
			);

	QVBoxLayout * labelLayout = new QVBoxLayout;
	labelLayout->addWidget( informations );

	QGroupBox * grp = new QGroupBox( tr("&Informations"), &informationDialog );
	grp->setLayout( labelLayout );

	QDialogButtonBox * buttonBox = new QDialogButtonBox( QDialogButtonBox::Ok );
	buttonBox->setCenterButtons( true );

	QVBoxLayout * vbox = new QVBoxLayout;
	vbox->addWidget( grp );
	vbox->addWidget( buttonBox );

	informationDialog.setLayout( vbox );

	connect( buttonBox, SIGNAL(accepted()), &informationDialog, SLOT(accept()) );

	informationDialog.exec();
}


void PrivateCustomDialogImpl::aboutScript( PluginElement * plugin ) {
	Q_ASSERT( plugin );
	Q_ASSERT( dynamic_cast<ScriptElement*>( plugin ) );

	ScriptElement * script = dynamic_cast<ScriptElement*>( plugin );

	QDialog informationDialog;
	informationDialog.setWindowFlags( Qt::MSWindowsFixedSizeDialogHint | Qt::Dialog );

	QLabel * informations = new QLabel( &informationDialog );
	informations->setWordWrap( true );
	informations->setOpenExternalLinks( true );
	informations->setText(
	                       tr( "<table>"
	                           "<tr>"
	                           "<td colspan=\"3\"><b>%1</b></td>"
	                           "</tr>"
	                           "<tr><td colspan=\"3\"><hr/></td></tr>"
	                           "<tr>"
	                           "<td><b>Author</b></td>"
	                           "<td width=\"0\">:</td>"
	                           "<td>%2</td>"
	                           "</tr>"
	                           "<tr>"
	                           "<td><b>Version</b></td>"
	                           "<td width=\"0\">:</td>"
	                           "<td>%3</td>"
	                           "</tr>"
	                           "<tr>"
	                           "<td><b>Licence</b></td>"
	                           "<td width=\"0\">:</td>"
	                           "<td>%4</td>"
	                           "</tr>"
	                           "</table>" )
	                       .arg( script->name() )
	                       .arg( script->script().property( "author" ).toString() )
	                       .arg( script->script().property( "version" ).toString() )
	                       .arg( script->script().property( "licence" ).toString() )
	                     );

	QVBoxLayout * labelLayout = new QVBoxLayout;
	labelLayout->addWidget( informations );

	QGroupBox * grp = new QGroupBox( tr("&Informations"), &informationDialog );
	grp->setLayout( labelLayout );

	QDialogButtonBox * buttonBox = new QDialogButtonBox( QDialogButtonBox::Ok );
	buttonBox->setCenterButtons( true );

	QVBoxLayout * vbox = new QVBoxLayout;
	vbox->addWidget( grp );
	vbox->addWidget( buttonBox );

	informationDialog.setLayout( vbox );

	connect( buttonBox, SIGNAL(accepted()), &informationDialog, SLOT(accept()) );

	informationDialog.exec();
}

/* CustomDialogImpl */

CustomDialogImpl::CustomDialogImpl( QWidget * parent, Qt::WFlags f)  : QDialog( parent, f ) {
	d = new PrivateCustomDialogImpl( this );

	setupUi( this );

	// General
	connect( m_descriptionPathLineEdit->lineEdit(), SIGNAL(textChanged(QString)), this, SLOT(m_descriptionPathLineEdit_textChanged(QString)) );

	// XML Pres
	m_hideElementList->setDefaultVisible( false );

	// Font encodingas
	QList<QByteArray> encodings = QTextCodec::availableCodecs();
	qSort( encodings );
	foreach( const QByteArray & encoding, encodings ) {
		m_encodingComboBox->addItem( encoding );
	}

	// Editor
	/** \todo
	QFont font( "Monospace", 8 );
	font.setFixedPitch( true );

	m_customScheme->setFont( font );
	*/

	// Plugins
	connect( m_pluginListView, SIGNAL(configurePlugin(PluginElement*)), d, SLOT(configurePlugin(PluginElement*)) );
	connect( m_pluginListView, SIGNAL(aboutPlugin(PluginElement*)), d, SLOT(aboutPlugin(PluginElement*)) );
	connect( m_scriptListView, SIGNAL(aboutPlugin(PluginElement*)), d, SLOT(aboutScript(PluginElement*)) );

	// Style
	m_styleComboBox->addItem( QString() );
	foreach( const QString & style, QStyleFactory::keys() ) {
		m_styleComboBox->addItem( style );
	}
}

CustomDialogImpl::~CustomDialogImpl() {
	QList<PluginElement*> list = m_pluginListView->plugins();
	m_pluginListView->clear();
	qDeleteAll( list );

	list = m_scriptListView->plugins();
	m_scriptListView->clear();
	qDeleteAll( list );

	delete d;
}


void CustomDialogImpl::loadFromConfig( XINXConfig * config ) {
	Q_ASSERT( config );

	d->m_config = *config;
	d->showConfig();
}

void CustomDialogImpl::saveToConfig( XINXConfig * config ) {
	Q_ASSERT( config );

	d->storeConfig();
	*config = d->m_config;
}

void CustomDialogImpl::m_descriptionPathLineEdit_textChanged( QString text ) {
	if( ! d->m_snipetModel ) return;
	SnipetList list;
	try {
		d->m_snipetModel->clear();
		list.loadFromFile( QDir( text ).absoluteFilePath( "template.xml" ) );
		d->m_snipetModel->loadSnipetList( list );
	} catch( SnipetListException e ) {
		qWarning( qPrintable(e.getMessage() )) ;
	}
	m_snipetTreeView->expandAll();
}

void CustomDialogImpl::on_m_importPushButton_clicked() {
	QString importedFilename = QFileDialog::getOpenFileName( this, tr("Import snipets"), "datas:/", "*.xml" );
	if( ! importedFilename.isEmpty() ) {
		SnipetList list;
		list.loadFromFile( importedFilename );
		d->m_snipetModel->loadSnipetList( list );
		m_snipetTreeView->expandAll();
	}
}

void CustomDialogImpl::on_m_exportPushButton_clicked() {
	QModelIndexList indexes = m_snipetTreeView->selectionModel()->selectedRows();
	foreach( const QModelIndex & i, indexes ) {
		if( i.internalId() == -1 ) indexes.removeAll( i );
	}

	QString exportedFilename = QFileDialog::getSaveFileName( this, tr("Export snipets"), "datas:/", "*.xml" );
	if( ! exportedFilename.isEmpty() ) {
		SnipetList list;
		for( int i = 0 ; i < indexes.size() ; i++ ) {
			const Snipet & s = indexes.at( i ).data( Qt::UserRole ).value<Snipet>();

			list << s;
		}
		list.saveToFile( exportedFilename );
	}
}

void CustomDialogImpl::on_m_addPushButton_clicked() {
	SnipetDialogImpl dlg( QString(), this );
	if( dlg.exec() ) {
		Snipet s = dlg.getSnipet();
		d->m_snipetModel->addSnipet( s );
		m_snipetTreeView->expandAll();
	}
}

void CustomDialogImpl::on_m_removePushButton_clicked() {
	QModelIndexList indexes = m_snipetTreeView->selectionModel()->selectedRows();
	foreach( const QModelIndex & i, indexes ) {
		if( i.internalId() == -1 ) indexes.removeAll( i );
	}

	d->m_snipetModel->removeSnipet( indexes );
	m_snipetTreeView->expandAll();
}

void CustomDialogImpl::on_m_modifyPushButton_clicked() {
	QModelIndexList index = m_snipetTreeView->selectionModel()->selectedRows();
	Snipet s = index.at( 0 ).data( Qt::UserRole ).value<Snipet>();
	SnipetDialogImpl dlg( s, this );
	if( dlg.exec() ) {
		d->m_snipetModel->removeSnipet( index );
		d->m_snipetModel->addSnipet( dlg.getSnipet() );
		m_snipetTreeView->expandAll();
	}
}

void CustomDialogImpl::on_m_duplicatePushButton_clicked() {
	QModelIndexList index = m_snipetTreeView->selectionModel()->selectedRows();
	Snipet s = index.at( 0 ).data( Qt::UserRole ).value<Snipet>();
	SnipetDialogImpl dlg( s, this );
	if( dlg.exec() ) {
		d->m_snipetModel->addSnipet( dlg.getSnipet() );
		m_snipetTreeView->expandAll();
	}
}

void CustomDialogImpl::m_snipetTreeView_selectionChanged() {
	QModelIndexList indexes = m_snipetTreeView->selectionModel()->selectedRows();
	foreach( const QModelIndex & i, indexes ) {
		if( i.internalId() == -1 ) indexes.removeAll( i );
	}
	m_removePushButton->setEnabled( indexes.count() > 0 );
	m_duplicatePushButton->setEnabled( indexes.count() == 1 );
	m_modifyPushButton->setEnabled( indexes.count() == 1 );
	m_exportPushButton->setEnabled( indexes.count() > 0 );
}

void CustomDialogImpl::on_m_highlighterComboBox_activated( QString text ) {
	m_customScheme->setHiddenFormat( QStringList() << "normal" << "search" << "match" << "braceMatch" << "braceMismatch" );
	m_customScheme->setFormatScheme( d->m_config.scheme( text ) );
	m_customScheme->setLanguageFactory( d->m_config.languageFactory() );
	m_customScheme->setLanguageDefinition( text );
	m_customScheme->setExample( XinxPluginsLoader::self()->exampleOfHighlighter( text ) );

	/*	m_formatsListView->clear();
	QStringList filtered = d->m_config.config().formats.keys();
	filtered = filtered.filter( text, Qt::CaseInsensitive );
	foreach( const QString & key, filtered ) {
		m_formatsListView->addItem( key.remove( text + "_", Qt::CaseInsensitive ) );
	}
	m_formatsListView->setCurrentRow( 0 );

	// Example
	if( text != d->m_previousFormat ) {
		QString example;
		example = XinxPluginsLoader::self()->exampleOfHighlighter( text );
		m_exempleTextEdit->setHighlighter( text, &(d->m_config) );
		m_exempleTextEdit->setPlainText( example );
	}

	d->m_previousFormat = text;*/
}
/*
void CustomDialogImpl::on_m_formatsListView_currentRowChanged( int currentRow ) {
	if( currentRow < 0 ) return ;
	QString format = m_highlighterComboBox->currentText(),
			token = format + "_" + m_formatsListView->item( currentRow )->text();
	token = token.toLower();

	// Bold
	m_boldCheckBox->setChecked( d->m_config.config().formats[ token ].fontWeight() != QFont::Normal );

	// Italic
	m_italicCheckBox->setChecked( d->m_config.config().formats[ token ].fontItalic() );

	// Underline
	m_underlineCheckBox->setChecked( d->m_config.config().formats[ token ].fontUnderline() );

	// Strikeout
	m_StrikeoutCheckBox->setChecked( d->m_config.config().formats[ token ].fontStrikeOut() );

	// Color
	m_colorComboBox->setColor( d->m_config.config().formats[ token ].foreground().color() );
}

void CustomDialogImpl::on_m_colorComboBox_activated(QColor color) {
	QString format = m_highlighterComboBox->currentText(),
			token = format + "_" + m_formatsListView->item( m_formatsListView->currentRow() )->text();
	token = token.toLower();

	d->m_config.config().formats[ token ].setForeground( color );
	m_exempleTextEdit->updateHighlighter();
}

void CustomDialogImpl::on_m_boldCheckBox_toggled(bool checked) {
	QString format = m_highlighterComboBox->currentText(),
			token = format + "_" + m_formatsListView->item( m_formatsListView->currentRow() )->text();
	token = token.toLower();

	d->m_config.config().formats[ token ].setFontWeight( checked ? QFont::Bold : QFont::Normal );
	m_exempleTextEdit->updateHighlighter();
}

void CustomDialogImpl::on_m_italicCheckBox_toggled(bool checked) {
	QString format = m_highlighterComboBox->currentText(),
			token = format + "_" + m_formatsListView->item( m_formatsListView->currentRow() )->text();
	token = token.toLower();

	d->m_config.config().formats[ token ].setFontItalic( checked );
	m_exempleTextEdit->updateHighlighter();
}

void CustomDialogImpl::on_m_StrikeoutCheckBox_toggled(bool checked) {
	QString format = m_highlighterComboBox->currentText(),
			token = format + "_" + m_formatsListView->item( m_formatsListView->currentRow() )->text();
	token = token.toLower();

	d->m_config.config().formats[ token ].setFontStrikeOut( checked );
	m_exempleTextEdit->updateHighlighter();
}

void CustomDialogImpl::on_m_underlineCheckBox_toggled(bool checked) {
	QString format = m_highlighterComboBox->currentText(),
			token = format + "_" + m_formatsListView->item( m_formatsListView->currentRow() )->text();
	token = token.toLower();

	d->m_config.config().formats[ token ].setFontUnderline( checked );
	m_exempleTextEdit->updateHighlighter();
}
*/
void CustomDialogImpl::on_m_buttonBox_clicked( QAbstractButton * button ) {
	if( m_buttonBox->buttonRole( button ) == QDialogButtonBox::ResetRole ) {
		d->m_config.setDefault();
		d->showConfig();
	}
}

void CustomDialogImpl::on_m_labelLink_linkActivated( const QString & link ) {
	if( link == "#modules" )
		m_listWidget->setCurrentRow( 9 );
}
