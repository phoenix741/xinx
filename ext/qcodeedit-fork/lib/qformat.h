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

#ifndef _QFORMAT_H_
#define _QFORMAT_H_

#include <QFont>
#include <QColor>
#include <QTextCharFormat>

template <typename T>
class QVector;

struct QFormat
{
	inline QFormat()
	 : weight(QFont::Normal), italic(false), underline(false), strikeout(false)
	{}
	
	inline QFormat(const QColor& c)
	 : weight(QFont::Normal), italic(false), underline(false), strikeout(false), foreground(c)
	{}
	
	inline QFormat(int w, const QColor& c)
	 : weight(w), italic(false), underline(false), strikeout(false), foreground(c)
	{}
	
	inline QFormat(int w, bool i, bool u, bool s, const QColor& c)
	 : weight(w), italic(i), underline(u), strikeout(s), foreground(c)
	{}
	
	inline QFormat(const QFormat& f)
	 : weight(f.weight), italic(f.italic), underline(f.underline), strikeout(f.strikeout),
	   foreground(f.foreground)
	{}
	
	inline QFormat& operator = (const QFormat& f)
	{
		weight = f.weight;
		italic = f.italic;
		underline = f.underline;
		strikeout = f.strikeout;
		foreground = f.foreground;
		
		return *this;
	}
	
	inline bool operator == (const QFormat& f) const
	{
		return 		(weight == f.weight)
				&&
					(italic == f.italic)
				&&
					(underline == f.underline)
				&&
					(strikeout == f.strikeout)
				&&
					(foreground == f.foreground)
				&&
					(background == f.background)
				;
	}
	
	inline bool operator != (const QFormat& f) const
	{
		return 		(weight != f.weight)
				||
					(italic != f.italic)
				||
					(underline != f.underline)
				||
					(strikeout != f.strikeout)
				||
					(foreground != f.foreground)
				||
					(background != f.background)
				;
	}
	
	QTextCharFormat toTextCharFormat() const
	{
		QTextCharFormat f;
		f.setFontWeight(weight);
		f.setFontItalic(italic);
		f.setFontUnderline(underline);
		f.setFontStrikeOut(strikeout);
		
		if ( foreground.isValid() )
			f.setForeground(foreground);
		
		if ( background.isValid() )
			f.setBackground(background);
		
		return f;
	}
	
	int weight;
	bool italic;
	bool underline;
	bool strikeout;
	QColor foreground;
	QColor background;
};

Q_DECLARE_TYPEINFO(QFormat, Q_MOVABLE_TYPE);

struct QFormatRange
{
	inline QFormatRange()
	 : offset(0), length(0), format(0)
	{}
	
	inline QFormatRange(quint8 o, quint8 l, quint8 f)
	 : offset(o), length(l), format(f)
	{}
	
	inline bool operator == (const QFormatRange& o)
	{ return (offset == o.offset) && (length == o.length) && (format == o.format); }
	
	inline bool operator != (const QFormatRange& o)
	{ return (offset != o.offset) || (length != o.length) || (format != o.format); }
	
	quint8 offset;
	quint8 length;
	quint8 format;
};

Q_DECLARE_TYPEINFO(QFormatRange, Q_PRIMITIVE_TYPE);

#endif
