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
#include "dictionaryparser.h"
#include <contentview/contentviewnode.h>

// Qt header
#include <QTextCodec>
#include <QVariant>

DictionaryParser::DictionaryParser( bool autoDelete ) : ContentViewParser( autoDelete ), m_codec( 0 ) {
}

DictionaryParser::~DictionaryParser() {
}

/*
 * <?xml version = '1.0' encoding = 'ISO-8859-1'?>
 * <root default="FRA" xsi:noNamespaceSchemaLocation="trad.xsd" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
 *   <labels code="LBL.A">
 *      <label lang="FRA" ctx="a_client_dossier.xsl" value="�"/>
 *      <label lang="FRA" ctx="f_balance_agee_criteres.xsl" value="�"/>
 *   </labels>
 * </root>
 */
void DictionaryParser::loadFromDeviceImpl() {
	setDevice( inputDevice() );

	rootNode()->setData( ":/images/typexml.png", ContentViewNode::NODE_ICON );

	loadAttachedNode( rootNode() );

	while( ! atEnd() ) {
		readNext();

		if( isStartDocument() ) {
			m_codec = QTextCodec::codecForName( documentEncoding().toString().toLatin1() );
		}

		if( isStartElement() ) {
			if( name() == "root" )
				readRootNode();
			else
				raiseError(tr("The file is not an dictionary file."));
		}
	}

	if( !error() ) {
		detachAttachedNode();
	} else
		throw ContentViewException( errorString(), rootNode()->fileName(), lineNumber(), columnNumber() );
}


void DictionaryParser::readRootNode() {
	Q_ASSERT( isStartElement() && QXmlStreamReader::name() == "root" );

	while( !atEnd() ) {
		readNext();

		if( isEndElement() )
			break;

		if( isStartElement() ) {
			if( QXmlStreamReader::name() == "labels" )
				readLabelsNode();
			else
				raiseError( tr("Labels node expected. %1 found" ).arg( QXmlStreamReader::name().toString() ) );
		}
	}
}

void DictionaryParser::readLabelsNode() {
	Q_ASSERT( isStartElement() && QXmlStreamReader::name() == "labels" );

	QString code = attributes().value( "code" ).toString();
	ContentViewNode * labels = new ContentViewNode( code, lineNumber() );
	labels->setData( "DICTIONARY_LABELS", ContentViewNode::NODE_TYPE );
	labels->setData( ":/dictionary/images/dictionary16.png", ContentViewNode::NODE_ICON );
	attachNode( rootNode(), labels );

	while( !atEnd() ) {
		readNext();

		if( isEndElement() )
			break;

		if( isStartElement() ) {
			if( QXmlStreamReader::name() == "label" )
				readLabelNode( labels );
			else
				raiseError( tr("Labels node expected. %1 found" ).arg( QXmlStreamReader::name().toString() ) );
		}
	}
}

void DictionaryParser::readLabelNode( ContentViewNode * parent ) {
	Q_ASSERT( isStartElement() && QXmlStreamReader::name() == "label" );

	QString lang  = attributes().value( "lang" ).toString(),
					ctx   = attributes().value( "ctx" ).toString(),
					value = attributes().value( "value" ).toString();
	ContentViewNode * label = new ContentViewNode( value, lineNumber() );
	label->setData( QString( "%1 [Langue=%2] [Ctx=%3]" ).arg( value ).arg( lang ).arg( ctx ), ContentViewNode::NODE_DISPLAY_NAME );
	label->setData( "DICTIONARY_LABEL", ContentViewNode::NODE_TYPE );
	label->setData( ":/dictionary/images/label.png", ContentViewNode::NODE_ICON );
	label->setData( lang, ContentViewNode::NODE_USER_VALUE );
	label->setData( ctx, (ContentViewNode::RoleIndex)(ContentViewNode::NODE_USER_VALUE + 1) );
	attachNode( parent, label );

	while( !atEnd() ) {
		readNext();

		if( isEndElement() )
			break;

		if( isStartElement() ) {
				raiseError( tr("Label node expected. %1 found" ).arg( QXmlStreamReader::name().toString() ) );
		}
	}
}

