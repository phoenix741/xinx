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

static const QColor DEFAULT_SYNTAX_CHAR		= Qt::blue;
static const QColor DEFAULT_ELEMENT_NAME	= Qt::darkRed;
static const QColor DEFAULT_ATTRIBUTE_NAME	= Qt::red;
static const QColor DEFAULT_ATTRIBUTE_VALUE	= Qt::blue;
static const QColor DEFAULT_XPATH_VALUE		= Qt::darkMagenta;

// Regular expressions for parsing XML borrowed from:
// http://www.cs.sfu.ca/~cameron/REX.html
static const QString EXPR_COMMENT_BEGIN		= "<!--";
static const QString EXPR_COMMENT_TEXT		= "([^-]|(-(?!->)))*";
static const QString EXPR_COMMENT_END		= "-->";
static const QString EXPR_COMMENT			= EXPR_COMMENT_BEGIN + EXPR_COMMENT_TEXT + EXPR_COMMENT_END;

static const QString EXPR_NAME				= "([A-Za-z_:]|[^\\x00-\\x7F])([A-Za-z0-9_:.-]|[^\\x00-\\x7F])*";

static const QString EXPR_ATTRIBUTE_VALUE	= "[^<%1{]*[%1{]";
static const QString EXPR_XPATH_VALUE       = "[^<%1}]*[%1}]";

void XmlHighlighter::init( bool useConfig ) {
	SyntaxHighlighter::init( useConfig );

	if( ! useConfig ) {
		m_syntaxFormats["SyntaxChar"].setForeground( DEFAULT_SYNTAX_CHAR );
		m_syntaxFormats["ElementName"].setForeground( DEFAULT_ELEMENT_NAME );
		m_syntaxFormats["AttributeName"].setForeground( DEFAULT_ATTRIBUTE_NAME );	
		m_syntaxFormats["AttributeValue"].setForeground( DEFAULT_ATTRIBUTE_VALUE );	
		m_syntaxFormats["XPathValue"].setForeground( DEFAULT_XPATH_VALUE );	
	} else {
		foreach( QString key, global.m_xinxConfig->managedStructure()["xml"].color.keys() ) {
			m_syntaxFormats[ key ] = global.m_xinxConfig->managedStructure()["xml"].color[ key ];
		}
	}
}

bool XmlHighlighter::isFormat( QString type ) {
	if( type == "SyntaxChar" ) 
		return true;
	else 	
	if( type == "ElementName" ) 
		return true;
	else
	if( type == "AttributeName" ) 
		return true;
	else
	if( type == "AttributeValue" ) 
		return true;
	else
	if( type == "XPathValue" ) 
		return true;

	return SyntaxHighlighter::isFormat( type );
}

void XmlHighlighter::highlightBlock( const QString& text ) {
	int i = 0;
	int pos = 0;
	int brackets = 0;
	
	state = (previousBlockState() == InElement ? ExpectAttributeOrEndOfElement : NoState);
	
	setCurrentBlockState(NoBlock);

	if (previousBlockState() == InComment)
	{
		// search for the end of the comment
		QRegExp expression(EXPR_COMMENT_END);
		pos = expression.indexIn(text, i);

		if (pos >= 0)
		{
			// end comment found
//			const int iLength = expression.matchedLength();
			setFormat(0, pos, m_syntaxFormats["Comment"]);
			setFormat(pos, 3, m_syntaxFormats["SyntaxChar"]);
			i += pos + 3; // skip comment
		}
		else
		{
			// in comment
			setFormat(0, text.length(), m_syntaxFormats["Comment"]);
			setCurrentBlockState(InComment);
			return;
		}
	}

	for (; i < text.length(); i++)
	{	
		switch (text.at(i).toAscii())
		{
		case '<':
			brackets++;
			if (brackets == 1)
			{
				setFormat(i, 1, m_syntaxFormats["SyntaxChar"]);
				state = ExpectElementNameOrSlash;
			}
			else
			{
				// wrong bracket nesting
				setFormat(i, 1, m_syntaxFormats["Error"]);
			}
			break;

		case '>':
			brackets--;
			if (brackets == 0)
			{
				setFormat(i, 1, m_syntaxFormats["SyntaxChar"]);
			}
			else
			{
				// wrong bracket nesting
				setFormat( i, 1, m_syntaxFormats["Error"]);
			}
			state = NoState;
			break;

		case '/':
			if (state == ExpectElementNameOrSlash)
			{
				state = ExpectElementName;
				setFormat(i, 1, m_syntaxFormats["SyntaxChar"]);
			}
			else
			{
				if (state == ExpectAttributeOrEndOfElement)
				{
					setFormat(i, 1, m_syntaxFormats["SyntaxChar"]);
				}
				else
				{
					processDefaultText(i, text);
				}
			}
			break;

		case '=':
			if (state == ExpectEqual)
			{
				state = ExpectAttributeValue;
				setFormat(i, 1, m_syntaxFormats["Other"]);
			}
			else
			{
				processDefaultText(i, text);  
			}
			break;

		case '\'':
		case '\"':
			if (state == ExpectAttributeValue) {
				m_quoteType = text.at(i);
				state = ExpectAttributeTextOrPath;
				setFormat(i, 1, m_syntaxFormats["SyntaxChar"]);
			} else if( ( state == ExpectAttributeTextOrPath ) && ( m_quoteType == text.at(i) ) ) {
				state = ExpectAttributeOrEndOfElement;
				setFormat(i, 1, m_syntaxFormats["SyntaxChar"]);
			} else {
				processDefaultText(i, text);
			}
			break;
		case '{' :
			if( state == ExpectAttributeTextOrPath ) {
				state = ExpectPathTextOrEndOfPath;
				setFormat(i, 1, m_syntaxFormats["AttributeValue"]);
			} else
				processDefaultText(i, text);
			break;
		case '}' :
			if( state == ExpectPathTextOrEndOfPath ) {
				state = ExpectAttributeTextOrPath;
				setFormat(i, 1, m_syntaxFormats["AttributeValue"]);
			} else
				processDefaultText(i, text);
			break;
		case '!':
			if (state == ExpectElementNameOrSlash)
			{
				// search comment
				QRegExp expression(EXPR_COMMENT);
				pos = expression.indexIn(text, i - 1);

				if (pos == i - 1) // comment found ?
				{
					const int iLength = expression.matchedLength();

					setFormat(pos, 4, m_syntaxFormats["SyntaxChar"]);
					setFormat(pos + 4, iLength - 7, m_syntaxFormats["Comment"]);
					setFormat(pos + iLength - 3, 3, m_syntaxFormats["SyntaxChar"]);
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
						setFormat(i, 3, m_syntaxFormats["SyntaxChar"]);
						setFormat(i + 3, text.length() - i - 3, m_syntaxFormats["Comment"]);
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

				setFormat(pos, iLength, m_syntaxFormats["ElementName"]);
				state = ExpectAttributeOrEndOfElement;
			}
			else
			{
				setFormat(i, 1, m_syntaxFormats["Other"]);
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

				setFormat(pos, iLength, m_syntaxFormats["AttributeName"]);
				state = ExpectEqual;
			}
			else
			{
				setFormat(i, 1, m_syntaxFormats["Other"]);
			}
		}
		break;
	case ExpectAttributeTextOrPath: {
			QRegExp expression( EXPR_ATTRIBUTE_VALUE.arg( m_quoteType ) );
			const int pos = expression.indexIn( text, i );
			
			if( pos == i ) {
				iLength = expression.matchedLength() - 1;
				setFormat( pos, iLength, m_syntaxFormats["AttributeValue"]);
			} else
				setFormat( i, 1, m_syntaxFormats["Other"] );
		}
		break;
	case ExpectPathTextOrEndOfPath: {
			QRegExp expression( EXPR_XPATH_VALUE.arg( m_quoteType ) );
			const int pos = expression.indexIn( text, i );
			
			if( pos == i ) {
				iLength = expression.matchedLength() - 1;
				setFormat( pos, iLength, m_syntaxFormats["XPathValue"]);
			} else
				setFormat( i, 1, m_syntaxFormats["Other"] );
		}
		break;
	default:
		setFormat( i, 1, m_syntaxFormats["Other"] );
		break;
	}
	return iLength;
}  
