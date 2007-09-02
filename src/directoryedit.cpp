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

