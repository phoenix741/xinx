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
#include "xmlprettyprinter.h"
#include <core/xinxconfig.h>

// Qt header
#include <QTextCodec>
#include <QTextDocument>

/* XMLPrettyPrinter */

XMLPrettyPrinter::XMLPrettyPrinter() {
}

XMLPrettyPrinter::~XMLPrettyPrinter() {
}

void XMLPrettyPrinter::setText( const QString & text ) {
	m_text = text;
}

void XMLPrettyPrinter::process() {
	addData( m_text );
	//QByteArray buffer = m_text.toUtf8();
	//QBuffer dev( &buffer );
	//dev.open( QIODevice::ReadOnly );
	//setDevice( &dev );

	m_resultBuffer.open( QIODevice::WriteOnly | QIODevice::Truncate );
	m_result.setDevice( &m_resultBuffer );
	m_result.setCodec( QTextCodec::codecForName( qPrintable( XINXConfig::self()->config().editor.defaultTextCodec ) ) );

	constructXML();

	m_resultBuffer.close();
	//clear();

	if( error() )
		throw XMLPrettyPrinterException( errorString(), lineNumber(), columnNumber() );
}

QString XMLPrettyPrinter::getResult() {
	return QString( m_resultBuffer.data() );
}

void XMLPrettyPrinter::writeLevel( int level ) {
	for( int i = 0; i < level; i++ )
		m_result << "\t";
}

void XMLPrettyPrinter::constructXML( int level ) {
	QXmlStreamReader::TokenType prevType = QXmlStreamReader::NoToken;
	QString plainText;
	bool firstTour = true;

	while( ! atEnd() ) {
		readNext();

		if( firstTour && ( ( level > 0 ) && ( prevType == QXmlStreamReader::NoToken ) && ( !isWhitespace() ) ) ) {
			if( isEndElement() ) {
				m_result << "/>";
			} else
				m_result << ">";
		}


		if( !isWhitespace() && ( isEntityReference() || isCharacters() ) ) {
			if( isCDATA() )
				plainText += "<![CDATA[" + text().toString() + "]]>";
			else if( isEntityReference() )
				plainText += name().toString();
			else if( isCharacters() )
				plainText += Qt::escape( text().toString() );
		} else if( ! plainText.isEmpty() ) {
			plainText = plainText.replace( QChar(QChar::Nbsp), "&#160;" );
			m_result << plainText.trimmed();
			plainText = QString();
		}

		if( isStartElement() ) {
			if( prevType != QXmlStreamReader::Characters ) {
				m_result << "\n";
				writeLevel( level );
			}

			m_result << "<" + qualifiedName().toString();

			foreach( const QXmlStreamAttribute & a, attributes() ) {
				m_result << " " + a.qualifiedName().toString() + "=\"" + Qt::escape( a.value().toString() ) + "\"";
			}

			QXmlStreamNamespaceDeclarations ns = namespaceDeclarations();
			for (int i = 0; i < ns.size(); ++i) {
				const QXmlStreamNamespaceDeclaration &namespaceDeclaration = ns.at(i);
				if( namespaceDeclaration.prefix().isEmpty() )
					m_result << " xmlns" << "=\"" << namespaceDeclaration.namespaceUri().toString() << "\"";
				else
					m_result << " xmlns:" << namespaceDeclaration.prefix().toString() << "=\"" << namespaceDeclaration.namespaceUri().toString() << "\"";
			}

			constructXML( level + 1 );
		} else if( isEndElement() ) {
			if( prevType != QXmlStreamReader::NoToken ) {
				if( prevType != QXmlStreamReader::Characters ) {
					m_result << "\n";
					writeLevel( level - 1 );
				}
				m_result << "</" + qualifiedName().toString() + ">";
			}
			break;
		} else if( isDTD() ) {
			m_result << text().toString();
		} else if( isProcessingInstruction() ) {
			m_result << "<?" << processingInstructionTarget().toString() << " " << processingInstructionData().toString() << "?>";
		} else if( isStartDocument() ) {
			if( ! documentEncoding().toString().isEmpty() )
				m_result.setCodec( QTextCodec::codecForName( qPrintable( documentEncoding().toString() ) ) );
			else
				m_result.setCodec( QTextCodec::codecForName( qPrintable( XINXConfig::self()->config().editor.defaultTextCodec ) ) );
			m_result << "<?xml version=\"1.0\" encoding=\"" << m_result.codec()->name().constData() << "\"?>";
		} else if( isComment() ) {
			if( prevType != QXmlStreamReader::Characters ) {
				m_result << "\n";
				writeLevel( level );
			}
			m_result << "<!--" << text().toString() << "-->";
		}

		if( !isWhitespace() ) {
			prevType = tokenType();
			firstTour = false;
		}
	}
}

