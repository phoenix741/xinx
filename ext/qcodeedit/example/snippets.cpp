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

#include "qeditor.h"

#include "qdocumentcommand.h"

#include <QMap>

class SnippetInsertionCommand : public QDocumentCommandBlock
{
	public:
		SnippetInsertionCommand(QEditor *e)
		 : QDocumentCommandBlock(e->document()), m_editor(e), m_cursor(e->cursor())
		{
		}
		
		void addPlaceHolder(const QEditor::PlaceHolder& ph)
		{
			m_placeHolders << ph;
		}
		
		virtual void addCommand(QDocumentCommand *c)
		{
			c->setTargetCursor(m_cursor.handle());
			QDocumentCommandBlock::addCommand(c);
		}
		
		virtual void removeCommand(QDocumentCommand *c)
		{
			c->setTargetCursor(0);
			QDocumentCommandBlock::removeCommand(c);
		}
		
		virtual void redo()
		{
			m_editor->clearPlaceHolders();
			QDocumentCommandBlock::redo();

			foreach ( const QEditor::PlaceHolder& ph, m_placeHolders )
				m_editor->addPlaceHolder(ph);
			
			m_editor->nextPlaceHolder();
		}
		
		virtual void undo()
		{
			// TODO : backup and restore previous placeholders?
			m_editor->clearPlaceHolders();
			QDocumentCommandBlock::undo();
			m_editor->setCursor(m_cursor);
		}
		
	private:
		QEditor *m_editor;
		QDocumentCursor m_cursor;
		QList<QEditor::PlaceHolder> m_placeHolders;
};

class PlainTextSnippet : public Snippet
{
	public:
		PlainTextSnippet(const QString& s);
		virtual ~PlainTextSnippet();
		
		virtual void insert(QEditor *e) const;
		
	private:
		QString m_data;
};

class SimpleSnippet : public Snippet
{
	public:
		struct Anchor
		{
			Anchor() : lineOffset(0), columnOffset(0) {}
			
			int lineOffset;
			int columnOffset;
		};
		
		struct PlaceHolder : public Anchor
		{
			PlaceHolder() : length(-1) {}
			
			int length;
			QString defaultValue;
			QList<Anchor> mirrors;
			QList<int> unresolvedMirrors;
		};
		
		SimpleSnippet(const QString& s);
		virtual ~SimpleSnippet();
		
		virtual void insert(QEditor *e) const;
		
	private:
		QString m_base;
		QList<PlaceHolder> m_placeHolders;
};

class ScriptedSnippet : public Snippet
{
	public:
		ScriptedSnippet(const QString& s);
		virtual ~ScriptedSnippet();
		
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
	return m_snippets.count();
}

Snippet* SnippetManager::snippet(int i) const
{
	return i >= 0 && i < m_snippets.count() ? m_snippets.at(i) : 0;
}

void SnippetManager::removeSnippet(int i, bool cleanup)
{
	if ( i < 0 || i >= m_snippets.count() )
		return;
	
	Snippet *snip = m_snippets.takeAt(i);
	
	if ( cleanup )
		delete snip;
}

void SnippetManager::addSnippet(Snippet *snip)
{
	if ( snip )
		m_snippets << snip;
}

void SnippetManager::reloadSnippetFromString(int i, const QString& s, int type)
{
	if ( i < 0 || i >= m_snippets.count() )
		return;
	
	Snippet *snip = loadString(s, type);
	
	if ( snip )
	{
		Snippet *old = m_snippets.at(i);
		snip->setName(old->name());
		snip->setContexts(old->contexts());
		m_snippets[i] = snip;
		delete old;
	}
}

void SnippetManager::reloadSnippetFromFile(int i, const QString& file, int type)
{
	if ( i < 0 || i >= m_snippets.count() )
		return;
	
}

Snippet* SnippetManager::loadSnippetFromString(const QString& s, int type)
{
	Snippet *snip = loadString(s, type);
	addSnippet(snip);
	
	return snip;
}

Snippet* SnippetManager::loadSnippetFromFile(const QString& file, int type)
{
	Snippet *snip = loadFile(file, type);
	addSnippet(snip);
	
	return snip;
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
		return new ScriptedSnippet(s);
	
	return 0;
}

//

PlainTextSnippet::PlainTextSnippet(const QString& s)
: m_data(s)
{
	
}

PlainTextSnippet::~PlainTextSnippet()
{
	
}

void PlainTextSnippet::insert(QEditor *e) const
{
	QDocumentCursor c = e->cursor();
	c.insertText(m_data);
	e->setCursor(c);
}

//

QString parsePlaceHolder(const QString& s, int& index, int max, QMap<int, SimpleSnippet::PlaceHolder>& p, int& line, int& column, int baseSize)
{
	QChar c;
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
	
	if ( ph.length == -1 && segments.count() > 1 )
	{
		// new placeholder
		ph.length = segments.last().count();
		ph.lineOffset = line;
		ph.columnOffset = column;
		ph.defaultValue = segments.last();
		// TODO : support recursive snippetting of default value...
	} else {
		// mirror of an existing placeholder
		SimpleSnippet::Anchor a;
		a.lineOffset = line;
		a.columnOffset = column;
		if ( ph.defaultValue.isEmpty() )
			ph.unresolvedMirrors << baseSize << ph.mirrors.count();
		ph.mirrors << a;
	}
	
	index = i + 1;
	return ph.defaultValue;
}

void performRelocation(SimpleSnippet::Anchor& a, const QHash<int, QList<int> >& relocationTable, int length)
{
	QHash<int, QList<int> >::const_iterator reloc = relocationTable.constFind(a.lineOffset);
	
	if ( reloc == relocationTable.constEnd() )
		return;
	
	int idx = 0;
	int relocOffset = 0;
	const QList<int>& offsets = *reloc;
	
	while ( ((idx + 1) < offsets.count()) && (offsets.at(idx) <= a.columnOffset) )
	{
		int off = offsets.at(++idx);
		
		if ( offsets.at(idx - 1) < a.columnOffset || off != length )
			relocOffset += off;
		
		++idx;
	}
	
	a.columnOffset += relocOffset;
}

SimpleSnippet::SimpleSnippet(const QString& s)
{
	int index = 0, line = 0, column = 0, max = s.length();
	
	QString tmp;
	QStringList base;
	QMap<int, PlaceHolder> p;
	
	while ( index < max )
	{
		QChar c = s.at(index);
		
		if ( c == QLatin1Char('$') )
		{
			base << tmp;
			tmp.clear();
			
			c = s.at(++index);
			
			if ( c == QLatin1Char('{') )
			{
				QString val = parsePlaceHolder(s, index, max, p, line, column, base.count());
				base << val;
				
				if ( val.count() )
				{
					int nl = val.count(QLatin1Char('\n'));
					
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
		
		tmp += c;
		++index;
	}
	
	if ( tmp.count() )
		base << tmp;
	
	QHash<int, QList<int> > relocationTable;
	QMap<int, PlaceHolder>::iterator it = p.begin();
	
	// first : build relocation table (in case several placeholders are on same line
	while ( it != p.end() )
	{
		if ( it->unresolvedMirrors.count() && it->length )
		{
			for ( int i = 0; i + 1 < it->unresolvedMirrors.count(); ++i )
			{
				int idx = it->unresolvedMirrors.at(i);
				int anchor = it->unresolvedMirrors.at(++i);
				
				base[idx] = it->defaultValue;
				
				const Anchor& a = it->mirrors.at(anchor);
				relocationTable[a.lineOffset] << a.columnOffset << it->length;
			}
			
			it->unresolvedMirrors.clear();
		}
		
		++it;
	}
	
	it = p.begin();
	
	// then : apply relocation and store the corrected placeholder data
	while ( it != p.end() )
	{
		performRelocation(*it, relocationTable, it->length);
		
		for ( int i = 0; i < it->mirrors.count(); ++i )
			performRelocation(it->mirrors[i], relocationTable, it->length);
		
		m_placeHolders << *it;
		++it;
	}
	
	m_base = base.join(QString::null);
}

SimpleSnippet::~SimpleSnippet()
{
	
}

void SimpleSnippet::insert(QEditor *e) const
{
	e->clearPlaceHolders();
	
	QDocument *d = e->document();
	QDocumentCursor c = e->cursor();
	
	if ( c.isNull() )
		c = QDocumentCursor(d);
	
	int line = qMax(c.lineNumber(), 0), column = qMax(c.columnNumber(), 0);
	
	if ( line != c.lineNumber() || column != c.columnNumber() )
		c = QDocumentCursor(d, line, column);
	
	SnippetInsertionCommand *cmd = new SnippetInsertionCommand(e);
	cmd->addCommand(new QDocumentInsertCommand(line, column, m_base, d));
	
	if ( m_placeHolders.count() )
	{
		foreach ( const PlaceHolder& ph, m_placeHolders )
		{
			QEditor::PlaceHolder eph;
			eph.length = ph.length;
			eph.cursor = QDocumentCursor(d, line + ph.lineOffset, ph.columnOffset + (ph.lineOffset ? 0 : column));
			
			foreach ( const Anchor& a, ph.mirrors )
				eph.mirrors << QDocumentCursor(d, line + a.lineOffset, a.columnOffset + (a.lineOffset ? 0 : column));
			
			cmd->addPlaceHolder(eph);
			//e->addPlaceHolder(eph);
		}
		
		//e->nextPlaceHolder();
	}
	
	e->document()->execute(cmd);
}

//

ScriptedSnippet::ScriptedSnippet(const QString& s)
{
	
}

ScriptedSnippet::~ScriptedSnippet()
{
	
}

void ScriptedSnippet::insert(QEditor *e) const
{
	
}


//
