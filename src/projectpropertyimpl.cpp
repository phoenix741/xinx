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
#include <QInputDialog>
#include <assert.h>

#include "projectpropertyimpl.h"
#include "xslproject.h"
#include "xinxconfig.h"

ProjectPropertyImpl::ProjectPropertyImpl( QWidget * parent, Qt::WFlags f) : QDialog(parent, f) {
	setupUi(this);
}


void ProjectPropertyImpl::on_m_projectButton_clicked() {
	QString value = m_projectLineEdit->text();
	if( value.isEmpty() ) 
		value = xinxConfig->xinxProjectPath();
		
	value = QFileDialog::getExistingDirectory( this, tr("Project path"), value );
	if( ! value.isEmpty() ) {
		m_projectLineEdit->setText( value );		
	}
}

void ProjectPropertyImpl::on_m_specifiquePathButton_clicked() {
	QString value = m_specifiquePathLineEdit->text();
	if( value.isEmpty() ) 
		value = xinxConfig->xinxProjectPath();
		
	value = QFileDialog::getExistingDirectory( this, tr("Specifique path"), value );
	if( ! value.isEmpty() ) {
		m_specifiquePathLineEdit->setText( value );		
	}
}

void ProjectPropertyImpl::on_m_projectTypeCombo_currentIndexChanged( int index ) {
	m_webServiceGroupBox->setEnabled( (XSLProject::enumProjectType)index == XSLProject::SERVICES );
}

void ProjectPropertyImpl::on_m_specifiquePathLineEdit_textChanged( QString text ) {
	QDir dir (text);
	QPalette palette( m_specifiquePathLineEdit->palette() );
	
	if( dir.exists() ) {
		palette.setColor( QPalette::Text, QColor() );
	} else {
		palette.setColor( QPalette::Text, Qt::red );
	}
	m_specifiquePathLineEdit->setPalette( palette );
	
	updateOkButton();
}

void ProjectPropertyImpl::on_m_prefixLineEdit_textChanged( QString text ) {
	Q_UNUSED( text );
	
	m_prefixLineEdit->setText( text.toUpper() );
	updateOkButton();
}



void ProjectPropertyImpl::on_m_projectLineEdit_textChanged( QString text ) {
	QDir dir (text);
	QPalette palette( m_projectLineEdit->palette() );
	
	if( dir.exists() ) {
		palette.setColor( QPalette::Text, QColor() );
	} else {
		palette.setColor( QPalette::Text, Qt::red );
	}
	m_projectLineEdit->setPalette( palette );

	updateOkButton();
	updateSpecifiquePath();
}

void ProjectPropertyImpl::loadFromProject( XSLProject * project ) {
	m_nameLineEdit->setText( project->projectName() );
	m_projectLineEdit->setText( project->projectPath() );
	m_langComboBox->setCurrentIndex( m_langComboBox->findText( project->defaultLang() ) );
	m_navigatorComboBox->setCurrentIndex( m_navigatorComboBox->findText( project->defaultNav() ) );
	m_specifiquePathLineEdit->setText( project->specifPath() );
	m_prefixLineEdit->setText( project->specifPrefix() );
	m_projectTypeCombo->setCurrentIndex( (int)project->projectType() );
	
	switch( project->projectVersion() ) {
	case XSLProject::EGX500ES1 :
		m_projectVersionCombo->setCurrentIndex( 0 );
		break;
	case XSLProject::EGX500ES2 :
		m_projectVersionCombo->setCurrentIndex( 1 );
		break;
	case XSLProject::GCE110 :
		m_projectVersionCombo->setCurrentIndex( 2 );
		break;
	case XSLProject::GCE120 :
		m_projectVersionCombo->setCurrentIndex( 3 );
		break;
	}
	
	m_webServiceList->clear();
	foreach( QString link, project->serveurWeb() ) {
		m_webServiceList->addItem( link );
	}

	m_webServiceBtnDel->setEnabled( m_webServiceList->count() > 0 );
}

void ProjectPropertyImpl::saveToProject( XSLProject * project ) {
	project->setProjectName( m_nameLineEdit->text() );
	project->setProjectPath( m_projectLineEdit->text() );
	project->setDefaultLang( m_langComboBox->currentText() );
	project->setDefaultNav( m_navigatorComboBox->currentText() );
	project->setSpecifPath( m_specifiquePathLineEdit->text() );
	project->setSpecifPrefix( m_prefixLineEdit->text() );
	project->setProjectType( (XSLProject::enumProjectType)m_projectTypeCombo->currentIndex() );
	
	switch( m_projectVersionCombo->currentIndex() ) {
	case 0 : project->setProjectVersion( XSLProject::EGX500ES1 ); break;
	case 1 : project->setProjectVersion( XSLProject::EGX500ES2 ); break;
	case 2 : project->setProjectVersion( XSLProject::GCE110 ); break;
	case 3 : project->setProjectVersion( XSLProject::GCE120 ); break;
	}
	
	project->serveurWeb().clear();
	for( int i = 0; i < m_webServiceList->count(); i++ ) {
		project->serveurWeb().append( m_webServiceList->item( i )->text() );
	}
}

void ProjectPropertyImpl::updateSpecifiquePath() {
	QString path = QString("%1/langue/%2/nav/projet").arg( m_projectLineEdit->text() ).arg( m_langComboBox->currentText().toLower() );
	
	m_specifiquePathLineEdit->setText( QDir::cleanPath( path ) );
}

void ProjectPropertyImpl::updateOkButton() {
	bool okButtonEnabled = ! (
		m_specifiquePathLineEdit->text().isEmpty() ||
		m_prefixLineEdit->text().isEmpty() ||
		m_projectLineEdit->text().isEmpty() );

	okButton->setEnabled( okButtonEnabled );
}

void ProjectPropertyImpl::on_m_langComboBox_currentIndexChanged( QString str ) {
	Q_UNUSED( str );
	
	updateSpecifiquePath();
}


void ProjectPropertyImpl::on_m_webServiceBtnDel_clicked() {
	assert( m_webServiceList->currentRow() >= 0 );
	
	delete m_webServiceList->currentItem();
	
	m_webServiceBtnDel->setEnabled( m_webServiceList->count() > 0 );
}

void ProjectPropertyImpl::on_m_webServiceBtnAdd_clicked() {
	QString text = QInputDialog::getText( this, tr("Add WebService"), tr("URL of the WebServices"), QLineEdit::Normal, "http://localhost:8888/gce120/services/" );
	if( ! text.isEmpty() )
		m_webServiceList->addItem( text );

	m_webServiceBtnDel->setEnabled( m_webServiceList->count() > 0 );
}

