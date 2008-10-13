/****************************************************************************
**
** Copyright (C) 2006-2008 fullmetalcoder <fullmetalcoder@hotmail.fr>
**
** This file is part of the Edyuk project <http://edyuk.org>
** 
** This file may be used under the terms of the GNU General Public License
** version 3 as published by the Free Software Foundation and appearing in the
** file GPL.txt included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "qhighlighter.h"

/*!
	\file qhighlighter.cpp
	\brief Implementation of QHighlighter
	
	\see QHighlighter
*/

/*!
	\ingroup language
	@{
	
	\class QHighlighter
	\brief Wrapper class that "connect" a text editor to a QHighlighterInterface.
	
	\see QHighlighterInterface
*/

#include "qnfa.h"

#include "qdocument.h"
#include "qdocumentline.h"

#include "qformat.h"
#include "qformatfactory.h"

#include "qlanguagedefinition.h"
#include "qhighlighterinterface.h"

#include <QTimer>
#include <QApplication>

/*!
	\brief Constructor
	
	\param i interface used when highlighting triggered
	\param d language definition used to create the interface
*/
QHighlighter::QHighlighter(QHighlighterInterface *i, QLanguageDefinition *d)
 : QObject(d), pDefinition(d), pInterface(i)
{
	
}

/*!
	\brief Destructor
*/
QHighlighter::~QHighlighter()
{
	setDocument(0);
}

/*!
	\brief Attach the highlighter to a text document
	
	\param d target document
	
	\note Previously attached document is detached.
*/
void QHighlighter::setDocument(QDocument *d)
{
	if ( doc )
	{
		disconnect(	doc	, SIGNAL( contentsChange(int, int) ),
					this, SLOT  ( reformat(int, int) ) );
		
	}
	
	doc = d;
	
	if ( doc && pInterface )
	{
		connect(doc	, SIGNAL( contentsChange(int, int) ),
				this, SLOT  ( reformat(int, int) ) );
		
		//doc->setFormatFactory(formatFactory());
	}
}

/*!
	 \return the document to which this highlighter is attached
*/
QDocument *QHighlighter::document() const
{
	return doc;
}

/*!
	\return the language definition that created this matcher
*/
QLanguageDefinition* QHighlighter::definition() const
{
	return pDefinition;
}

/*!
	\return A string which would cause a single line to be considered as comment, if supported by the language
*/
QString QHighlighter::singleLineComment() const
{
	return pInterface ? pInterface->singleLineComment() : QString();
}

/*!
	\internal
*/
bool QHighlighter::reformat(int l, int n)
{
	if ( !pInterface || !doc )
		return false;
	
	bool cont = n;
	QNFA *prevcxt;
	QDocumentLine line, prev = doc->line(l - 1);
	
	//qDebug("reformating %i lines from %i...", n, l);
	
	while ( cont )
	{
		line = doc->line(l++);
		
		if ( !line.isValid() )
			break;
		
		// get last context nest to check for noteworthy modifications needing further work
		prevcxt = line.matchContext()->context;
		
		if ( prev.isValid() )
		{
			// go on using previous match context (NFA naming...)
			
			line.matchContext()->context = prev.matchContext()->context;
			line.matchContext()->parents = prev.matchContext()->parents;
			line.matchContext()->meaningless = prev.matchContext()->meaningless;
		} else {
			// first line : reset context....
			
			line.matchContext()->reset();
			
			/*
			qDebug("reset : popping down by %i contexts", line.matchContext()->parents.count());
			
			while ( line.matchContext()->parents.count() )
				line.matchContext()->context = line.matchContext()->parents.pop();
			*/
		}
		
		// call the implementation
		pInterface->highlight(line, formatFactory());
		
		// update cont state, i.e. whether or not highlighting info of next block shall be updated
		cont = (--n > 0) || (prevcxt != line.matchContext()->context);
		
		prev = line;
	}
	
	//qDebug("went farther than %i lines : %i", base, n < 0);
	
	return n < 0;
}

/*! @} */
