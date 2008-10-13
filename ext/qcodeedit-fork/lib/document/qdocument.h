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

#ifndef _QDOCUMENT_H_
#define _QDOCUMENT_H_

#include "qce-config.h"

#include <QList>
#include <QVector>
#include <QLinkedList>

#include <QObject>
#include <QPalette>
#include <QMetaType>

class QFont;
class QRect;

class QFormatFactory;

struct QDocumentSelection;

class QDocumentLine;
class QDocumentCursor;
class QDocumentPrivate;
class QDocumentCommand;
class QDocumentLineHandle;
class QDocumentCursorHandle;

typedef QVector<QDocumentLineHandle*>::iterator QDocumentIterator;
typedef QVector<QDocumentLineHandle*>::const_iterator QDocumentConstIterator;

Q_DECLARE_METATYPE(QDocumentIterator)
Q_DECLARE_METATYPE(QDocumentConstIterator)

class QCE_EXPORT QDocument : public QObject
{
	friend class QMatcher;
	friend class QDocumentPrivate;
	friend class QDocumentCommand;
	
	Q_OBJECT
	
	public:
		struct PaintContext
		{
			int width;
			int height;
			int xoffset;
			int yoffset;
			QPalette palette;
			bool blinkingCursor;
			bool fillCursorRect;
			QList<QDocumentCursorHandle*> cursors;
			QList<QDocumentSelection> selections;
		};
		
		enum LineEnding
		{
			Conservative,
			Local,
			Unix,
			Windows,
			Mac
		};
		
		enum WhiteSpaceFlag
		{
			ShowNone		= 0x00,
			ShowTrailing	= 0x01,
			ShowLeading		= 0x02,
			ShowTabs		= 0x04
		};
		
		Q_DECLARE_FLAGS(WhiteSpaceMode, WhiteSpaceFlag)
		
		explicit QDocument(QObject *p = 0);
		virtual ~QDocument();
		
		QString text(bool removeTrailing = false, bool preserveIndent = true) const;
		void setText(const QString& s);
		
		void startChunkLoading();
		void stopChunkLoading();
		void addChunk(const QString& txt);
		
		LineEnding lineEnding() const;
		LineEnding originalLineEnding() const;
		void setLineEnding(LineEnding le);
		
		int width() const;
		int height() const;
		
		int lines() const;
		
		int y(int line) const;
		int lineNumber(int ypos) const;
		int y(const QDocumentLine& l) const;
		
		QRect lineRect(int line) const;
		QRect lineRect(const QDocumentLine& l) const;
		
		QDocumentCursor* editCursor() const;
		void setEditCursor(QDocumentCursor *c);
		
		QDocumentLine lineAt(const QPoint& p) const;
		QDocumentCursor cursorAt(const QPoint& p, int xoff) const;
		
		QDocumentLine line(int line) const;
		QDocumentLine line(QDocumentConstIterator iterator) const;
		
		QDocumentCursor cursor(int line, int column = 0) const;
		
		QDocumentLine findLine(int& position) const;
		
		bool isLineModified(const QDocumentLine& l) const;
		
		virtual void draw(QPainter *p, PaintContext& cxt);
		
		void execute(QDocumentCommand *cmd);
		
		inline QDocumentPrivate* impl() { return m_impl; }
		
		QDocumentConstIterator begin() const;
		QDocumentConstIterator end() const;
		
		QDocumentConstIterator iterator(int ln) const;
		QDocumentConstIterator iterator(const QDocumentLine& l) const;
		
		void beginMacro();
		void endMacro();
		
		static QFont font();
		static void setFont(const QFont& f);
		static const QFontMetrics& fontMetrics();
		
		static int tabStop();
		static void setTabStop(int n);
		
		static WhiteSpaceMode showSpaces();
		static void setShowSpaces(WhiteSpaceMode y);
		
		static QFormatFactory* formatFactory();
		static void setFormatFactory(QFormatFactory *f);
		
		static int screenLength(const QChar *d, int l, int tabStop);
		static QString screenable(const QChar *d, int l, int tabStop);
		
		inline void markViewDirty() { formatsChanged(); }
		
		bool isClean() const;
		
	public slots:
		void clear();
		
		void undo();
		void redo();
		
		void setClean();
		
	signals:
		void cleanChanged(bool m);
		
		void undoAvailable(bool y);
		void redoAvailable(bool y);
		
		void formatsChanged();
		void contentsChanged();
		
		void formatsChange (int line, int lines);
		void contentsChange(int line, int lines);
		
		void widthChanged(int width);
		void heightChanged(int height);
		void sizeChanged(const QSize& s);
		
		void lineDeleted(QDocumentLineHandle *h);
		void markChanged(QDocumentLineHandle *l, int m, bool on);
		
		void lineEndingChanged(int lineEnding);
		
	private:
		QString m_leftOver;
		QDocumentPrivate *m_impl;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QDocument::WhiteSpaceMode)

#endif
