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

#ifndef _QLANGUAGE_DEFINITION_H_
#define _QLANGUAGE_DEFINITION_H_

#include "qce-config.h"

/*!
	\file qlanguagedefinition.h
	\brief Definition of the QLanguageDefinition class.
	
	\see QLanguageDefinition
*/

#include <QObject>
#include <QString>

class QLanguageFactory;

class QMarkerInterface;
class QMatcherInterface;
class QIndenterInterface;
class QHighlighterInterface;

class QCE_EXPORT QLanguageDefinition : public QObject
{
	public:
		QLanguageDefinition(QLanguageFactory *f);
		virtual ~QLanguageDefinition();
		
		QLanguageFactory* factory();
		
		virtual const QString& language() const = 0;
		virtual const QStringList& extensions() const = 0;
		
		virtual QMarkerInterface* marker() = 0;
		virtual QMatcherInterface* matcher() = 0;
		virtual QIndenterInterface* indenter() = 0;
		virtual QHighlighterInterface* highlighter() = 0;
		
	private:
		QLanguageFactory *pFactory;
};

#endif // _QLANGUAGE_DEFINITION_H_

