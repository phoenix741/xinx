/***************************************************************************
 *   Copyright (C) 2007 by Ulrich Van Den Hekke                            *
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
#include "configurationfile.h"
#include "private/p_configurationfile.h"

// Qt header
#include <QDir>
#include <QFile>
#include <QXmlSimpleReader>
#include <QDebug>

/* 	ConfigurationVerstionIncorectException */

ConfigurationVersionIncorectException::ConfigurationVersionIncorectException( QString version ) : XinxException( QString("Wrong version number %1").arg( version ) ) {
	
}


/* ConfigurationVersion */

ConfigurationVersion::ConfigurationVersion( int major, int minor, int release, int build ) {
	m_major = major;
	m_minor = minor;
	m_release = release;
	m_build = build;
}

ConfigurationVersion::ConfigurationVersion( const QString & version, int build ) {
	QString major, minor, release;

	m_build = build;
	if( ! version.length() )
		throw ConfigurationVersionIncorectException( version );
	if( version.at( 0 ) != 'V' )
		throw ConfigurationVersionIncorectException( version );
	int posDot = version.indexOf( "." ),
	    posLine = version.indexOf( "-", posDot );
	if( ( posDot < 0 ) || ( posLine < 0 ) )
		throw ConfigurationVersionIncorectException( version );
	major = version.mid( 1, posDot - 1 );
	minor = version.mid( posDot + 1, posLine - posDot - 1 );
	release = version.mid( posLine + 1 );
	m_major = major.toInt();
	m_minor = minor.toInt();
	m_release = release.toInt();
}

ConfigurationVersion::~ConfigurationVersion() {
}

int ConfigurationVersion::major() const {
	return m_major;
}

int ConfigurationVersion::minor() const {
	return m_minor;
}

int ConfigurationVersion::release() const {
	return m_release;
}

int ConfigurationVersion::build() const {
	return m_build;
}

QString ConfigurationVersion::toString() const {
	QString version = QString( "V%1.%2-%3" ).arg( m_major, 2, 10, QLatin1Char('0') )
											.arg( m_minor, 2, 10, QLatin1Char('0') )
								 			.arg( m_release, 2, 10, QLatin1Char('0') );
	if( m_build > 0 )
		version += QString( " (%1)" )		.arg( m_build, 2, 10, QLatin1Char('0') );
		
	return version;
}

bool ConfigurationVersion::isValid() {
	return ( m_major >= 0 ) && ( m_minor >= 0 ) && ( m_release >= 0 );
}
	
inline bool ConfigurationVersion::operator== ( ConfigurationVersion version ) const {
	if( m_major == version.m_major ) {
		if( m_minor == version.m_minor ) {
			if( m_release == version.m_release ) {
				if( m_build == version.m_build ) {
					return true;
				} else 
					return false;
			} else 
				return false;
		} else 
			return false;
	} else 
		return false;
}

inline bool ConfigurationVersion::operator>  ( ConfigurationVersion version ) const {
	if( m_major == version.m_major ) {
		if( m_minor == version.m_minor ) {
			if( m_release == version.m_release ) {
				if( m_build == version.m_build ) {
					return false;
				} else 
					return m_build > version.m_build;
			} else 
				return m_release > version.m_release;
		} else 
			return m_minor > version.m_minor;
	} else 
		return m_major > version.m_major;
}

inline bool ConfigurationVersion::operator>= ( ConfigurationVersion version ) const {
	return ( *this > version ) || ( *this == version );
}

inline bool ConfigurationVersion::operator<  ( ConfigurationVersion version ) const {
	return ! ( *this >= version );
}

inline bool ConfigurationVersion::operator<= ( ConfigurationVersion version ) const {
	return ! ( *this > version );
}

/* ConfigurationFile */


bool ConfigurationFile::exists( const QString & DirectoryPath ) {
	return QFile::exists( QDir( DirectoryPath ).absoluteFilePath( "configuration.xml" ) );
}

ConfigurationVersion ConfigurationFile::version( const QString & DirectoryPath ) {
	QString fileName = QDir( DirectoryPath ).absoluteFilePath( "configuration.xml" );
	
	ParseVersionHandler handler;
	QXmlSimpleReader reader;
	reader.setContentHandler( &handler );
	reader.setErrorHandler( &handler );
	
	QFile file( fileName );
	if( file.open( QFile::ReadOnly | QFile::Text ) ) {
		QXmlInputSource xmlInputSource( &file );
		reader.parse( xmlInputSource );
		try {
			return ConfigurationVersion( handler.m_version, handler.m_build );
		} catch( ConfigurationVersionIncorectException ) {
			return ConfigurationVersion();
		}
	}
	return ConfigurationVersion();
}

/* ParseVersionHandler */

ParseVersionHandler::ParseVersionHandler() {
	m_parserState = STATE_START;
	m_build = -1;
}

ParseVersionHandler::~ParseVersionHandler() {
	
}
	
bool ParseVersionHandler::startElement(const QString &namespaceURI, const QString &localName, 
									   const QString &qName, const QXmlAttributes &attributes) {
	Q_UNUSED( namespaceURI );
	Q_UNUSED( localName );
	Q_UNUSED( attributes );

	if( m_parserState == ParseVersionHandler::STATE_START && qName != "config" ) {
		m_errorStr = "The file is not a configuration file";
		return false;
	}
	if( qName == "config" ) {
		m_parserState = ParseVersionHandler::STATE_CONFIG;
	} else
	if( m_parserState == ParseVersionHandler::STATE_CONFIG && qName == "version" ) {
		m_parserState = ParseVersionHandler::STATE_VERSION;
	} else
	if( m_parserState == ParseVersionHandler::STATE_VERSION && qName == "numero" ) {
		m_parserState = ParseVersionHandler::STATE_NUMERO;
	} else
	if( m_parserState == ParseVersionHandler::STATE_VERSION && qName == "edition_speciale" ) {
		m_parserState = ParseVersionHandler::STATE_EDITIONSPECIAL;
	}
	m_text = "";
	return true;
}

bool ParseVersionHandler::endElement(const QString &namespaceURI, const QString &localName,
									 const QString &qName) {
	Q_UNUSED( namespaceURI );
	Q_UNUSED( localName );

	if( m_parserState == ParseVersionHandler::STATE_CONFIG && qName == "config" ) {
		m_parserState = ParseVersionHandler::STATE_START;
	} else
	if( m_parserState == ParseVersionHandler::STATE_VERSION && qName == "version" ) {
		m_parserState = ParseVersionHandler::STATE_CONFIG;
		m_errorStr = "Version founded";
		return false;
	} else
	if( m_parserState == ParseVersionHandler::STATE_NUMERO && qName == "numero" ) {
		m_version = m_text;
		m_parserState = ParseVersionHandler::STATE_VERSION;
	} else
	if( m_parserState == ParseVersionHandler::STATE_EDITIONSPECIAL && qName == "edition_speciale" ) {
		m_build = m_text.toInt();
		m_parserState = ParseVersionHandler::STATE_VERSION;
	}
	return true;
}

bool ParseVersionHandler::characters(const QString &str) {
	m_text += str;
	return true;
}

bool ParseVersionHandler::fatalError(const QXmlParseException &exception) {
	qDebug() << "Parse error at line " << exception.lineNumber() << ", column " << exception.columnNumber() << ": " << exception.message();
	return false;	
}
