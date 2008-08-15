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

#include <QDomDocument>
#include <QDomElement>

// TODO: Delete this workaround
#undef major
#undef minor

/* 	ConfigurationVerstionIncorectException */

ConfigurationVersionIncorectException::ConfigurationVersionIncorectException( QString version ) : XinxException( QString("Wrong version number %1").arg( version ) ) {

}

/* MetaConfigurationException */

MetaConfigurationException::MetaConfigurationException( QString message ) : XinxException( message ) {

}

/* MetaConfigurationNotExistException */

MetaConfigurationNotExistException::MetaConfigurationNotExistException( QString filename ) : MetaConfigurationException( QString( "Meta configuration \"%1\" not found" ).arg( filename ) ) {

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

bool ConfigurationVersion::isValid() const {
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

ConfigurationFile::ConfigurationFile() {
}

ConfigurationFile::ConfigurationFile( const QString & filename ) {
	ConfigurationParser parser( false );

	QFile file( filename );
	if( file.open( QFile::ReadOnly | QFile::Text ) ) {
		parser.loadFromDevice( &file );

		m_xmlPresentationFile = parser.xmlPresentationFile();
		m_configurations      = parser.configuration();
		try {
			m_version = ConfigurationVersion( parser.version(), parser.build() );
		} catch( ConfigurationVersionIncorectException ) {
			m_version = ConfigurationVersion();
		}
	}
}

ConfigurationFile::ConfigurationFile( const ConfigurationVersion & version, const QString & xmlPresentationFile )
		  : m_version( version ), m_xmlPresentationFile( xmlPresentationFile ) {

}

const ConfigurationVersion & ConfigurationFile::version() const {
	return m_version;
}

const QString & ConfigurationFile::xmlPresentationFile() const {
	return m_xmlPresentationFile;
}

bool ConfigurationFile::exists( const QString & directoryPath ) {
	return QFile::exists( QDir( directoryPath ).absoluteFilePath( "configuration.xml" ) );
}

ConfigurationFile ConfigurationFile::simpleConfigurationFile( const QString & directoryPath ) {
	QString fileName = QFileInfo( directoryPath ).isDir() ?
			QDir( directoryPath ).absoluteFilePath( "configuration.xml" ) :
			directoryPath;

	ConfigurationParser parser;
	ConfigurationFile configuration;

	QFile file( fileName );
	if( file.open( QFile::ReadOnly | QFile::Text ) ) {
		parser.loadFromDevice( &file );
		configuration.m_xmlPresentationFile = parser.xmlPresentationFile();
		try {
			configuration.m_version = ConfigurationVersion( parser.version(), parser.build() );
		} catch( ConfigurationVersionIncorectException ) {
			configuration.m_version = ConfigurationVersion();
		}
	}
	return configuration;
}

/* MetaConfigurationFile */

MetaConfigurationFile::MetaConfigurationFile( const QString & filename ) {
	QDomDocument document( "configurationdef.xml" );
	QFile file( filename );
	if( ! file.open( QIODevice::ReadOnly ) ) {
		// If MetaConfigurationFile not exists, there is one ;)
		m_files << "configuration.xml";
	} else if( ! document.setContent( &file ) ) {
		file.close();
		throw MetaConfigurationException( QString( "I can't read \"%1\" as XML Document." ).arg( filename ) );
	}

	QDomElement root = document.documentElement();

	QDomElement configuration = root.firstChildElement( "configuration" );
	if( ! configuration.isNull() ) {
		QDomElement conffile = configuration.firstChildElement( "file" );
		while( ! conffile.isNull() ) {
			m_files.insert( 0, conffile.attribute( "name" ) );
			conffile = conffile.nextSiblingElement( "file" );
		}
	}

	for( int i = 0 ; i < m_files.size() ; i++ ) {
		m_files.append( 0 );
	}

	if( m_files.count() == 0 )
		throw MetaConfigurationException( QString( "No configuration file found in \"%1\"" ).arg( filename ) );
}

MetaConfigurationFile::~MetaConfigurationFile() {

}

bool MetaConfigurationFile::exists( const QString & directoryPath ) {
	return QFile::exists( QDir( directoryPath ).absoluteFilePath( "configurationdef.xml" ) );
}

ConfigurationFile MetaConfigurationFile::simpleConfigurationFile( const QString & directoryPath ) {
	try {
		MetaConfigurationFile meta( QDir( directoryPath ).absoluteFilePath( "configurationdef.xml" ) );
		foreach( QString file, meta.m_files ) {
			QString path = QDir( directoryPath ).absoluteFilePath( file );
			ConfigurationFile configuration = ConfigurationFile::simpleConfigurationFile( path );
			if( configuration.version().isValid() ) {
				return configuration;
			}
		}
		return ConfigurationFile::simpleConfigurationFile( directoryPath );
	} catch( MetaConfigurationException ) {
		return ConfigurationFile::simpleConfigurationFile( directoryPath );
	}
}

int MetaConfigurationFile::count() const {
	return m_configurations.size();
}

ConfigurationFile * MetaConfigurationFile::configurations( int index ) {
	Q_ASSERT( ( index >= 0 ) && ( index < m_files.count() ) );
	Q_ASSERT( ( index >= 0 ) && ( index < m_configurations.count() ) );

	if( m_configurations.at( index ) ) return m_configurations.at( index );

	ConfigurationFile * configuration = new ConfigurationFile( m_files.at( index ) );
	m_configurations.replace( index, configuration );
	return configuration;
}

/* ConfigurationParser */

ConfigurationParser::ConfigurationParser( bool minimal ) : m_minimal( minimal ) {
	m_build = -1;
	m_elementToRead = 2;
	m_xmlPresentationFile = "Presentation.xml";
}

bool ConfigurationParser::loadFromDevice( QIODevice * device ) {
	Q_ASSERT( device );

	setDevice( device );

	while( ! atEnd() ) {
		readNext();

		if( isStartElement() ) {
			if( name() == "config" )
				readConfigElement();
			else
				raiseError("The file is not a configuration file.");
		}
	}

	return ! error();
}

void ConfigurationParser::readUnknownElement() {
	Q_ASSERT( isStartElement() );

	while( !atEnd() ) {
		readNext();

		if( isEndElement() )
			break;

		if( isStartElement() )
			readUnknownElement();
	}
}

void ConfigurationParser::readConfigElement() {
	Q_ASSERT( isStartElement() && ( QXmlStreamReader::name() == "config" ) );

	while( !atEnd() ) {
		readNext();

		if( isEndElement() )
			break;

		if( isStartElement() ) {
			if( QXmlStreamReader::name() == "version" )
				readVersionElement();
			else
			if( QXmlStreamReader::name() == "application" )
				readApplicationElement();
			else
				readUnknownElement();
		}
	}
}

void ConfigurationParser::readVersionElement() {
	Q_ASSERT( isStartElement() && ( QXmlStreamReader::name() == "version" ) );

	while( !atEnd() ) {
		readNext();

		if( isEndElement() )
			break;

		if( isStartElement() ) {
			if( QXmlStreamReader::name() == "numero" )
				m_version = readElementText();
			else
			if( QXmlStreamReader::name() == "edition_speciale" )
				m_build = readElementText().toInt();
			else
				readUnknownElement();
		}
	}

	if( m_minimal && ( --m_elementToRead == 0 ) )
		raiseError("Simple configuration file is read");
}

void ConfigurationParser::readApplicationElement() {
	Q_ASSERT( isStartElement() && ( QXmlStreamReader::name() == "application" ) );

	while( !atEnd() ) {
		readNext();

		if( isEndElement() )
			break;

		if( isStartElement() ) {
			if( QXmlStreamReader::name() == "properties" ) {
				if( ! attributes().value( "xmlPresentationFile" ).isEmpty() )
					m_xmlPresentationFile = attributes().value( "xmlPresentationFile" ).toString();

				if( m_minimal && ( --m_elementToRead == 0 ) )
					raiseError("Simple configuration file is read");

			}
			if( !error() )
				readUnknownElement();
		}
	}
}

void ConfigurationParser::readBusinessViewDef() {
	Q_ASSERT( isStartElement() && ( QXmlStreamReader::name() == "businessview_def" ) );

	while( !atEnd() ) {
		readNext();

		if( isEndElement() )
			break;

		if( isStartElement() ) {
			if( QXmlStreamReader::name() == "businessview" ) {
				QString name = attributes().value( "name" ).toString();
				m_configurations[ name ].target = attributes().value( "target" ).toString();
				m_configurations[ name ].viewstruct = attributes().value( "viewstruct" ).toString();
			}
			if( !error() )
				readUnknownElement();
		}
	}
}

void ConfigurationParser::readPresentation() {
	Q_ASSERT( isStartElement() && ( QXmlStreamReader::name() == "presentation" ) );

	while( !atEnd() ) {
		readNext();

		if( isEndElement() )
			break;

		if( isStartElement() )
			readPresentationElement();
	}
}

void ConfigurationParser::readPresentationElement() {
	Q_ASSERT( isStartElement() );

	while( !atEnd() ) {
		readNext();

		if( isEndElement() )
			break;

		if( isStartElement() ) {
			QString name = attributes().value( "businessview" ).toString();
			m_configurations[ name ].type = QXmlStreamReader::name().toString();
			m_configurations[ name ].fileRef = attributes().value( "fileRef" ).toString();
			m_configurations[ name ].lang = attributes().value( "lang" ).toString();
			m_configurations[ name ].support = attributes().value( "support" ).toString();
			if( !error() )
				readUnknownElement();
		}
	}
}

