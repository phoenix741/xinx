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
#include "snipetdialog.h"
#include "xinxpluginsloader.h"
#include "snipetlist.h"
#include "xinxconfig.h"

// Qt header
#include <QSyntaxHighlighter>

/* SnipetDialogImpl */

SnipetDialogImpl::SnipetDialogImpl( const QString & text, QWidget * parent, Qt::WFlags f ) : QDialog( parent, f ), m_highlighter(0) {
	setupUi( this );

	m_textEdit->setPlainText( text );
	updateFont();
}

SnipetDialogImpl::SnipetDialogImpl( const QString & type, const QString & text, QWidget * parent, Qt::WFlags f ) : QDialog( parent, f ), m_highlighter(0) {
	setupUi( this );

	m_extLineEdit->setText( type );
	m_textEdit->setPlainText( text );
	updateFont();
}

SnipetDialogImpl::SnipetDialogImpl( const Snipet & snipet, QWidget * parent, Qt::WFlags f ) : QDialog( parent, f ), m_highlighter(0) {
	setupUi( this );
	
	m_extLineEdit->setText( snipet.type() );
	m_nameLineEdit->setText( snipet.name() );
	m_descriptionTextEdit->setPlainText( snipet.description() );
	m_keyLineEdit->setText( snipet.key() );
	m_iconLineEdit->setText( snipet.icon() );
	m_categoryComboBox->setEditText( snipet.category() );
	m_textEdit->setPlainText( snipet.text() );
	updateFont();
	
	int index = 0;
	QListIterator< QPair<QLabel*,QLineEdit*> > i( m_paramList );
	while( i.hasNext() ) {
		QPair<QLabel*,QLineEdit*> pair = i.next();
		if( index < snipet.params().size() )
			( pair.second )->setText( snipet.params().at( index++ ) );
	}
}

SnipetDialogImpl::~SnipetDialogImpl() {

}

void SnipetDialogImpl::updateFont() {
	QFont font = XINXConfig::self()->config().editor.defaultFormat;
	QFontMetrics fm( font );
	m_textEdit->setFont( font );
	m_textEdit->setTabStopWidth( fm.width("M") * XINXConfig::self()->config().editor.tabulationSize );
}

void SnipetDialogImpl::setupUi( QDialog * parent ) {
	Ui::SnipetDialog::setupUi( parent );
	
	m_categoryComboBox->clear();
	m_categoryComboBox->addItems( SnipetListManager::self()->snipets().categories() );

	m_paramGrid = new QGridLayout( m_paramGroupBox );
	m_paramGroupBox->setVisible( false );
}

void SnipetDialogImpl::on_m_extLineEdit_textChanged( const QString & text ) {
	if( m_highlighter ) delete m_highlighter;
	m_highlighter = XinxPluginsLoader::self()->createHighlighter( XinxPluginsLoader::self()->highlighterOfSuffix( text ), m_textEdit );
}

void SnipetDialogImpl::on_m_textEdit_textChanged() {
	QString text = m_textEdit->toPlainText();
	bool finded = true;
	int i;
	/* Create missing parameter */
	for( i = 0; finded; i++ ) {
		if( text.contains( QString("%%1").arg( i + 1 ) ) ) {
			if( i >= m_paramList.size() ) {
				addParamLine();
			}
		} else
			finded = false;
	}
	/* Delete wrong parameter */
	i -= 1;
	for( int j = m_paramList.size() - 1; j >= i ; j-- ) {
		delete m_paramList.at( j ).first;
		delete m_paramList.at( j ).second;
		m_paramList.removeAt( j );
	}
	/* The parameter groupe box is show only if there is one parameter */
	m_paramGroupBox->setVisible( m_paramList.size() > 0 );
}

void SnipetDialogImpl::addParamLine() {
	QLabel * label;
	label = new QLabel( QApplication::translate("SnipetDialogImpl", "Param &%1 : ").arg( m_paramList.size() + 1 ), this );
		
	QLineEdit * lineEdit = new QLineEdit( this );
	label->setBuddy( lineEdit );
	
	m_paramGrid->addWidget( label, m_paramList.size(), 0 );
	m_paramGrid->addWidget( lineEdit, m_paramList.size(), 1 );
	m_paramList.append( qMakePair( label, lineEdit ) );
}

Snipet SnipetDialogImpl::getSnipet() {
	Snipet s;
	
	s.setType( m_extLineEdit->text() );
	s.setName( m_nameLineEdit->text() );
	s.setKey( m_keyLineEdit->text() );
	s.setDescription( m_descriptionTextEdit->toPlainText() );
	s.setIcon( m_iconLineEdit->text() );
	s.setCategory( m_categoryComboBox->currentText() );
	s.setText( m_textEdit->toPlainText() );
		
	QListIterator< QPair<QLabel*,QLineEdit*> > i( m_paramList );
	s.params().clear();
	while( i.hasNext() ) {
		QPair<QLabel*,QLineEdit*> pair = i.next();
		s.params().append( (pair.second)->text() );
	}
	return s;
}
