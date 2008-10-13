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

#ifndef _QINDENTER_H_
#define _QINDENTER_H_

/*!
	\file qindenter.h
	\brief Definition of the QIndenter class.
	
	\see QIndenter
*/

#include "qce-config.h"

#include <QStack>
#include <QObject>
#include <QPointer>

class QKeyEvent;

class QEditor;
class QDocument;
class QDocumentCursor;
class QIndenterInterface;
class QLanguageDefinition;

class QCE_EXPORT QIndenter : public QObject
{
	Q_OBJECT
	
	friend class QLanguageFactory;
	
	public:
		QIndenter(QIndenterInterface *i, QLanguageDefinition *d);
		
		QEditor* editor() const;
		void setEditor(QEditor *e);
		
		QDocument* document() const;
		QLanguageDefinition* definition() const;
		
		QString indent(const QDocumentCursor& c);
		bool unindent (const QDocumentCursor& c, QKeyEvent *k);
		
	protected:
		virtual bool eventFilter(QObject *o, QEvent *e);
		
	private:
		QPointer<QEditor> pEdit;
		QIndenterInterface *pInterface;
		
		QLanguageDefinition *pDefinition;
};

#endif // _QINDENTER_H_


