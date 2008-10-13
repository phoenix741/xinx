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

#include "qdocument.h"

/*
	Document model :
	
	Goals :
		* KISS : Keep It Simple Stupid
		* FAST : ...
		* LIGHTWEIGHT : reduce memory usage
		* FLEXIBLE : allow punctual bidi through QTextLayout
	
	Implementation :
		QDocument
		QDocumentPrivate
		
		QDocumentLine and QDocumentLineHandle => equivalent of QTextBlock
		QDocumentCursor and QDocumentCursorHandle => equivalent of QTextCursor
*/

#include "qdocument_p.h"
#include "qdocumentcommand.h"

#include "qformat.h"
#include "qformatfactory.h"

#include "qlinemarksinfocenter.h"

#include <QPen>
#include <QTime>
#include <QRect>
#include <QLine>
#include <QPainter>
#include <QTextLayout>
#include <QApplication>
#include <QVarLengthArray>

static int m_spaceSignOffset = 2;

static QPoint m_spaceSign[] = {
	QPoint(2, -1),
	QPoint(2, 0),
	QPoint(3, 0)
};

inline static bool isWord(QChar c)
{ return c.isLetterOrNumber() || (c == QLatin1Char('_')); }

int QDocument::screenLength(const QChar *d, int l, int tabStop)
{
	if ( tabStop == 1 )
		return l;
	
	int idx, column = idx = 0;
	
	while ( idx < l )
	{
		QChar c = d[idx];
		
		if ( c == QLatin1Char('\t') )
		{
			int taboffset = tabStop - (column % tabStop);
			column += taboffset;
		} else {
			++column;
		}
		
		++idx;
	}
	
	//qDebug("%s : %i", qPrintable(QString(d, l)), column);
	
	return column;
}

QString QDocument::screenable(const QChar *d, int l, int tabStop)
{
	if ( tabStop == 1 )
		return QString(d, l);
	
	QString fragment;
	int idx, column = idx = 0;
	
	while ( idx < l )
	{
		QChar c = d[idx];
		
		if ( c == QLatin1Char('\t') )
		{
			int taboffset = tabStop - (column % tabStop);
			
			fragment += QString(taboffset, QLatin1Char(' '));
			column += taboffset;
		} else {
			fragment += c;
			++column;
		}
		
		++idx;
	}
	
	return fragment;
}

struct InitStruct
{
	InitStruct()
	{
		qRegisterMetaType<QDocumentIterator>("QDocumentIterator");
		qRegisterMetaType<QDocumentConstIterator>("QDocumentConstIterator");
	}
};

static InitStruct init_inst;

QDocument::QDocument(QObject *p)
 : QObject(p), m_impl(new QDocumentPrivate(this))
{
	if ( !QDocumentPrivate::m_font )
	{
		// must not happen if config dialog plugged in...
		setFont(QFont("Monospace", 10));
	}
	
	setText(QString());
	setLineEnding(QDocument::Local);
	
	connect(&(m_impl->m_commands)	, SIGNAL( cleanChanged(bool) ),
			this					, SIGNAL( cleanChanged(bool) ) );
	
	connect(&(m_impl->m_commands)	, SIGNAL( canUndoChanged(bool) ),
			this					, SIGNAL( undoAvailable(bool) ) );
	
	connect(&(m_impl->m_commands)	, SIGNAL( canRedoChanged(bool) ),
			this					, SIGNAL( redoAvailable(bool) ) );
	
	connect(this							,
			SIGNAL( lineDeleted(QDocumentLineHandle*) ),
			QLineMarksInfoCenter::instance(),
			SLOT  ( lineDeleted(QDocumentLineHandle*) ) );
	
}

QDocument::~QDocument()
{
	delete m_impl;
}

void QDocument::clear()
{
	setText(QString());
}

void QDocument::undo()
{
	if ( m_impl )
		m_impl->m_commands.undo();
	
}

void QDocument::redo()
{
	if ( m_impl )
		m_impl->m_commands.redo();
	
}
QString QDocument::text(bool removeTrailing, bool preserveIndent) const
{
	QString s;
	
	if ( !m_impl )
		return s;
	
	foreach ( QDocumentLineHandle *l, m_impl->m_lines )
	{
		if ( removeTrailing )
		{
			QString buf = l->text();
			int len = 0, idx = buf.length();
			
			while ( --idx >= 0 )
			{
				if ( !buf.at(idx).isSpace() )
					break;
				
				++len;
			}
			
			++idx;
			
			if ( len && (idx || !preserveIndent) )
				l->textBuffer().remove(idx, len);
		}
		
		s += l->text() + m_impl->m_lineEndingString;
	}
	
	s.chop(m_impl->m_lineEndingString.count());
	
	return s;
}

void QDocument::setText(const QString& s)
{
	if ( !m_impl )
		return;
	
	int last = 0, idx = 0;
	
	m_impl->m_deleting = true;
	
	//qDeleteAll(m_impl->m_lines);
	foreach ( QDocumentLineHandle *h, m_impl->m_lines )
		h->deref();
	
	m_impl->m_lines.clear();
	m_impl->m_marks.clear();
	m_impl->m_status.clear();
	m_impl->m_hidden.clear();
	m_impl->m_commands.clear();
	
	m_impl->m_deleting = false;
	
	if ( s.isEmpty() )
		m_impl->m_lines << new QDocumentLineHandle(QString(), this);
	
	m_impl->_nix = 0;
	m_impl->_dos = 0;
	
	while ( idx < s.length() )
	{
		if ( s.at(idx) == '\n' )
		{
			if ( (idx > 0) && (s.at(idx - 1) == '\r') )
			{
				++(m_impl->_dos);
				
				m_impl->m_lines << new QDocumentLineHandle(
										s.mid(last, idx - last - 1),
										this
									);
			} else {
				++(m_impl->_nix);
				
				m_impl->m_lines << new QDocumentLineHandle(
										s.mid(last, idx - last),
										this
									);
			}
			
			last = ++idx;
		} else {
			++idx;
		}
	}
	
	if ( idx != last )
	{
		m_impl->m_lines << new QDocumentLineHandle(
								s.mid(last, s.length() - last),
								this
							);
		
	}
	
	if ( (idx > 0) && ((idx - 1) < s.length()) && ((s.at(idx - 1) == '\n') || (s.at(idx - 1) == '\r')) )
		m_impl->m_lines << new QDocumentLineHandle(this);
	
	//if ( _dos > _nix )
	//	setLineEnding(Windows);
	//else
	//	setLineEnding(Unix);
	
	//qDebug("[one go] dos : %i; nix : %i", m_impl->_dos, m_impl->_nix);
	
	if ( lineEnding() == Conservative )
		setLineEnding(Conservative);
	
	m_impl->setWidth();
	m_impl->setHeight();
	
	emit m_impl->emitContentsChange(0, m_impl->m_lines.count());
}

void QDocument::startChunkLoading()
{
	if ( !m_impl )
		return;
	
	m_impl->m_deleting = true;
	
	//qDeleteAll(m_impl->m_lines);
	foreach ( QDocumentLineHandle *h, m_impl->m_lines )
		h->deref();
	
	m_impl->m_lines.clear();
	m_impl->m_marks.clear();
	m_impl->m_status.clear();
	m_impl->m_hidden.clear();
	m_impl->m_commands.clear();
	
	m_impl->m_deleting = false;
	
	m_impl->_nix = 0;
	m_impl->_dos = 0;
	
	m_leftOver.clear();
}

void QDocument::stopChunkLoading()
{
	if ( m_leftOver.count() )
	{
		m_impl->m_lines << new QDocumentLineHandle(
								m_leftOver,
								this
							);
		
		m_leftOver.clear();
		
	} else {
		m_impl->m_lines << new QDocumentLineHandle(this);
	}
	
	//if ( _dos >_nix )
	//	setLineEnding(Windows);
	//else
	//	setLineEnding(Unix);
	
	//qDebug("[chunk] dos : %i; nix : %i", m_impl->_dos, m_impl->_nix);
	
	if ( lineEnding() == Conservative )
		setLineEnding(Conservative);
	
	m_impl->setWidth();
	m_impl->setHeight();
	
	emit m_impl->emitContentsChange(0, m_impl->m_lines.count());
}

void QDocument::addChunk(const QString& txt)
{
	if ( !m_impl || txt.isEmpty() )
		return;
	
	m_leftOver += txt;
	int idx = 0, last = 0;
	
	while ( idx < m_leftOver.length() )
	{
		if ( m_leftOver.at(idx) == '\n' )
		{
			if ( (idx > 0) && (m_leftOver.at(idx - 1) == '\r') )
			{
				++(m_impl->_dos);
				
				m_impl->m_lines << new QDocumentLineHandle(
										m_leftOver.mid(last, idx - last - 1),
										this
									);
			} else {
				++(m_impl->_nix);
				
				m_impl->m_lines << new QDocumentLineHandle(
										m_leftOver.mid(last, idx - last),
										this
									);
			}
			
			last = ++idx;
		} else {
			++idx;
		}
	}
	
	if ( idx != last )
		m_leftOver = m_leftOver.mid(last);
	else
		m_leftOver.clear();
	
}

QDocument::LineEnding QDocument::lineEnding() const
{
	return m_impl ? m_impl->m_lineEnding : Local;
}

QDocument::LineEnding QDocument::originalLineEnding() const
{
	return m_impl ? (m_impl->_dos > m_impl->_nix ? Windows : Unix) : Local;
}

void QDocument::setLineEnding(LineEnding le)
{
	if ( !m_impl )
		return;
	
	m_impl->m_lineEnding = le;
	QString& les = m_impl->m_lineEndingString;
	
	switch ( le )
	{
		case Conservative :
			
			
			if ( m_impl->_dos > m_impl->_nix )
				les = "\r\n";
			else
				les = "\n";
			
			break;
			
		case Local :
			#ifdef Q_OS_WIN
			les = "\r\n";
			//#elif defined(Q_OS_MAC)
			//les = "\r";
			#else
			les = "\n";
			#endif
			break;
			
		case Unix :
			les = "\n";
			break;
			
		case Mac :
			//les = "\r";
			les = "\n";
			break;
			
		case Windows :
			les = "\r\n";
			break;
			
		default :
			les = "\n";
			break;
	}
	
	emit lineEndingChanged(le);
}

QFont QDocument::font()
{
	return *(QDocumentPrivate::m_font);
}

void QDocument::setFont(const QFont& f)
{
	QDocumentPrivate::setFont(f);
	//emit contentsChanged();
}

const QFontMetrics& QDocument::fontMetrics()
{
	return *(QDocumentPrivate::m_fontMetrics);
}

int QDocument::tabStop()
{
	return QDocumentPrivate::m_tabStop;
}

void QDocument::setTabStop(int n)
{
	QDocumentPrivate::m_tabStop = n;
	
	foreach ( QDocumentPrivate *d, QDocumentPrivate::m_documents )
		d->emitFormatsChanged();
	
}

QDocument::WhiteSpaceMode QDocument::showSpaces()
{
	return QDocumentPrivate::m_showSpaces;
}

void QDocument::setShowSpaces(WhiteSpaceMode m)
{
	QDocumentPrivate::m_showSpaces = m;
	
	foreach ( QDocumentPrivate *d, QDocumentPrivate::m_documents )
		d->emitFormatsChanged();
	
}

QDocumentCursor* QDocument::editCursor() const
{
	return m_impl ? m_impl->m_editCursor : 0;
}

void QDocument::setEditCursor(QDocumentCursor *c)
{
	if ( m_impl )
		m_impl->m_editCursor = c;
	
}

int QDocument::width() const
{
	return m_impl ? m_impl->m_width : 0;
}

int QDocument::height() const
{
	return m_impl ? m_impl->m_height : 0;
}

int QDocument::lines() const
{
	return m_impl ? m_impl->m_lines.count() : 0;
}

QDocumentLine QDocument::line(int line) const
{
	if ( !m_impl || (line < 0) || (line >= lines()) )
		return QDocumentLine();
	
	//qWarning("QDocument::line(int) is slow... Avoid it!");
	
	return QDocumentLine(m_impl->at(line));
}

int QDocument::lineNumber(int ypos) const
{
	int ln = ypos / m_impl->m_lineSpacing;
	
	return m_impl->textLine(ln);
}

QDocumentLine QDocument::line(QDocumentConstIterator iterator) const
{
	return (m_impl && (m_impl->constEnd() != iterator)) ? QDocumentLine(*iterator) : QDocumentLine();
}

QDocumentCursor QDocument::cursor(int line, int column) const
{
	return QDocumentCursor(const_cast<QDocument*>(this), line, column);
}

QDocumentLine QDocument::findLine(int& position) const
{
	if ( !m_impl )
		return QDocumentLine();
	
	return QDocumentLine(m_impl->lineForPosition(position));
}

int QDocument::y(int ln) const
{
	if ( !m_impl )
		return -1;
	
	return m_impl->m_lineSpacing * m_impl->visualLine(ln);
}

int QDocument::y(const QDocumentLine& l) const
{
	qDebug("bad perf...");
	//return y(l.lineNumber());
	
	if ( !m_impl )
		return -1;
	
	int pos = 0, ls = m_impl->m_lineSpacing;
	
	QDocumentConstIterator it = begin();
	
	while ( (it != end()) && (*it != l.handle()) )
	{
		if ( !line(it).isHidden() )
			pos += ls;
		
		++it;
	}
	
	return pos;
}

QRect QDocument::lineRect(int line) const
{
	const int yoff = y(line);
	
	return (yoff != -1) ? QRect(0, yoff, width(), m_impl->m_lineSpacing) : QRect();
}

QRect QDocument::lineRect(const QDocumentLine& l) const
{
	//return lineRect(l.lineNumber());
	const int yoff = y(l);
	
	return (yoff != -1) ? QRect(0, yoff, width(), m_impl->m_lineSpacing) : QRect();
}

QDocumentLine QDocument::lineAt(const QPoint& p) const
{
	if ( !m_impl )
		return QDocumentLine();
	
	return line(lineNumber(p.y()));
}

QDocumentConstIterator QDocument::begin() const
{
	Q_ASSERT(m_impl);
	
	return m_impl->m_lines.constBegin();
}

QDocumentConstIterator QDocument::end() const
{
	Q_ASSERT(m_impl);
	
	return m_impl->m_lines.constEnd();
}

QDocumentConstIterator QDocument::iterator(int ln) const
{
	Q_ASSERT(m_impl);
	
	return begin() + ln;
}

QDocumentConstIterator QDocument::iterator(const QDocumentLine& l) const
{
	Q_ASSERT(m_impl);
	
	QDocumentConstIterator it = begin(), e = end();
	
	while ( (*it != l.handle()) && (it != e) )
		++it;
	
	return it;
}

QDocumentCursor QDocument::cursorAt(const QPoint& p, int xoff) const
{
	if ( !m_impl )
		return QDocumentCursor();
	
	int ln = lineNumber(p.y());
	QDocumentLine l = line(ln);
	
	//qDebug("(%i, %i) => %i", p.x(), p.y(), ln);
	
	if ( !l.isValid() )
		return QDocumentCursor();
	
	int col = l.xToCursor(p.x(), xoff);
	
	//printf("QPoint(%i, %i) => %i:%i\n", p.x(), p.y(), ln, col);
	//fflush(stdout);
	
	return QDocumentCursor(const_cast<QDocument*>(this), ln, col);
}

void QDocument::draw(QPainter *p, PaintContext& cxt)
{
	m_impl->draw(p, cxt);
}

void QDocument::execute(QDocumentCommand *cmd)
{
	if ( m_impl && cmd )
		m_impl->execute(cmd);
	
}

QFormatFactory* QDocument::formatFactory()
{
	return QDocumentPrivate::m_formatFactory;
}

void QDocument::setFormatFactory(QFormatFactory *f)
{
	QDocumentPrivate::m_formatFactory = f;
	QDocumentPrivate::updateFormatCache();
}

void QDocument::beginMacro()
{
	if ( m_impl )
		m_impl->beginChangeBlock();
	
}

void QDocument::endMacro()
{
	if ( m_impl )
		m_impl->endChangeBlock();
	
}

bool QDocument::isClean() const
{
	return m_impl ? m_impl->m_commands.isClean() : true;
}

void QDocument::setClean()
{
	if ( m_impl )
	{
		m_impl->m_commands.setClean();
		m_impl->m_status.clear();
	}
}

bool QDocument::isLineModified(const QDocumentLine& l) const
{
	return (!m_impl || !m_impl->m_status.contains(l.handle()))
			?
				false
			:
				m_impl->m_status.value(l.handle())
			;
}

/////////////////////////
//	QDocumentLineHandle
/////////////////////////
//static quint32 q_line_counter = 0;

QDocumentLineHandle::QDocumentLineHandle(QDocument *d)
 : m_doc(d)
#if QT_VERSION >= 0x040400
 , m_ref(1)
#endif
 , m_state(QDocumentLine::LayoutDirty)
 , m_layout(0)
{
	#if QT_VERSION < 0x040400
	m_ref.init(1);
	#endif
}

QDocumentLineHandle::QDocumentLineHandle(const QString& s, QDocument *d)
 : m_text(s)
 , m_doc(d)
#if QT_VERSION >= 0x040400
 , m_ref(1)
#endif
 , m_state(QDocumentLine::LayoutDirty)
 , m_layout(0)
{
	#if QT_VERSION < 0x040400
	m_ref.init(1);
	#endif
}

QDocumentLineHandle::~QDocumentLineHandle()
{
	Q_ASSERT(!m_ref);
	
	if ( m_doc && m_doc->impl() )
		m_doc->impl()->emitLineDeleted(this);
}

int QDocumentLineHandle::count() const
{
	return m_text.count();
}

int QDocumentLineHandle::length() const
{
	return m_text.length();
}

int QDocumentLineHandle::line() const
{
	return (m_doc && m_doc->impl()) ? m_doc->impl()->indexOf(this) : -1;
}

int QDocumentLineHandle::position() const
{
	return (m_doc && m_doc->impl()) ? m_doc->impl()->position(this) : -1;
}

QString QDocumentLineHandle::text() const
{
	return m_text;
}

int QDocumentLineHandle::nextNonSpaceChar(uint pos) const
{
	const int len = m_text.length();
	const QChar *unicode = m_text.unicode();
	
	for ( int i = pos; i < len; ++i )
	{
		if ( !unicode[i].isSpace() )
			return i;
	}
	
	return -1;
}

int QDocumentLineHandle::previousNonSpaceChar(int pos) const
{
	const int len = m_text.length();
	const QChar *unicode = m_text.unicode();
	
	if ( pos < 0 )
		pos = 0;
	
	if ( pos >= len )
		pos = len - 1;
	
	for ( int i = pos; i >= 0; --i )
	{
		if ( !unicode[i].isSpace() )
			return i;
	}
	
	return -1;
}

QDocument* QDocumentLineHandle::document() const
{
	return m_doc;
}

bool QDocumentLineHandle::hasFlag(QDocumentLine::State s) const
{
	return m_state & s;
}

void QDocumentLineHandle::setFlag(QDocumentLine::State s, bool y) const
{
	if ( y )
		m_state |= s;
	else
		m_state &= ~s;
}

QDocumentLineHandle* QDocumentLineHandle::next() const
{
	return (m_doc && m_doc->impl()) ? m_doc->impl()->next(this) : 0;
}

QDocumentLineHandle* QDocumentLineHandle::previous() const
{
	return (m_doc && m_doc->impl()) ? m_doc->impl()->previous(this) : 0;
}

int QDocumentLineHandle::cursorToX(int cpos) const
{
	cpos = qBound(0, cpos, m_text.length());
	
	if ( m_layout )
		return qRound(m_layout->lineAt(0).cursorToX(cpos));
	
	int tabStop = QDocument::tabStop();
	
	if ( QDocumentPrivate::m_fixedPitch )
	{
		return QDocument::screenLength(m_text.constData(), cpos, tabStop)
				* QDocumentPrivate::m_spaceWidth
				+ QDocumentPrivate::m_leftMargin;
	}
	
	//qDebug("c->x(%i) unsafe computations...", cpos);
	
	QMediumArray composited = compose();
	
	if ( (composited.count() < cpos) || QDocumentPrivate::m_fonts.isEmpty() )
		return QDocumentPrivate::m_fontMetrics->width(m_text.left(cpos));
	
	int idx = 0, column = 0, cwidth;
	int screenx = QDocumentPrivate::m_leftMargin;
	
	while ( idx < cpos )
	{
		QChar c = m_text.at(idx);
		QFontMetrics fm(QDocumentPrivate::m_fonts.at(composited[idx]));
		
		if ( c == '\t' )
		{
			int taboffset = tabStop - (column % tabStop);
			
			column += taboffset;
			cwidth = fm.width(' ') * taboffset;
		} else {
			++column;
			cwidth = fm.width(c);
		}
		
		screenx += cwidth;
		++idx;
	}
	
	//qDebug("cursorToX(%i) = %i", cpos, screenx);
	
	return screenx;
}

int QDocumentLineHandle::xToCursor(int xpos, int xoff) const
{
	//qDebug("x->c(%i) unsafe computations...", xpos);
	if ( m_layout )
		return m_layout
				->lineAt(0)
				.xToCursor(xpos + xoff - QDocumentPrivate::m_leftMargin);
	
	int screenx = xpos;
	int tabStop = QDocument::tabStop();
	
	if ( QDocumentPrivate::m_fixedPitch )
	{
		int screenPos =
					(screenx - (QDocumentPrivate::m_leftMargin >> 1))
				/
					QDocumentPrivate::m_spaceWidth
				;
		
		if ( tabStop == 1 )
			return screenPos;
		
		int idx = 0, column = 0;
		
		while ( (column < screenPos) && (idx < m_text.length()) )
		{
			QChar c = m_text.at(idx);
			
			if ( c == QLatin1Char('\t') )
			{
				int taboffset = tabStop - (column % tabStop);
				column += taboffset;
			} else {
				++column;
			}
				
			++idx;
		}
		
		return idx;
	} else {
		if ( screenx <= QDocumentPrivate::m_leftMargin )
			return 0;
		
		QMediumArray composited = compose();
		
		int idx = 0, x = 0, column = 0, cwidth;
		screenx -= QDocumentPrivate::m_leftMargin;
		
		while ( idx < m_text.length() )
		{
			QFontMetrics fm(QDocumentPrivate::m_fonts.at(composited[idx]));
			
			if ( m_text.at(idx) == '\t' )
			{
				int taboffset = tabStop - (column % tabStop);
				
				column += taboffset;
				cwidth = fm.width(' ') * taboffset;
			} else {
				++column;
				cwidth = fm.width(m_text.at(idx));
			}
			
			int mid = (x + (cwidth / 2) + 1);
			
			if ( screenx <= mid )
				return idx;
			else if ( screenx <= (x + cwidth) )
				return idx + 1;
			
			x += cwidth;
			++idx;
		}
		
		return m_text.length();
	}
}

void QDocumentLineHandle::clearOverlays()
{
	m_overlays.clear();
	
	//setFlag(QDocumentLine::LayoutDirty, true);
	setFlag(QDocumentLine::FormatsApplied, false);
	//applyOverlays();
}

void QDocumentLineHandle::addOverlay(const QFormatRange& over)
{
	m_overlays << over;
	
	//setFlag(QDocumentLine::LayoutDirty, true);
	setFlag(QDocumentLine::FormatsApplied, false);
	//applyOverlays();
}

void QDocumentLineHandle::removeOverlay(const QFormatRange& over)
{
	int i = m_overlays.removeAll(over);
	
	//setFlag(QDocumentLine::LayoutDirty, true);
	setFlag(QDocumentLine::FormatsApplied, false);
	//applyOverlays();
}

void QDocumentLineHandle::setFormats(const QVector<quint8>& fmts)
{
	m_formats = fmts;
	
	while ( m_formats.count() > m_text.length() )
		m_formats.pop_back();
	
	while ( m_formats.count() < m_text.length() )
		m_formats.append(0);
	
	//setFlag(QDocumentLine::LayoutDirty, true);
	setFlag(QDocumentLine::FormatsApplied, false);
	//applyOverlays();
}

QMediumArray QDocumentLineHandle::compose() const
{
	//QMediumArray m_composited(m_text.length());
	if ( hasFlag(QDocumentLine::FormatsApplied) )
		return m_cache;
	
	m_cache.resize(m_text.length());
	
	for ( int i = 0; i < qMin(m_formats.count(), m_text.length()); ++i )
		m_cache[i] = m_formats.at(i);
	
	for ( int i = m_formats.count(); i < m_text.length(); ++i )
		m_cache[i] = 0;
	
	// compositing formats and overlays
	foreach ( QFormatRange r, m_overlays )
	{
		r.offset = qMax(quint8(0), r.offset);
		int end = qMin(r.offset + r.length, m_cache.count());
		
		for ( int i = r.offset; i < end; ++i )
		{
			m_cache[i] = r.format;
		}
	}
	
	setFlag(QDocumentLine::FormatsApplied, true);
	
	return m_cache;
}

QList<QTextLayout::FormatRange> QDocumentLineHandle::decorations() const
{
	QMediumArray m_composited = compose();
	
	// turning format "map" into ranges that QTextLayout can understand...
	QList<QTextLayout::FormatRange> m_ranges;
	
	int fid = 0;
	QTextLayout::FormatRange r;
	r.start = r.length = -1;
	
	int i = 0;
	while ( i < m_composited.count() )
	{
		while ( (i < m_composited.count()) && !m_composited[i] )
			++i;
		
		if ( i >= m_composited.count() )
			break;
		
		fid = m_composited[i];
		
		r.start = i;
		r.format = QDocument::formatFactory()->format(fid).toTextCharFormat();
		
		while ( (i < m_composited.count()) && (m_composited[i] == fid) )
			++i;
		
		if ( i >= m_composited.count() )
			break;
		
		r.length = i - r.start;
		m_ranges << r;
		
		r.start = r.length = -1;
	}
	
	if ( r.start != -1 )
	{
		r.length = m_composited.count() - r.start;
		m_ranges << r;
	}
	
	return m_ranges;
}

void QDocumentLineHandle::applyOverlays() const
{
	if ( !m_layout )
		return;
	
	m_layout->setAdditionalFormats(decorations());
	
	//setFlag(QDocumentLine::FormatsApplied, true);
}

/*
	Algorithm borrowed from KDE libs 4 :
	KateRenderer::layoutLine(KateLineLayoutPtr lineLayout, int maxwidth, bool cacheLayout) const
	
	apart from the indenting and names little has been changed
*/
void QDocumentLineHandle::layout() const
{
	bool needLayout = false;
	static QList<QChar::Direction> m_layoutRequirements = QList<QChar::Direction>()
		<< QChar::DirR
		<< QChar::DirAL
		<< QChar::DirRLE
		<< QChar::DirRLO
		<< QChar::DirPDF
		<< QChar::DirAN;
	
	for ( int i = 0; (i < m_text.length()) && !needLayout; ++i )
	{
		QChar c = m_text.at(i);
		
		needLayout = m_layoutRequirements.contains(c.direction());
	}
	
	if ( needLayout )
	{
		//qDebug("layout needed at line %i", this->line());
		
		if ( !m_layout )
		{
			m_layout = new QTextLayout(m_text, QDocument::font());
		} else {
			m_layout->setText(m_text);
			//m_layout->setFont(config()->font());
		}
		
		m_layout->setCacheEnabled(false); //true); //cacheLayout);
		
		// Initial setup of the QTextLayout.
		
		applyOverlays();
		
		// Tab width
		QTextOption opt;
		opt.setFlags(QTextOption::IncludeTrailingSpaces);
		opt.setTabStop(QDocument::tabStop() * QDocument::fontMetrics().width(' '));
		
		// <CHANGE author=fullmetalcoder>
		opt.setWrapMode(QTextOption::NoWrap);
		//opt.setWrapMode(QTextOption::WrapAnywhere);//QTextOption::WrapAtWordBoundaryOrAnywhere);
		// </CHANGE>
		
		// Find the first strong character in the string.
		// If it is an RTL character, set the base layout direction of the string to RTL.
		//
		// See http://www.unicode.org/reports/tr9/#The_Paragraph_Level (Sections P2 & P3).
		// Qt's text renderer ("scribe") version 4.2 assumes a "higher-level protocol"
		// (such as KatePart) will specify the paragraph level, so it does not apply P2 & P3
		// by itself. If this ever change in Qt, the next code block could be removed.
		if ( m_text.isRightToLeft() )
			opt.setTextDirection(Qt::RightToLeft);
		
		m_layout->setTextOption(opt);
		
		// Syntax highlighting, inbuilt and arbitrary
		//m_layout->setAdditionalFormats(m_ranges);
		
		// Begin layouting
		m_layout->beginLayout();
		
		int height = 0;
		
		forever
		{
			QTextLine line = m_layout->createLine();
			
			if ( !line.isValid() )
			{
				if ( m_layout->lineCount() > 1 )
					qWarning("Troubles expected on line %i", this->line());
				
				break;
			}
			
			line.setPosition(QPoint(0, height));
			
			height += QDocument::fontMetrics().height();
		}
		
		m_layout->endLayout();
	} else {
		if ( m_layout )
			delete m_layout;
		
		m_layout = 0;
	}
	
	setFlag(QDocumentLine::LayoutDirty, false);
	
	if ( m_doc && m_doc->impl() )
	{
		if ( needLayout )
			m_doc->impl()->checkWidth(
										const_cast<QDocumentLineHandle*>(this),
										qRound(m_layout->boundingRect().width())
									);
		else
			m_doc->impl()->checkWidth(
										const_cast<QDocumentLineHandle*>(this),
										m_text //cursorToX(m_text.length())
									);
	}
}

void QDocumentLineHandle::draw(	QPainter *p,
								int xOffset,
								int vWidth,
								const QSmallArray& sel,
								const QSmallArray& cursor,
								const QPalette& pal,
								bool fullSel) const
{
	if ( hasFlag(QDocumentLine::LayoutDirty) )
		layout();
	
	QMediumArray m_composited = compose();
	
	if ( m_layout )
	{
		if ( !hasFlag(QDocumentLine::FormatsApplied) )
			applyOverlays();
		
		const int lh = QDocument::fontMetrics().height();
		
		QVector<QTextLayout::FormatRange> selections;
		
		QTextCharFormat fmt;
		fmt.setBackground(pal.highlight());
		fmt.setForeground(pal.highlightedText());
		
		QTextLayout::FormatRange range;
		
		for ( int i = 0; i < sel.count(); ++i )
		{
			range.start = sel[i];
			range.format = fmt;
			
			if ( (i + 1) < sel.count() )
			{
				// regular selection subset
				range.length = sel[++i] - range.start;
				
			} else {
				// span to end of line, not only text
				range.length = m_text.length() - range.start;
				
				const int endX = qRound(m_layout->lineAt(0).naturalTextWidth()) - xOffset;
				
				QRect area(endX, lh * i, vWidth - endX, lh);
				
				p->fillRect(area, fmt.background());
			}
			
			selections << range;
		}
		
		m_layout->draw(p, QPoint(QDocumentPrivate::m_leftMargin - xOffset, 0), selections);
		
		const QPoint off(QDocumentPrivate::m_leftMargin - xOffset, 0);
		
		for ( int i = 0; i < cursor.count(); ++i )
			m_layout->drawCursor(p, off, cursor[i]);
		
		//m_layout->clearAdditionalFormats();
	} else if ( m_text.isEmpty() ) {
		// enforce selection drawing on empty lines
		if ( sel.count() == 1 )
			p->fillRect(
						qMax(xOffset, QDocumentPrivate::m_leftMargin),
						0,
						vWidth,
						QDocumentPrivate::m_lineSpacing,
						pal.highlight()
						);
		
		// enforce cursor drawing on empty lines
		if ( cursor.count() && (xOffset < QDocumentPrivate::m_leftMargin) )
			p->drawLine(
						QDocumentPrivate::m_leftMargin,
						0,
						QDocumentPrivate::m_leftMargin,
						QDocumentPrivate::m_lineSpacing
						);
		
	} else /*if ( !QDocumentPrivate::m_fixedPitch )*/ {
		QChar c;
		
		bool leading = true,
			showTabs = QDocument::showSpaces() & QDocument::ShowTabs,
			showLeading = QDocument::showSpaces() & QDocument::ShowLeading,
			showTrailing = QDocument::showSpaces() & QDocument::ShowTrailing;
		
		int cwidth,
			idx = 0, column = 0,
			selidx = 0, sellen = 0,
			xpos = QDocumentPrivate::m_leftMargin;
		
		const int ts = QDocument::tabStop();
		
		// find start of trailing whitespaces
		int last = m_text.length();
		
		while ( (last > 0) && m_text.at(last - 1).isSpace() )
			--last;
		
		//p->save();
		
		while ( idx < m_text.length() )
		{
			if ( (selidx < sel.count()) && (idx == sel[selidx]) )
			{
				if ( (selidx + 1) < sel.count() )
				{
					sellen = sel[selidx + 1] - sel[selidx];
					selidx += 2;
				} else {
					// unbounded selection
					
					++selidx;
					sellen = m_text.length() - idx;
					
					//p->fillRect(
					//			xpos, 0,
					//			xOffset + vWidth - xpos, QDocumentPrivate::m_lineHeight,
					//			pal.highlight()
					//		);
					
					p->fillRect(
								xpos, 0,
								xOffset + vWidth - xpos, QDocumentPrivate::m_lineSpacing,
								pal.highlight()
							);
				}
			}
			
			c = m_text.at(idx);
			
			if ( idx < m_composited.count() )
			{
				m_doc->impl()->tunePainter(p, m_composited[idx]);
				
				// make sure bold/italicized/.. chars are taken into account...
				cwidth = p->fontMetrics().width((c == '\t') ? ' ' : c);
			} else {
				// char uses default font...
				cwidth = QDocumentPrivate::m_fontMetrics->width((c == '\t') ? ' ' : c);
			}
			
			if ( c == '\t' )
			{
				int toff = ts - (column % ts);
				cwidth = toff * cwidth;
				column += toff - 1;
			}
			
			// MUST be done after setting the proper chararcter width!
			if ( sellen )
			{
				//p->fillRect(
				//			xpos, 0,
				//			cwidth, QDocumentPrivate::m_lineHeight,
				//			pal.highlight()
				//		);
				
				p->fillRect(
							xpos, 0,
							cwidth, QDocumentPrivate::m_lineSpacing,
							pal.highlight()
						);
				
			}
			
			if ( !c.isSpace() )
			{
				leading = false;
				
				if ( fullSel || sellen )
					p->setPen(pal.highlightedText().color());
				
				p->drawText(xpos, QDocumentPrivate::m_ascent, QString(c));
			} else if (
					(
						(c == ' ')
					&&
						(
							(
								leading
							&&
								showLeading
							)
						||
							(
								(idx > last)
							&&
								showTrailing
							)
						)
					)
				||
					(
						(c == '\t')
					&&
						showTabs
					)
				)
			{
				p->translate(xpos - m_spaceSignOffset, QDocumentPrivate::m_ascent);
				p->drawPoints(m_spaceSign, sizeof(m_spaceSign) / sizeof(QPoint));
				p->translate(m_spaceSignOffset - xpos,-QDocumentPrivate::m_ascent);
			}
			
			for ( int cidx = 0; cidx < cursor.count(); ++cidx )
			{
				if ( cursor[cidx] == idx )
					p->drawLine(xpos, 0, xpos, QDocumentPrivate::m_lineSpacing);
					//p->drawLine(xpos, 1, xpos, QDocumentPrivate::m_lineHeight - 1);
			}
			
			if ( sellen )
				--sellen;
			
			xpos += cwidth;
			++column;
			++idx;
		}
		
		// ensure drawing of cursor at EOL
		for ( int cidx = 0; cidx < cursor.count(); ++cidx )
		{
			if ( cursor[cidx] == idx )
				p->drawLine(xpos, 0, xpos, QDocumentPrivate::m_lineSpacing);
				//p->drawLine(xpos, 1, xpos, QDocumentPrivate::m_lineHeight - 1);
		}
		
		//p->restore();
	}/* else {
		int selidx = 0, cidx = 0;
		
		// draw selection backgrounds
		while ( selidx < sel.count() )
		{
			int selstart = sel[selidx];
			int x = cursorToX(selstart);
			
			if ( (selidx + 1) < sel.count() )
			{
				// bounded selection
				int selend = sel[++selidx];
				
				int w = cursorToX(selend) - x;
				
				//p->fillRect(x, 0, w, QDocumentPrivate::m_lineHeight,
				//			pal.highlight());
				
				p->fillRect(x, 0, w, QDocumentPrivate::m_lineSpacing,
							pal.highlight());
				
			} else {
				// "unbounded" selection : up to block end
				//p->fillRect(x, 0, xOffset + vWidth - x, QDocumentPrivate::m_lineHeight,
				//			pal.highlight());
				
				p->fillRect(x, 0, xOffset + vWidth - x, QDocumentPrivate::m_lineSpacing,
							pal.highlight());
				
			}
			
			++selidx;
		}
		
		selidx = 0;
		p->translate(0, QDocumentPrivate::m_ascent);
		
		// draw formated text
		if ( m_composited.count() == m_text.length() )
		{
			QChar c;
			int fid, column = 0, idx = 0, xpos = QDocumentPrivate::m_leftMargin;
			const int	ts = m_doc->tabStop(),
						space = m_doc->impl()->m_spaceWidth;
			
			// leading spaces
			while ( (idx < m_text.length()) && m_text.at(idx).isSpace() )
			{
				if ( QDocument::showSpaces() & QDocument::ShowLeading )
				{
					p->translate(xpos - m_spaceSignOffset, 0);
					p->drawPoints(m_spaceSign, sizeof(m_spaceSign) / sizeof(QPoint));
					p->translate(m_spaceSignOffset - xpos, 0);
				}
				
				if ( m_text.at(idx) == QLatin1Char('\t') )
				{
					int toff =  ts - (column % ts);
					
					column += toff;
					xpos += space * toff;
				} else {
					++column;
					xpos += space;
				}
				
				++idx;
			}
			
			// find start of trailing whitespaces
			int last = m_text.length();
			
			while ( (last > idx) && m_text.at(last - 1).isSpace() )
			{
				--last;
			}
			
			p->save();
			
			QSmallArray tabstops;
			
			// text (until first trailing whitespace
			while ( idx < last ) //m_text.length() )
			{
				QString buffer;
				//xpos = column * space + QDocumentPrivate::m_leftMargin;
				
				fid = m_composited[idx];
				
				m_doc->impl()->tunePainter(p, fid);
				
				bool inSel = (selidx < sel.count()) && (idx >= sel[selidx]) &&
							(((selidx + 1) < sel.count()) ? (idx < sel[selidx+1]) : true);
				
				if ( inSel )
				{
					p->setPen(pal.highlightedText().color());
				}
				
				int xlen = 0;
				bool stillInSel;
				
				do
				{
					c = m_text.at(idx);
					
					++idx;
					
					if ( c == QLatin1Char('\t') )
					{
						if ( QDocument::showSpaces() & QDocument::ShowTabs )
						{
							// defered because drawing spaces erases fg
							tabstops.append(xpos + xlen - m_spaceSignOffset);
						}
						
						int toff = ts - (column % ts);
						buffer += QString(toff, ' ');
						column += toff;
						
						xlen += toff * space;
					} else {
						++column;
						xlen += space;
						
						buffer += c;
					}
					
					stillInSel = (selidx < sel.count()) && (idx >= sel[selidx]) &&
							(((selidx + 1) < sel.count()) ? (idx < sel[selidx+1]) : true);
					
				} while (
							((idx) < last) //m_text.length())
						&&
							(m_composited[idx] == fid)
						&&
							(stillInSel == inSel)
						);
				
				//++frag;
				
				//if ( !safe )
				//	xlen = QDocument::fontMetrics().width(buffer);
				
				p->drawText(xpos, 0, buffer);
				xpos += xlen;
			}
			
			p->restore();
			
			// embedded tabs
			for ( int ts = 0; ts < tabstops.count(); ++ts )
			{
				p->translate( tabstops[ts], 0);
				p->drawPoints(m_spaceSign, sizeof(m_spaceSign) / sizeof(QPoint));
				p->translate(-tabstops[ts], 0);
			}
			
			// trailing whitespaces
			if ( QDocument::showSpaces() & QDocument::ShowTrailing )
			{
				while ( idx < m_text.length() )
				{
					p->translate(xpos - m_spaceSignOffset, 0);
					p->drawPoints(m_spaceSign, sizeof(m_spaceSign) / sizeof(QPoint));
					p->translate(m_spaceSignOffset - xpos, 0);
					
					if ( m_text.at(idx) == QLatin1Char('\t') )
					{
						int toff = (ts - (column % ts));
						
						xpos += space * toff;
						column += toff;
					} else {
						xpos += space;
						++column;
					}
					
					++idx;
				}
			}
			
			p->translate(0, -QDocumentPrivate::m_ascent);
			
			while ( cidx < cursor.count() )
			{
				int x = cursorToX(cursor[cidx]);
				
				//p->drawLine(x, 1, x, QDocumentPrivate::m_lineHeight - 1);
				p->drawLine(x, 0, x, QDocumentPrivate::m_lineSpacing);
				
				++cidx;
			}
			
			//qDebug("drawing line %i : %i fragments", line(), frag);
			//qDebug("drawing line in %i ms", t.elapsed());
			
		} else {
			//qDebug("drawing fallback!");
			p->drawText(
					0,
					0,
					QDocument::screenable(
						m_text.constData(),
						m_text.length(),
						QDocument::tabStop()
					)
				);
			
			while ( cidx < cursor.count() )
			{
				int x = cursorToX(cursor[cidx]);
				
				//p->drawLine(x, 1, x, QDocumentPrivate::m_lineHeight - 1);
				p->drawLine(x, 0, x, QDocumentPrivate::m_lineSpacing);
				
				++cidx;
			}
		}
	}*/
}

//////////////////


/////////////////////////
//	QDocumentCursorHandle
/////////////////////////
QDocumentCursorHandle::QDocumentCursorHandle(QDocument *d, int line)
 :	m_silent(false), m_doc(d),
	#if QT_VERSION >= 0x040400
	m_ref(0),
	#endif
	m_begOffset(0), m_endOffset(0), m_max(0), m_begLine(line), m_endLine(-1)
{
	#if QT_VERSION < 0x040400
	m_ref.init(0);
	#endif
	
	//m_blocks.push(0);
	//qDebug("Cursor handle created : 0x%x", this);
}

QDocumentCursorHandle::~QDocumentCursorHandle()
{
	//qDebug("Cursor handle deleted : 0x%x", this);
	Q_ASSERT(!m_ref);
	
	QDocumentCommand::disableAutoUpdate(this);
}

void QDocumentCursorHandle::copy(const QDocumentCursorHandle *c)
{
	if ( !c )
		return;
	
	m_begLine = c->m_begLine;
	m_begOffset = c->m_begOffset;
	m_endLine = c->m_endLine;
	m_endOffset = c->m_endOffset;
	m_silent = c->m_silent;
	m_max = c->m_max;
}

QDocument* QDocumentCursorHandle::document() const
{
	return m_doc;
}

QDocumentCursorHandle* QDocumentCursorHandle::clone() const
{
	QDocumentCursorHandle *c = new QDocumentCursorHandle(m_doc);
	c->copy(this);
	
	return c;
}

bool QDocumentCursorHandle::atEnd() const
{
	if ( !m_doc )
		return true;
	
	bool atLineEnd;
	QDocumentLine l = m_doc->line(m_begLine);
	
	//qDebug("Cursor handle : 0x%x->atEnd() => 0x%x", this, m_begLine.handle());
	
	if ( l.isValid() )
	{
		atLineEnd = m_begOffset == l.length();
		l = m_doc->line(m_begLine + 1);
	} else {
		//qWarning("Invalid cursor...");
		return true;
	}
	
	return l.isNull() && atLineEnd;
}

bool QDocumentCursorHandle::atStart() const
{
	if ( !m_doc )
		return true;
	
	QDocumentLine l = m_doc->line(m_begLine - 1);
	
	return l.isNull() && !m_begOffset;
}

bool QDocumentCursorHandle::atBlockEnd() const
{
	return atLineEnd();
}

bool QDocumentCursorHandle::atBlockStart() const
{
	return atLineStart();
}

bool QDocumentCursorHandle::atLineEnd() const
{
	if ( !m_doc )
		return true;
	
	QDocumentLine l = m_doc->line(m_begLine);
	
	return l.isValid() ? l.length() == m_begOffset : false;
}

bool QDocumentCursorHandle::atLineStart() const
{
	if ( !m_doc )
		return true;
	
	QDocumentLine l = m_doc->line(m_begLine);
	
	return l.isValid() ? !m_begOffset : false;
}

bool QDocumentCursorHandle::hasSelection() const
{
	if ( !m_doc )
		return false;
	
	QDocumentLine l1 = m_doc->line(m_begLine), l2 = m_doc->line(m_endLine);
	
	return l1.isValid() && l2.isValid();
}

bool QDocumentCursorHandle::isSilent() const
{
	return m_silent;
}

void QDocumentCursorHandle::setSilent(bool y)
{
	m_silent = y;
}

bool QDocumentCursorHandle::isAutoUpdated() const
{
	return QDocumentCommand::isAutoUpdated(this);
}

void QDocumentCursorHandle::setAutoUpdated(bool y)
{
	if ( y )
		QDocumentCommand::enableAutoUpdate(this);
	else
		QDocumentCommand::disableAutoUpdate(this);
}

QDocumentLine QDocumentCursorHandle::line() const
{
	if ( !m_doc )
		return QDocumentLine();
	
	return m_doc->line(m_begLine);
}

int QDocumentCursorHandle::lineNumber() const
{
	return m_begLine;
}

int QDocumentCursorHandle::anchorColumn() const
{
	if ( !m_doc )
		return -1;
	
	return m_doc->line(m_endLine).isValid() ? m_endOffset : m_begOffset;
}

int QDocumentCursorHandle::visualColumn() const
{
	return QDocument::screenLength(
						line().text().constData(),
						m_begOffset,
						QDocument::tabStop()
					);
	
}

int QDocumentCursorHandle::columnNumber() const
{
	return m_begOffset;
}

void QDocumentCursorHandle::setColumnNumber(int c, QDocumentCursor::MoveMode m)
{
	if ( !m_doc )
		return;
	
	QDocumentLine l1 = m_doc->line(m_begLine), l2 = m_doc->line(m_endLine);
	
	if ( m == QDocumentCursor::MoveAnchor )
	{
		m_endLine = -1;
		m_endOffset = 0;
		m_begOffset = qBound(0, c, l1.length());
	} else {
		if ( l2.isNull() )
		{
			m_endLine = m_begLine;
			m_endOffset = m_begOffset;
		}
		
		m_begOffset = qBound(0, c, l1.length());
	}
}

int QDocumentCursorHandle::position() const
{
	if ( !m_doc )
		return -1;
	
	int pos = m_doc->line(m_begLine).position();
	
	return (pos != -1) ? pos + m_begOffset : pos;
}

void QDocumentCursorHandle::shift(int offset)
{
	if ( !m_doc )
		return;
	
	QDocumentLine l1 = m_doc->line(m_begLine), l2 = m_doc->line(m_endLine);
	
	if ( l1.isValid() )
		m_begOffset = qBound(0, m_begOffset + offset, l1.length());
	
	if ( l2.isValid() )
		m_endOffset = qBound(0, m_endOffset + offset, l2.length());
}

void QDocumentCursorHandle::setPosition(int pos, QDocumentCursor::MoveMode m)
{
	qWarning("Set position to cursor using character index : forbidden...");
	/*
	if ( m == QDocumentCursor::MoveAnchor )
	{
		m_begLine = m_doc->findLine(pos);
		m_begOffset = (m_begLine.isValid() ? pos : 0);
		
		m_endLine = QDocumentLine();
		m_endOffset = 0;
		
		m_max = m_begLine.cursorToX(m_begOffset);
	} else {
		m_endLine = m_doc->findLine(pos);
		m_endOffset = (m_begLine.isValid() ? pos : 0);
		
		//m_max = 0;
	}
	*/
}

bool QDocumentCursorHandle::movePosition(int count, QDocumentCursor::MoveOperation op,
										QDocumentCursor::MoveMode m)
{
	if ( !m_doc )
		return false;
	
	QDocumentLine l, l1 = m_doc->line(m_begLine), l2 = m_doc->line(m_endLine);
	
	int &line = m_begLine;
	int &offset = m_begOffset;
	static QRegExp wordStart("\\b\\w+$"), wordEnd("^\\w+\\b");
	
	if ( m == QDocumentCursor::MoveAnchor )
	{
		m_endLine = -1;
		m_endOffset = 0;
	} else if ( !l2.isValid() ) {
		m_endLine = m_begLine;
		m_endOffset = m_begOffset;
	}
	
	int beg = 0, end = m_doc->lines();
	
	switch ( op )
	{
		case QDocumentCursor::Left :
		{
			if ( atStart() )
				return false;
			
			int remaining = offset;
			
			do
			{
				if ( remaining >= count )
				{
					offset = remaining - count;
					break;
				} else if ( line == beg ) {
					offset = 0;
					break;
				}
				
				do
				{
					--line;
				} while ( (line != beg) && m_doc->line(line).hasFlag(QDocumentLine::Hidden) );
				
				//*line = *it;
				
				count -= remaining + 1; // jumping a line is one char
				offset = remaining = m_doc->line(line).length();
			} while ( count && remaining );
			
			m_max = m_doc->line(line).cursorToX(offset);
			
			break;
		}
			
		case QDocumentCursor::Right :
		{
			if ( atEnd() )
				return false;
			
			int remaining = m_doc->line(line).length() - offset;
			
			do
			{
				if ( remaining >= count )
				{
					offset += count;
					break;
				} else if ( (line + 1) == end ) {
					offset = remaining;
					break;
				}
				
				do
				{
					++line;
				} while ( ((line+1) != end) && m_doc->line(line).hasFlag(QDocumentLine::Hidden) );
				
				//*line = *it;
				
				offset = 0;
				count -= remaining + 1;
				remaining = m_doc->line(line).length();
			} while ( count && remaining );
			
			m_max = m_doc->line(line).cursorToX(offset);
			
			break;
		}
			
		case QDocumentCursor::Up :
			
			if ( atStart() )
				return false;
			
			while ( count && (line != beg) )
			{
				--line;
				
				if ( !m_doc->line(line).hasFlag(QDocumentLine::Hidden) )
					--count;
				
			}
			
			//*line = QDocumentLine(*it);
			//*offset = line->xToCursor(qMin(line->cursorToX(*offset), m_max), 0);
			l = m_doc->line(line);
			
			if ( count )
				offset = 0;
			else if ( m == QDocumentCursor::MoveAnchor )
				offset = l.xToCursor(
						qMax(
							l.cursorToX(
								qMin(
									offset,
									l.length()
								)
							),
							m_max
						),
						0
					);
			else
				offset = qMin(l.length(), offset);
			
			break;
			
		case QDocumentCursor::Down :
			
			if ( atEnd() )
				return false;
			
			while ( count && ((line + 1) != end) )
			{
				++line;
				
				if ( !m_doc->line(line).hasFlag(QDocumentLine::Hidden) )
					--count;
				
			}
			
			//*line = QDocumentLine(*it);
			l = m_doc->line(line);
			
			if ( count )
				offset = l.length();
			else if ( m == QDocumentCursor::MoveAnchor )
				offset = l.xToCursor(
						qMax(
							l.cursorToX(
								qMin(
									offset,
									l.length()
								)
							),
							m_max
						),
						0
					);
			else
				offset = qMin(l.length(), offset);
			
			break;
			
		case QDocumentCursor::Start :
			if ( atStart() )
				return false;
			
			m_max = offset = 0;
			line = 0; //m_doc->line(0);
			break;
			
		case QDocumentCursor::End :
			if ( atEnd() )
				return false;
			
			line = end - 1; //QDocumentLine(*(m_doc->impl()->end() - 1));
			m_max = offset = m_doc->line(line).length();
			break;
			
		case QDocumentCursor::StartOfBlock :
			if ( atBlockStart() )
				return false;
			
			m_max = offset = 0;
			break;
			
		case QDocumentCursor::EndOfBlock :
			if ( atBlockEnd() )
				return false;
			
			m_max = offset = m_doc->line(line).length();
			break;
			
		case QDocumentCursor::NextBlock :
			
			if ( atEnd() )
				return false;
			
			while ( ((line + 1) != end) && count )
			{
				++line;
				
				if ( !m_doc->line(line).hasFlag(QDocumentLine::Hidden) )
					--count;
				
			}
			
			if ( !count )
			{
				//*line = *it;
				offset = 0;
			} else {
				//*line = QDocumentLine(*(m_doc->impl()->end() - 1));
				offset = m_doc->line(line).length();
			}
			
			break;
			
		case QDocumentCursor::PreviousBlock :
			
			if ( atStart() )
				return false;
			
			offset = 0;
			
			while ( (line != beg) && count )
			{
				--line;
				
				if ( !m_doc->line(line).hasFlag(QDocumentLine::Hidden) )
					--count;
				
			}
			
			if ( !count && (line != beg) )
			{
				//*line = *it;
				offset = m_doc->line(line).length();
			} else {
				offset = 0;
				//*line = QDocumentLine(*(m_doc->impl()->begin()));
			}
			
			//*line = *it;
			
			break;
			
		case QDocumentCursor::WordLeft :
		case QDocumentCursor::PreviousWord :
		{
			if ( atStart() )
				return false;
			
			l = m_doc->line(line);
			
			//for ( int loop = 0; loop <= 1; ++loop )
			//{
			//	while ( l.isValid() )
			
			// -- patch --
			/* eats up white space */
			while ( (offset > 0) && !isWord(l.text().at(offset - 1)) )
				--offset;
			
			/* start of line */
			if ( offset == 0 )
			{
				/* first line, first char => nothing to do */
				if( line == beg )
					return true;
				
				do
			// -- patch --
				{
// 					//offset = qMin(offset, l.length() - 1);
// 					bool next = (l.length() && offset >= 0) ? isWord(l.text().at(offset)) : true;
// 					
// 					if ( loop )
// 						next = !next;
// 					
// 					if ( !next )
// 						break;
// 					
// 					if ( offset > 0 )
// 					{
// 						--offset;
// 					} else if ( line != beg ) {
// 						do
// 						{
// 							//*line = *(--it);
// 							--line;
// 							l = m_doc->line(line);
// 							offset = l.length() - 1;
// 						} while ( l.isValid() && (line != beg) && l.hasFlag(QDocumentLine::Hidden) );
// 					} else {
// 						break;
// 					}
// 				}
// 			}
// 			
// 			while ( l.isValid() )
// 			{
// 				offset = qMin(offset, l.length());
// 				bool next = (offset <= 0) ? false : isWord(l.text().at(offset - 1));
// 				
// 				if ( !next )
// 					break;
// 				
// 				--offset;
			
			// -- patch --
					--line;
					l = m_doc->line(line);
					offset = l.length();
				} while ( (line != beg) && l.isValid() && l.hasFlag(QDocumentLine::Hidden) );
				return true;
			// -- patch --
			}
			
			// -- patch --
			/* eats up whole word */
			while ( (offset > 0) && isWord(l.text().at(offset - 1)) )
				--offset;
			// -- patch --
			
			break;
		}
			
		case QDocumentCursor::WordRight :
		case QDocumentCursor::NextWord :
		{
			if ( atEnd() )
				return false;
			
			l = m_doc->line(line);
			int lineLength = l.text().length();
			
// 			for ( int loop = 0; loop <= 1; ++loop )
			// -- patch --
			/* end of line */
			if ( offset == lineLength )
 			{
// 				while ( l.isValid() )
				/* last line, last char => nothing to do */
				if ( line == end )
					return true;
			// -- patch --
				do
				{
// 					//offset = qBound(0, offset, l.length() - 1);
// 					bool next = (offset < l.length()) ? isWord(l.text().at(offset)) : true;
// 					
// 					if ( loop )
// 						next = !next;
// 					
// 					if ( !next )
// 						break;
// 					
// 					if ( offset < l.length() )
// 					{
// 						++offset;
// 					} else if ( (line + 1) != end ) {
// 						offset = 0;
// 						do
// 						{
// 							++line;
// 							l = m_doc->line(line);
// 						} while ( l.isValid() && ((line + 1) != end) && (l.hasFlag(QDocumentLine::Hidden) || !l.length()) );
// 					} else {
			// -- patch --
					++line;
					l = m_doc->line(line);
					offset = 0;
				} while ( (line != end) && l.isValid() && l.hasFlag(QDocumentLine::Hidden) );
				
				lineLength = l.text().length();
				/* empty line */
				if ( lineLength == 0 )
					return true;
				
				/* eats up white space */
				while ( !isWord(l.text().at(offset)) )
				{
					++offset;
					/* move to end of line */
					if ( offset == lineLength )
						break;
			// -- patch --
//					}
				}
			// -- patch --
				return true;
			// -- patch --
			}
			
			// -- patch --
			/* next char */
			++offset;
			/* eats up whole word */
			while ( (offset < lineLength) && isWord(l.text().at(offset)) )
				++offset;
			
			/* eats up white space */
			while ( (offset < lineLength) && !isWord(l.text().at(offset)) )
				++offset;
			// -- patch --
			
			break;
		}
			
		case QDocumentCursor::StartOfWord :
		{
			int x = wordStart.indexIn(m_doc->line(line).text().left(offset));
			
			if ( x != -1 )
			{
				offset = x;
			} else {
				qDebug("failed to find SOW : %i + %i != %i",
						x, wordStart.matchedLength(), offset);
				
				return false;
			}
			
			break;
		}
			
		case QDocumentCursor::EndOfWord :
		{
			int x = wordEnd.indexIn(m_doc->line(line).text(), offset, QRegExp::CaretAtOffset);
			
			if ( x == offset )
			{
				offset += wordEnd.matchedLength();
			} else {
				qDebug("failed to find EOW");
				return false;
			}
			
			break;
		}
			
		default:
			qWarning("Unhandled move operation...");
			return false;
	};
	
	return true;
}

void QDocumentCursorHandle::moveTo(const QDocumentCursor &c)
{
	if ( !c.isValid() || !m_doc )
		return;
	
	m_begLine = c.handle()->m_begLine;
	m_begOffset = c.handle()->m_begOffset;
	
	m_endLine = -1;
	m_endOffset = 0;
	
	m_max = m_doc->line(m_begLine).cursorToX(m_begOffset);
}

void QDocumentCursorHandle::moveTo(int line, int column)
{
	m_begLine = line;
	m_begOffset = column;
	
	m_endLine = -1;
	m_endOffset = 0;
	
	m_max = m_doc->line(m_begLine).cursorToX(m_begOffset);
}

void QDocumentCursorHandle::insertText(const QString& s)
{
	if ( !m_doc || s.isEmpty() || m_doc->line(m_begLine).isNull() )
		return;
	
	QDocumentCommand *command = new QDocumentInsertCommand(
										m_begLine,
										m_begOffset,
										s,
										m_doc
									);
	
	command->setTargetCursor(this);
	execute(command);
}

void QDocumentCursorHandle::deleteChar()
{
	if ( !m_doc )
		return;
	
	QDocumentLine l = m_doc->line(m_begLine);
	
	if ( l.isNull() || atEnd() )
		return;
	
	QDocumentCommand *command = 0;
	
	if ( !atLineEnd() )
	{
		command = new QDocumentEraseCommand(
											m_begLine,
											m_begOffset,
											m_begLine,
											m_begOffset + 1,
											m_doc
										);
		
	} else {
		// merge two blocks...
		command = new QDocumentEraseCommand(
											m_begLine,
											m_begOffset,
											m_begLine + 1,
											0,
											m_doc
										);
		
	}
	
	command->setTargetCursor(this);
	command->setUndoOffset(-1);
	execute(command);
}

void QDocumentCursorHandle::deletePreviousChar()
{
	if ( !m_doc )
		return;
	
	QDocumentLine l = m_doc->line(m_begLine);
	
	if ( l.isNull() || atStart() )
		return;
	
	QDocumentCommand *command = 0;
	
	if ( !atLineStart() )
	{
		command = new QDocumentEraseCommand(
											m_begLine,
											m_begOffset - 1,
											m_begLine,
											m_begOffset,
											m_doc
										);
		
	} else {
		// merge two blocks...
		QDocumentLine prev = m_doc->line(m_begLine - 1);
		
		command = new QDocumentEraseCommand(
											m_begLine - 1,
											prev.length(),
											m_begLine,
											m_begOffset,
											m_doc
										);
		
	}
	
	command->setTargetCursor(this);
	execute(command);
}

void QDocumentCursorHandle::execute(QDocumentCommand *c)
{
	if ( !m_doc )
		return;
	
	if ( isSilent() && !c->isSilent() )
		c->setSilent(isSilent());
	
	if ( m_blocks.count() )
	{
		c->redo();
		m_blocks.top()->addCommand(c);
		
	} else if ( m_doc ) {
		//qDebug("Cursor handle executing command : 0x%x", this);
		
		m_doc->execute(c);
	}
}

void QDocumentCursorHandle::beginEditBlock()
{
	m_blocks.push(new QDocumentCommandBlock(m_doc));
}

void QDocumentCursorHandle::endEditBlock()
{
	if ( !m_doc || m_blocks.isEmpty() )
		return;
	
	//qDebug("Cursor handle executing command : 0x%x [block]", this);
	
	QDocumentCommandBlock *block = m_blocks.pop();
	
	// special trick to prevent double redo() while getting rid of
	// bugs occuring in when inserting/erasing in overlapping lines
	// inside a command block
	block->setWeakLock(true);
	
	m_doc->execute(block);
}

QDocumentCursor QDocumentCursorHandle::selectionStart() const
{
	if ( !m_doc )
		return QDocumentCursor();
	
	if ( !hasSelection() )
		return QDocumentCursor(clone());
	
	QDocumentCursor pos(m_doc, m_begLine, m_begOffset),
					anc(m_doc, m_endLine, m_endOffset);
	
	return (pos < anc) ? pos : anc;
}

QDocumentCursor QDocumentCursorHandle::selectionEnd() const
{
	if ( !m_doc )
		return QDocumentCursor();
	
	if ( !hasSelection() )
		return QDocumentCursor(clone());
	
	QDocumentCursor pos(m_doc, m_begLine, m_begOffset),
					anc(m_doc, m_endLine, m_endOffset);
	
	return (pos > anc) ? pos : anc;
}

bool QDocumentCursorHandle::eq(const QDocumentCursorHandle *h)
{
	return (m_begLine == h->m_begLine) && (m_begOffset == h->m_begOffset);
	/*
	if ( !hasSelection() )
		return (m_begLine == h->m_begLine) && (m_begOffset == h->m_begOffset);
	
	return
			(m_begLine == h->m_begLine)
		&&
			(m_begOffset == h->m_begOffset)
		&&
			(m_endLine == h->m_endLine)
		&&
			(m_endOffset == h->m_endOffset)
		;
	*/
}

bool QDocumentCursorHandle::lt(const QDocumentCursorHandle *h)
{
	return
				(m_begLine < h->m_begLine)
			||
				((m_begLine == h->m_begLine) && (m_begOffset < h->m_begOffset))
			;
}

bool QDocumentCursorHandle::gt(const QDocumentCursorHandle *h)
{
	return
				(m_begLine > h->m_begLine)
			||
				((m_begLine == h->m_begLine) && (m_begOffset > h->m_begOffset))
			;
}

QString QDocumentCursorHandle::selectedText() const
{
	if ( !m_doc )
		return QString();
	
	QDocumentLine l1 = m_doc->line(m_begLine), l2 = m_doc->line(m_endLine);
	
	if ( l1.isNull() || l2.isNull() )
		return QString();
	
	QString s;
	
	if ( m_begLine == m_endLine )
	{
		int min = qMin(m_begOffset, m_endOffset),
			max = qMax(m_begOffset, m_endOffset);
		
		s = l1.text().mid(min, max - min);
	} else if ( m_begLine < m_endLine ) {
		s = l1.text().mid(m_begOffset);
		
		int it = m_begLine;
		//QDocumentConstIterator it = m_doc->impl()->index(m_begLine.handle());
		
		while ( ++it < m_endLine )
		{
			s += "\n";
			s += m_doc->line(it).text();
		}
		
		s += "\n" + l2.text().left(m_endOffset);
		
	} else {
		s = l2.text().mid(m_endOffset);
		
		int it = m_endLine;
		//QDocumentConstIterator it = m_doc->impl()->index(m_endLine.handle());
		
		while ( ++it < m_begLine )
		{
			s += "\n";
			s += m_doc->line(it).text();
		}
		
		s += "\n" + l1.text().left(m_begOffset);
		
	}
	
	//qDebug("selected text : \"%s\"", qPrintable(s));
	return s;
}

void QDocumentCursorHandle::clearSelection()
{
	if ( m_doc && m_doc->line(m_endLine).isValid() )
	{
		m_begLine = m_endLine;
		m_begOffset = m_endOffset;
		
		m_endLine = -1;
		m_endOffset = -1;
	}
}

void QDocumentCursorHandle::select(QDocumentCursor::SelectionType t)
{
	if ( !m_doc || !m_doc->line(m_begLine).isValid() )
		return;
	
	if ( t == QDocumentCursor::LineUnderCursor )
	{
		movePosition(1, QDocumentCursor::StartOfLine, QDocumentCursor::MoveAnchor);
		movePosition(1, QDocumentCursor::EndOfLine, QDocumentCursor::KeepAnchor);
		
	} else if ( t == QDocumentCursor::WordUnderCursor ) {
		
		movePosition(1, QDocumentCursor::StartOfWord, QDocumentCursor::MoveAnchor);
		movePosition(1, QDocumentCursor::EndOfWord, QDocumentCursor::KeepAnchor);
	}
}

void QDocumentCursorHandle::setSelectionBoundary(const QDocumentCursor& c)
{
	if (
			!m_doc
		||
			(m_begLine == -1)
		||
			(
					(c.lineNumber() == m_begLine)
				&&
					(c.columnNumber() == m_begOffset)
			)
		)
		return;
	
	//qDebug("setting new selection boundary... ");
	
	if ( !hasSelection() )
	{
		m_endLine = m_begLine;
		m_endOffset = m_begOffset;
	}
	
	m_begLine = c.lineNumber();
	m_begOffset = c.columnNumber();
}

bool QDocumentCursorHandle::isWithinSelection(const QDocumentCursor& c)
{
	if ( !hasSelection() ) //|| c.hasSelection() )
		return false;
	
	int minOff, maxOff, min, max;
	
	if ( m_begLine > m_endLine )
	{
		max = m_begLine;
		maxOff = m_begOffset;
		
		min = m_endLine;
		minOff = m_endOffset;
	} else {
		min = m_begLine;
		minOff = m_begOffset;
		
		max = m_endLine;
		maxOff = m_endOffset;
	}
	
	return (m_begLine == m_endLine)
		?
			(
				(c.lineNumber() == m_begLine)
			&&
				(qMin(m_begOffset, m_endOffset) <= c.columnNumber())
			&&
				(qMax(m_begOffset, m_endOffset) >= c.columnNumber())
			)
		:
			(
				(
					(c.lineNumber() > min)
				&&
					(c.lineNumber() < max)
				)
			||
				(
					(c.lineNumber() == min)
				&&
					(minOff <= c.columnNumber())
				)
			||
				(
					(c.lineNumber() == max)
				&&
					(maxOff >= c.columnNumber())
				)
			)
		;
	
}

void QDocumentCursorHandle::removeSelectedText()
{
	if ( !m_doc )
		return;
	
	QDocumentLine l1 = m_doc->line(m_begLine), l2 = m_doc->line(m_endLine);
	
	if ( l1.isNull() || l2.isNull() )
		return;
	
	QDocumentCommand *c;
	
	if ( m_begLine < m_endLine )
	{
		c = new QDocumentEraseCommand(
										m_begLine,
										m_begOffset,
										m_endLine,
										m_endOffset,
										m_doc
									);
		
		m_endLine = -1;
		m_endOffset = -1;
	} else if ( m_begLine > m_endLine ) {
		c = new QDocumentEraseCommand(
										m_endLine,
										m_endOffset,
										m_begLine,
										m_begOffset,
										m_doc
									);
		
		m_begLine = m_endLine;
		m_begOffset = m_endOffset;
		
		m_endLine = -1;
		m_endOffset = -1;
	} else {
		c = new QDocumentEraseCommand(
										m_begLine,
										qMin(m_begOffset, m_endOffset),
										m_endLine,
										qMax(m_begOffset, m_endOffset),
										m_doc
									);
		
		m_begOffset = qMin(m_begOffset, m_endOffset);
		m_endLine = -1;
		m_endOffset = -1;
	}
	
	c->setTargetCursor(this);
	execute(c);
}

//////////////////

/////////////////////////
//	QDocumentPrivate
/////////////////////////


QFont* QDocumentPrivate::m_font = 0;// = QApplication::font();
QFontMetrics* QDocumentPrivate::m_fontMetrics = 0;//(m_font);

QFormatFactory* QDocumentPrivate::m_formatFactory = 0;

QVector<QFont> QDocumentPrivate::m_fonts;
QVector<QColor> QDocumentPrivate::m_colors;
QList<QDocumentPrivate*> QDocumentPrivate::m_documents;

bool QDocumentPrivate::m_fixedPitch;
int QDocumentPrivate::m_ascent;// = m_fontMetrics.ascent();
int QDocumentPrivate::m_descent;// = m_fontMetrics.descent();
int QDocumentPrivate::m_leading;// = m_fontMetrics.leading();
int QDocumentPrivate::m_spaceWidth;// = m_fontMetrics.width(' ');
int QDocumentPrivate::m_lineHeight;// = m_fontMetrics.height();
int QDocumentPrivate::m_lineSpacing;// = m_fontMetrics.lineSpacing();

int QDocumentPrivate::m_tabStop = 4;
int QDocumentPrivate::m_leftMargin = 5;
QDocument::WhiteSpaceMode QDocumentPrivate::m_showSpaces = QDocument::ShowNone;

QDocumentPrivate::QDocumentPrivate(QDocument *d)
 : 	m_doc(d),
	m_editCursor(0),
	m_width(0),
	m_height(0),
	_nix(0),
	_dos(0),
	m_lineEnding(QDocument::Conservative)
{
	m_documents << this;
}

QDocumentPrivate::~QDocumentPrivate()
{
	m_marks.clear();
	m_largest.clear();
	
	m_deleting = true;
	
	//qDeleteAll(m_lines);
	foreach ( QDocumentLineHandle *h, m_lines )
		h->deref();
	
	m_lines.clear();
	
	m_deleting = false;
	
	m_commands.clear();
	
	m_documents.removeAll(this);
}

void QDocumentPrivate::execute(QDocumentCommand *cmd)
{
	if ( !cmd )
		return;
	
	//qDebug("adding a command...");
	
	//cmd->setTarget(m_doc);
	
	m_commands.push(cmd);
}

void QDocumentPrivate::draw(QPainter *p, QDocument::PaintContext& cxt)
{
	QDocumentLineHandle *h;
	bool inSel = false, fullSel;
	QList<QDocumentCursorHandle*>::iterator cit;
	int i, realln = -1, pos = 0, xOffset,
		firstLine = qBound(0, cxt.yoffset / m_lineSpacing, m_lines.count() - 1),
		lastLine = qBound(0, firstLine + (cxt.height / m_lineSpacing) + 1, m_lines.count() - 1);
	
	pos += firstLine * m_lineSpacing;
	
	p->setFont(*m_font);
	
	QBrush bg,
		base = cxt.palette.base(),
		selbg = cxt.palette.highlight(),
		alternate = QLineMarksInfoCenter::instance()->markType("current").color;
	
	if ( !alternate.color().isValid() )
		alternate = cxt.palette.alternateBase();
	
	QSmallArray m_cursorLines(0), m_selectionBoundaries(0);
	
	i = textLine(firstLine);
	realln = firstLine;
	
	// adjust first line to take selections into account...
	foreach ( const QDocumentSelection& s, cxt.selections )
	{
		if ( (s.startLine < i) && (s.endLine >= i) )
		{
			inSel = true;
			break;
		}
	}
	
	//qDebug("[%i;%i] => [%i;%i]", firstLine, lastLine, i, i + lastLine - firstLine);
	
	for ( ; (realln <= lastLine) && (i < m_lines.count()); ++i )
	{
		h = m_lines.at(i);
		
		// selections stuff (must do it before whatever the visibility...)
		m_selectionBoundaries.clear();
		
		fullSel = false;
		
		if ( inSel )
			m_selectionBoundaries.prepend(0);
		
		foreach ( const QDocumentSelection& s, cxt.selections )
		{
			if ( i == s.startLine )
			{
				if ( !(m_selectionBoundaries.count() & 1) )
					m_selectionBoundaries.append(s.start);
				
				if ( i == s.endLine )
				{
					m_selectionBoundaries.append(s.end);
				} else {
					//++selLevel;
					inSel = true;
					//selEnd = h->m_text.length();
				}
			} else if ( inSel && (i == s.endLine) ) {
				
				if ( m_selectionBoundaries.count() % 2 )
					m_selectionBoundaries.append(s.end);
				
				//--selLevel;
				inSel = false;
			}
		}
		
		if ( inSel && m_selectionBoundaries.count() == 1 && m_selectionBoundaries.at(0) == 0 )
		{
			m_selectionBoundaries.clear();
			fullSel = true;
		}
		
		if ( h->hasFlag(QDocumentLine::Hidden) )
			continue;
		else
			++realln;
		
		m_cursorLines.clear();
		
		bg = base;
		
		// idx = column = 0;
		xOffset = m_leftMargin; // margin
		
		// cursor(s) stuff
		if ( cxt.cursors.count() )
		{
			cit = cxt.cursors.begin();
			
			while ( cit != cxt.cursors.end() )
			{
				if ( (*cit)->lineNumber() == i )
				{
					if ( cxt.blinkingCursor )
						m_cursorLines.append((*cit)->columnNumber());
					
					if ( cxt.fillCursorRect )
						bg = alternate;
					
					cit = cxt.cursors.erase(cit);
				} else {
					++cit;
				}
			}
		}
		
		QList<int> m = marks(h);
		
		// line marks stuff
		if ( m.count() )
		{
			QLineMarksInfoCenter *mic = QLineMarksInfoCenter::instance();
			
			QColor c = mic->markType(mic->priority(m)).color;
			
			if ( c.isValid() )
				bg = c;
			
		}
		
		if ( realln < firstLine )
			continue;
		
		//qDebug("drawing line %i (visual %i)", i, realln);
		
		p->fillRect(qMax(cxt.xoffset, m_leftMargin), pos,
					cxt.width, m_lineSpacing,
					fullSel ? selbg : bg);
		
		//p->fillRect(cxt.xoffset, pos + 1,
		//			cxt.width, m_lineHeight,
		//			bg);
		
		p->save();
		
		// simplify line drawing
		p->translate(0, pos);
		
		// draw text
		h->draw(p, cxt.xoffset, cxt.width, m_selectionBoundaries, m_cursorLines, cxt.palette, fullSel);
		
		// see above
		p->translate(0, -pos);
		
		// draw fold rect indicator
		if ( h->hasFlag(QDocumentLine::CollapsedBlockStart) )
		{
			//qDebug("drawing goddamn dash line...");
			
			p->setPen(QPen(Qt::blue, 1, Qt::DotLine));
			
			//p->drawRect(cxt.xoffset + 2, pos,
			//			cxt.width - 4, m_lineSpacing - 1);
			
			p->drawRect(m_leftMargin, pos,
						cxt.width - 4, m_lineSpacing - 1);
			
		}
		
		p->restore();
		
		// draw cursors
		
		//for ( int ci = 0; ci < m_cursorLines.size(); ++ci )
		//	p->drawLine(m_cursorLines[ci]);
		
		pos += m_lineSpacing;
		
		//qDebug("drawing line %i in %i ms", i, t.elapsed());
	}
	
	//qDebug("painting done"); // in %i ms...", t.elapsed());
}

int QDocumentPrivate::position(const QDocumentLineHandle *l) const
{
	int pos = 0;
	
	int idx = m_lines.indexOf(const_cast<QDocumentLineHandle*>(l));
	
	if ( idx == -1 )
		return -1;
	
	for ( int i = 0; i < idx; i++ )
		pos += m_lines.at(i)->length();
	
	return pos;
}

QDocumentLineHandle* QDocumentPrivate::lineForPosition(int& position) const
{
	int pos = 0, idx = 0;
	
	while ( (pos + m_lines.at(idx)->length()) < position )
		pos += m_lines.at(idx++)->length();
		
	
	return 0;
}

void QDocumentPrivate::setWidth()
{
	m_width = 0;
	m_largest.clear();
	
	foreach ( QDocumentLineHandle *l, m_lines )
	{
		if ( l->hasFlag(QDocumentLine::Hidden) )
			continue;
		
		if ( l->m_layout )
			checkWidth(l, qRound(l->m_layout->boundingRect().width()));
		else
			checkWidth(l, l->m_text);
	}
	
	//emitWidthChanged();
}

void QDocumentPrivate::setHeight()
{
	m_height = m_lines.count() * m_lineSpacing;
	
	int last = 0;
	QMap<int, int>::const_iterator it = m_hidden.constBegin();
	
	while ( it != m_hidden.constEnd() )
	{
		int n = it.key() + *it;
		
		if ( last > it.key() )
		{
			if ( last < n )
			{
				last = n;
				m_height -= m_lineSpacing * (n - last);
			}
		} else {
			last = n;
			m_height -= m_lineSpacing * (*it);
		}
		
		++it;
	}
	
	//foreach ( QDocumentLineHandle *h, m_lines )
	//	if ( !h->hasFlag(QDocumentLine::Hidden) )
	//		m_height += m_lineSpacing;
	
	emitHeightChanged();
}

void QDocumentPrivate::setFont(const QFont& f)
{
	if ( !m_font )
	{
		m_font = new QFont;
		m_fontMetrics = new QFontMetrics(*m_font);
	}
	
	*m_font = f;
	
	// ensures the font is fixed pitch to avoid idsplay glitches
	// and inconsistency of column selections
	// :( does not work well...
	//m_font->setFixedPitch(true);
	
	// set the styling so that if the font is not found Courier one will be used
	m_font->setStyleHint(QFont::Courier, QFont::PreferQuality);
	
	*m_fontMetrics = QFontMetrics(*m_font);
	
	m_spaceWidth = m_fontMetrics->width(' ');
	m_lineSpacing = m_fontMetrics->lineSpacing();
	m_ascent = m_fontMetrics->ascent();
	m_descent = m_fontMetrics->descent();
	m_leading = m_fontMetrics->leading();
	
	m_lineHeight = m_fontMetrics->height();
	//m_lineHeight = m_ascent + m_descent - 2;
	
	m_fixedPitch = QFontInfo(*m_font).fixedPitch();
	
	//if ( !m_fixedPitch )
	//	qDebug("unsafe computations...");
	
	foreach ( QDocumentPrivate *d, m_documents )
	{
		d->setWidth();
		d->setHeight();
	}
	
	updateFormatCache();
}

void QDocumentPrivate::tunePainter(QPainter *p, quint8 fid)
{
	if ( fid < m_fonts.count() )
	{
		p->setFont(m_fonts.at(fid));
		p->setPen(m_colors.at(fid));
	} else {
		p->setFont(*m_font);
		p->setPen(Qt::black);
	}
}

void QDocumentPrivate::updateFormatCache()
{
	m_fonts.clear();
	m_colors.clear();
	
	if ( !m_font || !m_formatFactory )
		return;
	
	QFont f(*m_font);
	const quint8 end = m_formatFactory->formats().count();
	
	m_fonts.reserve(end);
	m_colors.reserve(end);
	
	for ( quint8 i = 0; i < end; i++ )
	{
		QFormat fmt = m_formatFactory->format(i);
		
		f.setWeight(fmt.weight);
		f.setItalic(fmt.italic);
		f.setUnderline(fmt.underline);
		f.setStrikeOut(fmt.strikeout);
		
		m_fonts << f;
		m_colors << fmt.foreground;
	}
	
	foreach ( QDocumentPrivate *d, m_documents )
		d->emitFormatsChanged();
	
}

static const int widthCacheSize = 5;

void QDocumentPrivate::checkWidth(QDocumentLineHandle *l, int w)
{
	w += m_leftMargin << 1;
	
	if ( w > m_width )
	{
		m_width = w;
		emitWidthChanged();
		
		m_largest.clear();
		m_largest << qMakePair(l, w);
	} else if ( m_largest.count() && (m_largest.at(0).first == l) ) {
		int old = m_largest.at(0).second;
		
		if ( w < old )
			setWidth();
	}
}

void QDocumentPrivate::checkWidth(QDocumentLineHandle *l, const QString& s)
{
	//int w = (s.count() + s.count('\t') * (QDocument::tabStop()-1)) * m_spaceWidth;
	
	checkWidth(l, l->cursorToX(s.length()));
}

void QDocumentPrivate::emitWidthChanged()
{
	if ( !m_doc )
		return;
	
	emit m_doc->widthChanged(m_width);
	
	emit m_doc->sizeChanged(QSize(m_width, m_height));
}

void QDocumentPrivate::emitHeightChanged()
{
	if ( !m_doc )
		return;
	
	emit m_doc->heightChanged(m_height);
	
	emit m_doc->sizeChanged(QSize(m_width, m_height));
}

void QDocumentPrivate::insertLines(int after, const QList<QDocumentLineHandle*>& l)
{
	int i = 0, hIdx = -1, hCnt = 0;
	
	++after;
	updateHidden(after, l.count());
	
	while ( i < l.count() ) //after != m_lines.end() )
	{
		if ( l.at(i)->m_layout )
			checkWidth(l.at(i), qRound(l.at(i)->m_layout->boundingRect().width()));
		else
			checkWidth(l.at(i), l.at(i)->cursorToX(l.at(i)->length()) + m_leftMargin * 2);
		
		m_lines.insert(after + i, l.at(i));
		
		l.at(i)->setFlag(QDocumentLine::Hidden, false);
		l.at(i)->setFlag(QDocumentLine::CollapsedBlockStart, false);
		l.at(i)->setFlag(QDocumentLine::CollapsedBlockEnd, false);
// 		bool h = l.at(i)->hasFlag(QDocumentLine::Hidden);
// 		
// 		if ( hIdx != -1 )
// 		{
// 			if ( h )
// 			{
// 				++hCnt;
// 			} else {
// 				hideEvent(after + hIdx, hCnt);
// 				hIdx = -1;
// 				hCnt = 0;
// 			}
// 		} else if ( h ) {
// 			hIdx = i;
// 			hCnt = 1;
// 		}
		
		++i;
	}
	
// 	if ( hIdx != -1 )
// 	{
// 		hideEvent(after + hIdx, hCnt);
// 	}
	
	setHeight();
}

void QDocumentPrivate::removeLines(int after, int n)
{
	if ( (after >= 0) && (after < m_lines.count()) )
		m_lines.at(after)->setFlag(QDocumentLine::CollapsedBlockStart, false);
	
	QMap<int, int>::iterator it = m_hidden.begin();
	
	//qDebug("translating %i", visualLine);
	
	while ( it != m_hidden.end() )
	{
		if ( (it.key() >= after) && (it.key() < (after + n)) )
		{
			it = m_hidden.erase(it);
		} else if ( (it.key() < after) && ((it.key() + *it) >= after) ) {
			qDebug("rem(%i, %i) => (%i, %i) = (%i, %i)",
					after, n,
					it.key(), *it,
					it.key(), after - it.key()
					);
			
			*it = after - it.key();
			++it;
		} else {
			++it;
		}
	}
	
	++after;
	
	updateHidden(after, -n);
	m_lines.remove(after, n);
	
	setHeight();
}

QDocumentLineHandle* QDocumentPrivate::at(int line) const
{
	return ((line >= 0) && (line < m_lines.count())) ? m_lines.at(line) : 0;
}

int QDocumentPrivate::indexOf(const QDocumentLineHandle *l) const
{
	return m_lines.indexOf(const_cast<QDocumentLineHandle*>(l));
}

QDocumentIterator QDocumentPrivate::index(const QDocumentLineHandle *l)
{
	QDocumentIterator i = m_lines.begin();
	
	int idx = indexOf(l);
	
	return (idx != -1) ? i + idx : m_lines.end();
}

QDocumentConstIterator QDocumentPrivate::index(const QDocumentLineHandle *l) const
{
	QDocumentConstIterator i = m_lines.constBegin();
	
	int idx = indexOf(l);
	
	return (idx != -1) ? i + idx : m_lines.end();
}

QDocumentLineHandle* QDocumentPrivate::next(const QDocumentLineHandle *l) const
{
	if ( !l )
		return m_lines.count() ? m_lines.first() : 0;
	
	int idx = m_lines.indexOf(const_cast<QDocumentLineHandle*>(l));
	
	return ((idx != -1) && ((idx + 1) < m_lines.count())) ? m_lines.at(idx + 1) : 0;
}

QDocumentLineHandle* QDocumentPrivate::previous(const QDocumentLineHandle *l) const
{
	if ( !l )
		return m_lines.count() ? m_lines.last() : 0;
	
	int idx = m_lines.indexOf(const_cast<QDocumentLineHandle*>(l));
	
	return ((idx != -1) && (idx > 0)) ? m_lines.at(idx - 1) : 0;
}

void QDocumentPrivate::beginChangeBlock()
{
	//qDebug("<macro>");
	m_commands.beginMacro(QString());
}

void QDocumentPrivate::endChangeBlock()
{
	m_commands.endMacro();
	//qDebug("</macro>");
}

QList<int> QDocumentPrivate::marks(QDocumentLineHandle *h) const
{
	//return QList<int>() << 1; //testcase
	
	return m_marks.contains(h) ? m_marks.value(h) : QList<int>();
}

void QDocumentPrivate::addMark(QDocumentLineHandle *h, int mid)
{
	m_marks[h] << mid;
	
	emitMarkChanged(h, mid, true);
}

void QDocumentPrivate::toggleMark(QDocumentLineHandle *h, int mid)
{
	if ( m_marks[h].contains(mid) )
	{
		m_marks[h].removeAll(mid);
		emitMarkChanged(h, mid, false);
	} else {
		m_marks[h] << mid;
		emitMarkChanged(h, mid, true);
	}
}

void QDocumentPrivate::removeMark(QDocumentLineHandle *h, int mid)
{
	m_marks[h].removeAll(mid);
	
	emitMarkChanged(h, mid, false);
}

int QDocumentPrivate::visualLine(int textLine) const
{
	int hiddenLines = 0, next = 0;
	QMap<int, int>::const_iterator it = m_hidden.constBegin();
	
	//qDebug("translating %i", visualLine);
	
	while ( it != m_hidden.constEnd() )
	{
		if ( it.key() >= textLine )
			break;
		
		if ( it.key() < next )
		{
			++it;
			continue;
		}
		
		int cur = it.key(), max = 0;
		
		do
		{
			max = qMax(max, *it);
			++it;
		} while ( (it != m_hidden.constEnd()) && (it.key() == cur) );
		
		next = cur + max;
		hiddenLines += max;
	}
	
	//qDebug("[t->v] : %i => %i", textLine, textLine - hiddenLines);
	
	return textLine - hiddenLines;
}

int QDocumentPrivate::textLine(int visualLine) const
{
	int hiddenLines = 0, next = 0;
	QMap<int, int>::const_iterator it = m_hidden.constBegin();
	
	//qDebug("translating %i", visualLine);
	
	while ( it != m_hidden.constEnd() )
	{
		if ( (it.key() - hiddenLines) >= visualLine )
			break;
		
		if ( it.key() < next )
		{
			++it;
			continue;
		}
		
		int cur = it.key(), max = 0;
		
		do
		{
			max = qMax(max, *it);
			++it;
		} while ( (it != m_hidden.constEnd()) && (it.key() == cur) );
		
		next = cur + max;
		hiddenLines += max;
	}
	
	//qDebug("[v->t] : %i => %i", visualLine, visualLine + hiddenLines);
	
	return visualLine + hiddenLines;
}

void QDocumentPrivate::hideEvent(int line, int count)
{
//	qDebug("hiding %i lines from %i", count, line);
	
	m_hidden.insertMulti(line, count);
	
	setHeight();
}

void QDocumentPrivate::showEvent(int line, int count)
{
//	qDebug("trying to show %i lines from %i", count, line);
	QMap<int, int>::iterator it = m_hidden.find(line);
	
	while ( (it != m_hidden.end()) && (it.key() == line)  )
	{
		if ( *it == count )
		{
//			qDebug("showing %i lines from %i", count, line);
			it = m_hidden.erase(it);
		} else
			++it;
	}
	
	setHeight();
}

void QDocumentPrivate::updateHidden(int line, int count)
{
	QMap<int, int> prev = m_hidden;
	QMap<int, int>::iterator it = prev.begin();
	
	m_hidden.clear();
	
	//qDebug("shifting by %i from %i", count, line);
	
	while ( it != prev.end() )
	{
		if ( it.key() < line )
		{
			m_hidden.insertMulti(it.key(), *it);
		} else {
			m_hidden.insertMulti(it.key() + count, *it);
		}
		
		++it;
	}
}

void QDocumentPrivate::emitFormatsChange(int line, int lines)
{
	emit m_doc->formatsChange(line, lines);
}

void QDocumentPrivate::emitContentsChange(int line, int lines)
{
	//for ( int i = line; i < (line + lines); i++ )
	//	m_lines.at(i)->cache();
	
	emit m_doc->contentsChange(line, lines);
	emit m_doc->contentsChanged();
}

void QDocumentPrivate::emitFormatsChanged()
{
	emit m_doc->formatsChanged();
}

void QDocumentPrivate::emitContentsChanged()
{
	//emit m_doc->contentsChanged();
}

void QDocumentPrivate::emitLineDeleted(QDocumentLineHandle *h)
{
	if ( !m_deleting )
	{
		m_marks.remove(h);
		
		int idx = m_lines.indexOf(h);
		
		if ( idx != -1 )
		{
			//qDebug("removing line %i", idx);
			
			m_lines.remove(idx);
			
			if ( m_largest.count() && (m_largest.at(0).first == h) )
			{
				m_largest.remove(0);
				setWidth();
			}
			
			setHeight();
		}
	}
	
	emit m_doc->lineDeleted(h);
}

void QDocumentPrivate::emitMarkChanged(QDocumentLineHandle *l, int m, bool on)
{
	emitFormatsChanged();
	emit m_doc->markChanged(l, m, on);
}

//////////////////
