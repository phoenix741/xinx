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

#include "qlanguagedefinition.h"

/*!
	\file qlanguagedefinition.cpp
	\brief Implementation of QLanguageDefinition
	
	\see QLanguageDefinition
*/

/*!
	\ingroup language
	@{
	
	\class QLanguageDefinition
	\brief Interface for language definition.
	
	This class is meant to be subclassed, see \see QGenericDefinition for more
	informations, and added to a QLanguageFactory.
	
	A language definition is a wrapper that creates interfaces for a given file
	extension from internally handled data (XML files in the case of 
	QGenericDefinition)
	
	\see QLanguageFactory
*/

#include "qlanguagefactory.h"

/*!
	\brief Empty constructor
*/
QLanguageDefinition::QLanguageDefinition(QLanguageFactory *f)
 : QObject(f), pFactory(f)
{
}

/*!
	\brief Empty destructor
*/
QLanguageDefinition::~QLanguageDefinition()
{
}

/*!
	\return The factory owning this language definition
*/
QLanguageFactory* QLanguageDefinition::factory()
{
	return pFactory;
}

/*!
	\fn QLanguageDefinition::language() 
	
	\return The language supported by this definition
*/

/*!
	\fn QLanguageDefinition::extensions()
	
	\return the file extensions corrseponding to the supported language
	
	\see language()
	\see QFileInfo::completeSuffix()
*/

/*!
	\fn QLanguageDefinition::matcher(const QString& ext)
	
	\return The matcher interface that corresponds to the extension \a ext
	
	\see QMatcher
	\see QMatcherInterface
*/

/*!
	\fn QLanguageDefinition::indenter(const QString& ext)
	
	\brief The indenter interface that corresponds to the extension \a ext
	
	\see QIndenter
	\see QIndenterInterface
*/

/*!
	\fn QLanguageDefinition::highlighter(const QString& ext)
	
	\brief The highlighter interface that corresponds to the extension \a ext
	
	\see QHighlighter
	\see QHighlighterInterface
*/

/*! @} */
