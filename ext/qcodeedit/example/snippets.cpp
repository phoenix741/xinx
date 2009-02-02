/****************************************************************************
**
** Copyright (C) 2006-2009 fullmetalcoder <fullmetalcoder@hotmail.fr>
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

/*!
	\file snippets.cpp
	\brief
*/

#include "snippets.h"

class PlainTextSnippet : public Snippet
{
	public:
		PlainTextSnippet(const QString& s);
		virtual ~PlainTextSnippet();
		
		virtual QString name() const;
		virtual QString identifier() const;
		virtual void insert(QEditor *e) const;
		
	private:
		QString data;
};

class SimpleSnippet : public Snippet
{
	public:
		SimpleSnippet(const QString& s);
		virtual ~SimpleSnippet();
		
		virtual QString name() const;
		virtual QString identifier() const;
		virtual void insert(QEditor *e) const;
		
	private:
		struct Anchor
		{
			int lineOffset;
			int columnOffset;
		};
		
		struct PlaceHolder : public Anchor
		{
			int length;
			QList<Anchor> mirrors;
		};
		
		QString m_base;
		QList<PlaceHolder> m_placeHolders;
};

class ScriptedSnippet : public Snippet
{
	public:
		ScriptedSnippet(const QString& s);
		virtual ~ScriptedSnippet();
		
		virtual QString name() const;
		virtual QString identifier() const;
		virtual void insert(QEditor *e) const;
		
	private:
		QString m_base;
};

SnippetManager::SnippetManager()
{
	
}

SnippetManager::~SnippetManager()
{
	qDeleteAll(m_snippets);
}

int SnippetManager::snippetCount() const
{
	return _msnippets.count();
}

const Snippet* SnippetManager::snippet(int i) const
{
	return i >= 0 && i < m_snippets.count() ? m_snippets.at(i) : 0;
}

void SnippetManager::loadSnippetFromString(const QString& s, int type)
{
	Snippet *s = loadString(s, t);
	
	if ( s )
		m_snippets << s;
}

void SnippetManager::loadSnippetFromFile(const QString& file, int type)
{
	Snippet *s = loadFile(file, t);
	
	if ( s )
		m_snippets << s;
}

Snippet* SnippetManager::loadFile(const QString& s, int type)
{
	
	return 0;
}

Snippet* SnippetManager::loadString(const QString& s, int type)
{
	if ( type == Plain )
		return new PlainTextSnippet(s);
	else if ( type == Simple )
		return new SimpleSnippet(s);
	else if ( type == Scripted )
		return ScriptedSnippet(s);
	
	return 0;
}

//


//

QString parsePlaceHolder(const QString& s, int index, int max, QHash<int, SimpleSnippet::PlaceHolder>& p)
{
	QStringList segments;
	int i = index, depth = 1, last = index + 1;
	
	while ( i + 1 < max )
	{
		c = s.at(++i);
		
		if ( c == QLatin1Char('{') )
		{
			++depth;
		} else if ( c == QLatin1Char('}') ) {
			--depth;
			
			if ( !depth )
			{
				segments << s.mid(last, i - last);
				break;
			}
		} else if ( c == QLatin1Char(':') ) {
			if ( depth == 1 )
			{
				segments << s.mid(last, i - last);
				last = i + 1;
			}
		}
	}
	
	if ( segments.isEmpty() )
	{
		qWarning("invalid placeholder");
		return QString();
	}
	
	int id = segments.at(0).toInt();
	
	SimpleSnippet::PlaceHolder& ph = p[id];
	// TODO : support recursive snippetting...
	
	if ( segments.count() == 0 )
	{
		ph.length = 0;
	}
	
	ph.length =  ? segments.last().count() : 0;
	
	return segments.count() > 0 ? segments.last().count() : QString();
}

SimpleSnippet::SimpleSnippet(const QString& s)
{
	int index = 0, line = 0, column = 0, max = s.length();
	
	QHash<int, PlaceHolder> p;
	
	while ( index < max )
	{
		QChar c = s.at(index);
		
		if ( c == QLatin1Char('$') )
		{
			c = s.at(++index);
			
			if ( c == QLatin1Char('{') )
			{
				QString val = parsePlaceHolder(s, index, max, p);
				
				if ( val.count() )
				{
					int nl = val.count(QLatin1Char('\n'));
					
					m_base += val;
					
					line += nl;
					
					if ( nl )
						column = val.count() - val.lastIndexOf(QLatin1Char('\n')) - 1;
					else
						column += val.count();
				}
				continue;
			} else {
				if ( c != QLatin1Char('$') )
				{
					c = s.at(--index);
				}
				
				++column;
			}
		} else if ( c == QLatin1Char('\n') ) {
			column = 0;
			++line;
		} else {
			++column;
		}
		
		m_base += c;
		++index;
	}
}

SimpleSnippet::~SimpleSnippet()
{
	
}

QString SimpleSnippet::name() const
{
	return m_base.left(5);
}

QString SimpleSnippet::identifier() const
{
	return m_base;
}

void SimpleSnippet::insert(QEditor *e) const
{
	e->clearPlaceHolders();
	
	QDocument *d = e->document();
	QDocumentCursor c = e->cursor();
	int line = c.lineNumber(), column = c.columnNumber();
	
	c.insertText(m_base);
	e->setCursor(c);
	
	if ( placeholders.count() )
	{
		foreach ( const PlaceHolder& ph, placeholders )
		{
			QEditor::PlaceHolder eph;
			eph.length = ph.length;
			eph.cursor = QDocumentCursor(d, line + ph.lineOffset, ph.columnOffset + (ph.lineOffset ? column : 0));
			
			foreach ( const Anchor& a, ph.mirrors )
				eph.mirrors = QDocumentCursor(d, line + ph.lineOffset, ph.columnOffset + (ph.lineOffset ? column : 0));
			
			e->addPlaceHolder(eph);
		}
		
		e->nextPlaceHolder();
	}
}

//


//


