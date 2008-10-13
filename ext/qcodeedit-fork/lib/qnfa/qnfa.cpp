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

#include "qnfa.h"

#include <QHash>
#include <QList>

quint32 QNFA::_count = 0;
static QList<QNFA*> _deleted;

QNFA::QNFA()
 : type(Char), assertion(0), actionid(0)
{
	out.next = 0;
	
	++_count;
	
	//qDebug("alloc(0x%x) => QNFA syntax[%i];", this, _count);
}

QNFA::~QNFA()
{
	--_count;
	
	// some nfa nodes are shared... gotta make sure they are free'd once only
	_deleted << this;
	
	//qDebug("free(0x%x) => QNFA syntax[%i];", this, _count);
	
	tree.clear();
	
	if ( (type & CxtBeg) && out.branch )
	{
		delete out.branch;
		out.branch = 0;
	}
	
	if ( out.next && !_deleted.contains(out.next) )
	{
		delete out.next;
		out.next = 0;
	}
}


QNFABranch::~QNFABranch()
{
	//qDebug("branch to %i nodes", count());
	for ( int i = 0; i < count(); ++i )
	{
		if ( at(i) && !_deleted.contains(at(i)) )
		{
			delete (*this)[i];
			(*this)[i] = 0;
		}
	}
}

inline static bool isWord(QChar c)
{ return c.isLetterOrNumber() || (c == QLatin1Char('_')); }

static bool match(QChar cc, QNFA *chain)
{
	bool found = true;
	
	if ( chain->c.count() )
		found &= chain->c.contains(cc.unicode());
	
	if ( chain->assertion & Digit )
	{
		found &= cc.isDigit();
	} else if ( chain->assertion & NonDigit ) {
		found &= !cc.isDigit();
	}
	
	if ( chain->assertion & Word )
	{
		found &= cc.isLetterOrNumber() || (cc == '_');
	} else if ( chain->assertion & NonWord ) {
		found &= !cc.isLetterOrNumber() && (cc != '_');
	}
	
	if ( chain->assertion & Space )
	{
		found &= cc.isSpace();
	} else if ( chain->assertion & NonSpace ) {
		found &= !cc.isSpace();
	}
	
	if ( chain->c.count() && (chain->c.at(0) == '\0') )
		found = !found;
	
	return found;
}

void match(QNFAMatchContext *lexer, const QChar *d, int length, QNFAMatchNotifier notify)
{
	if ( !lexer || !lexer->context )
	{
		//qWarning("get off you scum!");
		return;
	}
	
	// restore message buffering
	
	notify.clear();
	
	int olvls = lexer->parents.count(),
		nlvls = 0,
		lvls = olvls;
	
	if ( lvls )
		notify.startBuffering();
	
	//
	
	quint16 c = 0;
	QNFA *chain = 0, *start = 0;
	int index = 0, lastCxt = 0, len;
	bool bFound, bEscape = false, bEscaped = false;
	
	while ( index < length )
	{
		bFound = false;
		bEscaped = false;
		bEscape &= !lexer->meaningless.contains(d[index].unicode());
		
		while ( lexer->meaningless.contains(d[index].unicode()) && ((index + 1) < length) )
			++index;
		
		if ( index >= length )
			break;
		
		c = d[index].unicode();
		
		// try fast plain matching
		if ( ((index - 1) < 0) || !isWord(d[index - 1]) )
		{
			//qDebug("trying plain...");
			
			len = 0;
			QCharTree::const_iterator it, match, end;
			
			it = lexer->context->tree.constFind(c);
			
			if ( it != lexer->context->tree.constEnd() )
			{
				do
				{
					++len;
					end = it->next.constEnd();
					match = it->next.constFind(0);
					
					if ( (index + len) < length )
					{
						c = d[index + len].unicode();
						it = it->next.constFind(c);
					} else {
						it = end;
					}
					
					if ( it == end )
					{
						if ( (match != end) && !isWord(d[index + len]) )
						{
							//word boundary found
							// corresponding token end found
							notify(index, len, match->value.action);
							bFound = true;
							index += len;
							
							//qDebug("next step : %c", d[index].toLatin1());
							//bMonitor = true;
						}
						
						break;
					}
					
				} while ( (index + len + 1) < length );
				
				if ( bFound )
				{
					bEscape = false;
					continue;
				}
			}
		}
		
		// fallback on regexp-like NFA-based matching
		QNFABranch* children = lexer->context->out.branch;
		
		if ( children )
		{
			//qDebug("trying %i sub nfas on %c", children->count(), d[index].toLatin1());
			
			for ( quint16 i = 0; i < children->count(); ++i )
			{
				len = 0;
				start = chain = children->at(i);
				
				//qDebug("%ith attempt on %c", i, d[index + len].toLatin1());
				
				while ( ((index + len) < length) || (chain->type & Match) )
				{
					bEscaped = false;
					
					if ( chain->type & Match )
					{
						if (
								(chain->assertion & WordEnd)
							&&
								((index + len) < length)
							&&
								(isWord(d[index + len]))
							)
						{
							//qDebug("end assertion failed...");
							break;
						}
						
						//qDebug("matched to end");
						
						if ( chain->type & CxtBeg )
						{
							//qDebug("entering context : 0x%x", chain);
							
							++nlvls;
							
							// notify content of previous context until nest
							notify(lastCxt, index - lastCxt, lexer->context->actionid | 0x80000000);
							
							if ( notify.bufferLevel() )
							{
								// notify sub matches so far to avoid tricky handling later on
								notify.stopBuffering();
								notify.flush();
								
								//notify.startBuffering();
							}
							
							// notify begin marker
							notify(index, len, start->actionid ? start->actionid : chain->actionid);
							
							// update context stack
							lexer->parents.push(lexer->context);
							lexer->context = chain;
							
							// update nest index
							lastCxt = index + len;
							
							// push a message buffer
							notify.startBuffering();
							
						} else if ( chain->type & CxtEnd ) {
							//qDebug("leaving context :");
							
							if ( lexer->parents.isEmpty() )
								qFatal("context nesting problem");
							
							if ( bEscape )
							{
								// not really end : escape found...
								
								bEscape = false;
								bEscaped = true;
							} else {
								
								if ( nlvls )
									--nlvls;
								else
									--lvls;
								
								// pop one message buffer
								notify.stopBuffering();
								
								// notify context content from last nest
								notify(lastCxt, index - lastCxt, lexer->context->actionid | 0x80000000);
								
								// flush sub matches
								notify.flush();
								
								// update context stack
								lexer->context = lexer->parents.pop();
								
								if ( lexer->parents.count() )
									notify.startBuffering();
								
								// update nest index
								lastCxt = index + len;
								
								// notify end marker
								notify(index, len, chain->actionid);
								
								//qDebug("cxt notif...");
								
								if ( chain->type & Exclusive )
									index += len - 1;
								else
									--index;
								
								bFound = true;
								break;
							}
						} else if ( chain->type & CxtEsc )  {
							//qDebug("matched %s", qPrintable(QString(index, len)));
							
							//notify(index, len, chain->actionid);
							bEscape = !bEscape;
						} else {
							//qDebug("matched %s", qPrintable(QString(d + index, len)));
							
							notify(index, len, chain->actionid);
							bEscape = false;
						}
						
						
						index += len - 1;
						bFound = true;
						
						//qDebug("next step : %c", d[index + 1].toLatin1());
						//bMonitor = true;
						
						break;
					} else {
						// "regular" nfa match (no match yet...)
						
						if (
								(chain->assertion & WordStart)
							&&
								((index + len - 1) >= 0)
							&&
								(isWord(d[index + len - 1]))
							)
						{
							//qDebug("beg assertion failed...");
							
							break;
						}
						
						QChar cc = d[index + len];
						bool found = match(cc, chain);
						
						if (
								!(chain->assertion & ZeroOrOne)
							&&
								!(chain->assertion & ZeroOrMore)
							&&
								!found
							)
						{
							//if ( cc.toLatin1() == ')' )
							//	qDebug("mismatch : %c != %c", cc.toLatin1(), chain->c.at(0));
							
							break;
						}
						
						if ( found )
						{
							//qDebug("%c", d[index + len].toLatin1());
							++len;		//matched one
						} else {
							//qDebug("! %c", d[index + len].toLatin1());
						}
						
						if (
								found
							&&
								(
									(chain->assertion & OneOrMore)
								||
									(chain->assertion & ZeroOrMore)
								)
							)
						{
							while (
										((index + len) < length)
									&&
										match(d[index + len], chain)
									)
							{
								//qDebug("%c", d[index + len].toLatin1());
								++len;
							}
						}
						
						chain = chain->out.next;
					}
				}
				
				if ( bFound )
					break;
				
			}
		}
		
		if ( !bFound )
		{
			bEscape = false;
			//++index;
		}
		
		++index;
	}
	
	// flush messages
	
	if ( !notify.bufferLevel() )
		return;
	
	//qDebug("%i context nests", notify.bufferLevel());
	//qDebug("[%i;+00[ : 0x%x", lastCxt, lexer->context->actionid | 0x80000000);
	
	// pop down one buffer
	notify.stopBuffering();
	
	// notify overlapping context so far
	notify(lastCxt, length - lastCxt, lexer->context->actionid | 0x80000000);
	
	// notify sub matches
	notify.flush();
	
	// make sure we leave a blank notifier...
	notify.clear();
	
	// preserve escape power...
	if ( bEscaped )
		return;
	
	// some existing left AND new one(s)
	if ( (olvls == lvls) && nlvls )
		++lvls;
	
	// close stay-on-line contexts, if any
	QStack<QNFA*>::iterator it = lexer->parents.begin() + lvls;
	
	while ( it != lexer->parents.end() )
	{
		if ( (*it)->type & StayOnLine )
		{
			//qDebug("staid...");
			it = lexer->parents.erase(it);
		} else {
			++it;
		}
	}
	
	if ( (lexer->context->type & StayOnLine) && nlvls && lexer->parents.count() )
		lexer->context = lexer->parents.pop();
	
}

QNFA* lexer()
{
	QNFA *lex = new QNFA;
	
	lex->type = ContextBegin;
	lex->out.branch = new QNFABranch;
	
	return lex;
}

QNFA* sharedContext(const QString& start, QNFA *other, bool cs)
{
	QNFA *nfa, *end, *beg = sequence(start.constData(), start.length(), &end, cs);
	
	nfa = new QNFA;
	nfa->type = ContextBegin;
	nfa->out.branch = other->out.branch;
	
	end->out.next = nfa;
	
	return beg;
}

QNFA* context(const QString& start, const QString& stop, const QString&, int action, QNFA **handler, bool cs)
{
	QNFA *nfa, *end, *beg = sequence(start.constData(), start.length(), &end, cs);
	
	nfa = new QNFA;
	nfa->type = ContextBegin;
	nfa->actionid = action;
	nfa->out.branch = new QNFABranch;
	
	if ( handler )
		*handler = nfa;
	//else
	//	qDebug("no handler set [0x%x]", nfa);
	
	end->out.next = nfa;
	end = nfa;
	
	QNFA *endmark, *begendmark = sequence(stop.constData(), stop.length(), &endmark, cs);
	
	nfa = new QNFA;
	nfa->type = ContextEnd;
	nfa->actionid = action;
	
	endmark->out.next = nfa;
	
	//end->out->branch->append(endmark);
	addNFA(end, begendmark);
	
	return beg;
}

void addWord(QNFA *lexer, const QString& w, int action, bool cs)
{
	if ( !lexer || (lexer->type != ContextBegin) || !lexer->out.branch )
		return;
	
	// try using the fastest way if possible
	
	QString pt;
	
	if ( plain(w, &pt) && cs )
	{
		addWord(lexer->tree, pt, action, cs);
		return;
	}
	
	// fallback on (fast) regexp-like NFA-based semi-compiled parsing
	QNFA *nfa, *word, *end;
	
	word = sequence(w.constData(), w.length(), &end, cs);
	word->assertion |= WordStart;
	
	nfa = new QNFA;
	nfa->type = Match;
	nfa->assertion = WordEnd;
	nfa->actionid = action;
	
	end->out.next = nfa;
	
	//lexer->out.branch->append(word);
	addNFA(lexer, word);
}

void addSequence(QNFA *lexer, const QString& w, int action, bool cs)
{
	if ( !lexer || (lexer->type != ContextBegin) || !lexer->out.branch )
		return;
	
	QNFA *seq, *end, *nfa;
	
	seq = sequence(w.constData(), w.length(), &end, cs);
	
	nfa = new QNFA;
	nfa->type = Match;
	nfa->actionid = action;
	
	end->out.next = nfa;
	
	//lexer->out.branch->append(seq);
	addNFA(lexer, seq);
}

QNFA* sequence(const QChar *d, int length, QNFA **end, bool cs)
{
	QNFA *nfa, *set = 0, *prev = 0, *first = 0;
	
	for ( int i = 0; i < length; ++i )
	{
		QChar c = d[i];
		
		if ( c == QLatin1Char('\\') )
		{
			c = d[++i];
			
			if ( c == QLatin1Char('n') )
			{
				c = '\n';
			} else if ( c == QLatin1Char('t') ) {
				c = '\t';
			} else if ( c == QLatin1Char('r') ) {
				c = '\r';
			}
			
			if ( set )
			{
				set->c << c.unicode();
			} else {
				nfa = new QNFA;
				nfa->c << c.unicode();
				
				if ( prev )
					prev->out.next = nfa;
				
				prev = nfa;
			}
		} else if ( c == QLatin1Char('$') ) {
			// char classes
			c = d[++i];
			
			if ( set )
			{
				if ( c == QLatin1Char('s') )
					set->assertion |= Space;
				else if ( c == QLatin1Char('S') )
					set->assertion |= NonSpace;
				else if ( c == QLatin1Char('d') )
					set->assertion |= Digit;
				else if ( c == QLatin1Char('D') )
					set->assertion |= NonDigit;
				else if ( c == QLatin1Char('w') )
					set->assertion |= Word;
				else if ( c == QLatin1Char('W') )
					set->assertion |= NonWord;
				else
					set->c << QLatin1Char('$').unicode() << c.unicode();
				
			} else {
				nfa = new QNFA;
				
				if ( c == QLatin1Char('s') )
					nfa->assertion |= Space;
				else if ( c == QLatin1Char('S') )
					nfa->assertion |= NonSpace;
				else if ( c == QLatin1Char('d') )
					nfa->assertion |= Digit;
				else if ( c == QLatin1Char('D') )
					nfa->assertion |= NonDigit;
				else if ( c == QLatin1Char('w') )
					nfa->assertion |= Word;
				else if ( c == QLatin1Char('W') )
					nfa->assertion |= NonWord;
				else {
					nfa->c << QLatin1Char('$').unicode();
					--i;
				}
				
				if ( prev )
					prev->out.next = nfa;
				
				prev = nfa;
			}
		} else if ( c == QLatin1Char('[') ) {
			
			if ( set )
			{
				set->c << c.unicode();
				//	qWarning("Nested sets are not supported (and useless BTW)...");
				continue;
			}
			
			// enter set...
			
			set = new QNFA;
			
			//qDebug("set start");
			
		} else if ( c == QLatin1Char(']') ) {
			
			if ( !set )
			{
				qWarning("Unmatched set closing marker");
				continue;
			}
			
			// leave set...
			
			if ( prev )
				prev->out.next = set;
			
			prev = set;
			set = 0;
			
			//qDebug("set end");
			
		} else if ( set ) {
			
			if ( (c == QLatin1Char('^')) && !set->c.count() )
			{
				set->c << '\0';
			} else {
				set->c << c.unicode();
			}
			
			quint16 prev = set->c.count() ? set->c.at(set->c.length() - 1) : '\0';
			
			if ( (c == '-') && (prev != '\0') && ((i + 1) < length) )
			{
				quint16 cse = d[++i].unicode();
				
				for ( quint16 csi = prev + 1; csi <= cse; ++csi )
				{
					QChar csc(csi);
					
					if ( c.isLetter() && !cs )
						set->c << c.toLower().unicode() << c.toUpper().unicode();
					else
						set->c << csi;
				}
			} else {
				if ( c.isLetter() && !cs )
					set->c << c.toLower().unicode() << c.toUpper().unicode();
				else
					set->c << c.unicode();
			}
			//qDebug("set << %c", c.toLatin1());
			
		} else if ( c == QLatin1Char('+') ) {
			if ( prev ) prev->assertion |= OneOrMore;
		} else if ( c == QLatin1Char('*') ) {
			if ( prev ) prev->assertion |= ZeroOrMore;
		} else if ( c == QLatin1Char('?') ) {
			if ( prev ) prev->assertion |= ZeroOrOne;
		} else {
			nfa = new QNFA;
			
			if ( c.isLetter() && !cs )
			{
				nfa->c << c.toLower().unicode() << c.toUpper().unicode();
			} else {
				nfa->c << c.unicode();
			}
			
			if ( prev )
				prev->out.next = nfa;
			
			prev = nfa;
		}
		
		if ( !first )
			first = prev;
	}
	
	if ( end )
	{
		*end = prev;
	}
	
	return first;
}

bool plain(const QString& word, QString *dest)
{
	if ( dest )
		dest->clear();
	
	for ( int i = 0; i < word.length(); i++ )
	{
		QChar c = word.at(i);
		
		if ( c == QLatin1Char('\\') )
		{
			if ( dest && ((i + 1) < word.length()) )
			{
				c = word.at(++i);
				
				if ( c == QLatin1Char('n') )
					dest->append('\n');
				else if ( c == QLatin1Char('t') )
					dest->append('\t');
				else if ( c == QLatin1Char('r') )
					dest->append('\r');
				else
					dest->append(c);
			}
		} else if (
						c == QLatin1Char('[')
					||
						c == QLatin1Char(']')
					||
						c == QLatin1Char('+')
					||
						c == QLatin1Char('*')
					||
						c == QLatin1Char('?')
					||
						c == QLatin1Char('$')
					)
		{
			if ( dest )
				dest->clear();
			
			return false;
		} else {
			
			if ( dest )
				dest->append(c);
			
		}
	}
	
	return true;
}

void addWord(QCharTree& tree, const QString& w, int action, bool cs)
{
	//qDebug("Adding word to char tree : %s", qPrintable(w));
	
	if ( cs )
	{
		quint16 u = w.at(0).unicode();
		QCharTree::iterator it = tree.find(u), tmp;
		
		if ( it == tree.end() )
			it = tree.insert(u, QCharTreeNode(u));
		
		for ( int i = 1; i < w.count(); i++ )
		{
			u = w.at(i).unicode();
			
			//qDebug("char %c", w.at(i).toLatin1());
			
			tmp = it->next.find(u);
			
			if ( tmp == it->next.end() )
				tmp = it->next.insert(u, QCharTreeNode(u));
			
			it = tmp;
		}
		
		// add action handler
		QCharTreeNode node;
		node.value.action = action;
		
		it->next[0] = node;
	} else if ( 0 ) {
		QChar c = w.at(0);
		quint16 u = c.unicode();
		
		QCharTree::iterator it, tmp;
		QList<QCharTree::iterator> l, ltmp;
		
		if ( c.isLetter() )
		{
			u = c.toLower().unicode();
			tmp = tree.find(u);
			
			if ( tmp == tree.end() )
				tmp = tree.insert(u, QCharTreeNode(u));
			
			l << tmp;
			
			u = c.toUpper().unicode();
			tmp = tree.find(u);
			
			if ( tmp == tree.end() )
				tmp = tree.insert(u, QCharTreeNode(u));
			
			l << tmp;
		} else {
			tmp = tree.find(u);
			
			if ( tmp == tree.end() )
				tmp = tree.insert(u, QCharTreeNode(u));
			
			l << tmp;
		}
		
		for ( int i = 1; i < w.count(); ++i )
		{
			c = w.at(i);
			QList<QChar> lc;
			
			if ( c.isLetter() )
				lc << c.toLower() << c.toUpper();
			else
				lc << c;
			
			foreach ( c, lc )
			{
				u = c.unicode();
				
				foreach ( it, l )
				{
					tmp = it->next.find(u);
					
					if ( tmp == it->next.end() )
						tmp = it->next.insert(u, QCharTreeNode(u));
					
					ltmp << tmp;
				}
			}
			
			l = ltmp;
		}
		
		// add action handler
		QCharTreeNode node;
		node.value.action = action;
		
		foreach ( it, l )
			it->next[0] = node;
	}
}

void squeeze(QNFA *nfa)
{
	squeeze(nfa->tree);
	
	if ( nfa->type & Match )
	{
		if ( nfa->out.branch )
			for ( int i = 0; i < nfa->out.branch->count(); ++i )
				squeeze(nfa->out.branch->at(i));
		
	} else if ( nfa->out.next ) {
		squeeze(nfa->out.next);
	}
}

void squeeze(QCharTreeLevel& lvl)
{
	lvl.squeeze();
	
	QCharTreeLevel::iterator it = lvl.begin();
	
	while ( it != lvl.end() )
		squeeze((it++)->next);
}
