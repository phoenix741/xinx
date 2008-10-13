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

#ifndef _QDOCUMENT_P_H_
#define _QDOCUMENT_P_H_

#include "qce-config.h"

#include "qnfa.h"

#include "qdocument.h"
#include "qdocumentline.h"
#include "qdocumentcursor.h"

#include <QPair>
#include <QHash>
#include <QFont>
#include <QStack>
#include <QQueue>
#if QT_VERSION < 0x040400
#include <QAtomic>
#else
#include <QAtomicInt>
#endif
#include <QUndoStack>
#include <QStringList>
#include <QTextLayout>
#include <QFontMetrics>
#include <QUndoCommand>

#include <QTextLayout>

template <typename T, int x>
class QVarLengthArray;

//typedef QVarLengthArray<int, 10> QSmallArray;
//typedef QVarLengthArray<quint8, 100> QMediumArray;

typedef QVector<int> QSmallArray;
typedef QVector<quint8> QMediumArray;

class QDocument;
class QDocumentPrivate;
class QDocumentCommand;
class QDocumentCommandBlock;

struct QCE_EXPORT QDocumentSelection
{
	int start, end;
	int startLine, endLine;
};

Q_DECLARE_TYPEINFO(QDocumentSelection, Q_PRIMITIVE_TYPE);

class QCE_EXPORT QDocumentLineHandle
{
	friend class QDocumentLine;
	friend class QDocumentPrivate;
	
	public:
		QDocumentLineHandle(QDocument *d);
		QDocumentLineHandle(const QString& s, QDocument *d);
		
		int count() const;
		int length() const;
		
		int position() const;
		
		QString text() const;
		
		int line() const;
		
		int xToCursor(int x, int xoff) const;
		int cursorToX(int i) const;
		
		int nextNonSpaceChar(uint pos) const;
		int previousNonSpaceChar(int pos) const;
		
		bool hasFlag(QDocumentLine::State s) const;
		void setFlag(QDocumentLine::State s, bool y = true) const;
		
		QDocument* document() const;
		
		QDocumentLineHandle* next() const;
		QDocumentLineHandle* previous() const;
		
		void setFormats(const QVector<quint8>& formats);
		
		void clearOverlays();
		void addOverlay(const QFormatRange& over);
		void removeOverlay(const QFormatRange& over);
		
		void draw(	QPainter *p,
					int xOffset,
					int vWidth,
					const QSmallArray& sel,
					const QSmallArray& cursors,
					const QPalette& pal,
					bool fullSel) const;
		
		inline QString& textBuffer() { setFlag(QDocumentLine::LayoutDirty, true); return m_text; }
		
		inline void ref() { m_ref.ref(); }
		inline void deref() { if ( m_ref ) m_ref.deref(); if ( !m_ref ) delete this; }
		
	protected:
		~QDocumentLineHandle();
		
	private:
		void layout() const;
		void applyOverlays() const;
		
		QMediumArray compose() const;
		QList<QTextLayout::FormatRange> decorations() const;
		
		QString m_text;
		QDocument *m_doc;
#if QT_VERSION < 0x040400
		QBasicAtomic m_ref;
#else
		QAtomicInt m_ref;
#endif
		mutable quint16 m_state;
		mutable QTextLayout *m_layout;
		mutable QVector<quint8> m_cache;
		
		QNFAMatchContext m_context;
		
		QVector<quint8> m_formats;
		QVector<QParenthesis> m_parens;
		QList<QFormatRange> m_overlays;
};

Q_DECLARE_TYPEINFO(QDocumentLineHandle*, Q_PRIMITIVE_TYPE);

class QCE_EXPORT QDocumentCursorHandle
{
	friend class QDocumentCursor;
	friend class QDocumentPrivate;
	friend class QDocumentCommand;
	
	public:
		QDocument* document() const;
		
		bool atEnd() const;
		bool atStart() const;
		
		bool atBlockEnd() const;
		bool atBlockStart() const;
		
		bool atLineEnd() const;
		bool atLineStart() const;
		
		bool hasSelection() const;
		
		bool isSilent() const;
		void setSilent(bool y);
		
		bool isAutoUpdated() const;
		void setAutoUpdated(bool y);
		
		QDocumentLine line() const;
		
		int lineNumber() const;
		int anchorColumn() const;
		int visualColumn() const;
		int columnNumber() const;
		void setColumnNumber(int c, QDocumentCursor::MoveMode m =
									QDocumentCursor::MoveAnchor);
		
		int position() const;
		
		void shift(int offset);
		void setPosition(int pos, QDocumentCursor::MoveMode m);
		bool movePosition(int offset, QDocumentCursor::MoveOperation op, QDocumentCursor::MoveMode m);
		
		void insertText(const QString& s);
		
		void deleteChar();
		void deletePreviousChar();
		
		QDocumentCursor selectionStart() const;
		QDocumentCursor selectionEnd() const;
		
		bool eq(const QDocumentCursorHandle *h);
		bool lt(const QDocumentCursorHandle *h);
		bool gt(const QDocumentCursorHandle *h);
		
		QString selectedText() const;
		
		void clearSelection();
		void removeSelectedText();
		
		void select(QDocumentCursor::SelectionType t);
		void setSelectionBoundary(const QDocumentCursor& c);
		
		bool isWithinSelection(const QDocumentCursor& c);
		
		void beginEditBlock();
		void endEditBlock();
		
		void moveTo(int line, int column);
		void moveTo(const QDocumentCursor &c);
		
		void copy(const QDocumentCursorHandle *c);
		
		virtual void execute(QDocumentCommand *c);
		
		inline void ref() { m_ref.ref(); }
		inline void deref() { if ( m_ref ) m_ref.deref(); if ( !m_ref ) delete this; }
		
	protected:
		QDocumentCursorHandle(QDocument *d, int line = 0);
		virtual ~QDocumentCursorHandle();
		
		QDocumentCursorHandle* clone() const;
		
	private:
		bool m_silent;
		QDocument *m_doc;
#if QT_VERSION < 0x040400
		QBasicAtomic m_ref;
#else
		QAtomicInt m_ref;
#endif
		int m_begOffset, m_endOffset, m_max, m_begLine, m_endLine;
		QStack<QDocumentCommandBlock*> m_blocks;
};

Q_DECLARE_TYPEINFO(QDocumentCursorHandle*, Q_PRIMITIVE_TYPE);

class QCE_EXPORT QDocumentPrivate
{
	friend class QEditConfig;
	
	friend class QDocument;
	friend class QDocumentCommand;
	
	friend int QDocumentLineHandle::xToCursor(int xpos, int xoff) const;
	friend int QDocumentLineHandle::cursorToX(int cpos) const;
	
	friend void QDocumentLineHandle::draw(	QPainter *p,
											int xOffset,
											int vWidth,
											const QSmallArray& sel,
											const QSmallArray& cursor,
											const QPalette& pal,
											bool fullSel) const;
	
	public:
		QDocumentPrivate(QDocument *d);
		~QDocumentPrivate();
		
		void execute(QDocumentCommand *cmd);
		
		void draw(QPainter *p, QDocument::PaintContext& cxt);
		
		QDocumentLineHandle* lineForPosition(int& position) const;
		int position(const QDocumentLineHandle *l) const;
		
		QDocumentLineHandle* at(int line) const;
		int indexOf(const QDocumentLineHandle *l) const;
		
		QDocumentIterator index(const QDocumentLineHandle *l);
		QDocumentConstIterator index(const QDocumentLineHandle *l) const;
		
		QDocumentLineHandle* next(const QDocumentLineHandle *l) const;
		QDocumentLineHandle* previous(const QDocumentLineHandle *l) const;
		
		void checkWidth(QDocumentLineHandle *l, int w);
		void checkWidth(QDocumentLineHandle *l, const QString& s);
		
		void setWidth();
		void setHeight();
		
		static void setFont(const QFont& f);
		
		void beginChangeBlock();
		void endChangeBlock();
		
		inline bool hasMarks() { return m_marks.count(); }
		QList<int> marks(QDocumentLineHandle *h) const;
		
		void addMark(QDocumentLineHandle *h, int mid);
		void toggleMark(QDocumentLineHandle *h, int mid);
		void removeMark(QDocumentLineHandle *h, int mid);
		
		void emitFormatsChange (int line, int lines);
		void emitContentsChange(int line, int lines);
		
		int visualLine(int textLine) const;
		int textLine(int visualLine) const;
		void hideEvent(int line, int count);
		void showEvent(int line, int count);
		
		void emitFormatsChanged();
		void emitContentsChanged();
		
		void emitLineDeleted(QDocumentLineHandle *h);
		void emitMarkChanged(QDocumentLineHandle *l, int m, bool on);
		
		inline QDocumentIterator begin() { return m_lines.begin(); }
		inline QDocumentIterator end() { return m_lines.end(); }
		
		inline QDocumentConstIterator constBegin() const { return m_lines.constBegin(); }
		inline QDocumentConstIterator constEnd() const { return m_lines.constEnd(); }
		
	protected:
		void updateHidden(int line, int count);
		
		void insertLines(int after, const QList<QDocumentLineHandle*>& l);
		void removeLines(int after, int n);
		
		void emitWidthChanged();
		void emitHeightChanged();
		
		static void updateFormatCache();
		void tunePainter(QPainter *p, quint8 fid);
		
	private:
		QDocument *m_doc;
		QUndoStack m_commands;
		QDocumentCursor *m_editCursor;
		
		bool m_suspend, m_deleting;
		QQueue<QPair<int, int> > m_notifications;
		
		QMap<int, int> m_hidden;
		QVector< QPair<QDocumentLineHandle*, int> > m_largest;
		
		int m_width, m_height;
		
		static QFont *m_font;
		static bool m_fixedPitch;
		static QFontMetrics *m_fontMetrics;
		static int m_leftMargin;
		static int m_tabStop;
		static QDocument::WhiteSpaceMode m_showSpaces;
		static int m_lineHeight;
		static int m_lineSpacing;
		static int m_spaceWidth;
		static int m_ascent;
		static int m_descent;
		static int m_leading;
		
		static QFormatFactory *m_formatFactory;
		
		static QVector<QFont> m_fonts;
		static QVector<QColor> m_colors;
		
		static QList<QDocumentPrivate*> m_documents;
		
		QHash<QDocumentLineHandle*, int> m_status;
		QHash<QDocumentLineHandle*, QList<int> > m_marks;
		
		int _nix, _dos;
		QString m_lineEndingString;
		QDocument::LineEnding m_lineEnding;
		
		QVector<QDocumentLineHandle*> m_lines;
};

#endif
