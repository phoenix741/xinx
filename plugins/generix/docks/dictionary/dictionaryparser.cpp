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
#include <QtConcurrentRun>

DictionaryParser::DictionaryParser() : ContentViewParser( true ), m_codec( 0 ) {
	m_rootNode = new ContentViewNode( "dictionary", -1 );
	m_rootNode->setAutoDelete( false );
	setRootNode( m_rootNode );
	setAttachId( (unsigned long)m_rootNode );
}

DictionaryParser::~DictionaryParser() {
	setRootNode( 0 );
	m_rootNode->setAutoDelete( true );
}

QString DictionaryParser::trad( const QString & text, const QString & lang ) const {
}

QString DictionaryParser::trad( const QString & text, const QString & lang, const QString & ctxt ) const {
}

void DictionaryParser::setFilename( const QString & filename ) {
	m_files.clear();
	m_files << filename;
}

void DictionaryParser::setFileList( const QStringList & files ) {
	m_files = files;
}

void DictionaryParser::loads() {
	try {
		this->loadFromMember();
	} catch( ContentViewException e ) {
		qWarning() << e.getMessage();
	}
}

void DictionaryParser::refresh() {
	QFuture<void> f = QtConcurrent::run( this, &DictionaryParser::loads );
}

/*
 * <?xml version = '1.0' encoding = 'ISO-8859-1'?>
 * <root default="FRA" xsi:noNamespaceSchemaLocation="trad.xsd" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">
 *   <labels code="LBL.A">
 *      <label lang="FRA" ctx="a_client_dossier.xsl" value="a"/>
 *      <label lang="FRA" ctx="f_balance_agee_criteres.xsl" value="a"/>
 *   </labels>
 * </root>
 */
void DictionaryParser::loadFromDeviceImpl() {
	rootNode()->setData( ":/images/typexml.png", ContentViewNode::NODE_ICON );

	loadAttachedNode( rootNode() );

	foreach( const QString & filename, m_files ) {
		ContentViewParser::setFilename( filename );
		setDevice( inputDevice() );

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

		if( error() ) {
			throw ContentViewException( errorString(), rootNode()->fileName(), lineNumber(), columnNumber() );
		}
	}

	detachAttachedNode();
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
	labels->setCompareData( ContentViewNode::NODE_DISPLAY_NAME );
	labels->setData( "DICTIONARY_LABELS", ContentViewNode::NODE_TYPE );
	labels->setData( ":/generix/images/dictionary16.png", ContentViewNode::NODE_ICON );
	labels = attachNode( rootNode(), labels );

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
	label->setData( ":/generix/images/label.png", ContentViewNode::NODE_ICON );
	label->setData( lang, ContentViewNode::NODE_USER_VALUE );
	label->setData( ctx, ContentViewNode::NODE_USER_VALUE + 1 );
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

