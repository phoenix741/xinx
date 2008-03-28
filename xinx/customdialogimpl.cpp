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
#include "exceptions.h"
#include "xinxpluginsloader.h"
#include "private/p_customdialogimpl.h"

// Qt header
#include <QDir>
#include <QFileDialog>
#include <QPainter>
#include <QMessageBox>

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

SpecifiqueModelIndex::SpecifiqueModelIndex( QHash<QString,struct_extentions> * extentions, QObject * parent ) : QAbstractTableModel( parent ) {
	foreach( QString key, XinxPluginsLoader::self()->extentions().keys() ) {
		m_extentions.insert( key, extentions->value( key ) );
	}
}

SpecifiqueModelIndex::~SpecifiqueModelIndex() {
	
}

void SpecifiqueModelIndex::setExtentions( QHash<QString,struct_extentions> * extentions ) {
	emit layoutAboutToBeChanged();
	m_extentions.clear();
	foreach( QString key, XinxPluginsLoader::self()->extentions().keys() ) {
		m_extentions.insert( key, extentions->value( key ) );
	}
	emit layoutChanged();
}

const QHash<QString,struct_extentions> & SpecifiqueModelIndex::extentions() const {
	return m_extentions;
}

int SpecifiqueModelIndex::rowCount( const QModelIndex & parent ) const {
	Q_UNUSED( parent );
	return m_extentions.count();
}

int SpecifiqueModelIndex::columnCount( const QModelIndex & parent ) const {
	Q_UNUSED( parent );
	return 3;
}

QVariant SpecifiqueModelIndex::headerData ( int section, Qt::Orientation orientation, int role ) const {
	if( ( orientation == Qt::Horizontal ) && ( role == Qt::DisplayRole ) )
		switch( section ) {
		case 0 :
			return tr( "File type" );
		case 1 :
			return tr( "Can be specifique" );
		case 2 :
			return tr( "Sub-directory" );
		}
	return QVariant();
}

bool SpecifiqueModelIndex::setData ( const QModelIndex & index, const QVariant & value, int role ) {
	if( index.isValid() && ( role == Qt::EditRole ) ) {
		QString key = m_extentions.keys().at( index.row() );
		struct_extentions ext = m_extentions.value( key );
		switch( index.column() ) {
		case 1:
			ext.canBeSpecifique = value.toBool();
			break;
		case 2:
			ext.customPath = value.toString();
			break;
		}
		m_extentions.insert( key, ext ); 
		emit dataChanged( index, index );
		return true;
	}
	return false;
}

QVariant SpecifiqueModelIndex::data ( const QModelIndex & index, int role ) const {
	if( index.isValid() && ( role == Qt::DisplayRole ) ) {
		switch( index.column() ) {
		case 0 :
			return XinxPluginsLoader::self()->filter( m_extentions.keys().at( index.row() ) );
		case 1 :
			return m_extentions.values().at( index.row() ).canBeSpecifique;
		case 2 : 
			return m_extentions.values().at( index.row() ).customPath;
		}
	}
	if( index.isValid() && ( role == Qt::EditRole ) ) {
		switch( index.column() ) {
		case 0 :
			return m_extentions.keys().at( index.row() );
		case 1 :
			return m_extentions.values().at( index.row() ).canBeSpecifique;
		case 2 : 
			return m_extentions.values().at( index.row() ).customPath;
		}
	}
	
	return QVariant();
}

Qt::ItemFlags SpecifiqueModelIndex::flags ( const QModelIndex & index ) const {
	if( index.isValid() && ( index.column() >= 1 ) )
		return QAbstractTableModel::flags( index ) | Qt::ItemIsEditable;
	return QAbstractTableModel::flags( index );
}

/* PrivateCustomDialogImpl */

PrivateCustomDialogImpl::PrivateCustomDialogImpl( CustomDialogImpl * parent ) {
	m_highlighter = 0;
	m_parent = parent;
}

void PrivateCustomDialogImpl::showConfig() {//m_specifiqueTableView
	// Application description path
	m_parent->m_descriptionPathLineEdit->setText( QDir::toNativeSeparators( m_config.config().descriptions.datas ) );

	// Language
	m_parent->m_langComboBox->setCurrentIndex( 0 );
	for( int i = 0; i < m_parent->m_langComboBox->count(); i++ ) {
		if( m_parent->m_langComboBox->itemText( i ).contains( QString("(%1)").arg( m_config.config().language ) ) ) {
			m_parent->m_langComboBox->setCurrentIndex( i );
			break;
		}
	}

	// Create backup file when saving
	m_parent->m_createBakCheckBox->setChecked( m_config.config().editor.createBackupFile );
	
	// Popup when file modified
	m_parent->m_popupWhenFileModifiedCheckBox->setChecked( m_config.config().editor.popupWhenFileModified );

	// Pretty print on saving
	m_parent->m_prettyPrintOnSavingCheckBox->setChecked( m_config.config().editor.autoindentOnSaving );
	
	// Complession level
	m_parent->m_completionLevelComboBox->setCurrentIndex( m_config.config().editor.completionLevel );
	
	// Tab close button
	if( m_config.config().editor.closeButtonOnEachTab ) {
		if( m_config.config().editor.hideCloseTab )
			m_parent->m_closeBtnRadioButton->setChecked( true );
		else
			m_parent->m_closeAndCornerBtnRadioButton->setChecked( true );
	} else
		m_parent->m_cornerBtnRadioButton->setChecked( true );
	
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
	m_parent->m_fontComboBox->setCurrentFont(  m_config.config().editor.defaultFormat );
	
	// Alert user when file is standard
	m_parent->m_alertStandardCheckBox->setChecked( m_config.config().project.alertWhenSavingStandardFile );
	
	// Save project with session
	m_parent->m_saveSessionCheckBox->setChecked( m_config.config().project.saveWithSessionByDefault );
	
	// Close the version management log automaticaly.
	m_parent->m_closeLogCheckBox->setChecked( m_config.config().project.closeVersionManagementLog );
	
	// Default project directory
	m_parent->m_projectPathLineEdit->setText( QDir::toNativeSeparators( m_config.config().project.defaultPath ) );
	
	// Default specifique path name
	m_parent->m_lineEditDefaultProjectPathName->setText( QDir::toNativeSeparators( m_config.config().project.defaultProjectPathName ) );
	
	// Tools
	ToolsModelIndex * toolsModel = new ToolsModelIndex( &(m_config.config().tools), m_parent->m_toolsTable );
	m_parent->m_toolsTable->setModel( toolsModel );
	m_parent->m_toolsTable->setItemDelegate( new DirectoryEditDelegate( m_parent->m_toolsTable ) );
	m_parent->m_toolsTable->resizeColumnsToContents();
	QObject::connect( toolsModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), m_parent->m_toolsTable, SLOT(resizeColumnsToContents()) );
	
	// CVS: Progress message
	if( m_config.config().cvs.progressMessages.isEmpty() ) 
		m_parent->m_cvsVerboseComboBox->setCurrentIndex( 2 );
	else
	if( m_config.config().cvs.progressMessages == "-q" ) 
		m_parent->m_cvsVerboseComboBox->setCurrentIndex( 1 );
	else
	if( m_config.config().cvs.progressMessages == "-Q" ) 
		m_parent->m_cvsVerboseComboBox->setCurrentIndex( 0 );
	
	// CVS: Compression
	m_parent->m_cvsCompressionComboBox->setCurrentIndex( m_config.config().cvs.compressionLevel );
	
	// CVS: Prune empty directories
	m_parent->m_cvsPruneCheckBox->setChecked( m_config.config().cvs.pruneEmptyDirectories );
	
	// CVS: Create any directories that exist in the repository
	m_parent->m_cvsCreateDirCheckBox->setChecked( m_config.config().cvs.createDirectories );
	
	// CVS : Create Change Log automatically
	m_parent->m_changeLogCheckBox->setChecked( m_config.config().cvs.createChangelog );

	// Syntax highlighter
	m_previousFormat = QString();
	m_parent->m_highlighterComboBox->clear();
	m_parent->m_highlighterComboBox->addItems( XinxPluginsLoader::self()->highlighterOfPlugins() );
	m_parent->m_highlighterComboBox->setCurrentIndex( 0 );
	m_parent->on_m_highlighterComboBox_activated( m_parent->m_highlighterComboBox->currentText() );
	
	// Extentions
	SpecifiqueModelIndex * specifiqueModel = new SpecifiqueModelIndex( &(m_config.config().files) , m_parent->m_specifiqueTableView );
	m_parent->m_specifiqueTableView->setModel( specifiqueModel );
	m_parent->m_specifiqueTableView->resizeColumnsToContents();
	QObject::connect( specifiqueModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), m_parent->m_specifiqueTableView, SLOT(resizeColumnsToContents()) );
}

void PrivateCustomDialogImpl::storeConfig() {
	// Application description path
	m_config.setXinxDataFiles( QDir::fromNativeSeparators( m_parent->m_descriptionPathLineEdit->text() ) );
	
	// Language
	QRegExp exp("^\\((.*)\\).*$");
	if( exp.indexIn( m_parent->m_langComboBox->currentText() ) >= 0 )
		m_config.config().language = exp.cap( 1 );

	// Create backup file when saving
	m_config.config().editor.createBackupFile = m_parent->m_createBakCheckBox->isChecked();
	
	// Popup when file modified
	m_config.config().editor.popupWhenFileModified = m_parent->m_popupWhenFileModifiedCheckBox->isChecked();
	
	// Pretty print on saving
	m_config.config().editor.autoindentOnSaving = m_parent->m_prettyPrintOnSavingCheckBox->isChecked();
	
	// Complession level
	m_config.config().editor.completionLevel = m_parent->m_completionLevelComboBox->currentIndex();

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
	
	// Alert user when file is standard
	m_config.config().project.alertWhenSavingStandardFile = m_parent->m_alertStandardCheckBox->isChecked();
	
	// Save project with session
	m_config.config().project.saveWithSessionByDefault = m_parent->m_saveSessionCheckBox->isChecked();
	
	// Close the version management log automaticaly.
	m_config.config().project.closeVersionManagementLog = m_parent->m_closeLogCheckBox->isChecked();

	// Default project directory
	m_config.config().project.defaultPath = QDir::fromNativeSeparators( m_parent->m_projectPathLineEdit->text() );
	
	// Default specifique path name
	m_config.config().project.defaultProjectPathName = QDir::fromNativeSeparators( m_parent->m_lineEditDefaultProjectPathName->text() );

	// CVS: Progress message
	switch( m_parent->m_cvsVerboseComboBox->currentIndex() ) {
	case 2: 
		m_config.config().cvs.progressMessages = "";
		break;
	case 1: 
		m_config.config().cvs.progressMessages = "-q";
		break;
	case 0: 
		m_config.config().cvs.progressMessages = "-Q";
		break;
	}

	// CVS: Compression
	m_config.config().cvs.compressionLevel = m_parent->m_cvsCompressionComboBox->currentIndex();
	
	// CVS: Prune empty directories
	m_config.config().cvs.pruneEmptyDirectories = m_parent->m_cvsPruneCheckBox->isChecked();
	
	// CVS: Create any directories that exist in the repository
	m_config.config().cvs.createDirectories = m_parent->m_cvsCreateDirCheckBox->isChecked();

	// CVS : Create Change Log automatically
	m_config.config().cvs.createChangelog = m_parent->m_changeLogCheckBox->isChecked();
	
	// Extentions
	SpecifiqueModelIndex * specifiqueModel = qobject_cast<SpecifiqueModelIndex*>( m_parent->m_specifiqueTableView->model() );
	m_config.config().files.clear();
	m_config.config().files = specifiqueModel->extentions();
}

void PrivateCustomDialogImpl::configurePlugin( XinxPluginElement * plugin ) {
	XINX_ASSERT( plugin );
	XINX_ASSERT( dynamic_cast<IXinxPluginConfiguration*>( plugin->plugin ) );
	
	IXinxPluginConfiguration * p = dynamic_cast<IXinxPluginConfiguration*>( plugin->plugin );
	
	QDialog configureDialog;
	configureDialog.setWindowFlags( Qt::MSWindowsFixedSizeDialogHint | Qt::Dialog );
	
	QWidget * settings = p->createSettingsDialog();
	
	QPushButton *okButton = new QPushButton( QIcon(":/images/button_ok.png"), tr("&Ok"), &configureDialog );
	
	QHBoxLayout * hbox = new QHBoxLayout;
	hbox->addStretch();
	hbox->addWidget( okButton );
	hbox->addStretch();
	
	QVBoxLayout * vbox = new QVBoxLayout;
	vbox->addWidget( settings );
	vbox->addLayout( hbox );
	
	configureDialog.setLayout( vbox );
	
	connect( okButton, SIGNAL(clicked()), &configureDialog, SLOT(accept()) );
	
	configureDialog.exec();
}

void PrivateCustomDialogImpl::aboutPlugin( XinxPluginElement * plugin ) {
	XINX_ASSERT( plugin );
	
	QDialog informationDialog;
	informationDialog.setWindowFlags( Qt::MSWindowsFixedSizeDialogHint | Qt::Dialog );
	
	QLabel * informations = new QLabel( &informationDialog );
	informations->setWordWrap( true );
	informations->setOpenExternalLinks( true );
	informations->setText(
			tr( "<table>"
					"<tr>"
						"<td rowspan=\"2\"><img src=\":/images/dialog-information.png\"/></td>"
						"<td colspan=\"2\"><b>%1</b></td>"
					"</tr>"
					"<tr>"
						"<td colspan=\"2\">%2</td>"
					"</tr>"
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
				.arg( plugin->plugin->getPluginAttribute( IXinxPlugin::PLG_NAME ).toString() )
				.arg( plugin->plugin->getPluginAttribute( IXinxPlugin::PLG_DESCRIPTION ).toString() )
				.arg( plugin->plugin->getPluginAttribute( IXinxPlugin::PLG_AUTHOR ).toString() )
				.arg( plugin->plugin->getPluginAttribute( IXinxPlugin::PLG_EMAIL ).toString() )
				.arg( plugin->plugin->getPluginAttribute( IXinxPlugin::PLG_WEBSITE ).toString() )
				.arg( plugin->plugin->getPluginAttribute( IXinxPlugin::PLG_VERSION ).toString() )
				.arg( plugin->plugin->getPluginAttribute( IXinxPlugin::PLG_LICENCE ).toString() )
			);
	
	QVBoxLayout * labelLayout = new QVBoxLayout;
	labelLayout->addWidget( informations );
	
	QGroupBox * grp = new QGroupBox( tr("&Informations"), &informationDialog );
	grp->setLayout( labelLayout );
	
	QPushButton *okButton = new QPushButton( QIcon(":/images/button_ok.png"), tr("&Ok"), &informationDialog );
	
	QHBoxLayout * hbox = new QHBoxLayout;
	hbox->addStretch();
	hbox->addWidget( okButton );
	hbox->addStretch();
	
	QVBoxLayout * vbox = new QVBoxLayout;
	vbox->addWidget( grp );
	vbox->addLayout( hbox );
	
	informationDialog.setLayout( vbox );
	
	connect( okButton, SIGNAL(clicked()), &informationDialog, SLOT(accept()) );
	
	informationDialog.exec();
}

/* CustomDialogImpl */

CustomDialogImpl::CustomDialogImpl( QWidget * parent, Qt::WFlags f)  : QDialog( parent, f ) {
	d = new PrivateCustomDialogImpl( this );

	setupUi( this );
	
	QFont font( "Monospace", 8 );
	font.setFixedPitch( true );
  
	m_exempleTextEdit->setFont( font );

	// Plugins
	connect( m_pluginListView, SIGNAL(configurePlugin(XinxPluginElement*)), d, SLOT(configurePlugin(XinxPluginElement*)) );
	connect( m_pluginListView, SIGNAL(aboutPlugin(XinxPluginElement*)), d, SLOT(aboutPlugin(XinxPluginElement*)) );
	foreach( XinxPluginElement plugin, XinxPluginsLoader::self()->plugins() ) {
		XinxPluginElement * e = new XinxPluginElement();
		*e = plugin; 
		m_pluginListView->addPlugin( e );
	}
}

CustomDialogImpl::~CustomDialogImpl() {
	// TODO : Delete plugin selector element
	delete d;
}


void CustomDialogImpl::loadFromConfig( XINXConfig * config ) {
	XINX_ASSERT( config );
	
	d->m_config = *config;
	d->showConfig();
}

void CustomDialogImpl::saveToConfig( XINXConfig * config ) {
	XINX_ASSERT( config );
	
	d->storeConfig();
	*config = d->m_config;
}

void CustomDialogImpl::on_m_changeProjectPathBtn_clicked() {
	m_projectPathLineEdit->changePath( this );
}

void CustomDialogImpl::on_m_changeApplicationDescriptionPathBtn_clicked() {
	m_descriptionPathLineEdit->changePath( this );
}

void CustomDialogImpl::on_m_highlighterComboBox_activated( QString text ) {
	m_formatsListView->clear();
	QStringList filtered = d->m_config.config().formats.keys();
	filtered = filtered.filter( text, Qt::CaseInsensitive );
	foreach( QString key, filtered ) {
		m_formatsListView->addItem( key.remove( text + "_", Qt::CaseInsensitive ) );
	}
	m_formatsListView->setCurrentRow( 0 );
	 
	// Example
	if( text != d->m_previousFormat ) {
		if( d->m_highlighter ) { delete d->m_highlighter; d->m_highlighter = NULL; };
		QString example;
		IPluginSyntaxHighlighter* plugin = XinxPluginsLoader::self()->highlighterOfPlugin( text );
		if( plugin )
			example = plugin->exampleOfHighlighter( text );
		m_exempleTextEdit->setText( example );
		d->m_highlighter = new SyntaxHighlighter( qMakePair( plugin, text ), m_exempleTextEdit, &(d->m_config) );
	}

	d->m_previousFormat = text;
}

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
	if( d->m_highlighter )
		d->m_highlighter->rehighlight();
}

void CustomDialogImpl::on_m_boldCheckBox_toggled(bool checked) {
	QString format = m_highlighterComboBox->currentText(),
			token = format + "_" + m_formatsListView->item( m_formatsListView->currentRow() )->text();
	token = token.toLower();

	d->m_config.config().formats[ token ].setFontWeight( checked ? QFont::Bold : QFont::Normal );
	if( d->m_highlighter )
		d->m_highlighter->rehighlight();
}

void CustomDialogImpl::on_m_italicCheckBox_toggled(bool checked) {
	QString format = m_highlighterComboBox->currentText(),
			token = format + "_" + m_formatsListView->item( m_formatsListView->currentRow() )->text();
	token = token.toLower();

	d->m_config.config().formats[ token ].setFontItalic( checked );
	if( d->m_highlighter )
		d->m_highlighter->rehighlight();
}

void CustomDialogImpl::on_m_StrikeoutCheckBox_toggled(bool checked) {
	QString format = m_highlighterComboBox->currentText(),
			token = format + "_" + m_formatsListView->item( m_formatsListView->currentRow() )->text();
	token = token.toLower();

	d->m_config.config().formats[ token ].setFontStrikeOut( checked );
	if( d->m_highlighter )
		d->m_highlighter->rehighlight();
}

void CustomDialogImpl::on_m_underlineCheckBox_toggled(bool checked) {
	QString format = m_highlighterComboBox->currentText(),
			token = format + "_" + m_formatsListView->item( m_formatsListView->currentRow() )->text();
	token = token.toLower();

	d->m_config.config().formats[ token ].setFontUnderline( checked );
	if( d->m_highlighter )
		d->m_highlighter->rehighlight();
}

void CustomDialogImpl::on_m_defaultPushButton_clicked() {
	d->m_config.setDefault();
	d->showConfig();
}
