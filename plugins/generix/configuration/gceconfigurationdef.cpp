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
#include "gceconfigurationdef.h"

// Qt header
#include <QXmlStreamReader>
#include <QIODevice>
#include <QDebug>
#include <QDomDocument>
#include <QFileInfo>
#include <QDir>
#include <QFile>

/* GceConfigurationDefParser */

class GceConfigurationDefParser : public QXmlStreamReader {
public:
	GceConfigurationDefParser();

	bool loadFromFile( const QString & filename );

	/* INPUT */
	int m_configurationNumber;

	/* OUTPUT */
	QString m_version, m_rootPath;
	int m_edition;
	QMultiHash<QString,BusinessViewInformation> m_fileRefToInformation;

private:
	void readUnknownElement();
	void readConfigElement();
	void readVersionElement();
	void readApplicationElement();

	void readBusinessViewDef();
	void readBusinessViewDefElement();
	void readPresentation();
	void readPresentationElement();

	QString m_configurationFileName;
	QHash<QString,BusinessViewInformation> m_nameToInformation;
	QMultiHash<QString,QString> m_fileRefToName;
};

GceConfigurationDefParser::GceConfigurationDefParser() {

}

bool GceConfigurationDefParser::loadFromFile( const QString & filename ) {
	m_configurationFileName = filename;

	QFile device( filename );
	if( ! device.open( QFile::ReadOnly ) ) {
		return false;
	}

	setDevice( &device );

	while( ! atEnd() ) {
		readNext();

		if( isStartElement() ) {
			if( name() == "config" )
				readConfigElement();
			else
				raiseError("The file is not a configuration file.");
		}
	}

	m_fileRefToInformation.clear();
	foreach( const QString & bvName, m_fileRefToName.keys() ) {
		foreach( const QString & fileRef, m_fileRefToName.values( bvName ) ) {
			const QString ref = m_rootPath + "/" + fileRef;
			m_fileRefToInformation.insert( ref, m_nameToInformation.value( bvName ) );
		}
	}

	return ! error();
}

void GceConfigurationDefParser::readUnknownElement() {
	Q_ASSERT( isStartElement() );

	while( !atEnd() ) {
		readNext();

		if( isEndElement() )
			break;

		if( isStartElement() )
			readUnknownElement();
	}
}

void GceConfigurationDefParser::readConfigElement() {
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

void GceConfigurationDefParser::readVersionElement() {
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
				m_edition = readElementText().toInt();
			else
				readUnknownElement();
		}
	}
}

void GceConfigurationDefParser::readApplicationElement() {
	Q_ASSERT( isStartElement() && ( QXmlStreamReader::name() == "application" ) );

	while( !atEnd() ) {
		readNext();

		if( isEndElement() )
			break;

		if( isStartElement() ) {
			if( QXmlStreamReader::name() == "businessview_def" ) {
				readBusinessViewDef();
			} else if( QXmlStreamReader::name() == "presentation" ) {
				readPresentation();
			} else
				readUnknownElement();
		}
	}
}

void GceConfigurationDefParser::readBusinessViewDef() {
	Q_ASSERT( isStartElement() && ( QXmlStreamReader::name() == "businessview_def" ) );

	while( !atEnd() ) {
		readNext();

		if( isEndElement() )
			break;

		if( isStartElement() ) {
			if( QXmlStreamReader::name() == "businessview" ) {
				readBusinessViewDefElement();
			} else
				readUnknownElement();
		}
	}
}

void GceConfigurationDefParser::readBusinessViewDefElement() {
	Q_ASSERT( isStartElement() && ( QXmlStreamReader::name() == "businessview" ) );

	BusinessViewInformation information;
	information.setBusinessViewName( attributes().value( "name" ).toString() );
	information.setTargetName( attributes().value( "target" ).toString() );
	information.setViewstructName( attributes().value( "viewstruct" ).toString() );

	information.setConfigurationFileName( m_configurationFileName );
	information.setConfigurationNumber( m_configurationNumber );

	m_nameToInformation.insert( information.businesViewName(), information );

	readUnknownElement();
}

void GceConfigurationDefParser::readPresentation() {
	Q_ASSERT( isStartElement() && ( QXmlStreamReader::name() == "presentation" ) );
	if( ! attributes().value( "rootPath" ).isEmpty() )
		m_rootPath = attributes().value( "rootPath" ).toString();

	while( !atEnd() ) {
		readNext();

		if( isEndElement() )
			break;

		if( isStartElement() )
			readPresentationElement();
	}
}

void GceConfigurationDefParser::readPresentationElement() {
	Q_ASSERT( isStartElement() );

	const QString bv = attributes().value( "businessview" ).toString();
	const QString fr = attributes().value( "fileRef" ).toString();

	if( ! m_fileRefToName.values( bv ).contains( fr ) )
		m_fileRefToName.insert( bv, fr );

	readUnknownElement();
}

/* GceConfigurationDef */

GceConfigurationDef::GceConfigurationDef() {

}

GceConfigurationDef::GceConfigurationDef( const QString & filename ) {
	m_directoryPath = QFileInfo( filename ).absolutePath();

	readConfigurationDef( filename );
}

GceConfigurationDef::~GceConfigurationDef() {

}

void GceConfigurationDef::readConfigurationDef( const QString & configurationdefFileName ) {
	m_filenames.append( configurationdefFileName );

	m_configurationFileName = configurationdefFileName;

	QFile configurationdef( configurationdefFileName );
	if( ! configurationdef.open( QFile::ReadOnly ) )
		throw GceInterfaceException( tr("Can't open configurationdef file" ) );

	QDomDocument document;
	if( ! document.setContent( &configurationdef ) )
		throw GceInterfaceException( tr("Can't read the content of the configurationdef file" ) );

	QDomElement root = document.documentElement();

	QDomElement configuration = root.firstChildElement( "configuration" );
	if( ! configuration.isNull() ) {
		int configurationNumber = 0;
		QDomElement conffile = configuration.firstChildElement( "file" );
		while( ! conffile.isNull() ) {
			readConfigurationFile( configurationNumber++, QDir( m_directoryPath ).absoluteFilePath( conffile.attribute( "name" ) ) );

			conffile = conffile.nextSiblingElement( "file" );
		}
	}
	QDomElement translation = root.firstChildElement( "translation" );
	if( ! translation.isNull() ) {
		QDomElement translationFile = translation.firstChildElement( "file" );
		while( ! translationFile.isNull() ) {
			m_dictionnaries += QDir( m_directoryPath ).absoluteFilePath( translationFile.attribute( "name" ) );

			translationFile = translationFile.nextSiblingElement( "file" );
		}
	}
}

void GceConfigurationDef::readConfigurationFile( int configurationIndex, const QString & configurationFileName ) {
	GceConfigurationDefParser parser;
	parser.m_configurationNumber = configurationIndex;
	parser.loadFromFile( configurationFileName );
	m_filenames.append( configurationFileName );

	m_fileToInformation += parser.m_fileRefToInformation;
	m_version			 = ConfigurationVersion( parser.m_version, parser.m_edition );
}

ConfigurationVersion GceConfigurationDef::version() {
	return m_version;
}

QStringList GceConfigurationDef::dictionnaries() {
	return m_dictionnaries;
}

QString GceConfigurationDef::rootFilename() {
	return m_configurationFileName;
}

QStringList GceConfigurationDef::filenames() {
	return m_filenames;
}

QList<BusinessViewInformation> GceConfigurationDef::businessView( const QString & filename ) {
	return m_fileToInformation.values( filename );
}

QList<BusinessViewInformation> GceConfigurationDef::businessViews() {
	return m_fileToInformation.values();
}

QString GceConfigurationDef::resolveFileName( const QString & filename ) {
	return filename;
}
