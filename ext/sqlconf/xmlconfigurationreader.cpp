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

#include "xmlconfigurationreader.h"

#include <QSqlQuery>
#include <QVariant>

XmlConfigurationReader::XmlConfigurationReader() {
	
}

XmlConfigurationReader::~XmlConfigurationReader() {
	
}
	
bool XmlConfigurationReader::read( QIODevice * device ) {
	setDevice( device );

	while( ! atEnd() ) {
		readNext();

		if( isStartElement() ) {
			if( name() == "config" )
				readConfiguration();
			else
				raiseError(QObject::tr("The file is not an Generix configuration file."));
		}
	}

	return ! error();
}

void XmlConfigurationReader::readConfiguration() {
    Q_ASSERT( isStartElement() && name() == "config" );

    while( !atEnd() ) {
        readNext();

        if( isEndElement() )
            break;

        if( isStartElement() ) {
            if(name() == "version")
                readVersion();
            else if (name() == "application")
            	readApplication();
            else
                readUnknownElement();
        }
    }
}

void XmlConfigurationReader::readUnknownElement() {
	Q_ASSERT( isStartElement() );

	while( !atEnd() ) {
		readNext();

		if( isEndElement( ))
			break;

		if( isStartElement() )
			readUnknownElement();
    }	
}

void XmlConfigurationReader::readVersion() {
	Q_ASSERT( isStartElement() && name() == "version" );
	
	QString version;
	while( ! atEnd() ) {
		readNext();
		
		if( isEndElement() )
			break;
		
		if( isStartElement() ) {
			if( name() == "numero" )
				version += readElementText();
			else if( name() == "edition_special" ) {
				QString special = readElementText();
				if( ! special.isEmpty() )
					version += " ES" + special;
			} else
                readUnknownElement();
		}
	}
	emit findElement( tr("Version %1.").arg( version ) );
}


void XmlConfigurationReader::readApplication() {
	Q_ASSERT( isStartElement() && name() == "application" );
	
	while( ! atEnd() ) {
		readNext();
		
		if( isEndElement() )
			break;
		
		if( isStartElement() ) {
			if( name() == "businessview_def" )
				readBusinessviewDef();
			else if( name() == "documentrow_def" )
				readDocumentRowDef();
			else if( name() == "document_def" )
				readDocumentDef();
			else if( name() == "view_def" )
				readViewDef();
			else if( name() == "viewstruct_def" )
				readViewStructDef();
			else if( name() == "presentation" )
				readPresentation();
            else
                readUnknownElement();
		} 
	}
}

void XmlConfigurationReader::readDocumentRowDef() {
	Q_ASSERT( isStartElement() && name() == "documentrow_def" );
	
	QSqlQuery query;
	query.prepare( "insert into documentrow_def values( :name, :class, :service )");

	while( ! atEnd() ) {
		readNext();
		
		if( isEndElement() )
			break;
		
		if( isStartElement() ) {
			if( name() == "documentrow" ) {
				query.bindValue( ":name", attributes().value( "name" ).toString() );
				query.bindValue( ":class", attributes().value( "class" ).toString() );
				query.bindValue( ":service", attributes().value( "service" ).toString() );
				query.exec();
				emit findElement( tr("Document row %1 added in documentrow_def.").arg( attributes().value( "name" ).toString() ) );
				
				readElementText();
			} else
                readUnknownElement();
		}
	}
}

void XmlConfigurationReader::readDocumentDef() {
	Q_ASSERT( isStartElement() && name() == "document_def" );
	
	QSqlQuery queryViewObject;
	queryViewObject.prepare( "insert into document_def(name, class, viewobject) values( :name, :class, :viewobject )");
	QSqlQuery queryAPI;
	queryAPI.prepare( "insert into document_def(name, class, applicationmodule, documentrow) values( :name, :class, :applicationmodule, :documentrow )");

	while( ! atEnd() ) {
		readNext();
		
		if( isEndElement() )
			break;
		
		if( isStartElement() ) {
			if( name() == "document" ) {
				if( attributes().value( "class" ) == "fr.generix.technicalframework.application.document.APIDocumentImpl" ) {
					queryAPI.bindValue( ":name", attributes().value( "name" ).toString() );
					queryAPI.bindValue( ":class", attributes().value( "class" ).toString() );
					queryAPI.bindValue( ":applicationmodule", attributes().value( "applicationmodule" ).toString() );
					queryAPI.bindValue( ":documentrow", attributes().value( "documentrow" ).toString() );
					queryAPI.exec();
					emit findElement( tr("API %1 added in document_def.").arg( attributes().value( "name" ).toString() ) );
				} else {
					queryViewObject.bindValue( ":name", attributes().value( "name" ).toString() );
					queryViewObject.bindValue( ":class", attributes().value( "class" ).toString() );
					queryViewObject.bindValue( ":viewobject", attributes().value( "viewobject" ).toString() );
					queryViewObject.exec();
					emit findElement( tr("Document %1 added in document_def.").arg( attributes().value( "name" ).toString() ) );
				}
				
				readElementText();
			} else
                readUnknownElement();
		}
	}
}

void XmlConfigurationReader::readViewDef() {
	Q_ASSERT( isStartElement() && name() == "view_def" );
	
	QSqlQuery query;
	query.prepare( "insert into view_def values( :name, :document )");

	while( ! atEnd() ) {
		readNext();
		
		if( isEndElement() )
			break;
		
		if( isStartElement() ) {
			if( name() == "view_type" ) {
				query.bindValue( ":name", attributes().value( "name" ).toString() );
				query.bindValue( ":document", attributes().value( "document" ).toString() );
				query.exec();
				emit findElement( tr("View type %1 added in view_def.").arg( attributes().value( "name" ).toString() ) );
				
				readUnknownElement();
			} else
                readUnknownElement();
		}
	}
}

void XmlConfigurationReader::readViewStructDef() {
	Q_ASSERT( isStartElement() && name() == "viewstruct_def" );
	
	QSqlQuery query;
	query.prepare( "insert into viewstruct_def values( :name, :defaultRetrieve, :defaultSort )");

	while( ! atEnd() ) {
		readNext();
		
		if( isEndElement() )
			break;
		
		if( isStartElement() ) {
			if( name() == "viewstruct" ) {
				query.bindValue( ":name", attributes().value( "name" ).toString() );
				query.bindValue( ":defaultRetrieve", attributes().value( "defaultRetrieve" ).toString() );
				query.bindValue( ":defaultSort", attributes().value( "defaultSort" ).toString() );
				query.exec();
				emit findElement( tr("ViewStruct %1 added in viewstruct_def.").arg( attributes().value( "name" ).toString() ) );
				
				readUnknownElement();
			} else
                readUnknownElement();
		}
	}
}

void XmlConfigurationReader::readBusinessviewDef() {
	Q_ASSERT( isStartElement() && name() == "businessview_def" );
	
	QSqlQuery query;
	query.prepare( "insert into businessview_def values( :name, :target, :viewstruct )");

	while( ! atEnd() ) {
		readNext();
		
		if( isEndElement() )
			break;
		
		if( isStartElement() ) {
			if( name() == "businessview" ) {
				QString name = attributes().value( "name" ).toString();
				query.bindValue( ":name", name );
				query.bindValue( ":target", attributes().value( "target" ).toString() );
				query.bindValue( ":viewstruct", attributes().value( "viewstruct" ).toString() );
				query.exec();
				emit findElement( tr("BusinessView %1 added in businessview_def.").arg( name ) );
				
				readElementText();
			} else
                readUnknownElement();
		}
	}
}

void XmlConfigurationReader::readPresentation() {
	Q_ASSERT( isStartElement() && name() == "presentation" );
	
	QSqlQuery query;
	query.prepare( "insert into presentation values( :bv, :fileref, :lang, :support, :type )");

	while( ! atEnd() ) {
		readNext();
		
		if( isEndElement() )
			break;
		
		if( isStartElement() ) {
			QString bv = attributes().value( "businessview" ).toString(),
					type = name().toString(),
					fileRef = name() == "pdf" ? attributes().value( "fileRefFo" ).toString() : attributes().value( "fileRef" ).toString();

			query.bindValue( ":bv", bv );
			query.bindValue( ":type", type );
			query.bindValue( ":fileref", fileRef );
			query.bindValue( ":lang", attributes().value( "lang" ).toString() );
			query.bindValue( ":support", attributes().value( "support" ).toString() );
			query.exec();
			emit findElement( tr("Presentation %1 added in presentation.").arg( bv ) );

			readElementText();
		}
	}
}

