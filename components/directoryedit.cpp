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
#include "directoryedit.h"

// Qt header
#include <QFile>
#include <QPalette>
#include <QCompleter>
#include <QFileDialog>
#include <QToolButton>
#include <QHBoxLayout>
#include <QToolButton>

/* CompleterDirModel */

CompleterDirModel::CompleterDirModel( QObject *parent ) : QDirModel( parent ) {
}

QVariant CompleterDirModel::data( const QModelIndex &index, int role ) const {
	if( role == Qt::DisplayRole && index.column() == 0 ) {
		QString path = QDir::toNativeSeparators( filePath(index) );
		if( path.endsWith(QDir::separator()) )
			path.chop(1);
		return path;
	}

	return QDirModel::data(index, role);
}
 
/* DirectoryEdit */

DirectoryEdit::DirectoryEdit( QWidget * parent ) : QLineEdit( parent ) {
	connect( this, SIGNAL(textChanged(QString)), this, SLOT(slotTextChanged(QString)) );
	QCompleter * completer = new QCompleter( this );
	this->setCompleter( completer );
	completer->setModel( new CompleterDirModel( completer ) );
}

DirectoryEdit::DirectoryEdit( const QString & contents, QWidget * parent ) : QLineEdit( contents, parent ) {
	connect( this, SIGNAL(textChanged(QString)), this, SLOT(slotTextChanged(QString)) );
}

void DirectoryEdit::slotTextChanged( QString text ) {
	QFile file ( text );
	QPalette palette( this->palette() );
	
	if( file.exists() ) {
		palette.setColor( QPalette::Text, QColor() );
	} else {
		palette.setColor( QPalette::Text, Qt::red );
	}
	setPalette( palette );
}


void DirectoryEdit::changePath( QWidget * parent, const QString & defaultValue, bool directory ) {
	QString value = this->text();
	if( value.isEmpty() ) value = defaultValue;
		
	if( directory )
		value = QFileDialog::getExistingDirectory( parent, tr("Change the path"), value );
	else
		value = QFileDialog::getOpenFileName( parent, tr("Change the file"), value );
	
	if( ! value.isEmpty() ) {
		this->setText( QDir::toNativeSeparators( value ) );		
	}
}

/* PrivateDirectoryEditWidget */

class PrivateDirectoryEditWidget {
public:
	PrivateDirectoryEditWidget( QWidget * o );
	
	DirectoryEdit * m_lineEdit;
	QToolButton * m_button;
	bool m_directory;
	QString m_default;
private:
};

PrivateDirectoryEditWidget::PrivateDirectoryEditWidget( QWidget * o ) {
	QHBoxLayout * layout = new QHBoxLayout( o );
	
	m_lineEdit = new DirectoryEdit( o );
	m_button   = new QToolButton( o );
	m_button->setIcon( QIcon(":/images/folder.png") );
	
	layout->addWidget( m_lineEdit );
	layout->addWidget( m_button );
	
	layout->setMargin( 0 );
}

/* DirectoryEditWidget */

DirectoryEditWidget::DirectoryEditWidget( bool isDirectory, QWidget * parent ) : QWidget( parent ) {
	d = new PrivateDirectoryEditWidget( this );
	d->m_directory = isDirectory;
	connect( d->m_button, SIGNAL(clicked()), this, SLOT(changePath()) );
}

DirectoryEditWidget::DirectoryEditWidget( QWidget * parent ) : QWidget( parent ) {
	d = new PrivateDirectoryEditWidget( this );
	d->m_directory = true;
	connect( d->m_button, SIGNAL(clicked()), this, SLOT(changePath()) );
}

DirectoryEditWidget::~DirectoryEditWidget() {
	delete d;
}

bool DirectoryEditWidget::isDirectory() const {
	return d->m_directory;
}

void DirectoryEditWidget::setDirectory( bool value ) {
	d->m_directory = value;
}

QString DirectoryEditWidget::defaultValue() const {
	return d->m_default;
}

void DirectoryEditWidget::setDefaultValue( const QString & value ) {
	d->m_default = value;
}

void DirectoryEditWidget::changePath() {
	d->m_lineEdit->changePath( parentWidget(), d->m_default, d->m_directory );
}

DirectoryEdit * DirectoryEditWidget::lineEdit() const {
	return d->m_lineEdit;
}