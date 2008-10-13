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

#ifndef _QLANGUAGE_FACTORY_H_
#define _QLANGUAGE_FACTORY_H_

/*!
	\file qlanguagefactory.h
	\brief Definition of the QLanguageFactory class.
	
	\see QLanguageFactory
*/

/*!
	\defgroup language Language framework
*/

#include "qce-config.h"

#include <QList>
#include <QObject>

class QStringList;

class QEditor;
class QFormatFactory;
class QCodeCompletionEngine;
class QLanguageDefinition;

class QCE_EXPORT QLanguageFactory : public QObject
{
	Q_OBJECT
	
	public:
		QLanguageFactory(QFormatFactory *fmt, QObject *p = 0);
		virtual ~QLanguageFactory();
		
		QStringList languages() const;
		QStringList fileFilters() const;
		
		void addDefinitionPath(const QString& path);
		
		virtual QLanguageDefinition* definition(const QString& file);
		virtual QCodeCompletionEngine* completionEngine(const QString& file);
		
		inline QFormatFactory* formatFactory() const { return pFormatFactory; }
		
	public slots:
		void addLanguageDefinition(QLanguageDefinition *d);
		void addCompletionEngine(QCodeCompletionEngine *e);
		
		virtual void setLanguage(QEditor *e, const QString& f);
		
		virtual void setLanguage(QEditor *ed,
						QLanguageDefinition *ld,
						QCodeCompletionEngine *ce);
		
	private:
		QFormatFactory *pFormatFactory;
		QList<QCodeCompletionEngine*> m_engines;
		QList<QLanguageDefinition*> definitions;
};

#endif // _QLANGUAGE_FACTORY_H_
