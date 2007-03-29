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

/* CustomDialogImpl */

//
CustomDialogImpl::CustomDialogImpl( QWidget * parent, Qt::WFlags f)  : QDialog(parent, f), m_highlighter(0) {
	setupUi(this);
	
	QFont font( "Monospace", 8 );
	font.setFixedPitch( true );
  
	m_exempleTextEdit->setFont( font );
}
//

void CustomDialogImpl::loadFromConfig( XINXConfig * config ) {
	m_config = *config;

	m_alertStandardCheckBox->setChecked( m_config.isAlertWhenStdFile() );
	m_createBakCheckBox->setChecked( m_config.isCreateBackupFile() );
	int index = -1;
	for( int i = 0; i < m_langComboBox->count(); i++ ) {
		if( m_langComboBox->itemText( i ).contains( QString("(%1)").arg( m_config.lang() ) ) )
			index = i;
	}
	if( index == -1 ) 
		m_langComboBox->setCurrentIndex( 0 );
	else
		m_langComboBox->setCurrentIndex( index );
	
	m_projectPathLineEdit->setText( m_config.xinxProjectPath() );
	m_objectDescriptionPathLineEdit->setText( m_config.objectDescriptionPath() );
	
	m_fileTypeComboBox->clear();
	foreach( struct XINXConfig::managedFile file, m_config.managedFile() ) {
		m_fileTypeComboBox->addItem( QString("%1 (%2)").arg( file.name ).arg( file.extentions ) );
	}

	m_syntaxFileTypeComboBox->clear();
	foreach( QString cle, m_config.managedStructure().keys() ) {
		m_syntaxFileTypeComboBox->addItem( cle );
	}
}

void CustomDialogImpl::saveToConfig( XINXConfig * config ) {
	*config = m_config;
}

void CustomDialogImpl::on_m_syntaxTypeComboBox_currentIndexChanged( QString text ){
	QTextCharFormat format = m_config.managedStructure()[ m_syntaxFileTypeComboBox->currentText() ].color[ text ];
	m_italicCheckBox->setChecked( format.fontItalic() );
	m_underlineCheckBox->setChecked( format.fontUnderline() );
	m_StrikeoutCheckBox->setChecked( format.fontStrikeOut() );
	m_boldCheckBox->setChecked( format.fontWeight() != QFont::Normal );
	
	m_colorComboBox->setColor( format.foreground().color() );
	if( ! format.fontFamily().isEmpty() ) 
		m_fontComboBox->setCurrentFont( format.font() );
	else
		m_fontComboBox->setCurrentFont( m_exempleTextEdit->font() );
}


void CustomDialogImpl::on_m_syntaxFileTypeComboBox_currentIndexChanged( QString text ) {
	m_exempleTextEdit->setText( m_config.managedStructure()[ text ].example );
	m_syntaxTypeComboBox->clear();
	foreach( QString name, m_config.managedStructure()[ text ].color.keys() ) {
		m_syntaxTypeComboBox->addItem( name );
	}
	m_syntaxTypeComboBox->setCurrentIndex( 0 );
	
	if( m_highlighter ) { delete m_highlighter; m_highlighter = NULL; };
	if( text == "js" ) {
		m_highlighter = new JsHighlighter( m_exempleTextEdit->document() );
	} else if( text == "xml" ) {
		m_highlighter = new XmlHighlighter( m_exempleTextEdit->document() );
	}
}

void CustomDialogImpl::on_m_fileTypeComboBox_currentIndexChanged( int index ) {
	if( index >= 0 ) {
		m_fileTypePath->setText( m_config.managedFile()[ index ].customPath );
		m_specifiqueCheckBox->setChecked( m_config.managedFile()[ index ].canBeCustomize );
	}
}


void CustomDialogImpl::on_m_underlineCheckBox_toggled( bool checked ) {
	QTextCharFormat format = m_config.managedStructure()[ m_syntaxFileTypeComboBox->currentText() ].color[ m_syntaxTypeComboBox->currentText() ]; 
	format.setFontUnderline( checked );
	m_highlighter->setHighlightFormat( m_syntaxTypeComboBox->currentText(), format );
	m_config.managedStructure()[ m_syntaxFileTypeComboBox->currentText() ].color[ m_syntaxTypeComboBox->currentText() ] = format;	
}

void CustomDialogImpl::on_m_StrikeoutCheckBox_toggled( bool checked ) {
	QTextCharFormat format = m_config.managedStructure()[ m_syntaxFileTypeComboBox->currentText() ].color[ m_syntaxTypeComboBox->currentText() ]; 
	format.setFontStrikeOut( checked );
	m_highlighter->setHighlightFormat( m_syntaxTypeComboBox->currentText(), format );
	m_config.managedStructure()[ m_syntaxFileTypeComboBox->currentText() ].color[ m_syntaxTypeComboBox->currentText() ] = format;	
}

void CustomDialogImpl::on_m_italicCheckBox_toggled( bool checked ) {
	QTextCharFormat format = m_config.managedStructure()[ m_syntaxFileTypeComboBox->currentText() ].color[ m_syntaxTypeComboBox->currentText() ]; 
	format.setFontItalic( checked );
	m_highlighter->setHighlightFormat( m_syntaxTypeComboBox->currentText(), format );
	m_config.managedStructure()[ m_syntaxFileTypeComboBox->currentText() ].color[ m_syntaxTypeComboBox->currentText() ] = format;	
}

void CustomDialogImpl::on_m_boldCheckBox_toggled( bool checked ) {
	QTextCharFormat format = m_config.managedStructure()[ m_syntaxFileTypeComboBox->currentText() ].color[ m_syntaxTypeComboBox->currentText() ]; 
	format.setFontWeight( checked ? QFont::Bold : QFont::Normal );
	m_highlighter->setHighlightFormat( m_syntaxTypeComboBox->currentText(), format );
	m_config.managedStructure()[ m_syntaxFileTypeComboBox->currentText() ].color[ m_syntaxTypeComboBox->currentText() ] = format;	
}

void CustomDialogImpl::on_m_fontComboBox_currentFontChanged( QFont f ) {
	if( m_highlighter ) {
		QTextCharFormat format = m_config.managedStructure()[ m_syntaxFileTypeComboBox->currentText() ].color[ m_syntaxTypeComboBox->currentText() ]; 
		format.setFontFamily( f.family() );
		m_highlighter->setHighlightFormat( m_syntaxTypeComboBox->currentText(), format );
		m_config.managedStructure()[ m_syntaxFileTypeComboBox->currentText() ].color[ m_syntaxTypeComboBox->currentText() ] = format;	
	}
}

void CustomDialogImpl::on_m_colorComboBox_activated( QColor c ) {
	if( m_highlighter ) {
		QTextCharFormat format = m_config.managedStructure()[ m_syntaxFileTypeComboBox->currentText() ].color[ m_syntaxTypeComboBox->currentText() ]; 
		format.setForeground( c );
		m_highlighter->setHighlightColor( m_syntaxTypeComboBox->currentText(), c );
		m_config.managedStructure()[ m_syntaxFileTypeComboBox->currentText() ].color[ m_syntaxTypeComboBox->currentText() ] = format;	
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

	m_config.setXinxProjectPath( path );
}

void CustomDialogImpl::on_m_fileTypePath_textChanged( QString path ) {
	int index = m_fileTypeComboBox->currentIndex();
	if( index >= 0 ) {
		m_config.managedFile()[ index ].customPath = m_fileTypePath->text();
	}
}

void CustomDialogImpl::on_m_specifiqueCheckBox_toggled( bool checked ) {
	int index = m_fileTypeComboBox->currentIndex();
	if( index >= 0 ) {
		m_config.managedFile()[ index ].canBeCustomize = m_specifiqueCheckBox->isChecked();
	}
}

void CustomDialogImpl::on_m_createBakCheckBox_toggled( bool checked ) {
	m_config.setCreateBackupFile( checked );
}

void CustomDialogImpl::on_m_alertStandardCheckBox_toggled( bool checked ) {
	m_config.setAlertWhenStdFile( checked );
}

void CustomDialogImpl::on_m_langComboBox_currentIndexChanged( QString lang ) {
	QRegExp exp("^\\((.*)\\).*$");
	if( exp.indexIn( lang ) >= 0 )
		m_config.setLang( exp.cap( 1 ) );
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

	m_config.setObjectDescriptionPath( path );
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

