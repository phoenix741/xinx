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

// Xinx header
#include "customdialogimpl.h"
#include "syntaxhighlighter.h"
#include "exceptions.h"
#include "globals.h"
#include "xinxpluginsloader.h"

// Qt header
#include <QDir>
#include <QFileDialog>

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
	m_parent->m_descriptionPathLineEdit->setText( QDir::toNativeSeparators( m_config.config().descriptions.completion ) );

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
	
	// CVS Tools
	m_parent->m_CVSToolsLineEdit->setText( QDir::toNativeSeparators( m_config.config().tools[ "cvs" ] ) );
	
	// Diff Tools
	m_parent->m_mergeToolLineEdit->setText( QDir::toNativeSeparators( m_config.config().tools[ "diff" ] ) );
	
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
		m_parent->m_extentionsListWidget->addItem( global.m_pluginsLoader->filter( key ) );
	}
	m_parent->m_extentionsListWidget->setCurrentRow( 0 );
}

void PrivateCustomDialogImpl::storeConfig() {
	// Application description path
	m_config.config().descriptions.completion = QDir::fromNativeSeparators( m_parent->m_descriptionPathLineEdit->text() );
	
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

	// CVS Tools
	m_config.config().tools[ "cvs" ] = QDir::fromNativeSeparators( m_parent->m_CVSToolsLineEdit->text() );
	
	// Diff Tools
	m_config.config().tools[ "diff" ] = QDir::fromNativeSeparators( m_parent->m_mergeToolLineEdit->text() );
	
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
}

/* CustomDialogImpl */

CustomDialogImpl::CustomDialogImpl( QWidget * parent, Qt::WFlags f)  : QDialog( parent, f ) {
	d = new PrivateCustomDialogImpl( this );

	setupUi( this );
	
	QFont font( "Monospace", 8 );
	font.setFixedPitch( true );
  
	m_exempleTextEdit->setFont( font );
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
		QString example;
		foreach( SyntaxHighlighterInterface * interface, global.m_pluginsLoader->syntaxPlugins() ) {
			example = interface->exampleOfHighlighter( format );
			if( ! example.isEmpty() )
				break;
		}
		if( format == "js" ) {
			d->m_highlighter = new SyntaxHighlighter( m_exempleTextEdit->document(), "JS", &(d->m_config) );
		} else 
		if( format == "xml" ) {
			d->m_highlighter = new SyntaxHighlighter( m_exempleTextEdit->document(), "XML", &(d->m_config) );
		}
		m_exempleTextEdit->setText( example );
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
		m_subDirectoryLineEdit->setText( QDir::toNativeSeparators( d->m_config.config().files[ suffix ].customPath ) );
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
		d->m_config.config().files[ suffix ].customPath = QDir::fromNativeSeparators( text );
	}
}

void CustomDialogImpl::on_m_defaultPushButton_clicked() {
	d->m_config.setDefault();
	d->showConfig();
}
