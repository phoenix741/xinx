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

#include "projectpropertyimpl.h"
#include "xslproject.h"

ProjectPropertyImpl::ProjectPropertyImpl( QWidget * parent, Qt::WFlags f) : QDialog(parent, f) {
	setupUi(this);
}


void ProjectPropertyImpl::on_m_projectButton_clicked() {
	QString value = QFileDialog::getExistingDirectory( this, tr("Project path"), m_projectLineEdit->text() );
	if( ! value.isEmpty() ) {
		m_projectLineEdit->setText( value );		
	}
}

void ProjectPropertyImpl::on_m_specifiquePathButton_clicked() {
	QString value = QFileDialog::getExistingDirectory( this, tr("Specifique path"), m_specifiquePathLineEdit->text() );
	if( ! value.isEmpty() ) {
		m_specifiquePathLineEdit->setText( value );		
	}
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
	updateSpecifiquePath();
}

void ProjectPropertyImpl::loadFromProject( XSLProject * project ) {
	m_nameLineEdit->setText( project->projectName() );
	m_projectLineEdit->setText( project->projectPath() );
	m_langComboBox->setCurrentIndex( m_langComboBox->findText( project->defaultLang() ) );
	m_navigatorComboBox->setCurrentIndex( m_navigatorComboBox->findText( project->defaultNav() ) );
	m_specifiquePathLineEdit->setText( project->specifPath() );
	m_prefixLineEdit->setText( project->specifPrefix() );
}

void ProjectPropertyImpl::saveToProject( XSLProject * project ) {
	project->setProjectName( m_nameLineEdit->text() );
	project->setProjectPath( m_projectLineEdit->text() );
	project->setDefaultLang( m_langComboBox->currentText() );
	project->setDefaultNav( m_navigatorComboBox->currentText() );
	project->setSpecifPath( m_specifiquePathLineEdit->text() );
	project->setSpecifPrefix( m_prefixLineEdit->text() );
}

void ProjectPropertyImpl::updateSpecifiquePath() {
	QString path = QString("%1/langue/%2/nav/projet").arg( m_projectLineEdit->text() ).arg( m_langComboBox->currentText().toLower() );
	
	m_specifiquePathLineEdit->setText( QDir::cleanPath( path ) );
}

void ProjectPropertyImpl::on_m_langComboBox_currentIndexChanged( QString str ) {
	Q_UNUSED( str );
	
	updateSpecifiquePath();
}

