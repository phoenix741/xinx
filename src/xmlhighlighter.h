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
#ifndef XMLHIGHLIGHTER_H
#define XMLHIGHLIGHTER_H

#include "syntaxhighlighter.h"
#include <QTextCharFormat>
#include <QColor>

class XmlHighlighter : public SyntaxHighlighter {
	Q_OBJECT
public:
	XmlHighlighter( QObject* parent = NULL, XINXConfig * config = NULL ) : SyntaxHighlighter( parent, config ) { };
	XmlHighlighter( QTextDocument* parent, XINXConfig * config = NULL ) : SyntaxHighlighter( parent, config ) { };
	XmlHighlighter( QTextEdit* parent, XINXConfig * config = NULL ) : SyntaxHighlighter( parent, config ) { };
	virtual ~XmlHighlighter() {};
protected:
	void highlightBlock(const QString& rstrText);
	virtual int processDefaultText(int i, const QString& rstrText);
private:
	enum ParsingState {
		NoState = 0,
		ExpectElementNameOrSlash,
		ExpectElementName,
		ExpectAttributeOrEndOfElement,
		ExpectEqual,
		ExpectAttributeValue,
		ExpectAttributeTextOrPath,
		ExpectPathTextOrEndOfPath
	};

	enum BlockState {
		NoBlock = -1,
		InComment,
		InElement
	};

	ParsingState state;
	QChar m_quoteType;
};

#endif // XMLHIGHLIGHTER_H
