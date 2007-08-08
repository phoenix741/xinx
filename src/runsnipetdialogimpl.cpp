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

#include "runsnipetdialogimpl.h"

#include <QLabel>
#include <QLineEdit>

#include "snipetdialog.h"

/* PrivateRunSnipetDialogImpl */

class PrivateRunSnipetDialogImpl : public QObject {
	Q_OBJECT
public:
	PrivateRunSnipetDialogImpl( RunSnipetDialogImpl * parent );
	virtual ~PrivateRunSnipetDialogImpl();
	
	QGridLayout * m_paramGrid;
	QList< QPair<QLabel*,QLineEdit*> > m_paramList;
	QString m_text;
	Snipet * m_snipet;
	
	void setupUi();
	
public slots:
	void changeSnipet();
private:
	RunSnipetDialogImpl * m_parent;
};

PrivateRunSnipetDialogImpl::PrivateRunSnipetDialogImpl( RunSnipetDialogImpl * parent ) : m_parent( parent ) {
	
}

PrivateRunSnipetDialogImpl::~PrivateRunSnipetDialogImpl() {
	
}

void PrivateRunSnipetDialogImpl::setupUi() {
	connect( m_parent->m_detailPushButton, SIGNAL(clicked()), this, SLOT(changeSnipet()) );
}

void PrivateRunSnipetDialogImpl::changeSnipet() {
	SnipetDialogImpl dlg( m_snipet );
	if( dlg.exec() == QDialog::Accepted ) {
		dlg.getSnipet();
	}
}


/* RunSnipetDialogImpl */

RunSnipetDialogImpl::RunSnipetDialogImpl( Snipet * snipet, QWidget * parent, Qt::WFlags f) : QDialog(parent, f) {
	setupUi(this);
	
	d = new PrivateRunSnipetDialogImpl( this );
	d->setupUi();
	d->m_snipet = snipet;
	
	d->m_text = snipet->text();
	
	d->m_paramGrid = new QGridLayout( m_paramGroupBox );

	m_labelName->setText( QString( "<b>%1</b>" ).arg( snipet->name() ) );
	m_descriptionLabel->setText( snipet->description() );
	foreach( QString params, snipet->params() ) {
		QLabel * label = new QLabel( params, m_paramGroupBox );
		QLineEdit * edit = new QLineEdit( m_paramGroupBox );

		d->m_paramGrid->addWidget( label, d->m_paramList.size(), 0 );
		d->m_paramGrid->addWidget( edit, d->m_paramList.size(), 1 );

		d->m_paramList.append( qMakePair( label, edit ) );
	}
	
	m_paramGroupBox->setVisible( d->m_paramList.size() > 0 );
	if( d->m_paramList.size() > 0 ) {
		d->m_paramList.at( 0 ).second->setFocus( Qt::OtherFocusReason );
	}
}

RunSnipetDialogImpl::~RunSnipetDialogImpl() {
	delete d;
}


QString RunSnipetDialogImpl::getResult() {
	QString text = d->m_text;
	for( int i = 0 ; i < d->m_paramList.size() ; i++ ) {
		text = text.arg( d->m_paramList.at( i ).second->text() );
	}
	return text;
}

#include "runsnipetdialogimpl.moc"
