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
#include "cssmodeldata.h"
#include "iextendededitor.h"

// Qt header
#include <QFileInfo>
#include <QTextStream>

/* CssParserException */

CssParserException::CssParserException( const QString & message, int line, int column ) : FileContentException( message, line, column ) {
}

/* CSSFileContentParser */

CSSFileContentParser::CSSFileContentParser( IXinxExtendedEditor * editor, FileContentElement * parent, const QString & filename, int lineNumber ) : FileContentElement( parent, filename, lineNumber ), m_editor( editor ), m_isLoaded( true ) {
	
}

CSSFileContentParser::~CSSFileContentParser() {
	
}

void CSSFileContentParser::loadFromContent( const QString & content ) {
	m_isLoaded = true;

	if( content.isEmpty() ) {
		clear();
		return;
	}

	markAllDeleted();
	m_line = 1;

	
	
	removeMarkedDeleted();
}

void CSSFileContentParser::loadFromFile( const QString & filename ) {
	setFilename( filename );
	setName( QFileInfo( filename ).fileName() );

	QFile file( filename );

	// Open the file
	if (!file.open(QFile::ReadOnly | QFile::Text)) 
		throw CssParserException( QObject::tr("Cannot read file %1:\n%2.").arg(filename).arg(file.errorString()), 0, 0 );

	QTextStream text( &file );
	QString content = text.readAll();
	loadFromContent( content );
}

void CSSFileContentParser::loadFromFileDelayed( const QString & filename ) {
	setFilename( filename );
	setName( QFileInfo( filename ).fileName() );
	
	m_isLoaded = false;
}

bool CSSFileContentParser::isLoaded() {
	return m_isLoaded;
}
	
int CSSFileContentParser::rowCount() {
	if( ! m_isLoaded )
		loadFromFile( filename() );
	return FileContentElement::rowCount();
}
