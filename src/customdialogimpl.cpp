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
	SyntaxHighlighter * m_highlighter;	
private:
	CustomDialogImpl * m_parent;
};

PrivateCustomDialogImpl::PrivateCustomDialogImpl( CustomDialogImpl * parent ) {
	m_highlighter = 0;
	m_parent = parent;
}

/* CustomDialogImpl */

//
CustomDialogImpl::CustomDialogImpl( QWidget * parent, Qt::WFlags f)  : QDialog(parent, f) {
	d = new PrivateCustomDialogImpl( this );
	setupUi(this);
	
	QFont font( "Monospace", 8 );
	font.setFixedPitch( true );
  
	m_exempleTextEdit->setFont( font );
}
//

void CustomDialogImpl::loadFromConfig( XINXConfig * config ) {
	d->m_config = *config;

	m_alertStandardCheckBox->setChecked( d->m_config.isAlertWhenStdFile() );
	m_createBakCheckBox->setChecked( d->m_config.isCreateBackupFile() );
	m_saveSessionCheckBox->setChecked( d->m_config.saveSessionByDefault() );
	m_popupWhenFileModifiedCheckBox->setChecked( d->m_config.popupWhenFileModified() );

	m_cvsCompressionComboBox->setCurrentIndex( d->m_config.cvsCompressionLevel() );
	m_cvsPruneCheckBox->setChecked( d->m_config.cvsPruneEmptyDirectories() );
	if( d->m_config.cvsProgressMessages().isEmpty() ) 
		m_cvsVerboseComboBox->setCurrentIndex( 0 );
	else
	if( d->m_config.cvsProgressMessages() == "-q" ) 
		m_cvsVerboseComboBox->setCurrentIndex( 1 );
	else
	if( d->m_config.cvsProgressMessages() == "-Q" ) 
		m_cvsVerboseComboBox->setCurrentIndex( 2 );
	m_cvsCreateDirCheckBox->setChecked( d->m_config.cvsCreateDirectories() );


	int index = -1;
	for( int i = 0; i < m_langComboBox->count(); i++ ) {
		if( m_langComboBox->itemText( i ).contains( QString("(%1)").arg( d->m_config.lang() ) ) )
			index = i;
	}
	if( index == -1 ) 
		m_langComboBox->setCurrentIndex( 0 );
	else
		m_langComboBox->setCurrentIndex( index );
	
	m_projectPathLineEdit->setText( d->m_config.xinxProjectPath() );
	m_objectDescriptionPathLineEdit->setText( d->m_config.objectDescriptionPath() );
	
	m_fileTypeComboBox->clear();
	foreach( struct XINXConfig::managedFile file, d->m_config.managedFile() ) {
		m_fileTypeComboBox->addItem( QString("%1 (%2)").arg( file.name ).arg( file.extentions ) );
	}

	m_syntaxFileTypeComboBox->clear();
	foreach( QString cle, d->m_config.managedStructure().keys() ) {
		m_syntaxFileTypeComboBox->addItem( cle );
	}
	
	m_CVSToolsLineEdit->setText( d->m_config.toolsPath()[ "cvs" ] );
	
	m_lineEditDefaultProjectPathName->setText( d->m_config.getDefaultProjectPathName() );
}

void CustomDialogImpl::saveToConfig( XINXConfig * config ) {
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
	m_fontSizeSpinBox->setValue( format.fontPointSize() );
	
	m_colorComboBox->setColor( format.foreground().color() );
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

