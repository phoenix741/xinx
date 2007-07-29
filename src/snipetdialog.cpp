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

#include "snipetdialog.h"

#include <QApplication>
#include <QTextEdit>
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
	
	void setupUi( QDialog * dlg );
    void retranslateUi( QDialog * dlg );

	void addParamLine();

	QGroupBox * m_textEditGroupBox;
	QTextEdit * m_textEdit;
	QGroupBox * m_paramGroupBox;
	QPushButton * m_okButton;
	QPushButton * m_cancelButton;
	QGridLayout * m_paramGrid;
	
	QList< QPair<QLabel*,QLineEdit*> > m_paramList;
public slots:
	void textChanged();
private:
	SnipetDialogImpl * m_parent;
};

PrivateSnipetDialogImpl::PrivateSnipetDialogImpl( SnipetDialogImpl * parent ) : m_parent( parent ) {
	
}

PrivateSnipetDialogImpl::~PrivateSnipetDialogImpl() {
	
}
	
void PrivateSnipetDialogImpl::setupUi( QDialog * dlg ) {
    QVBoxLayout * vbox1 = new QVBoxLayout( dlg );
	
	m_textEditGroupBox = new QGroupBox( dlg );
	QGridLayout * grid1 = new QGridLayout( m_textEditGroupBox );

	m_textEdit = new QTextEdit( m_textEditGroupBox );
	m_textEdit->setWordWrapMode( QTextOption::NoWrap );
	m_textEdit->setLineWrapMode( QTextEdit::NoWrap );
	grid1->addWidget( m_textEdit );
	
	vbox1->addWidget( m_textEditGroupBox );
	
	m_paramGroupBox = new QGroupBox( dlg );
	m_paramGrid = new QGridLayout( m_paramGroupBox );
	
	m_paramGroupBox->setVisible( false );
	
	vbox1->addWidget( m_paramGroupBox );

	QHBoxLayout * hbox1 = new QHBoxLayout();
	hbox1->addStretch( 0 );
	
	m_okButton = new QPushButton( dlg );
	m_okButton->setIcon( QIcon(":/images/button_ok.png") );
	hbox1->addWidget( m_okButton );

	m_cancelButton = new QPushButton( dlg );
	m_cancelButton->setIcon( QIcon(":/images/button_cancel.png") );
	hbox1->addWidget( m_cancelButton );
	
	vbox1->addLayout( hbox1 );
	
	retranslateUi( dlg );
	
    QSize size( 560, 300 );
    size = size.expandedTo( dlg->minimumSizeHint() );
    dlg->resize( size );
	
    QObject::connect( m_okButton, SIGNAL(clicked()), dlg, SLOT(accept()) );
    QObject::connect( m_cancelButton, SIGNAL(clicked()), dlg, SLOT(reject()) );
    QObject::connect( m_textEdit, SIGNAL(textChanged()), this, SLOT(textChanged()) );
}

void PrivateSnipetDialogImpl::retranslateUi( QDialog * dlg ) {
    dlg->setWindowTitle( QApplication::translate("SnipetDialogImpl", "Template", 0, QApplication::UnicodeUTF8) );
    m_textEditGroupBox->setTitle(QApplication::translate("SnipetDialogImpl", "Template", 0, QApplication::UnicodeUTF8));
    m_okButton->setText( QApplication::translate("SnipetDialogImpl", "OK", 0, QApplication::UnicodeUTF8) );
    m_cancelButton->setText( QApplication::translate("SnipetDialogImpl", "Cancel", 0, QApplication::UnicodeUTF8) );
}

void PrivateSnipetDialogImpl::textChanged() {
	QString text = m_textEdit->toPlainText();
	bool finded = true;
	int i;
	for( i = 0; finded; i++ ) {
		if( text.contains( QString("%%1").arg( i + 1 ) ) ) {
			if( i >= m_paramList.size() ) {
				addParamLine();
			}
		} else
			finded = false;
	}
	i -= 1;
	for( int j = m_paramList.size() - 1; j >= i ; j-- ) {
		delete m_paramList.at( j ).first;
		delete m_paramList.at( j ).second;
		m_paramList.removeAt( j );
	}
	m_paramGroupBox->setVisible( m_paramList.size() > 0 );
}

void PrivateSnipetDialogImpl::addParamLine() {
	QLabel * label = new QLabel( QApplication::translate("SnipetDialogImpl", "Param &%1", 0, QApplication::UnicodeUTF8).arg( m_paramList.size() + 1 ), m_parent );
	QLineEdit * lineEdit = new QLineEdit( m_parent );
	label->setBuddy( lineEdit );
	
	m_paramGrid->addWidget( label, m_paramList.size(), 0 );
	m_paramGrid->addWidget( lineEdit, m_paramList.size(), 1 );
	m_paramList.append( qMakePair( label, lineEdit ) );
}


/* SnipetDialogImpl */

SnipetDialogImpl::SnipetDialogImpl( QWidget * parent, Qt::WFlags f ) : QDialog( parent, f ) {
	d = new PrivateSnipetDialogImpl( this );
	d->setupUi( this );
}

SnipetDialogImpl::~SnipetDialogImpl() {
	delete d;
}

void SnipetDialogImpl::setText( const QString & text ) {
	d->m_textEdit->setPlainText( text );
}

#include "snipetdialog.moc"
