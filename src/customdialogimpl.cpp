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

#include <QDir>
#include <QFileDialog>

#include "customdialogimpl.h"
#include "jshighlighter.h"
#include "xmlhighlighter.h"

/* PrivateCustomDialogImpl */

class PrivateCustomDialogImpl {
public:
	PrivateCustomDialogImpl( CustomDialogImpl * parent );

	XINXConfig m_config;
	QString m_previousFormat;
	SyntaxHighlighter * m_highlighter;
	
	void showConfig();
	void storeConfig();
private:
	CustomDialogImpl * m_parent;
};

PrivateCustomDialogImpl::PrivateCustomDialogImpl( CustomDialogImpl * parent ) {
	m_highlighter = 0;
	m_parent = parent;
}

void PrivateCustomDialogImpl::showConfig() {
	// Application description path
	m_parent->m_descriptionPathLineEdit->setText( m_config.config().descriptions.completion );

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
	
	// Compression level
	m_parent->m_completionLevelComboBox->setCurrentIndex( m_config.config().editor.completionLevel );
	
	// Show tabulation and space in the editor
	m_parent->m_showTabulationCheckBox->setChecked( m_config.config().editor.showTabulationAndSpace );
	
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
	
	// Default project directory
	m_parent->m_projectPathLineEdit->setText( m_config.config().project.defaultPath );
	
	// Default specifique path name
	m_parent->m_lineEditDefaultProjectPathName->setText( m_config.config().project.defaultProjectPathName );
	
	// CVS Tools
	m_parent->m_CVSToolsLineEdit->setText( m_config.config().tools[ "cvs" ] );
	
	// Diff Tools
	m_parent->m_mergeToolLineEdit->setText( m_config.config().tools[ "diff" ] );
	
	// CVS: Progress message
	if( m_config.config().cvs.progressMessages.isEmpty() ) 
		m_parent->m_cvsVerboseComboBox->setCurrentIndex( 0 );
	else
	if( m_config.config().cvs.progressMessages == "-q" ) 
		m_parent->m_cvsVerboseComboBox->setCurrentIndex( 1 );
	else
	if( m_config.config().cvs.progressMessages == "-Q" ) 
		m_parent->m_cvsVerboseComboBox->setCurrentIndex( 2 );
	
	// CVS: Compression
	m_parent->m_cvsCompressionComboBox->setCurrentIndex( m_config.config().cvs.compressionLevel );
	
	// CVS: Prune empty directories
	m_parent->m_cvsPruneCheckBox->setChecked( m_config.config().cvs.pruneEmptyDirectories );
	
	// CVS: Create any directories that exist in the repository
	m_parent->m_cvsCreateDirCheckBox->setChecked( m_config.config().cvs.createDirectories );
	
	// Syntax highlighter
	m_previousFormat = QString();
	m_parent->m_formatsListView->clear();
	foreach( QString key, m_config.config().formats.keys() ) {
		m_parent->m_formatsListView->addItem( key );
	}
	m_parent->m_formatsListView->setCurrentRow( 0 );
	
	// Extetions
	m_parent->m_extentionsListWidget->clear();
	foreach( QString key, m_config.config().files.keys() ) {
		m_parent->m_extentionsListWidget->addItem( m_config.filter( key ) );
	}
	m_parent->m_extentionsListWidget->setCurrentRow( 0 );
}

void PrivateCustomDialogImpl::storeConfig() {
	// Application description path
	m_config.config().descriptions.completion = m_parent->m_descriptionPathLineEdit->text();
	
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
	
	// Compression level
	m_config.config().editor.completionLevel = m_parent->m_completionLevelComboBox->currentIndex();
	
	// Show tabulation and space in the editor
	m_config.config().editor.showTabulationAndSpace = m_parent->m_showTabulationCheckBox->isChecked();
	
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
	
	// Default project directory
	m_config.config().project.defaultPath = m_parent->m_projectPathLineEdit->text();
	
	// Default specifique path name
	m_config.config().project.defaultProjectPathName = m_parent->m_lineEditDefaultProjectPathName->text();

	// CVS Tools
	m_config.config().tools[ "cvs" ] = m_parent->m_CVSToolsLineEdit->text();
	
	// Diff Tools
	m_config.config().tools[ "diff" ] = m_parent->m_mergeToolLineEdit->text();
	
	// CVS: Progress message
	switch( m_parent->m_cvsVerboseComboBox->currentIndex() ) {
	case 0: 
		m_config.config().cvs.progressMessages = "";
		break;
	case 1: 
		m_config.config().cvs.progressMessages = "-q";
		break;
	case 2: 
		m_config.config().cvs.progressMessages = "-Q";
		break;
	}

	// CVS: Compression
	m_config.config().cvs.compressionLevel = m_parent->m_cvsCompressionComboBox->currentIndex();
	
	// CVS: Prune empty directories
	m_config.config().cvs.pruneEmptyDirectories = m_parent->m_cvsPruneCheckBox->isChecked();
	
	// CVS: Create any directories that exist in the repository
	m_config.config().cvs.createDirectories = m_parent->m_cvsCreateDirCheckBox->isChecked();
}

/* CustomDialogImpl */

CustomDialogImpl::CustomDialogImpl( QWidget * parent, Qt::WFlags f)  : QDialog(parent, f) {
	d = new PrivateCustomDialogImpl( this );

	setupUi( this );
	
	QFont font( "Monospace", 8 );
	font.setFixedPitch( true );
  
	m_exempleTextEdit->setFont( font );
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
/*
void CustomDialogImpl::loadFromConfig( XinxAppSettings * config ) {
	Q_ASSERT( config );
	
	d->m_config = *config;

	m_alertStandardCheckBox->setChecked( d->m_config.config().editor.alertWhenSavingStandardFile );
	m_saveSessionCheckBox->setChecked( d->m_config.config().project.saveWithSessionByDefault );
	m_createBakCheckBox->setChecked( d->m_config.config().editor.createBackupFile );
	m_popupWhenFileModifiedCheckBox->setChecked( d->m_config.config().editor.popupWhenFileModified );

	m_cvsCompressionComboBox->setCurrentIndex( d->m_config.config().cvs.compressionLevel );
	m_cvsPruneCheckBox->setChecked( d->m_config.config().cvs.pruneEmptyDirectories );
	if( d->m_config.config().cvs.progressMessages.isEmpty() ) 
		m_cvsVerboseComboBox->setCurrentIndex( 0 );
	else
	if( d->m_config.config().cvs.progressMessages == "-q" ) 
		m_cvsVerboseComboBox->setCurrentIndex( 1 );
	else
	if( d->m_config.config().cvs.progressMessages == "-Q" ) 
		m_cvsVerboseComboBox->setCurrentIndex( 2 );
	m_cvsCreateDirCheckBox->setChecked( d->m_config.config().cvs.createDirectories );

	int index = -1;
	for( int i = 0; i < m_langComboBox->count(); i++ ) {
		if( m_langComboBox->itemText( i ).contains( QString("(%1)").arg( d->m_config.config().language ) ) )
			index = i;
	}
	if( index == -1 ) 
		m_langComboBox->setCurrentIndex( 0 );
	else
		m_langComboBox->setCurrentIndex( index );
	
	m_projectPathLineEdit->setText( d->m_config.config().project.defaultPath );
	m_objectDescriptionPathLineEdit->setText( d->m_config.config().descriptions.completion );
	
	m_fileTypeComboBox->clear();
	foreach( struct XINXConfig::managedFile file, d->m_config.managedFile() ) {
		m_fileTypeComboBox->addItem( QString("%1 (%2)").arg( file.name ).arg( file.extentions ) );
	}

	m_syntaxFileTypeComboBox->clear();
	foreach( QString cle, d->m_config.managedStructure().keys() ) {
		m_syntaxFileTypeComboBox->addItem( cle );
	}
	
	m_CVSToolsLineEdit->setText( d->m_config.toolsPath()[ "cvs" ] );
	m_mergeToolLineEdit->setText( d->m_config.toolsPath()[ "merge" ] );
	
	m_lineEditDefaultProjectPathName->setText( d->m_config.getDefaultProjectPathName() );
}

void CustomDialogImpl::saveToConfig( XinxAppSettings * config ) {
	Q_ASSERT( config );
	
	*config = d->m_config;
}

void CustomDialogImpl::on_m_lineEditDefaultProjectPathName_textChanged( QString text ) {
	d->m_config.setDefaultProjectPathName( text );
}

void CustomDialogImpl::on_m_syntaxTypeComboBox_currentIndexChanged( QString text ){
	QTextCharFormat format = d->m_config.managedStructure()[ m_syntaxFileTypeComboBox->currentText() ].color[ text ];
	m_italicCheckBox->setChecked( format.fontItalic() );
	m_underlineCheckBox->setChecked( format.fontUnderline() );
	m_StrikeoutCheckBox->setChecked( format.fontStrikeOut() );
	m_boldCheckBox->setChecked( format.fontWeight() != QFont::Normal );
	
	m_colorComboBox->setColor( format.foreground().color() );
	m_fontSizeSpinBox->setValue( format.fontPointSize() );
	if( ! format.fontFamily().isEmpty() ) 
		m_fontComboBox->setCurrentFont( format.font() );
	else
		m_fontComboBox->setCurrentFont( m_exempleTextEdit->font() );
}


void CustomDialogImpl::on_m_syntaxFileTypeComboBox_currentIndexChanged( QString text ) {
	m_exempleTextEdit->setText( d->m_config.managedStructure()[ text ].example );
	m_syntaxTypeComboBox->clear();
	foreach( QString name, d->m_config.managedStructure()[ text ].color.keys() ) {
		m_syntaxTypeComboBox->addItem( name );
	}
	m_syntaxTypeComboBox->setCurrentIndex( 0 );
	
	if( d->m_highlighter ) { delete d->m_highlighter; d->m_highlighter = NULL; };
	if( text == "js" ) {
		d->m_highlighter = new JsHighlighter( m_exempleTextEdit->document() );
	} else if( text == "xml" ) {
		d->m_highlighter = new XmlHighlighter( m_exempleTextEdit->document() );
	}
}

void CustomDialogImpl::on_m_fileTypeComboBox_currentIndexChanged( int index ) {
	if( index >= 0 ) {
		m_fileTypePath->setText( d->m_config.managedFile()[ index ].customPath );
		m_specifiqueCheckBox->setChecked( d->m_config.managedFile()[ index ].canBeCustomize );
	}
}


void CustomDialogImpl::on_m_underlineCheckBox_toggled( bool checked ) {
	if( d->m_highlighter ) {
		QTextCharFormat format = d->m_config.managedStructure()[ m_syntaxFileTypeComboBox->currentText() ].color[ m_syntaxTypeComboBox->currentText() ]; 
		format.setFontUnderline( checked );
		d->m_highlighter->setHighlightFormat( m_syntaxTypeComboBox->currentText(), format );
		d->m_config.managedStructure()[ m_syntaxFileTypeComboBox->currentText() ].color[ m_syntaxTypeComboBox->currentText() ] = format;	
	}
}

void CustomDialogImpl::on_m_StrikeoutCheckBox_toggled( bool checked ) {
	if( d->m_highlighter ) {
		QTextCharFormat format = d->m_config.managedStructure()[ m_syntaxFileTypeComboBox->currentText() ].color[ m_syntaxTypeComboBox->currentText() ]; 
		format.setFontStrikeOut( checked );
		d->m_highlighter->setHighlightFormat( m_syntaxTypeComboBox->currentText(), format );
		d->m_config.managedStructure()[ m_syntaxFileTypeComboBox->currentText() ].color[ m_syntaxTypeComboBox->currentText() ] = format;	
	}
}

void CustomDialogImpl::on_m_italicCheckBox_toggled( bool checked ) {
	if( d->m_highlighter ) {
		QTextCharFormat format = d->m_config.managedStructure()[ m_syntaxFileTypeComboBox->currentText() ].color[ m_syntaxTypeComboBox->currentText() ]; 
		format.setFontItalic( checked );
		d->m_highlighter->setHighlightFormat( m_syntaxTypeComboBox->currentText(), format );
		d->m_config.managedStructure()[ m_syntaxFileTypeComboBox->currentText() ].color[ m_syntaxTypeComboBox->currentText() ] = format;	
	}
}

void CustomDialogImpl::on_m_boldCheckBox_toggled( bool checked ) {
	if( d->m_highlighter ) {
		QTextCharFormat format = d->m_config.managedStructure()[ m_syntaxFileTypeComboBox->currentText() ].color[ m_syntaxTypeComboBox->currentText() ]; 
		format.setFontWeight( checked ? QFont::Bold : QFont::Normal );
		d->m_highlighter->setHighlightFormat( m_syntaxTypeComboBox->currentText(), format );
		d->m_config.managedStructure()[ m_syntaxFileTypeComboBox->currentText() ].color[ m_syntaxTypeComboBox->currentText() ] = format;	
	}
}

void CustomDialogImpl::on_m_fontComboBox_currentFontChanged( QFont f ) {
	if( d->m_highlighter ) {
		QTextCharFormat format = d->m_config.managedStructure()[ m_syntaxFileTypeComboBox->currentText() ].color[ m_syntaxTypeComboBox->currentText() ]; 
		format.setFontFamily( f.family() );
		d->m_highlighter->setHighlightFormat( m_syntaxTypeComboBox->currentText(), format );
		d->m_config.managedStructure()[ m_syntaxFileTypeComboBox->currentText() ].color[ m_syntaxTypeComboBox->currentText() ] = format;	
	}
}

void CustomDialogImpl::on_m_colorComboBox_activated( QColor c ) {
	if( d->m_highlighter ) {
		QTextCharFormat format = d->m_config.managedStructure()[ m_syntaxFileTypeComboBox->currentText() ].color[ m_syntaxTypeComboBox->currentText() ]; 
		format.setForeground( c );
		d->m_highlighter->setHighlightFormat( m_syntaxTypeComboBox->currentText(), format );
		d->m_config.managedStructure()[ m_syntaxFileTypeComboBox->currentText() ].color[ m_syntaxTypeComboBox->currentText() ] = format;	
	}
}

void CustomDialogImpl::on_m_fontSizeSpinBox_valueChanged( double value ) {
	if( d->m_highlighter ) {
		QTextCharFormat format = d->m_config.managedStructure()[ m_syntaxFileTypeComboBox->currentText() ].color[ m_syntaxTypeComboBox->currentText() ]; 
		format.setFontPointSize( value );
		d->m_highlighter->setHighlightFormat( m_syntaxTypeComboBox->currentText(), format );
		d->m_config.managedStructure()[ m_syntaxFileTypeComboBox->currentText() ].color[ m_syntaxTypeComboBox->currentText() ] = format;	
	}
}

void CustomDialogImpl::on_m_projectPathLineEdit_textChanged( QString path ) {
	QDir dir ( path );
	QPalette palette( m_projectPathLineEdit->palette() );
	
	if( dir.exists() ) {
		palette.setColor( QPalette::Text, QColor() );
	} else {
		palette.setColor( QPalette::Text, Qt::red );
	}
	m_projectPathLineEdit->setPalette( palette );

	d->m_config.setXinxProjectPath( path );
}

void CustomDialogImpl::on_m_fileTypePath_textChanged( QString path ) {
	int index = m_fileTypeComboBox->currentIndex();
	if( index >= 0 ) {
		d->m_config.managedFile()[ index ].customPath = path;
	}
}

void CustomDialogImpl::on_m_specifiqueCheckBox_toggled( bool checked ) {
	int index = m_fileTypeComboBox->currentIndex();
	if( index >= 0 ) {
		d->m_config.managedFile()[ index ].canBeCustomize = checked;
	}
}

void CustomDialogImpl::on_m_createBakCheckBox_toggled( bool checked ) {
	d->m_config.setCreateBackupFile( checked );
}

void CustomDialogImpl::on_m_alertStandardCheckBox_toggled( bool checked ) {
	d->m_config.setAlertWhenStdFile( checked );
}

void CustomDialogImpl::on_m_langComboBox_currentIndexChanged( QString lang ) {
	QRegExp exp("^\\((.*)\\).*$");
	if( exp.indexIn( lang ) >= 0 )
		d->m_config.setLang( exp.cap( 1 ) );
}

void CustomDialogImpl::on_m_objectDescriptionPathLineEdit_textChanged( QString path ) {
	QDir dir ( path );
	QPalette palette( m_objectDescriptionPathLineEdit->palette() );
	
	if( dir.exists() ) {
		palette.setColor( QPalette::Text, QColor() );
	} else {
		palette.setColor( QPalette::Text, Qt::red );
	}
	m_objectDescriptionPathLineEdit->setPalette( palette );

	d->m_config.setObjectDescriptionPath( path );
}


void CustomDialogImpl::on_m_changeObjectPathBtn_clicked() {
	QString value = QFileDialog::getExistingDirectory( this, tr("Object path"), m_objectDescriptionPathLineEdit->text() );
	if( ! value.isEmpty() ) {
		m_objectDescriptionPathLineEdit->setText( value );		
	}
}

void CustomDialogImpl::on_m_changeProjectPathBtn_clicked() {
	QString value = QFileDialog::getExistingDirectory( this, tr("XINX Project path"), m_projectPathLineEdit->text() );
	if( ! value.isEmpty() ) {
		m_projectPathLineEdit->setText( value );		
	}
}


void CustomDialogImpl::on_m_saveSessionCheckBox_toggled(bool checked) {
	d->m_config.setSaveSessionByDefault( checked );
}

void CustomDialogImpl::on_m_mergeToolLineEdit_textChanged( QString path ) {
	d->m_config.toolsPath()[ "merge" ] = path;
}

void CustomDialogImpl::on_m_mergeToolButton_clicked() {
	QString value = QFileDialog::getOpenFileName( this, tr("Merge path"), m_mergeToolLineEdit->text() );
	if( ! value.isEmpty() ) {
		m_mergeToolLineEdit->setText( value );		
	}
}

void CustomDialogImpl::on_m_CVSToolsLineEdit_textChanged(QString path) {
	d->m_config.toolsPath()[ "cvs" ] = path;
}


void CustomDialogImpl::on_m_cvsToolButton_clicked() {
	QString value = QFileDialog::getOpenFileName( this, tr("CVS path"), m_CVSToolsLineEdit->text() );
	if( ! value.isEmpty() ) {
		m_CVSToolsLineEdit->setText( value );		
	}
}

void CustomDialogImpl::on_m_popupWhenFileModifiedCheckBox_toggled(bool checked) {
	d->m_config.setPopupWhenFileModified( checked );
}

void CustomDialogImpl::on_m_cvsCompressionComboBox_currentIndexChanged(int index) {
	d->m_config.setCVSCompressionLevel( index );
}

void CustomDialogImpl::on_m_cvsPruneCheckBox_toggled(bool checked) {
	d->m_config.setCVSPruneEmptyDirectories( checked );
}

void CustomDialogImpl::on_m_cvsVerboseComboBox_currentIndexChanged(int index) {
	switch( index ) {
	case 0: 
		d->m_config.setCVSProgressMessages( "" );
		break;
	case 1: 
		d->m_config.setCVSProgressMessages( "-q" );
		break;
	case 2: 
		d->m_config.setCVSProgressMessages( "-Q" );
		break;
	}
}

void CustomDialogImpl::on_m_cvsCreateDirCheckBox_toggled(bool checked) {
	d->m_config.setCVSCreateDirectories( checked );
}
*/

void CustomDialogImpl::on_m_mergeToolBtn_clicked() {
	QString value = QFileDialog::getOpenFileName( this, tr("Diff path"), m_mergeToolLineEdit->text() );
	if( ! value.isEmpty() ) {
		m_mergeToolLineEdit->setText( value );		
	}
}

void CustomDialogImpl::on_m_cvsToolBtn_clicked() {
	QString value = QFileDialog::getOpenFileName( this, tr("CVS Path"), m_CVSToolsLineEdit->text() );
	if( ! value.isEmpty() ) {
		m_CVSToolsLineEdit->setText( value );		
	}
}

void CustomDialogImpl::on_m_changeProjectPathBtn_clicked() {
	QString value = QFileDialog::getExistingDirectory( this, tr("Project path"), m_projectPathLineEdit->text() );
	if( ! value.isEmpty() ) {
		m_projectPathLineEdit->setText( value );		
	}
}

void CustomDialogImpl::on_m_changeApplicationDescriptionPathBtn_clicked() {
	QString value = QFileDialog::getExistingDirectory( this, tr("Descriptions path"), m_descriptionPathLineEdit->text() );
	if( ! value.isEmpty() ) {
		m_descriptionPathLineEdit->setText( value );		
	}
}

void CustomDialogImpl::on_m_formatsListView_currentRowChanged( int currentRow ) {
	if( currentRow < 0 ) return ;
	QString token = m_formatsListView->item( currentRow )->text(), format;
	int posUnderline = token.indexOf( "_" );
	format = token.left( posUnderline );
	// Example
	if( format != d->m_previousFormat ) {
		if( d->m_highlighter ) { delete d->m_highlighter; d->m_highlighter = NULL; };
		if( format == "js" ) {
			d->m_highlighter = new JsHighlighter( m_exempleTextEdit->document(), &(d->m_config) );
			m_exempleTextEdit->setText(
				"/**\n"
				" * This is a comment\n"
				"**/\n"
				"\n"
				"function myfunction( param1, param2, param3 ) {\n"
				"	var variable = window.open('number' + 56 + \"othertext\",'frame','options');\n"
				"	alert( param& );\n"
				"	variable.close();\n"
				"}\n"
				);
		} else 
		if( format == "xml" ) {
			d->m_highlighter = new XmlHighlighter( m_exempleTextEdit->document(), &(d->m_config) );
			m_exempleTextEdit->setText(
				"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
				"<!-- This is a comment -->\n"
				"<xsl:stylesheet version=\"1.0\">\n"
				"\t<xsl:import href=\"myimport.xsl\"/>\n"
				"\t<xsl:variable name=\"MYVARIABLE\"/>\n"
				"\t<xsl:template match=\"/\">\n"
				"\t\t<xsl:comment> This is a text </xsl:comment>>>>>>>\n"
				"\n"
				"\t\t<xsl:choose>\n"
				"\t\t\t<xsl:when test=\"test\">\n"
				"\t\t\t\t<input type=\"hidden\" value=\"{$MYVARIABLE}\"/>\n"
				"\t\t\t</xsl:when>\n"
				"\t\t\t<xsl:otherwise>Otherwise not</xsl:otherwise>\n"
				"\t\t</xsl:choose>\n"
				"\t</xsl:template>\n"
				"</xsl:stylesheet>\n"
				);
		}
	}

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
	
	d->m_previousFormat = format;
}

void CustomDialogImpl::on_m_colorComboBox_activated(QColor color) {
	QString token = m_formatsListView->item( m_formatsListView->currentRow() )->text();
	d->m_config.config().formats[ token ].setForeground( color );
	if( d->m_highlighter )
		d->m_highlighter->rehighlight();
}

void CustomDialogImpl::on_m_boldCheckBox_toggled(bool checked) {
	QString token = m_formatsListView->item( m_formatsListView->currentRow() )->text();
	d->m_config.config().formats[ token ].setFontWeight( checked ? QFont::Bold : QFont::Normal );
	if( d->m_highlighter )
		d->m_highlighter->rehighlight();
}

void CustomDialogImpl::on_m_italicCheckBox_toggled(bool checked) {
	QString token = m_formatsListView->item( m_formatsListView->currentRow() )->text();
	d->m_config.config().formats[ token ].setFontItalic( checked );
	if( d->m_highlighter )
		d->m_highlighter->rehighlight();
}

void CustomDialogImpl::on_m_StrikeoutCheckBox_toggled(bool checked) {
	QString token = m_formatsListView->item( m_formatsListView->currentRow() )->text();
	d->m_config.config().formats[ token ].setFontStrikeOut( checked );
	if( d->m_highlighter )
		d->m_highlighter->rehighlight();
}

void CustomDialogImpl::on_m_underlineCheckBox_toggled(bool checked) {
	QString token = m_formatsListView->item( m_formatsListView->currentRow() )->text();
	d->m_config.config().formats[ token ].setFontUnderline( checked );
	if( d->m_highlighter )
		d->m_highlighter->rehighlight();
}

void CustomDialogImpl::on_m_extentionsListWidget_currentRowChanged(int currentRow) {
	if( currentRow < 0 ) return ;
	QRegExp exp("^.*\\(\\*\\.(.*)\\).*$");
	QString suffix, currentText = m_extentionsListWidget->item( currentRow )->text();
	int index = exp.indexIn( currentText );

	if( index >= 0 ) {
		suffix = exp.cap( 1 );
		m_canBeSpecifiqueCheckBox->setChecked( d->m_config.config().files[ suffix ].canBeSpecifique );
		m_subDirectoryLineEdit->setText( d->m_config.config().files[ suffix ].customPath );
	}
}

void CustomDialogImpl::on_m_canBeSpecifiqueCheckBox_toggled(bool checked) {
	QRegExp exp("^.*\\(\\*\\.(.*)\\).*$");
	QString suffix, currentText = m_extentionsListWidget->item( m_extentionsListWidget->currentRow() )->text();
	int index = exp.indexIn( currentText );

	if( index >= 0 ) {
		suffix = exp.cap( 1 );
		d->m_config.config().files[ suffix ].canBeSpecifique = checked;
	}
}

void CustomDialogImpl::on_m_subDirectoryLineEdit_textChanged(QString text) {
	QRegExp exp("^.*\\(\\*\\.(.*)\\).*$");
	QString suffix, currentText = m_extentionsListWidget->item( m_extentionsListWidget->currentRow() )->text();
	int index = exp.indexIn( currentText );

	if( index >= 0 ) {
		suffix = exp.cap( 1 );
		d->m_config.config().files[ suffix ].customPath = text;
	}
}

void CustomDialogImpl::on_m_defaultPushButton_clicked() {
	d->m_config.setDefault();
	d->showConfig();
}
