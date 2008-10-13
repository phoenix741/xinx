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

#include "qnfadefinition.h"

/*!
	\file qnfadefinition.cpp
	
	\brief Implementation of the QNFADefinition class.
*/

#include "qnfa.h"

#include "qformatfactory.h"
#include "qlanguagefactory.h"

#include "qmarker.h"
#include "qmatcher.h"
#include "qindenter.h"
#include "qhighlighter.h"

#include "qmarkerinterface.h"
#include "qmatcherinterface.h"
#include "qindenterinterface.h"
#include "qhighlighterinterface.h"

#include "qlinemarksinfocenter.h"

#include "qdocument.h"
#include "qdocument_p.h"
#include "qdocumentline.h"
#include "qdocumentcursor.h"

#include <QFile>
#include <QKeyEvent>

#include <QDomText>
#include <QDomElement>
#include <QDomDocument>

class QNFANotifier : public QNFAMatchHandler
{
	public:
		QNFANotifier(const QString& line)
		{
			m_formats.fill(0, line.length());
		}
		
		QNFANotifier(const QDocumentLine& line)
		 : m_line(line)
		{
			m_formats.fill(0, line.length());
		}
		
		~QNFANotifier()
		{
			if ( m_line.isValid() )
			{
				m_line.setFormats(m_formats);
				m_line.setParentheses(m_parens);
			}
		}
		
		const QVector<quint8>& formats() const
		{
			return m_formats;
		}
		
		const QVector<QParenthesis>& parentheses() const
		{
			return m_parens;
		}
		
		virtual void matched(int pos, int len, int action)
		{
			//qDebug("action 0x%x on [%s]", action, qPrintable(m_line.text().mid(pos, len)));
			
			if ( !len )
				return;
			
			//qDebug("matched \"%s\"", qPrintable(m_line.text().mid(pos, len)));
			
			if ( action & QNFAAction::Highlight )
			{
				//for ( int i = pos; (i < len) && (i < m_formats.count()); i++ )
				int i = qBound(0, pos, m_formats.count()),
					e = qBound(0, pos + len, m_formats.count());
				
				while ( i < e )
					m_formats[i++] = action & QNFAAction::FormatMask;
			}
			
			if (
					!(action & QNFAAction::Content)
				&&
					(
						action
					&
						(
							QNFAAction::ParenOpen
						|
							QNFAAction::ParenClose
						|
							QNFAAction::Indent
						|
							QNFAAction::Fold
						)
					)
				)
			{
				int i = qBound(0, pos, m_formats.count()),
					e = qBound(0, pos + len, m_formats.count());
				
				//qDebug("matched paren : %s", qPrintable(m_line.text().mid(pos, len)));
				
				QParenthesis par(
									(action & QNFAAction::ParenMask) >> 8,
									0,
									i,
									e - i
								);
				
				if ( action & QNFAAction::ParenOpen )
					par.role |= QParenthesis::Open;
				else if ( action & QNFAAction::ParenClose )
					par.role |= QParenthesis::Close;
				
				if ( action & QNFAAction::MatchParen )
					par.role |= QParenthesis::Match;
				
				if ( action & QNFAAction::Indent )
					par.role |= QParenthesis::Indent;
				
				if ( action & QNFAAction::Fold )
					par.role |= QParenthesis::Fold;
				
				m_parens << par;
			}
		}
		
	private:
		QDocumentLine m_line;
		QVector<quint8> m_formats;
		QVector<QParenthesis> m_parens;
};

extern QString *_singleLineCommentTarget;
void embed(QNFA *src, QNFA *dest, int index);
void fillContext(QNFA *cxt, QDomElement e, QFormatFactory *f, QHash<QString, int>& pids, bool cs);

static inline bool match(const QParenthesis& open, const QParenthesis& close)
{
	return open.id == close.id;
}

/*
	Generic marker class : app wide?
	
*/

class QNFAMarker : public QMarkerInterface
{
	public:
		QNFAMarker(QNFADefinition *d)
		 : m_definition(d)
		{
		
		}
		
		virtual QString defaultMark() const
		{
			return m_definition->m_defaultMark;
		}
		
		virtual QStringList ids() const
		{
			return QLineMarksInfoCenter::instance()
					->availableMarkTypes(m_definition->language());
		}
		
		virtual QList<QStringList> items() const
		{
			return QLineMarksInfoCenter::instance()
					->marksLayout(m_definition->language());
		}
		
		virtual QPixmap icon(const QString& s) const
		{
			return QLineMarksInfoCenter::instance()
					->markType(s).icon;
		}
		
		virtual QColor color(const QString& s) const
		{
			return QLineMarksInfoCenter::instance()
					->markType(s).color;
		}
		
		virtual bool needFocus(const QString& s) const
		{
			return QLineMarksInfoCenter::instance()
					->markType(s).focus;
		}
		
		virtual bool isPrivate(const QString& s) const
		{
			return !QLineMarksInfoCenter::instance()
					->markType(s).user;
		}
		
		virtual int persistency(const QString& s) const
		{
			return QLineMarksInfoCenter::instance()
					->markType(s).persistency;
		}
		
		virtual QString priority(const QStringList& l) const
		{
			return QLineMarksInfoCenter::instance()
					->priority(l);
		}
		
	private:
		QNFADefinition *m_definition;
};

class QNFAMatcher : public QMatcherInterface
{
	public:
		QNFAMatcher(QNFADefinition *d, bool indent = false)
		 : m_definition(d), m_indentFold(indent)
		{
		
		}
		
		virtual void match(const QDocumentCursor& c, QMatcher *m)
		{
			//qDebug("trying to match...");
			
			QDocument *d = c.document();
			QDocumentLine b = c.line();
			
			if ( !d || !b.isValid() )
				return;
			
			int pos = c.columnNumber();
			const QVector<QParenthesis>& m_parens = b.parentheses();
			
			if ( m_parens.isEmpty() )
				return;
			
			QParenthesis p;
			int line = c.lineNumber(),
				beg = -1,
				end = -1,
				beglen = 0,
				endlen = 0;
			
			foreach ( p, m_parens )
			{
				if (
						(pos == p.offset)
					&&
						(p.role & QParenthesis::Open)
					&&
						(p.role & QParenthesis::Match)
					)
				{
					beg = p.offset;
					beglen = p.length;
					endlen = matchOpen(d, line, p, end);
					
					if ( endlen )
						m->setMatch(c.lineNumber(), beg, beglen, line, end, endlen);
					
					break;
					
				} else if (
								(pos == (p.offset + p.length))
							&&
								(p.role & QParenthesis::Close)
							&&
								(p.role & QParenthesis::Match)
							)
				{
					end = p.offset;
					endlen = p.length;
					beglen = matchClose(d, line, p, beg);
					
					if ( beglen )
						m->setMatch(line, beg, beglen, c.lineNumber(), end, endlen);
					
					break;
				}
			}
		}
		
		virtual QChar autoClose(QChar)
		{
			return QChar();
		}
		
		virtual int blockFlags(int b, int depth, QMatcher *m)
		{
			if ( m_indentFold )
			{
				QDocument *d = m->document();
				
				QDocumentLine
					prev = d->line(b - 1),
					curr = d->line(b),
					next = d->line(b + 1);
				
				if ( curr.hasFlag(QDocumentLine::CollapsedBlockStart) )
					return QMatcher::Collapsed;
				
				int id = m_indentGuess.value(d, 0),
					cc = curr.firstChar(),
					pc = prev.firstChar(),
					nc = next.firstChar();
				
				int k = b;
				
				do {
					prev = d->line(--k);
					pc = prev.firstChar();
				} while ( prev.isValid() && (pc == -1) );
				
				k = b;
				
				do {
					next = d->line(++k);
					nc = next.firstChar();
				} while ( next.isValid() && (nc == -1) );
				
				if ( !id && (depth > 0) && (pc > 0) )
				{
					id = pc / depth;
					m_indentGuess[d] = id;
				}
				
				if ( cc != -1 )
				{
					if ( nc > cc )
						return QMatcher::Collapsible;
					
					if ( nc < cc )
					{
						if ( !id && depth )
						{
							id = cc / depth;
							m_indentGuess[d] = id;
						}
						
						//qDebug("cc=%i; nc=%i; id=%i => do=%i", cc, nc, id, (cc - nc) / id);
						return QMatcher::Closure | ((cc - nc) / id);
					}
				}
				/*
				if ( (nc != cc) && (nc <= 0) && depth )
				{
					return QMatcher::Closure | depth;
				}*/
			}
			
			return collapseState(b, m, 0);
		}
		
		int collapseState(int ln, QMatcher *m, QParenthesis *l)
		{
			Q_UNUSED(m)
			
			QDocumentLine b = m->document()->line(ln);
			
			if ( !b.isValid() )
				return QMatcher::None;
			
			int open = 0;
			
			if ( b.hasFlag(QDocumentLine::CollapsedBlockStart) )
				return QMatcher::Collapsed;
			
			foreach ( QParenthesis p, b.parentheses() )
			{
				if ( !(p.role & QParenthesis::Fold) )
					continue;
				
				if ( p.role & QParenthesis::Open )
					++open;
				else
					--open;
				
				if ( l )
					*l = p;
			}
			
			if ( open > 0 )
				return QMatcher::Collapsible;
			else if ( open < 0 )
				return QMatcher::Closure | qAbs(open);
			
			return QMatcher::None;
		}
		
		virtual void expand(int ln, QMatcher *m)
		{
			Q_UNUSED(m)
			
			QDocument *d = m->document();
			QDocumentLine b = d->line(ln);
			
			if ( !b.isValid() || !b.hasFlag(QDocumentLine::CollapsedBlockStart) )
				return;
			
			int depth = 1,
				count = 1,
				indent = b.firstChar() / m_indentGuess.value(d, 1);
			
			QDocumentLine l = d->line(ln + count);
			
			while ( l.isValid() )
			{
				if ( depth == 1 )
					l.setFlag(QDocumentLine::Hidden, false);
				
				if ( l.hasFlag(QDocumentLine::CollapsedBlockStart) )
				{
					++depth;
				} else if ( l.hasFlag(QDocumentLine::CollapsedBlockEnd) ){
					int flags = blockFlags(ln + count, depth + indent, m);
					int doff = qMax(1, flags & QMatcher::DataMask);
					
					if ( (depth == 1) && (doff > 0) )
						l.setFlag(QDocumentLine::CollapsedBlockEnd, false);
					
					depth -= doff;
					
					//qDebug("depth offset = %i > depth = %i", doff, depth);
				}
				
				if ( depth <= 0 )
					break;
				
				++count;
				l = d->line(ln + count);
			}
			
			b.setFlag(QDocumentLine::CollapsedBlockStart, false);
			
			d->impl()->showEvent(ln, count);
			
			d->impl()->setHeight();
			d->impl()->emitFormatsChanged();
		}
		
		virtual void collapse(int ln, QMatcher *m)
		{
			QDocument *d = m->document();
			QDocumentLine b = d->line(ln);
			
			if ( !b.isValid() || b.hasFlag(QDocumentLine::CollapsedBlockStart) )
				return;
			
			int count = 1,
				indent = 0;
			
			QVector<QParenthesis> lp = b.parentheses();
			
			while ( lp.count() )
			{
				QParenthesis p = lp.first();
				
				if ( (p.role & QParenthesis::Open) && (p.role & QParenthesis::Fold) )
				{
				/*
				if ( !((p.role & QParenthesis::Open) && (p.role & QParenthesis::Fold)) )
				{
					if ( p.role & QParenthesis::Fold )
						++indent;
					
				} else if ( indent ) {
					--indent;
				} else {
					*/
					QParenthesis par;
					int depth = 1, cd = 0;
					QDocumentLine block = d->line(ln + count);
					
					while ( block.isValid() )
					{
						const int state = collapseState(ln + count, m, &par);
						
						if ( state & QMatcher::Closure )
						{
//							qDebug("at line %i depth fall by %i", ln + count, state & QMatcher::DataMask);
							depth -= state & QMatcher::DataMask;
							
							if ( depth <= 0 )
								break;
							
						} else if ( state & (QMatcher::Collapsible | QMatcher::Collapsed) ) {
							++depth;
						}
						
						++count;
						block = d->line(ln + count);
					}
					
					if ( !block.isValid() || (depth > 0) )
						return;
					
					b.setFlag(QDocumentLine::CollapsedBlockStart);
					
					for ( int i = ln + 1; i <= ln + count; ++i )
					{
						d->line(i).setFlag(QDocumentLine::Hidden);
					}
					
					block.setFlag(QDocumentLine::Hidden);
					block.setFlag(QDocumentLine::CollapsedBlockEnd);
					
					d->impl()->hideEvent(ln, count);
					d->impl()->setHeight();
					
					d->impl()->emitFormatsChanged();
					return;
				}
				
				lp.pop_front();
			}
			
			
			if ( !m_indentFold )
				return;
			
			int i = ln + 1, k;
			QDocumentLine block = d->line(i);
			
			if ( !block.isValid() )
				return;
			
			indent = block.firstChar();
			
			do
			{
				block = d->line(++i);
				k = block.firstChar();
			} while ( block.isValid() && ((k >= indent) || (k == -1)) );
			
			if ( k < indent )
			{
				block = d->line(--i);
				k = block.firstChar();
			}
			
			while ( block.isValid() && (i > ln) && (k == -1) )
			{
				block = d->line(--i);
				k = block.firstChar();
			}
			
			b.setFlag(QDocumentLine::CollapsedBlockStart);
			
			//qDebug("hidding from %i to %i", ln + 1, i);
			
			for ( int j = ln + 1; j <= i; ++j )
			{
				d->line(j).setFlag(QDocumentLine::Hidden);
			}
			
			d->line(i).setFlag(QDocumentLine::CollapsedBlockEnd);
			
			d->impl()->hideEvent(ln, i - ln);
			d->impl()->setHeight();
			
			d->impl()->emitFormatsChanged();
		}
		
	protected:
		int matchOpen(QDocument *d, int& line, QParenthesis p, int& end)
		{
			int pos = p.offset;
			QVector<QParenthesis> m_parens = d->line(line).parentheses();
			
			bool bMatch = false;
			QParenthesis par;
			QStack<QParenthesis> parens;
			
			forever
			{
				foreach ( par, m_parens )
				{
					if ( par.offset < pos )
						continue;
					
					if ( par.role & QParenthesis::Open )
					{
						parens.push(par);
					} else if ( par.role & QParenthesis::Close ) {
						if ( (bMatch = ::match(parens.top(), par)) )
						{
							bMatch &= parens.count() == 1;
							
							if ( bMatch )
								break;
							else if ( parens.count() )
								parens.pop();
							else
								qWarning("bad paren nesting...");
						} else {
							return 0;
						}
					}
				}
				
				if ( bMatch )
					break;
				
				pos = 0;
				++line;
				
				QDocumentLine b = d->line(line);
				
				if ( !b.isValid() )
					return 0;
				
				m_parens = b.parentheses();
			}
			
			end = par.offset;
			
			return par.length;
		}
		
		int matchClose(QDocument *d, int& line, QParenthesis p, int& beg)
		{
			int pos = p.offset;
			QVector<QParenthesis> m_parens = d->line(line).parentheses();
			
			QParenthesis par;
			bool bMatch = false;
			QStack<QParenthesis> parens;
			
			forever
			{
				for ( int i = m_parens.count() - 1; i >= 0; --i )
				{
					par = m_parens.at(i);
					
					if ( par.offset > pos )
						continue;
					
					if ( par.role & QParenthesis::Close )
					{
						parens.push(par);
					} else if ( par.role & QParenthesis::Open ) {
						if ( (bMatch = ::match(par, parens.top())) )
						{
							bMatch &= parens.count() == 1;
							
							if ( bMatch )
								break;
							else if ( parens.count() )
								parens.pop();
							else
								qWarning("bad paren nesting...");
						} else {
							return 0;
						}
					}
				}
				
				if ( bMatch )
					break;
				
				--line;
				
				QDocumentLine b = d->line(line);
				pos = b.length();
				
				if ( !b.isValid() )
					return 0;
				
				m_parens = b.parentheses();
			}
			
			beg = par.offset;
			
			return par.length;
		}
		
	private:
		QNFADefinition *m_definition;
		bool m_indentFold;
		QHash<QDocument*, int> m_indentGuess;
};

class QNFAIndenter : public QIndenterInterface
{
	public:
		QNFAIndenter(QNFADefinition *d)
		 : m_definition(d)
		{
		
		}
		
		virtual QString indent(const QDocumentCursor& c, QIndenter *i)
		{
			Q_UNUSED(i)
			
			QDocumentLine b = c.line();
			int pos, max = qMin(c.columnNumber(), b.text().size());
			
			QString s = b.text().left(max);
			
			//qDebug("line %i, column %i : %s", b.lineNumber(), c.columnNumber(), qPrintable(s));
			
			for ( pos = 0; pos < max; pos++ )
				if ( !s.at(pos).isSpace() )
					break;
			
			int indent = 0;
			bool open = false;
			QString spaces = s.left(pos);
			
			//qDebug("spaces : \"%s\"", qPrintable(spaces));
			
			foreach ( QParenthesis p, b.parentheses() )
			{
				if  ( p.offset >= max )
				{
					break;
				} else if ( !(p.role & QParenthesis::Indent) ) {
					open = true;
				} else if ( p.role & QParenthesis::Close ) {
					
					if ( open )
						--indent;
					
				} else { //if ( p.role & QParenthesis::Open ) {
					open = true;
					++indent;
				}
			}
			
			//qDebug("indent : %i", indent);
			
			if ( indent > 0 )
				spaces += QString(indent, '\t');
			
			return spaces;
		}
		
		virtual bool unindent (const QDocumentCursor& c, QKeyEvent *k)
		{
			QDocumentLine b = c.line();
			int pos, max = qMin(c.columnNumber(), b.text().size());
			
			QString ktxt = k->text(),
					s = b.text().left(max);
			
			if ( ktxt.isEmpty() )
				return false;
			
			for ( pos = 0; pos < max; pos++ ) 
				if ( !s.at(pos).isSpace() )
					break;
			
			QString spaces = s.left(pos),
					text = s.mid(pos) + ktxt;
			
			if ( spaces.isEmpty() || text.isEmpty() )
				return false;
			
			QNFAMatchContext cxt;
			QNFANotifier notify(text);
			
			QDocumentLine prev = b.previous();
			
			if ( prev.isValid() )
			{
				cxt = prev.matchContext();
			} else {
				// first line
				cxt = b.matchContext();
				cxt.reset();
			}
			
			match(&cxt, text, &notify);
			
			const QVector<QParenthesis>& parens = notify.parentheses();
			
			if ( parens.isEmpty() )
				return false;
			
			QParenthesis p = parens.last();
			
			return (
						(p.role & QParenthesis::Close)
					&&
						(p.role & QParenthesis::Indent)
					&&
						(p.offset == 0)
					&&
						(p.length == text.count())
					);
		}
		
	private:
		QNFADefinition *m_definition;
};

class QNFAHighlighter : public QHighlighterInterface
{
	public:
		QNFAHighlighter(QNFADefinition *d)
		 : m_definition(d)
		{
		
		}
		
		virtual QString singleLineComment() const
		{
			return m_definition->m_singleLineComment;
		}
		
		virtual void highlight(QDocumentLine& block, QFormatFactory *)
		{
			if ( !block.matchContext()->context )
				block.matchContext()->context = m_definition->m_root;
			
			QNFANotifier notifier(block);
			QString txt = block.text() + "\n";
			match(block.matchContext(), txt, &notifier);
		}
		
	private:
		QNFADefinition *m_definition;
};

extern bool stringToBool(const QString& s, bool previous);

QHash<QString, int> QNFADefinition::m_paren;
QHash<QString, QNFA*> QNFADefinition::m_contexts;
//QHash<QString, QNFADefinition*> QNFADefinition::m_definitions;
QHash<QString, QNFADefinition::EmbedRequestList> QNFADefinition::m_pendingEmbeds;

QNFADefinition::QNFADefinition(const QString& fn, QLanguageFactory *f)
 :	QLanguageDefinition(f), m_indentFold(false),  m_root(0),
	m_marker(0), m_matcher(0), m_indenter(0), m_highlighter(0)
{
	QFile file(fn);
	
	if ( !file.open(QFile::ReadOnly | QFile::Text) )
	{
		qWarning("QNFADefinition : failed to open file %s", qPrintable(fn));
		return;
	}
	
	QDomDocument doc;
	doc.setContent(&file);
	
	QDomElement root = doc.documentElement();
	
	m_language = root.attribute("language");
	m_indentFold = stringToBool(root.attribute("indentationFold"), false);
	m_extensions = root.attribute("extensions").split(";");
	m_defaultMark = root.attribute("defaultLineMark");
	
	/*
	qDebug("Generating definition for %s language from XML file : %s",
			qPrintable(m_language),
			qPrintable(fn));
	*/
	
	// create root entity
	m_root = lexer();
	
	_singleLineCommentTarget = &m_singleLineComment;
	fillContext(m_root, root, f->formatFactory(), m_paren, true);
	_singleLineCommentTarget = 0;
	
	squeeze(m_root);
	
	//m_definitions[m_language] = this;
	m_contexts[m_language] = m_root;
	
	flushEmbedRequests(m_language);
	
	//qDebug("Done generating definition.");
}

QNFADefinition::~QNFADefinition()
{
	delete m_root;
}

const QString& QNFADefinition::language() const
{
	return m_language;
}

const QStringList& QNFADefinition::extensions() const
{
	return m_extensions;
}

QMarkerInterface* QNFADefinition::marker()
{
	if ( !m_marker )
		m_marker = new QNFAMarker(this);
	
	return m_marker;
}

QMatcherInterface* QNFADefinition::matcher()
{
	if ( !m_matcher )
		m_matcher = new QNFAMatcher(this, m_indentFold);
	
	return m_matcher;
}

QIndenterInterface* QNFADefinition::indenter()
{
	if ( !m_indenter )
		m_indenter = new QNFAIndenter(this);
	
	return m_indenter;
}

QHighlighterInterface* QNFADefinition::highlighter()
{
	if ( !m_highlighter )
		m_highlighter = new QNFAHighlighter(this);
	
	return m_highlighter;
}

void QNFADefinition::addContext(const QString& id, QNFA *nfa)
{
	//qDebug("registering context : %s", qPrintable(id));
	m_contexts[id] = nfa;
}

void QNFADefinition::flushEmbedRequests(const QString& lang)
{
	//qDebug("flushing requests...");
	QHash<QString, EmbedRequestList>::iterator it;
	
	it = m_pendingEmbeds.begin();
	
	while ( it != m_pendingEmbeds.end() )
	{
		QString r = it.key();
		
		if (
				r.startsWith(lang)
			&&
				(
					(r.count() == lang.count())
				||
					(r.at(lang.count() + 1) == ':')
				)
			)
		{
			QNFA *src = m_contexts.value(r);
			
			if ( !src )
			{
				++it;
				continue;
			}
			
			foreach( const EmbedRequest& request, *it )
			{
				//qDebug("embedding %s in 0x%x at index %i",
				//		qPrintable(r), request.target, request.index);
				
				embed(src, request.target, request.index);
			}
			
			it = m_pendingEmbeds.erase(it);
		} else {
			++it;
		}
	}
}

void QNFADefinition::addEmbedRequest(const QString& cxt, QNFA *dest)
{
	//qDebug("Adding request : %s", qPrintable(cxt));
	
	if ( m_contexts.contains(cxt) )
	{
		//qDebug("dealing with request : %s", qPrintable(cxt));
		embed(m_contexts[cxt], dest, dest->out.branch->count());
	} else {
		m_pendingEmbeds[cxt] << EmbedRequest(dest, dest->out.branch->count());
	}
}

void QNFADefinition::shareEmbedRequests(QNFA *src, QNFA *dest)
{
	QHash<QString, EmbedRequestList>::iterator it = m_pendingEmbeds.begin();
	
	while ( it != m_pendingEmbeds.end() )
	{
		foreach ( const EmbedRequest& request, *it )
		{
			if ( request.target == src )
			{
				it->append(EmbedRequest(dest, dest->out.branch->count()));
				break;
			}
		}
		
		++it;
	}
}