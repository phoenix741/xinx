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
#include "customdialogimpl.h"
#include "customdialogmodeles.h"
#include "snipetdialog.h"
#include <core/exceptions.h>
#include <editors/xinxformatscheme.h>
#include <editors/xinxlanguagefactory.h>
#include <editors/xinxcodeedit.h>
#include <snipets/snipetmanager.h>

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
#include <modeltest.h>


/* CustomDialogImpl */

CustomDialogImpl::CustomDialogImpl( QWidget * parent, Qt::WFlags f)  : QDialog( parent, f ), m_snipetModel( 0 ) {
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
	connect( m_pluginListView, SIGNAL(configurePlugin(PluginElement*)), this, SLOT(configurePlugin(PluginElement*)) );
	connect( m_pluginListView, SIGNAL(aboutPlugin(PluginElement*)), this, SLOT(aboutPlugin(PluginElement*)) );
	connect( m_scriptListView, SIGNAL(aboutPlugin(PluginElement*)), this, SLOT(aboutScript(PluginElement*)) );

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
}


void CustomDialogImpl::loadFromConfig( XINXConfig * config ) {
	Q_ASSERT( config );

	m_config = *config;
	showConfig();
}

void CustomDialogImpl::saveToConfig( XINXConfig * config ) {
	Q_ASSERT( config );

	storeConfig();
	*config = m_config;
}

void CustomDialogImpl::showConfig() {//m_specifiqueTableView
	// Application description path
	m_descriptionPathLineEdit->lineEdit()->setText( QDir::toNativeSeparators( m_config.config().descriptions.datas ) );

	// Language
	m_langComboBox->setCurrentIndex( 0 );
	for( int i = 0; i < m_langComboBox->count(); i++ ) {
		if( m_langComboBox->itemText( i ).contains( QString("(%1)").arg( m_config.config().language ) ) ) {
			m_langComboBox->setCurrentIndex( i );
			break;
		}
	}

	// Trace Log File
	m_traceLogWidget->lineEdit()->setText( QDir::toNativeSeparators( m_config.config().xinxTrace ) );

	// Style
	int currentStyle = m_styleComboBox->findText( m_config.config().style, Qt::MatchFixedString );
	m_styleComboBox->setCurrentIndex( currentStyle );

	// Create backup file when saving
	m_createBakCheckBox->setChecked( m_config.config().editor.createBackupFile );

	// Popup when file modified
	m_popupWhenFileModifiedCheckBox->setChecked( m_config.config().editor.popupWhenFileModified );

	// Pretty print on saving
	m_prettyPrintOnSavingCheckBox->setChecked( m_config.config().editor.autoindentOnSaving );

	// Tab close button
	if( m_config.config().editor.closeButtonOnEachTab ) {
		if( m_config.config().editor.hideCloseTab )
			m_closeBtnRadioButton->setChecked( true );
		else
			m_closeAndCornerBtnRadioButton->setChecked( true );
	} else
		m_cornerBtnRadioButton->setChecked( true );

	// Default text encoding.
	m_encodingComboBox->setCurrentIndex( m_encodingComboBox->findText( m_config.config().editor.defaultTextCodec ) );

	// Text content refresh
	m_refreshContentTimeoutSpinBox->setValue( m_config.config().editor.automaticModelRefreshTimeout );

	// Auto highlight text
	m_autoHighlightCheckBox->setChecked( m_config.config().editor.autoHighlight );

	// Color of highlighted text
	m_highlightTextColorComboBox->setColor( m_config.config().editor.highlightWord );

	// Show tabulation and space in the editor
	m_showTabulationCheckBox->setChecked( m_config.config().editor.showTabulationAndSpace );

	// Show current line
	m_showCurrentLineCheckBox->setChecked( m_config.config().editor.highlightCurrentLine );

	// Size of tabulation
	m_sizeOfTabSpinBox->setValue( m_config.config().editor.tabulationSize );

	// Font size
	m_fontSizeSpinBox->setValue( m_config.config().editor.defaultFormat.pointSize() );

	// Font name
	m_fontComboBox->setCurrentFont( m_config.config().editor.defaultFormat );

	// Refresh Directory Timeout
	m_refreshProjectTimoutSpinBox->setValue( m_config.config().project.automaticProjectDirectoryRefreshTimeout );

	// Open the last project at start
	m_openLastProjectCheckBox->setChecked( m_config.config().project.openTheLastProjectAtStart );

	// Save project with session
	m_saveSessionCheckBox->setChecked( m_config.config().project.saveWithSessionByDefault );

	// Close the version management log automaticaly.
	m_closeLogCheckBox->setChecked( m_config.config().project.closeVersionManagementLog );

	// Default project directory
	m_projectPathLineEdit->lineEdit()->setText( QDir::toNativeSeparators( m_config.config().project.defaultPath ) );

	// Default specifique path name
	m_lineEditDefaultProjectPathName->setText( QDir::toNativeSeparators( m_config.config().project.defaultProjectPathName ) );

	// Tools
	ToolsModelIndex * toolsModel = new ToolsModelIndex( &(m_config.config().tools), m_toolsTable );
	m_toolsTable->setModel( toolsModel );
	m_toolsTable->setItemDelegate( new DirectoryEditDelegate( m_toolsTable ) );
	m_toolsTable->horizontalHeader()->setResizeMode( 0, QHeaderView::ResizeToContents );
	m_toolsTable->horizontalHeader()->setResizeMode( 1, QHeaderView::Stretch );
	//m_toolsTable->resizeColumnsToContents();
	//QObject::connect( toolsModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), m_toolsTable, SLOT(resizeColumnsToContents()) );

	// CVS : Create Change Log automatically
	m_changeLogCheckBox->setChecked( m_config.config().rcs.createChangelog );

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
	m_highlighterComboBox->clear();
	m_highlighterComboBox->addItems( languages );
	m_highlighterComboBox->setCurrentIndex( 0 );
	on_m_highlighterComboBox_activated( m_highlighterComboBox->currentText() );

	// Extentions
	SpecifiqueModelIndex * specifiqueModel = new SpecifiqueModelIndex( &(m_config.config().files) , m_specifiqueTableView );
	m_specifiqueTableView->setModel( specifiqueModel );
	m_specifiqueTableView->resizeColumnsToContents();
	QObject::connect( specifiqueModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), m_specifiqueTableView, SLOT(resizeColumnsToContents()) );

	// Xml pres
	m_expandPathLineEdit->setText( m_config.config().xmlPres.autoExpandedPath );
	m_hideElementList->setValues( m_config.config().xmlPres.hidePath );
	m_showSubEltCheckBox->setChecked( m_config.config().xmlPres.showFilteredSubTree );
	m_showNameAttributeCheckBox->setChecked( m_config.config().xmlPres.showNameAttributeIfExists );
	m_viewColorBox->setColor( m_config.config().xmlPres.viewColor );
	m_errorColorBox->setColor( m_config.config().xmlPres.errorColor );
	m_screenColorBox->setColor( m_config.config().xmlPres.screenDataColor );

	// Snipet
	m_snipetModel = SnipetDatabaseManager::self()->createSnipetItemModel( m_snipetTreeView );
	m_snipetModel->select();
	new ModelTest( m_snipetModel, this );
	m_snipetTreeView->setModel( m_snipetModel );
	m_snipetTreeView->header()->setResizeMode( QHeaderView::ResizeToContents );
	m_snipetTreeView->header()->setResizeMode( 2, QHeaderView::Stretch );
	m_snipetTreeView->expandAll();
	m_snipetTreeView_selectionChanged();
	connect( m_snipetTreeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(m_snipetTreeView_selectionChanged()) );

	// Load Script
	m_scriptListView->clear();
	foreach( const ScriptValue & s, ScriptManager::self()->objects() ) {
		m_scriptListView->addPlugin( new ScriptElement( s ) );
	}

	// Plugins
	m_pluginListView->clear();
	foreach( XinxPluginElement * plugin, XinxPluginsLoader::self()->plugins() ) {
		XinxPluginElement * e = new XinxPluginElement( plugin->plugin(), plugin->isStatic() );
		e->setActivated( plugin->isActivated() );
		m_pluginListView->addPlugin( e );
	}
}

void CustomDialogImpl::storeConfig() {
	// Application description path
	m_config.setXinxDataFiles( QDir::fromNativeSeparators( m_descriptionPathLineEdit->lineEdit()->text() ) );

	// Language
	QRegExp exp("^\\((.*)\\).*$");
	if( exp.indexIn( m_langComboBox->currentText() ) >= 0 )
		m_config.config().language = exp.cap( 1 );

	// Trace Log File
	m_config.config().xinxTrace = QDir::fromNativeSeparators( m_traceLogWidget->lineEdit()->text() );

	// Style
	m_config.config().style = m_styleComboBox->currentText();

	// Create backup file when saving
	m_config.config().editor.createBackupFile = m_createBakCheckBox->isChecked();

	// Popup when file modified
	m_config.config().editor.popupWhenFileModified = m_popupWhenFileModifiedCheckBox->isChecked();

	// Pretty print on saving
	m_config.config().editor.autoindentOnSaving = m_prettyPrintOnSavingCheckBox->isChecked();

	// Tab close button
	if( m_closeBtnRadioButton->isChecked() ) {
		m_config.config().editor.closeButtonOnEachTab = true;
		m_config.config().editor.hideCloseTab = true;
	} else
	if( m_cornerBtnRadioButton->isChecked() ) {
		m_config.config().editor.closeButtonOnEachTab = false;
		m_config.config().editor.hideCloseTab = false;
	} else {
		m_config.config().editor.closeButtonOnEachTab = true;
		m_config.config().editor.hideCloseTab = false;
	}

	// Default text encoding.
	m_config.config().editor.defaultTextCodec = m_encodingComboBox->itemText( m_encodingComboBox->currentIndex() );

	// Text content refresh
	m_config.config().editor.automaticModelRefreshTimeout = m_refreshContentTimeoutSpinBox->value();

	// Auto highlight text
	m_config.config().editor.autoHighlight = m_autoHighlightCheckBox->isChecked();

	// Color of highlighted text
	m_config.config().editor.highlightWord = m_highlightTextColorComboBox->color();

	// Show tabulation and space in the editor
	m_config.config().editor.showTabulationAndSpace = m_showTabulationCheckBox->isChecked();

	// Show current line
	m_config.config().editor.highlightCurrentLine = m_showCurrentLineCheckBox->isChecked();

	// Size of tabulation
	m_config.config().editor.tabulationSize = m_sizeOfTabSpinBox->value();

	// Font name
	m_config.config().editor.defaultFormat = m_fontComboBox->currentFont();

	// Font size
	m_config.config().editor.defaultFormat.setPointSize( m_fontSizeSpinBox->value() );

	// Refresh Directory Timeout
	m_config.config().project.automaticProjectDirectoryRefreshTimeout = m_refreshProjectTimoutSpinBox->value();

	// Open the last project at start
	m_config.config().project.openTheLastProjectAtStart = m_openLastProjectCheckBox->isChecked();

	// Save project with session
	m_config.config().project.saveWithSessionByDefault = m_saveSessionCheckBox->isChecked();

	// Close the version management log automaticaly.
	m_config.config().project.closeVersionManagementLog = m_closeLogCheckBox->isChecked();

	// Default project directory
	m_config.config().project.defaultPath = QDir::fromNativeSeparators( m_projectPathLineEdit->lineEdit()->text() );

	// Default specifique path name
	m_config.config().project.defaultProjectPathName = QDir::fromNativeSeparators( m_lineEditDefaultProjectPathName->text() );

	// CVS : Create Change Log automatically
	m_config.config().rcs.createChangelog = m_changeLogCheckBox->isChecked();

	// Extentions
	SpecifiqueModelIndex * specifiqueModel = qobject_cast<SpecifiqueModelIndex*>( m_specifiqueTableView->model() );
	m_config.config().files.clear();
	m_config.config().files = specifiqueModel->extentions();

	// Xml pres
	m_config.config().xmlPres.autoExpandedPath = m_expandPathLineEdit->text();
	m_config.config().xmlPres.hidePath = m_hideElementList->values();
	m_config.config().xmlPres.showFilteredSubTree = m_showSubEltCheckBox->isChecked();
	m_config.config().xmlPres.viewColor = m_viewColorBox->color();
	m_config.config().xmlPres.errorColor = m_errorColorBox->color();
	m_config.config().xmlPres.screenDataColor = m_screenColorBox->color();
	m_config.config().xmlPres.showNameAttributeIfExists = m_showNameAttributeCheckBox->isChecked();

	// Syntax highlighter
	m_config.putFormatsSchemeToConfig();

	// Snipet
	// TODO
	//	SnipetListManager::self()->setSnipets( m_snipetModel->getSnipetList() );

	// Plugins
	foreach( PluginElement * plugin, m_pluginListView->plugins() ) {
		QString name = dynamic_cast<XinxPluginElement*>( plugin )->plugin()->metaObject()->className();
		bool isActivated = plugin->isActivated();

		m_config.config().plugins[ name ] = isActivated;
		if( XinxPluginsLoader::self()->plugin( name ) ) {
			XinxPluginsLoader::self()->plugin( name )->setActivated( isActivated );
		}
	}
}

void CustomDialogImpl::configurePlugin( PluginElement * plugin ) {
	Q_ASSERT( plugin );
	Q_ASSERT( dynamic_cast<XinxPluginElement*>( plugin ) );
	Q_ASSERT( dynamic_cast<IXinxPluginConfiguration*>( dynamic_cast<XinxPluginElement*>( plugin )->plugin() ) );

	XinxPluginElement * xinxPlugin = dynamic_cast<XinxPluginElement*>( plugin );
	IXinxPluginConfiguration * p = dynamic_cast<IXinxPluginConfiguration*>( xinxPlugin->plugin() );

	QDialog configureDialog;
	configureDialog.setWindowFlags( Qt::MSWindowsFixedSizeDialogHint | Qt::Dialog );

	QWidget * settings = p->createSettingsDialog();
	if( ! p->loadSettingsDialog( settings ) )
		QMessageBox::warning( this, tr("Plugin Customization"), tr("Can't load the plugin configuration") );

	QDialogButtonBox * buttonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );

	QVBoxLayout * vbox = new QVBoxLayout;
	vbox->addWidget( settings );
	vbox->addWidget( buttonBox );

	configureDialog.setLayout( vbox );

	connect( buttonBox, SIGNAL(accepted()), &configureDialog, SLOT(accept()) );
	connect( buttonBox, SIGNAL(rejected()), &configureDialog, SLOT(reject()) );

	if( configureDialog.exec() == QDialog::Accepted ) {
		if( ! p->saveSettingsDialog( settings ) )
			QMessageBox::warning( this, tr("Plugin Customization"), tr("Can't save the plugin configuration") );
	}
}

void CustomDialogImpl::aboutPlugin( PluginElement * plugin ) {
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


void CustomDialogImpl::aboutScript( PluginElement * plugin ) {
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
	                       .arg( script->author() )
	                       .arg( script->version() )
	                       .arg( script->licence() )
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

void CustomDialogImpl::m_descriptionPathLineEdit_textChanged( QString text ) {
	if( ! m_snipetModel ) return;
	SnipetList list;
	try {
		// TODO
		// m_snipetModel->clear();
		list.loadFromFile( QDir( text ).absoluteFilePath( "template.xml" ) );
		// TODO
		// m_snipetModel->loadSnipetList( list );
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
		//m_snipetModel->loadSnipetList( list );
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
		// TODO
		//m_snipetModel->addSnipet( s );
		m_snipetTreeView->expandAll();
	}
}

void CustomDialogImpl::on_m_removePushButton_clicked() {
	QModelIndexList indexes = m_snipetTreeView->selectionModel()->selectedRows();
	foreach( const QModelIndex & i, indexes ) {
		if( i.internalId() == -1 ) indexes.removeAll( i );
	}

	m_snipetModel->removeSnipet( indexes );
	m_snipetTreeView->expandAll();
}

void CustomDialogImpl::on_m_modifyPushButton_clicked() {
	QModelIndexList index = m_snipetTreeView->selectionModel()->selectedRows();
	Snipet s = index.at( 0 ).data( Qt::UserRole ).value<Snipet>();
	SnipetDialogImpl dlg( s, this );
	if( dlg.exec() ) {
		// TODO
		//m_snipetModel->removeSnipet( index );
		//m_snipetModel->addSnipet( dlg.getSnipet() );
		m_snipetTreeView->expandAll();
	}
}

void CustomDialogImpl::on_m_duplicatePushButton_clicked() {
	QModelIndexList index = m_snipetTreeView->selectionModel()->selectedRows();
	Snipet s = index.at( 0 ).data( Qt::UserRole ).value<Snipet>();
	SnipetDialogImpl dlg( s, this );
	if( dlg.exec() ) {
		// TODO
		//m_snipetModel->addSnipet( dlg.getSnipet() );
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
	m_customScheme->setFormatScheme( m_config.scheme( text ) );
	m_customScheme->setLanguageFactory( m_config.languageFactory() );
	m_customScheme->setLanguageDefinition( text );
	m_customScheme->setExample( XinxPluginsLoader::self()->exampleOfHighlighter( text ) );
}

void CustomDialogImpl::on_m_buttonBox_clicked( QAbstractButton * button ) {
	if( m_buttonBox->buttonRole( button ) == QDialogButtonBox::ResetRole ) {
		m_config.setDefault();
		showConfig();
	}
}

void CustomDialogImpl::on_m_labelLink_linkActivated( const QString & link ) {
	if( link == "#modules" )
		m_listWidget->setCurrentRow( 9 );
}
