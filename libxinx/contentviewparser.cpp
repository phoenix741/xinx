/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2009 by Ulrich Van Den Hekke                              *
 * ulrich.vdh@shadoware.org                                                *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 * *********************************************************************** */

// Xinx header
#include "contentviewparser.h"

// Qt header
#include <QFile>
#include <QBuffer>

/* ContentViewException */

ContentViewException::ContentViewException( QString message, int line, int column )
: XinxException( QString("Error : %1 at line %2:%3").arg( message ).arg( line ).arg( column ) ), m_line( line ), m_column( column ) {

}

int ContentViewException::getLine() const {
	return m_line;
}

int ContentViewException::getColumn() const {
	return m_column;
}

/* ContentViewParser */

ContentViewParser::ContentViewParser( bool autoDelete ) : m_autoDelete( autoDelete ) {
}

ContentViewParser::~ContentViewParser() {
}

void ContentViewParser::setAutoDelete( bool value ) {
	m_autoDelete = value;
}

bool ContentViewParser::isAutoDelete() const {
	return m_autoDelete;
}

void ContentViewParser::loadFromContent( ContentViewNode * rootNode, const QString & content ) {
	QByteArray contentArray = content.toUtf8();
	QBuffer buffer( &contentArray );
	buffer.open( QIODevice::ReadOnly );

	loadFromDeviceImpl( rootNode, &buffer );

	if( m_autoDelete ) delete this;
}

void ContentViewParser::loadFromFile( ContentViewNode * rootNode, const QString & filename ) {
	QFile file( filename );

	// Open the file
	if (!file.open(QFile::ReadOnly))
		throw ContentViewException( QObject::tr("Cannot read file %1:\n%2.").arg(filename).arg(file.errorString()), 0, 0 );

	loadFromDeviceImpl( rootNode, & file );

	if( m_autoDelete ) delete this;
}

void ContentViewParser::loadFromDevice( ContentViewNode * rootNode, QIODevice * device ) {
	loadFromDeviceImpl( rootNode, device );

	if( m_autoDelete ) delete this;
}
