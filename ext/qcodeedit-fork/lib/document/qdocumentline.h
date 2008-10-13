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

#ifndef _QDOCUMENT_LINE_H_
#define _QDOCUMENT_LINE_H_

#include "qce-config.h"

#include "qformat.h"

class QString;

class QDocument;
class QDocumentLineHandle;

struct QNFAMatchContext;

struct QParenthesis
{
	enum Role
	{
		Open		= 1,
		Close		= 2,
		Indent		= 4,
		Fold		= 8,
		Match		= 16
	};
	
	inline QParenthesis()
	 : id(0), role(0), offset(0), length(0)
	{}
	
	inline QParenthesis(quint8 i, quint8 r, quint8 pos, quint8 len)
	 : id(i), role(r), offset(pos), length(len)
	{}
	
	quint8 id;
	quint8 role;
	quint8 offset;
	quint8 length;
};

Q_DECLARE_TYPEINFO(QParenthesis, Q_MOVABLE_TYPE);

class QCE_EXPORT QDocumentLine
{
	friend class QDocumentLineHandle;
	friend class QDocumentCursorHandle;
	
	public:
		enum State
		{
			None				= 0,
			Hidden				= 1,
			CollapsedBlockStart	= 2,
			CollapsedBlockEnd	= 4,
			
			LayoutDirty			= 16,
			FormatsApplied		= 32
		};
		
		Q_DECLARE_FLAGS(States, State);
		
		explicit QDocumentLine(QDocument *doc);
		QDocumentLine(const QDocumentLine& line);
		QDocumentLine(QDocumentLineHandle *h = 0);
		
		~QDocumentLine();
		
		bool isNull() const;
		bool isValid() const;
		
		bool operator == (const QDocumentLine& l) const;
		bool operator != (const QDocumentLine& l) const;
		
		bool operator < (const QDocumentLine& l) const;
		bool operator >= (const QDocumentLine& l) const;
		
		bool operator > (const QDocumentLine& l) const;
		bool operator <= (const QDocumentLine& l) const;
		
		QDocumentLine& operator ++ ();
		QDocumentLine& operator -- ();
		
		void operator ++ (int);
		void operator -- (int);
		
		QDocumentLine& operator = (const QDocumentLine& l);
		
		int lineNumber() const;
		int position() const;
		
		QString text() const;
		
		int length() const;
		
		int firstChar() const;
		int lastChar() const;
		
		int nextNonSpaceChar(int pos) const;
		int previousNonSpaceChar(int pos) const;
		
		inline bool isHidden() const
		{ return hasFlag(Hidden); }
		
		bool hasFlag(State s) const;
		void setFlag(State s, bool y = true);
		
		QDocumentLine next() const;
		QDocumentLine previous() const;
		
		QDocument* document() const;
		
		int xToCursor(int x, int xoff) const;
		int cursorToX(int i) const;
		
		void addMark(int id);
		void removeMark(int id);
		void toggleMark(int id);
		
		QList<int> marks() const;
		
		void clearOverlays();
		void addOverlay(const QFormatRange& over);
		void removeOverlay(const QFormatRange& over);
		
		void setFormats(const QVector<quint8>& formats);
		
		const QVector<QParenthesis>& parentheses() const;
		void setParentheses(const QVector<QParenthesis>& parentheses);
		
		inline QDocumentLineHandle* handle() const
		{ return m_handle; }
		
		QNFAMatchContext* matchContext();
		
	private:
		QDocumentLineHandle *m_handle;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QDocumentLine::States)

#endif