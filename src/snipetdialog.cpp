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

#include "globals.h"
#include "snipetdialog.h"
#include "xmleditor.h"
#include "jseditor.h"
#include "snipetlist.h"

#include <QApplication>
#include <QGroupBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>

/* PrivateSnipetDialogImpl */

class PrivateSnipetDialogImpl : public QObject {
	Q_OBJECT
public:
	PrivateSnipetDialogImpl( SnipetDialogImpl * parent );
	virtual ~PrivateSnipetDialogImpl();
	
	void setupUi();

	void addParamLine();

	QTextEdit * m_textEdit;
	QGridLayout * m_paramGrid;
	
	QList< QPair<QLabel*,QLineEdit*> > m_paramList;
	
	enum Snipet::SnipetType m_snipetType;
	Snipet * m_snipet;
public slots:
	void textChanged();
private:
	SnipetDialogImpl * m_parent;
};

PrivateSnipetDialogImpl::PrivateSnipetDialogImpl( SnipetDialogImpl * parent ) : m_parent( parent ) {
	m_snipet = NULL;
}

PrivateSnipetDialogImpl::~PrivateSnipetDialogImpl() {
	
}
	
void PrivateSnipetDialogImpl::setupUi() {
	m_parent->m_categoryComboBox->clear();
	m_parent->m_categoryComboBox->addItems( global.m_snipetList->categories() );
	
	QGridLayout * grid1 = new QGridLayout( m_parent->m_templateGroupBox );

	switch( m_snipetType ) {
	case Snipet::SNIPET_XSL:
		m_textEdit = new XMLEditor( m_parent->m_templateGroupBox );
		break;
	case Snipet::SNIPET_JAVASCRIPT:
		m_textEdit = new JSEditor( m_parent->m_templateGroupBox );
		break;
	}
	
	m_textEdit->setLineWrapMode( QTextEdit::NoWrap );
	grid1->addWidget( m_textEdit );
	
	m_paramGrid = new QGridLayout( m_parent->m_paramGroupBox );
	m_parent->m_paramGroupBox->setVisible( false );
	
    QObject::connect( m_textEdit, SIGNAL(textChanged()), this, SLOT(textChanged()) );
}

void PrivateSnipetDialogImpl::textChanged() {
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
	m_parent->m_paramGroupBox->setVisible( m_paramList.size() > 0 );
}

void PrivateSnipetDialogImpl::addParamLine() {
	QLabel * label;
	label = new QLabel( QApplication::translate("SnipetDialogImpl", "Param &%1 : ", 0, QApplication::UnicodeUTF8).arg( m_paramList.size() + 1 ), m_parent );
		
	QLineEdit * lineEdit = new QLineEdit( m_parent );
	label->setBuddy( lineEdit );
	
	m_paramGrid->addWidget( label, m_paramList.size(), 0 );
	m_paramGrid->addWidget( lineEdit, m_paramList.size(), 1 );
	m_paramList.append( qMakePair( label, lineEdit ) );
}

/* SnipetDialogImpl */

SnipetDialogImpl::SnipetDialogImpl( const QString & text, QWidget * parent, Qt::WFlags f ) : QDialog( parent, f ) {
	setupUi( this );

	d = new PrivateSnipetDialogImpl( this );
	d->m_snipetType   = Snipet::SNIPET_XSL;
	d->setupUi();

	d->m_textEdit->setPlainText( text );
}

SnipetDialogImpl::SnipetDialogImpl( enum Snipet::SnipetType type, const QString & text, QWidget * parent, Qt::WFlags f ) : QDialog( parent, f ) {
	setupUi( this );

	d = new PrivateSnipetDialogImpl( this );
	d->m_snipetType   = type;
	d->setupUi();

	d->m_textEdit->setPlainText( text );
}

SnipetDialogImpl::SnipetDialogImpl( Snipet * snipet, QWidget * parent, Qt::WFlags f ) : QDialog( parent, f ) {
	setupUi( this );

	d = new PrivateSnipetDialogImpl( this );
	d->m_snipet = snipet;
	d->m_snipetType = snipet->type();
	d->setupUi();
	
	m_nameLineEdit->setText( snipet->name() );
	m_descriptionTextEdit->setPlainText( snipet->description() );
	m_iconLineEdit->setText( snipet->icon() );
	m_categoryComboBox->setEditText( snipet->category() );
	d->m_textEdit->setPlainText( snipet->text() );
	
	QListIterator< QPair<QLabel*,QLineEdit*> > i( d->m_paramList );
	int index = 0;
	while( i.hasNext() ) {
		QPair<QLabel*,QLineEdit*> pair = i.next();
		if( index < snipet->params().size() )
			( pair.second )->setText( snipet->params().at( index++ ) );
	}
}

SnipetDialogImpl::~SnipetDialogImpl() {
	delete d;
}

Snipet * SnipetDialogImpl::getSnipet() {
	if( ! d->m_snipet )
		d->m_snipet = new Snipet();
		
	d->m_snipet->setType( d->m_snipetType );
	d->m_snipet->setName( m_nameLineEdit->text() );
	d->m_snipet->setDescription( m_descriptionTextEdit->toPlainText() );
	d->m_snipet->setIcon( m_iconLineEdit->text() );
	d->m_snipet->setCategory( m_categoryComboBox->currentText() );
	d->m_snipet->setText( d->m_textEdit->toPlainText() );
		
	QListIterator< QPair<QLabel*,QLineEdit*> > i( d->m_paramList );
	while( i.hasNext() ) {
		QPair<QLabel*,QLineEdit*> pair = i.next();
		d->m_snipet->params().append( (pair.second)->text() );
	}
	return d->m_snipet ;
}

#include "snipetdialog.moc"
