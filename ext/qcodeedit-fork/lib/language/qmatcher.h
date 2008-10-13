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

#ifndef _QMATCHER_H_
#define _QMATCHER_H_

/*!
	\file qmatcher.h
	\brief Definition of the QMatcher class.
	
	\see QMatcher
*/

#include "qce-config.h"

#include <QObject>
#include <QPointer>

#include "qformat.h"
#include "qdocumentline.h"

class QEditor;
class QDocumentLine;
class QDocumentCursor;

class QFormatFactory;
class QMatcherInterface;
class QLanguageDefinition;

class QCE_EXPORT QMatcher : public QObject
{
	Q_OBJECT
	
	friend class QEditor;
	friend class QLanguageFactory;
	
	public:
		enum BlockFoldFlags
		{
			None			= 0x00000000,
			Collapsible		= 0x01000000,
			Collapsed		= 0x02000000,
			Closure			= 0x04000000,
			DataMask		= 0x00ffffff
		};
		
		QMatcher(QMatcherInterface *i, QLanguageDefinition *d);
		
		QEditor* editor() const;
		void setEditor(QEditor *e);
		
		QDocument* document() const;
		QLanguageDefinition* definition() const;
		
		QChar autoClose(QChar c) const;
		
		void expand(int b);
		void collapse(int b);
		
		int blockFlags(int b, int depth);
		
		void setMatch(	int begline, int begpos, int beglen,
						int endline, int endpos, int endlen);
		
		inline QFormatFactory* formatFactory() const { return pFormatFactory; }
		
	private slots:
		void match();
		void cursorPositionChanged();
		
	private:
		QPointer<QEditor> pEdit;
		QMatcherInterface *pInterface;
		
		bool locked;
		
		int lBeg, lEnd;
		QFormatRange fBeg, fEnd;
		
		QFormatFactory *pFormatFactory;
		QLanguageDefinition *pDefinition;
};

#endif // _QMATCHER_H_
