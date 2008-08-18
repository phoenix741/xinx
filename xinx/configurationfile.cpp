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

// Qt header
#include <QDir>
#include <QFile>

#include <QtXmlPatterns>

#include <QDomDocument>
#include <QDomElement>

// TODO: Delete this workaround
#undef major
#undef minor

/* ConfigurationVerstionIncorectException */

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

ConfigurationFile::ConfigurationFile( const QString & filename ) : m_filename( filename ) {
	version();
	xmlPresentationFile();
}

const QString & ConfigurationFile::filename() const {
	return m_filename;
}

ConfigurationVersion ConfigurationFile::version() {
	if( m_version.isValid() ) return m_version;

	try {
		QFile sourceDocument;
		sourceDocument.setFileName( m_filename );
		if(! sourceDocument.open(QIODevice::ReadOnly))
			throw ConfigurationVersionIncorectException( "" );

		QString versionInfo;
		int buildInfo = 0;

		QXmlQuery query;
		QXmlResultItems result;

		query.bindVariable( "inputDocument", &sourceDocument );
		query.setQuery( "declare default element namespace 'http://www.generix.fr/technicalframework/configuration';\n"
		                "doc($inputDocument)/config/version/numero/\n"
		                "string(text())" );


		if( !query.isValid() )
			throw ConfigurationVersionIncorectException( "" );

		query.evaluateTo( &result );
		if( result.hasError() )
			throw ConfigurationVersionIncorectException( "" );

		QXmlItem item( result.next() );
		if( !item.isNull() ) versionInfo = item.toAtomicValue().toString();

		sourceDocument.reset();
		query.setQuery( "declare default element namespace 'http://www.generix.fr/technicalframework/configuration';\n"
		                "doc($inputDocument)/config/version/edition_speciale/\n"
		                "string(text())" );

		if( !query.isValid() )
			throw ConfigurationVersionIncorectException( "" );

		query.evaluateTo( &result );
		item = QXmlItem( result.next() );
		if( !item.isNull() ) buildInfo = item.toAtomicValue().toInt();

		m_version = ConfigurationVersion( versionInfo, buildInfo );
		return m_version;
	} catch( ConfigurationVersionIncorectException ) {
		return ConfigurationVersion();
	}
}

QString ConfigurationFile::xmlPresentationFile() {
	if( ! m_xmlPresentationFile.isNull() ) return m_xmlPresentationFile;

	m_xmlPresentationFile = "Presentation.xml";

	QFile sourceDocument;
	sourceDocument.setFileName( m_filename );
	sourceDocument.open(QIODevice::ReadOnly);

	QXmlQuery query;
	QXmlResultItems result;

	query.bindVariable( "inputDocument", &sourceDocument );
	query.setQuery( "declare default element namespace 'http://www.generix.fr/technicalframework/configuration';\n"
	                "doc($inputDocument)/config/application/properties/\n"
	                "string(@xmlPresentationFile)" );

	query.evaluateTo( &result );
	if( result.hasError() )
		throw ConfigurationVersionIncorectException( "" );
	QXmlItem item( result.next() );
	if( !item.isNull() ) m_xmlPresentationFile = item.toAtomicValue().toString();

	return m_xmlPresentationFile;
}

QStringList ConfigurationFile::businessViewPerFiles( const QString & filename ) {
	QStringList fileList;
	QString relativeFileName = filename, rootPath;

	QFile sourceDocument;
	sourceDocument.setFileName( m_filename );
	sourceDocument.open( QIODevice::ReadOnly );

	QXmlQuery query;
	QXmlResultItems result;

	query.bindVariable( "inputDocument", &sourceDocument );
	query.setQuery( "declare default element namespace 'http://www.generix.fr/technicalframework/configuration';\n"
	                "doc($inputDocument)/config/application/presentation/\n"
	                "string(@rootPath)" );

	query.evaluateTo( &result );

	QXmlItem item( result.next() );
	if( !item.isNull() ) rootPath = item.toAtomicValue().toString();

	relativeFileName.remove( rootPath );
	while( relativeFileName.startsWith( '/' ) ) relativeFileName.remove( 0, 1 );

	sourceDocument.reset();
	query.setQuery( "declare default element namespace 'http://www.generix.fr/technicalframework/configuration';\n"
	                "doc($inputDocument)/config/application/presentation/*[@fileRef='" + relativeFileName + "']/\n"
	                "string(@businessview)" );

	query.evaluateTo( &result );

	item = result.next();
	while( !item.isNull() ) {
		fileList << item.toAtomicValue().toString();

		item = result.next();
	}

	return fileList;
}

bool ConfigurationFile::exists( const QString & directoryPath ) {
	return QFile::exists( QDir( directoryPath ).absoluteFilePath( "configuration.xml" ) );
}

ConfigurationFile ConfigurationFile::simpleConfigurationFile( const QString & directoryPath ) {
	QString fileName = QFileInfo( directoryPath ).isDir() ?
			QDir( directoryPath ).absoluteFilePath( "configuration.xml" ) :
			directoryPath;

	return ConfigurationFile( fileName );
}

/* MetaConfigurationFile */

MetaConfigurationFile::MetaConfigurationFile( const QString & filename ) {
	QString directoryPath = QFileInfo( filename ).absolutePath();

	QDomDocument document( "configurationdef.xml" );
	QFile file( filename );
	if( ! file.open( QIODevice::ReadOnly ) ) {
		// If MetaConfigurationFile not exists, there is one ;)
		m_configurations << new ConfigurationFile( QDir( directoryPath ).absoluteFilePath( "configuration.xml" ) );
	} else if( ! document.setContent( &file ) ) {
		file.close();
		throw MetaConfigurationException( QString( "I can't read \"%1\" as XML Document." ).arg( filename ) );
	} else {
		QDomElement root = document.documentElement();

		QDomElement configuration = root.firstChildElement( "configuration" );
		if( ! configuration.isNull() ) {
			QDomElement conffile = configuration.firstChildElement( "file" );
			while( ! conffile.isNull() ) {
				m_configurations.insert( 0, new ConfigurationFile( QDir( directoryPath ).absoluteFilePath( conffile.attribute( "name" ) ) ) );
				conffile = conffile.nextSiblingElement( "file" );
			}
		}
	}

	if( m_configurations.count() == 0 )
		throw MetaConfigurationException( QString( "No configuration file found in \"%1\"" ).arg( filename ) );
}

MetaConfigurationFile::~MetaConfigurationFile() {
	QList<ConfigurationFile*> configuration = m_configurations;
	m_configurations.clear();
	qDeleteAll( configuration );
}

bool MetaConfigurationFile::exists( const QString & directoryPath ) {
	return QFile::exists( QDir( directoryPath ).absoluteFilePath( "configurationdef.xml" ) );
}

ConfigurationFile MetaConfigurationFile::simpleConfigurationFile( const QString & directoryPath ) {
	try {
		MetaConfigurationFile meta( QDir( directoryPath ).absoluteFilePath( "configurationdef.xml" ) );
		foreach( ConfigurationFile * conf, meta.m_configurations ) {
			if( conf->version().isValid() ) {
				return *conf;
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
	Q_ASSERT( ( index >= 0 ) && ( index < m_configurations.count() ) );

	return m_configurations.at( index );
}
