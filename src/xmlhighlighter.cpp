/****************************************************************************
**
** Copyright (C) 2006 J-P Nurmi. All rights reserved.
**
** The used XML syntax highlighting principles have been adapted from 
** KXESyntaxHighlighter, which is part of KXML Editor 1.1.4,
** (C) 2003 by The KXMLEditor Team (http://kxmleditor.sourceforge.net).
** 
** This file may be used under the terms of the GPL Version 2, June 1991.
** For details, see http://www.gnu.org/licenses/gpl.html
** 
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "xmlhighlighter.h"
#include "xinxconfig.h"
#include "globals.h"

// Regular expressions for parsing XML borrowed from:
// http://www.cs.sfu.ca/~cameron/REX.html
static const QString EXPR_COMMENT_BEGIN		= "<!--";
static const QString EXPR_COMMENT_TEXT		= "([^-]|(-(?!->)))*";
static const QString EXPR_COMMENT_END		= "-->";
static const QString EXPR_COMMENT			= EXPR_COMMENT_BEGIN + EXPR_COMMENT_TEXT + EXPR_COMMENT_END;

static const QString EXPR_NAME				= "([A-Za-z_:]|[^\\x00-\\x7F])([A-Za-z0-9_:.-]|[^\\x00-\\x7F])*";

static const QString EXPR_ATTRIBUTE_VALUE	= "[^<%1{]*[%1{]";
static const QString EXPR_XPATH_VALUE       = "[^<%1}]*[%1}]";

void XmlHighlighter::highlightBlock( const QString& text ) {
	int i = 0;
	int pos = 0;
	int brackets = 0;
	
	state = (previousBlockState() == InElement ? ExpectAttributeOrEndOfElement : NoState);
	
	setCurrentBlockState(NoBlock);

	if (previousBlockState() == InComment) {
		// search for the end of the comment
		QRegExp expression(EXPR_COMMENT_END);
		pos = expression.indexIn(text, i);

		if (pos >= 0) {
			// end comment found
//			const int iLength = expression.matchedLength();
			setFormat(0, pos, m_config->config().formats["xml_comment"]);
			setFormat(pos, 3, m_config->config().formats["xml_syntaxchar"]);
			i += pos + 3; // skip comment
		} else {
			// in comment
			setFormat(0, text.length(), m_config->config().formats["xml_comment"]);
			setCurrentBlockState(InComment);
			return;
		}
	}

	for (; i < text.length(); i++) {
		switch( text.at(i).toAscii() ) {
		case '<':
			brackets++;
			if (brackets == 1) {
				setFormat(i, 1, m_config->config().formats[ "xml_syntaxchar" ]);
				state = ExpectElementNameOrSlash;
			} else {
				// wrong bracket nesting
				setFormat(i, 1, m_config->config().formats[ "xml_error" ]);
			}
			break;

		case '>':
			brackets--;
			if (brackets == 0) {
				setFormat(i, 1, m_config->config().formats[ "xml_syntaxchar" ]);
			} else {
				// wrong bracket nesting
				setFormat( i, 1, m_config->config().formats["xml_error"] );
			}
			state = NoState;
			break;

		case '/':
			if (state == ExpectElementNameOrSlash) {
				state = ExpectElementName;
				setFormat(i, 1, m_config->config().formats[ "xml_syntaxchar" ]);
			} else {
				if (state == ExpectAttributeOrEndOfElement) {
					setFormat(i, 1, m_config->config().formats[ "xml_syntaxchar" ]);
				} else {
					processDefaultText(i, text);
				}
			}
			break;

		case '=':
			if (state == ExpectEqual) {
				state = ExpectAttributeValue;
				setFormat(i, 1, m_config->config().formats["xml_other"]);
			} else {
				processDefaultText(i, text);  
			}
			break;

		case '\'':
		case '\"':
			if (state == ExpectAttributeValue) {
				m_quoteType = text.at(i);
				state = ExpectAttributeTextOrPath;
				setFormat(i, 1, m_config->config().formats["xml_syntaxchar"]);
			} else if( ( state == ExpectAttributeTextOrPath ) && ( m_quoteType == text.at(i) ) ) {
				state = ExpectAttributeOrEndOfElement;
				setFormat(i, 1, m_config->config().formats["xml_syntaxchar"]);
			} else {
				processDefaultText(i, text);
			}
			break;
		case '{' :
			if( state == ExpectAttributeTextOrPath ) {
				state = ExpectPathTextOrEndOfPath;
				setFormat(i, 1, m_config->config().formats["xml_attributevalue" ] );
			} else
				processDefaultText(i, text);
			break;
		case '}' :
			if( state == ExpectPathTextOrEndOfPath ) {
				state = ExpectAttributeTextOrPath;
				setFormat(i, 1, m_config->config().formats["xml_attributevalue"]);
			} else
				processDefaultText(i, text);
			break;
		case '!':
			if (state == ExpectElementNameOrSlash) {
				// search comment
				QRegExp expression(EXPR_COMMENT);
				pos = expression.indexIn(text, i - 1);

				if (pos == i - 1) // comment found ?
				{
					const int iLength = expression.matchedLength();

					setFormat(pos, 4, m_config->config().formats["xml_syntaxchar"]);
					setFormat(pos + 4, iLength - 7, m_config->config().formats["xml_comment"]);
					setFormat(pos + iLength - 3, 3, m_config->config().formats["xml_syntaxchar"]);
					i += iLength - 2; // skip comment
					state = NoState;
					brackets--;
				}
				else
				{
					// Try find multiline comment
					QRegExp expression(EXPR_COMMENT_BEGIN); // search comment start
					pos = expression.indexIn(text, i - 1);

					//if (pos == i - 1) // comment found ?
					if (pos >= i - 1)
					{
						setFormat(i, 3, m_config->config().formats["xml_syntaxchar"]);
						setFormat(i + 3, text.length() - i - 3, m_config->config().formats["xml_comment"]);
						setCurrentBlockState(InComment);
						return;
					}
					else
					{
						processDefaultText(i, text);
					}
				}
			}
			else
			{
				processDefaultText(i, text);
			}

			break;  

		default:
			const int iLength = processDefaultText(i, text);
			if (iLength > 0)
				i += iLength - 1;
			break;
		}
	}

	if (state == ExpectAttributeOrEndOfElement)
	{
		setCurrentBlockState(InElement);
	}
}

int XmlHighlighter::processDefaultText(int i, const QString& text)
{
	// length of matched text
	int iLength = 0;

	switch(state)
	{
	case ExpectElementNameOrSlash:
	case ExpectElementName:
		{
			// search element name
			QRegExp expression(EXPR_NAME);
			const int pos = expression.indexIn(text, i);

			if (pos == i) // found ?
			{
				iLength = expression.matchedLength();

				setFormat(pos, iLength, m_config->config().formats["xml_elementname"]);
				state = ExpectAttributeOrEndOfElement;
			}
			else
			{
				setFormat(i, 1, m_config->config().formats["xml_other"]);
			}
		}  
		break;

	case ExpectAttributeOrEndOfElement:
		{
			// search attribute name
			QRegExp expression(EXPR_NAME);
			const int pos = expression.indexIn(text, i);

			if (pos == i) // found ?
			{
				iLength = expression.matchedLength();

				setFormat(pos, iLength, m_config->config().formats["xml_attributename"]);
				state = ExpectEqual;
			}
			else
			{
				setFormat(i, 1, m_config->config().formats["xml_other"]);
			}
		}
		break;
	case ExpectAttributeTextOrPath: {
			QRegExp expression( EXPR_ATTRIBUTE_VALUE.arg( m_quoteType ) );
			const int pos = expression.indexIn( text, i );
			
			if( pos == i ) {
				iLength = expression.matchedLength() - 1;
				setFormat( pos, iLength, m_config->config().formats["xml_attributevalue"]);
			} else
				setFormat( i, 1, m_config->config().formats["xml_other"] );
		}
		break;
	case ExpectPathTextOrEndOfPath: {
			QRegExp expression( EXPR_XPATH_VALUE.arg( m_quoteType ) );
			const int pos = expression.indexIn( text, i );
			
			if( pos == i ) {
				iLength = expression.matchedLength() - 1;
				setFormat( pos, iLength, m_config->config().formats["xml_xpathvalue"]);
			} else
				setFormat( i, 1, m_config->config().formats["xml_other"] );
		}
		break;
	default:
		setFormat( i, 1, m_config->config().formats["xml_other"] );
		break;
	}
	return iLength;
}  
