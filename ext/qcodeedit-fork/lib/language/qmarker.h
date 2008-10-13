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

#ifndef _QMARKER_H_
#define _QMARKER_H_

#include "qce-config.h"

/*!
	\file qmarker.h
	\brief Definition of the QMarker class.
*/

#include <QObject>
#include <QPointer>

#include <QList>
#include <QIcon>
#include <QStack>
#include <QColor>
#include <QString>

class QEditor;
class QDocument;
class QDocumentLine;

class QFormatFactory;
class QMarkerInterface;
class QLanguageDefinition;

//class QBlockData;
class QMarkerCommand;

class QCE_EXPORT QMarker : public QObject
{
	Q_OBJECT
	
	friend class QLanguageFactory;
	
	public:
		QMarker(QMarkerInterface *i, QLanguageDefinition *d);
		virtual ~QMarker();
		
		QEditor* editor() const;
		void setEditor(QEditor *e);
		
		QDocument* document() const;
		QLanguageDefinition* definition() const;
		
		void paintMarks(QPainter *p);
		void menu(QDocumentLine& b, const QPoint& pos);
		
		QString defaultMark() const;
		bool isDefaultMarkToggled(QDocumentLine& b) const;
		void toggleDefaultMark(QDocumentLine& b, int state);
		
		bool isMarkToggled(const QDocumentLine& b, const QString& id) const;
		
		void toggleMark(QDocumentLine& b, int mid, bool on);
		void toggleMark(QDocumentLine& b, const QString& id, bool on);
		
		inline QFormatFactory* formatFactory() const { return pFormatFactory; }
		
		QStringList ids() const;
		
		QPixmap icon(const QString& id) const;
		QColor color(const QString& id) const;
		bool needFocus(const QString& id) const;
		bool isPrivate(const QString& id) const;
		int persistency(const QString& id) const;
		QString priority(const QStringList& ids) const;
		
	signals:
		void marked(QDocumentLine& b, const QString& id, bool on);
		
	private slots:
		void cursorPositionChanged();
		
	private:
		QPointer<QEditor> pEdit;
		QList<QMarkerCommand*> marks;
		QMarkerInterface *pInterface;
		
		QFormatFactory *pFormatFactory;
		QLanguageDefinition *pDefinition;
};

#endif // _QMARKER_H_
