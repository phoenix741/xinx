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

#ifndef _QHIGHLIGHTER_H_
#define _QHIGHLIGHTER_H_

/*!
	\file qhighlighter.h
	\brief Definition of the QHighlighter class.
	
	\see QHighlighter
*/

#include "qce-config.h"

#include <QVector>
#include <QPointer>

#include "qdocument.h"

class QFormatFactory;
class QLanguageDefinition;
class QHighlighterInterface;

class QColor;

class QCE_EXPORT QHighlighter : public QObject
{
	Q_OBJECT
	
	friend class QLanguageFactory;
	
	public:
		QHighlighter(QHighlighterInterface *i, QLanguageDefinition *d);
		virtual ~QHighlighter();
		
		QDocument *document() const;
		void setDocument(QDocument *doc);
		
		QLanguageDefinition* definition() const;
		
		QString singleLineComment() const;
		
		inline QHighlighterInterface* iface() const { return pInterface; }
		inline QFormatFactory* formatFactory() const { return pFormatFactory; }
		
	public slots:
		bool reformat(int l, int n);
		
	private:
		/// \internal
		QPointer<QDocument> doc;
		
		/// \internal
		QFormatFactory *pFormatFactory;
		
		/// \internal
		QLanguageDefinition *pDefinition;
		
		/// \internal
		QHighlighterInterface *pInterface;
};

#endif // _QHIGHLIGHTER_H_
