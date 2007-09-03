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

/* ConfigurationVersion */

/*!
 * \class ConfigurationVersion
 * 
 * Represent a version of a XML Configuration file. The file version has
 * the form VXX.XX-XX. The version can also have a special edition number.
 */

/*!
 * Construct a configuration version object
 * \param major The major number of the version.
 * \param minor The minor number of the version.
 * \param release The release number of the version
 * \param build The build number (or special edition number) of the version
 */ 
ConfigurationVersion::ConfigurationVersion( int major, int minor, int release, int build ) {
	m_major = major;
	m_minor = minor;
	m_release = release;
	m_build = build;
}

/*!
 * Construct a configuration version as input a strint. The number version translate
 * the string into a version object. The special number is outside
 * \param version The version string used to construct the object
 * \param build The build number (or special edition number) of the version.
 */
ConfigurationVersion::ConfigurationVersion( const QString & version, int build ) {
	QString major, minor, release;

	m_build = build;
	if( ! version.length() )
		throw ConfigurationVerstionIncorectException();
	if( version.at( 0 ) != 'V' )
		throw ConfigurationVerstionIncorectException();
	int posDot = version.indexOf( "." ),
	    posLine = version.indexOf( "-", posDot );
	if( ( posDot < 0 ) || ( posLine < 0 ) )
		throw ConfigurationVerstionIncorectException();
	major = version.mid( 1, posDot - 1 );
	minor = version.mid( posDot + 1, posLine - posDot - 1 );
	release = version.mid( posLine + 1 );
	m_major = major.toInt();
	m_minor = minor.toInt();
	m_release = release.toInt();
}

/*!
 * Destroy the configuration version object.
 */
ConfigurationVersion::~ConfigurationVersion() {
}

/*!
 * Return the major part of the version
 * \return An integer represents the major parts
 */
int ConfigurationVersion::major() const {
	return m_major;
}

/*! 
 * Return the minor part of the version
 * \return An integer represents the minor parts
 */
int ConfigurationVersion::minor() const {
	return m_minor;
}

/*!
 * Return the release part of the version
 * \return An integer represents the release parts
 */
int ConfigurationVersion::release() const {
	return m_release;
}

/*!
 * Return the build part of the version (or called special edition).
 * \return An integer represents the special edition number
 */
int ConfigurationVersion::build() const {
	return m_build;
}

/*!
 * Return a string a representation of the version. The string has the form :
 * VXX.XX-XX (XX) where the number in bracket is the special edition number. This
 * is not necessary and can be omited, if not specified.
 * \return A string representation of the version.
 */	
QString ConfigurationVersion::toString() const {
	QString version = QString( "V%1.%2-%3" ).arg( m_major, 2, 10, QLatin1Char('0') )
											.arg( m_minor, 2, 10, QLatin1Char('0') )
								 			.arg( m_release, 2, 10, QLatin1Char('0') );
	if( m_build > 0 )
		version += QString( " (%1)" )		.arg( m_build, 2, 10, QLatin1Char('0') );
		
	return version;
}

/*!
 * Test if a version is valid.
 * A version is valid only if major, minor, and release number are greater than
 * zero.
 * \return true if the version is valid, else return false. 
 */
bool ConfigurationVersion::isValid() {
	return ( m_major >= 0 ) && ( m_minor >= 0 ) && ( m_release >= 0 );
}
	
/*!
 * Compare two version number.
 * \return Return true if the two version are equal
 */
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

/*!
 * Compare two version number.
 * \return Return true if the first is greater than the second
 */
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

/*!
 * Compare two version number.
 * \return Return true if the first is greater or equal than the second
 */
inline bool ConfigurationVersion::operator>= ( ConfigurationVersion version ) const {
	return ( *this > version ) || ( *this == version );
}

/*!
 * Compare two version number.
 * \return Return true if the first is lesser than the second
 */
inline bool ConfigurationVersion::operator<  ( ConfigurationVersion version ) const {
	return ! ( *this >= version );
}

/*!
 * Compare two version number.
 * \return Return true if the first is lesser or equal than the second
 */
inline bool ConfigurationVersion::operator<= ( ConfigurationVersion version ) const {
	return ! ( *this > version );
}

/* ConfigurationFile */

/*!
 * \class ConfigurationFile
 * 
 * Class represente a configuration file. This class contais method checking the
 * version number of a configuration file, testing if the file exists in the path
 * or other usuable functionnality.
 */

/*!
 * Test if the configuration file exists in the directory path.
 * The conifuration file has the name configuration.xml
 * \param DirectoryPath The path of the directory where we check if a configuration file exists.
 * \return True if the configuration file exist, else return false.
 */
bool ConfigurationFile::exists( const QString & DirectoryPath ) {
	return QFile::exists( QDir( DirectoryPath ).absoluteFilePath( "configuration.xml" ) );
}

/*!
 * Return the version of the configuration file (only if the file exists and the version
 * can be found.
 * This method parse the configuration file to find the version number. If he find him, 
 * the parser stop immediatly to not read the entierly.
 * \param DirectoryPath the path where the configuration file is.
 * \return A ConfigurationVersion with the version number or an invalid object if the version can't be found.
 */
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
		} catch( ConfigurationVerstionIncorectException ) {
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
