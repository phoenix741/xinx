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

#include "qeditor.h"

#include "qdocument.h"
#include "qdocument_p.h"
#include "qdocumentline.h"
#include "qdocumentcursor.h"

#include "qmarker.h"
#include "qmatcher.h"
#include "qindenter.h"
#include "qhighlighter.h"
#include "qcodecompletionengine.h"
#include "qlanguagedefinition.h"

#include "qcodeedit.h"
#include "qpanellayout.h"
#include "qlinemarksinfocenter.h"

#include <QFile>
#include <QMenu>
#include <QTimer>
#include <QStyle>
#include <QAction>
#include <QPointer>
#include <QPainter>
#include <QKeyEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QClipboard>
#include <QScrollBar>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QApplication>
#include <QActionGroup>

//#define Q_GL_EDITOR

#define Q_SHORTCUT(a, s, c) a->setShortcut( QKeySequence( tr(s, c) ) )

#ifdef Q_GL_EDITOR
#include <QGLWidget>
#endif

////////////////////////////////////////////////////////////////////////
//	Bindings handling
////////////////////////////////////////////////////////////////////////

QList<QEditor*> QEditor::m_editors;
QEditor::InputBinding* QEditor::m_defaultBinding = 0;
QHash<QString, QEditor::InputBinding*> QEditor::m_bindings;

QStringList QEditor::inputBindings()
{
	return m_bindings.keys();
}

QString QEditor::defaultInputBinding()
{
	return m_defaultBinding ? m_defaultBinding->name() : tr("Default");
}

void QEditor::addInputBinding(QEditor::InputBinding *b)
{
	m_bindings[b->id()] = b;

	foreach ( QEditor *e, m_editors )
		e->updateBindingsMenu();

}

void QEditor::removeInputBinding(QEditor::InputBinding *b)
{
	m_bindings.remove(b->id());

	foreach ( QEditor *e, m_editors )
		e->updateBindingsMenu();

}

void QEditor::setDefaultInputBinding(QEditor::InputBinding *b)
{
	m_defaultBinding = b;
}

void QEditor::setDefaultInputBinding(const QString& b)
{
	m_defaultBinding = m_bindings.value(b);
}

////////////////////////////////////////////////////////////////////////

QEditor::QEditor(QWidget *p)
 : QAbstractScrollArea(p), m_binding(m_defaultBinding), m_state(None)
{
	m_editors << this;

	init();
}

QEditor::QEditor(const QString& s, QWidget *p)
 : QAbstractScrollArea(p), m_binding(m_defaultBinding), m_state(None)
{
	m_editors << this;

	init();

	setText(s);
}

QEditor::~QEditor()
{
	m_editors.removeAll(this);

	if ( m_marker )
		delete m_marker;

	if ( m_matcher )
		delete m_matcher;

	if ( m_indenter )
		delete m_indenter;

	if ( m_completionEngine )
		delete m_completionEngine;

	if ( m_doc )
		delete m_doc;
}

void QEditor::init()
{
	#ifdef Q_GL_EDITOR
	setViewport(new QGLWidget);
	#endif

	viewport()->setCursor(Qt::IBeamCursor);
	viewport()->setBackgroundRole(QPalette::Base);
	//viewport()->setAttribute(Qt::WA_OpaquePaintEvent, true);
	viewport()->setAttribute(Qt::WA_InputMethodEnabled, true);

	verticalScrollBar()->setSingleStep(20);
	horizontalScrollBar()->setSingleStep(20);

	setAcceptDrops(true);
	//setDragEnabled(true);
	setFrameShadow(QFrame::Plain);
	setFocusPolicy(Qt::WheelFocus);
	setAttribute(Qt::WA_KeyCompression);

	connect(this							,
			SIGNAL( markChanged(QString, QDocumentLineHandle*, int, bool) ),
			QLineMarksInfoCenter::instance(),
			SLOT  ( markChanged(QString, QDocumentLineHandle*, int, bool) ) );

	m_doc = new QDocument(this);

	connect(m_doc	, SIGNAL( formatsChange (int, int) ),
			this	, SLOT  ( repaintContent(int, int) ) );

	connect(m_doc	, SIGNAL( contentsChange(int, int) ),
			this	, SLOT  ( updateContent (int, int) ) );

	connect(m_doc		, SIGNAL( formatsChanged() ),
			viewport()	, SLOT  ( update() ) );

	connect(m_doc	, SIGNAL( widthChanged(int) ),
			this	, SLOT  ( documentWidthChanged(int) ) );

	connect(m_doc	, SIGNAL( heightChanged(int) ),
			this	, SLOT  ( documentHeightChanged(int) ) );

	connect(m_doc	, SIGNAL( cleanChanged(bool) ),
			this	, SLOT  ( setContentClean(bool) ) );

	connect(m_doc	, SIGNAL( undoAvailable(bool) ),
			this	, SIGNAL( undoAvailable(bool) ) );

	connect(m_doc	, SIGNAL( redoAvailable(bool) ),
			this	, SIGNAL( redoAvailable(bool) ) );

	connect(m_doc	, SIGNAL( markChanged(QDocumentLineHandle*, int, bool) ),
			this	, SLOT  ( markChanged(QDocumentLineHandle*, int, bool) ) );

	connect(m_doc	, SIGNAL( lineEndingChanged(int) ),
			this	, SLOT  ( lineEndingChanged(int) ) );

	m_cursor = QDocumentCursor(m_doc);
	m_cursor.setAutoUpdated(true);

	m_doc->setEditCursor(&m_cursor);

	pMenu = new QMenu;

	aUndo = new QAction(QIcon(":/undo.png"), tr("&Undo"), this);
	Q_SHORTCUT(aUndo, "Ctrl+Z", "Edit");
	aUndo->setEnabled(false);
	connect(this , SIGNAL( undoAvailable(bool) ),
			aUndo, SLOT  ( setEnabled(bool) ) );
	connect(aUndo, SIGNAL( triggered() ),
			this , SLOT  ( undo() ) );

	aRedo = new QAction(QIcon(":/redo.png"), tr("&Redo"), this);
	Q_SHORTCUT(aRedo, "Ctrl+Y", "Edit");
	aRedo->setEnabled(false);
	connect(this , SIGNAL( redoAvailable(bool) ),
			aRedo, SLOT  ( setEnabled(bool) ) );
	connect(aRedo, SIGNAL( triggered() ),
			this , SLOT  ( redo() ) );

	aCut = new QAction(QIcon(":/cut.png"), tr("Cu&t"), this);
	Q_SHORTCUT(aCut, "Ctrl+X", "Edit");
	aCut->setEnabled(false);
	connect(this, SIGNAL( copyAvailable(bool) ),
			aCut, SLOT  ( setEnabled(bool) ) );
	connect(aCut, SIGNAL( triggered() ),
			this, SLOT  ( cut() ) );

	aCopy = new QAction(QIcon(":/copy.png"), tr("&Copy"), this);
	Q_SHORTCUT(aCopy, "Ctrl+C", "Edit");
	aCopy->setEnabled(false);
	connect(this , SIGNAL( copyAvailable(bool) ),
			aCopy, SLOT  ( setEnabled(bool) ) );
	connect(aCopy, SIGNAL( triggered() ),
			this , SLOT  ( copy() ) );

	aPaste = new QAction(QIcon(":/paste.png"), tr("&Paste"), this);
	//aPaste->setEnabled(QApplication::clipboard()->text().count());
	Q_SHORTCUT(aPaste, "Ctrl+V", "Edit");
	connect(QApplication::clipboard()	, SIGNAL( dataChanged() ),
			this						, SLOT  ( checkClipboard() ) );

	connect(aPaste	, SIGNAL( triggered() ),
			this	, SLOT  ( paste() ) );

	aIndent = new QAction(QIcon(":/indent.png"), tr("&Indent"), this);
	Q_SHORTCUT(aIndent, "Ctrl+I", "Edit");
	connect(aIndent	, SIGNAL( triggered() ),
			this	, SLOT  ( indentSelection() ) );

	aUnindent = new QAction(QIcon(":/unindent.png"), tr("&Unindent"), this);
	Q_SHORTCUT(aUnindent, "Ctrl+Shift+I", "Edit");
	connect(aUnindent	, SIGNAL( triggered() ),
			this		, SLOT  ( unindentSelection() ) );

	aComment = new QAction(QIcon(":/comment.png"), tr("Co&mment"), this);
	Q_SHORTCUT(aComment, "Ctrl+D", "Edit");
	connect(aComment, SIGNAL( triggered() ),
			this	, SLOT  ( commentSelection() ) );

	aUncomment = new QAction(QIcon(":/uncomment.png"), tr("Unc&omment"), this);
	Q_SHORTCUT(aUncomment, "Ctrl+Shift+D", "Edit");
	connect(aUncomment	, SIGNAL( triggered() ),
			this		, SLOT  ( uncommentSelection() ) );

	aSelectAll = new QAction(tr("&Select all"), this);
	Q_SHORTCUT(aSelectAll, "Ctrl+A", "Edit");
	connect(aSelectAll	, SIGNAL( triggered() ),
			this		, SLOT  ( selectAll() ) );

	aFind = new QAction(QIcon(":/find.png"), tr("&Find"), this);
	Q_SHORTCUT(aFind, "Ctrl+F", "Search");
	connect(aFind	, SIGNAL( triggered() ),
			this	, SLOT  ( find() ) );

	aFindNext = new QAction(QIcon(":/next.png"), tr("Fin&d next"), pMenu);
	Q_SHORTCUT(aFindNext, "F3", "Search");
	connect(aFindNext	, SIGNAL( triggered() ),
			this		, SLOT  ( findNext() ) );

	aReplace = new QAction(QIcon(":/replace.png"), tr("&Replace"), this);
	Q_SHORTCUT(aReplace, "Ctrl+R", "Search");
	connect(aReplace, SIGNAL( triggered() ),
			this	, SLOT  ( replace() ) );

	aGoto = new QAction(QIcon(":/goto.png"), tr("&Goto line..."), this);
	Q_SHORTCUT(aGoto, "Ctrl+G", "Search");
	connect(aGoto	, SIGNAL( triggered() ),
			this	, SLOT  ( gotoLine() ) );

	QAction *sep;

	addAction(aUndo, "&Edit", "Edit");
	addAction(aRedo, "&Edit", "Edit");

	sep = new QAction(this);
	sep->setSeparator(true);
	addAction(sep, "&Edit", "Edit");

	addAction(aCut, "&Edit", "Edit");
	addAction(aCopy, "&Edit", "Edit");
	addAction(aPaste, "&Edit", "Edit");

	sep = new QAction(this);
	sep->setSeparator(true);
	addAction(sep, "&Edit", "Edit");

	addAction(aIndent, "&Edit", "Edit");
	addAction(aUnindent, "&Edit", "Edit");

	sep = new QAction(this);
	sep->setSeparator(true);
	addAction(sep, "&Edit", "");

	addAction(aComment, "&Edit", "");
	addAction(aUncomment, "&Edit", "");

	sep = new QAction(this);
	sep->setSeparator(true);
	addAction(sep, "&Edit", "Edit");

	addAction(aSelectAll, "&Edit", "Edit");

	sep = new QAction(this);
	sep->setSeparator(true);
	addAction(sep, "&Edit", "");

	m_bindingsMenu = new QMenu(tr("Input binding"), this);
	m_bindingsActions = new QActionGroup(m_bindingsMenu);
	m_bindingsActions->setExclusive(true);

	connect(m_bindingsActions	, SIGNAL( triggered(QAction*) ),
			this				, SLOT  ( bindingSelected(QAction*) ) );

	aDefaultBinding = new QAction(tr("Default"), m_bindingsMenu);
	aDefaultBinding->setCheckable(true);
	aDefaultBinding->setData("default");

	m_bindingsMenu->addAction(aDefaultBinding);
	m_bindingsMenu->addSeparator();
	m_bindingsActions->addAction(aDefaultBinding);
	m_bindings["default"] = 0;

	updateBindingsMenu();

	addAction(m_bindingsMenu->menuAction(), "&Edit", "");

	sep = new QAction(this);
	sep->setSeparator(true);
	addAction(sep, QString());

	m_lineEndingsMenu = new QMenu(tr("Line endings"), this);
	m_lineEndingsActions = new QActionGroup(m_lineEndingsMenu);
	m_lineEndingsActions->setExclusive(true);

	connect(m_lineEndingsActions, SIGNAL( triggered(QAction*) ),
			this				, SLOT  ( lineEndingSelected(QAction*) ) );

	m_lineEndingsActions->addAction(tr("Conservative"))->setData("conservative");
	m_lineEndingsActions->addAction(tr("Local"))->setData("local");
	m_lineEndingsActions->addAction(tr("Unix/Linux"))->setData("unix");
	m_lineEndingsActions->addAction(tr("Dos/Windows"))->setData("dos");

	QList<QAction*> lle = m_lineEndingsActions->actions();

	foreach ( QAction *a, lle )
	{
		a->setCheckable(true);
		m_lineEndingsMenu->addAction(a);
	}

	lle.at(0)->setChecked(true);

	addAction(m_lineEndingsMenu->menuAction(), "&Edit", "");

	sep = new QAction(this);
	sep->setSeparator(true);
	addAction(sep, QString());

	addAction(aFind, "&Search", "Search");
	addAction(aFindNext, "&Search", "Search");
	addAction(aReplace, "&Search", "Search");

	sep = new QAction(this);
	sep->setSeparator(true);
	addAction(sep, "&Search", "Search");

	addAction(aGoto, "&Search", "Search");
}

/*!
	\return wether the flag \a f is set
*/
bool QEditor::flag(EditFlag f) const
{
	return m_state & f;
}

/*!
	\brief Sets the flag \a f
*/
void QEditor::setFlag(EditFlag f, bool b)
{
	if ( b )
		m_state |= f;
	else
		m_state &= ~f;
}

QString QEditor::text() const
{
	return m_doc ? m_doc->text() : QString();
}

void QEditor::setText(const QString& s)
{
	if ( m_doc )
		m_doc->setText(s);

	setCursor(QDocumentCursor(m_doc));

	documentWidthChanged(m_doc->width());
	documentHeightChanged(m_doc->height());
}

void QEditor::checkClipboard()
{
	// LOOKS LIKE THIS FUNCTION NEVER GETS CALLED DESPITE THE CONNECTION...

	const QMimeData *d = QApplication::clipboard()->mimeData();

	//qDebug("checking clipboard : %s", d);

	aPaste->setEnabled(d && d->hasText());
}

void QEditor::print()
{

}

void QEditor::retranslate()
{
	aUndo->setText(tr("&Undo"));
	aRedo->setText(tr("&Redo"));

	aCut->setText(tr("Cu&t"));
	aCopy->setText(tr("&Copy"));
	aPaste->setText(tr("&Paste"));

	aIndent->setText(tr("&Indent"));
	aUnindent->setText(tr("&Unindent"));
	aComment->setText(tr("Co&mment"));
	aUncomment->setText(tr("Unc&omment"));

	aSelectAll->setText(tr("&Select all"));

	aFind->setText(tr("&Find"));
	aFindNext->setText(tr("Fin&d next"));
	aReplace->setText(tr("&Replace"));

	aGoto->setText(tr("&Goto line..."));

	if ( m_completionEngine )
		m_completionEngine->retranslate();

	m_bindingsMenu->setTitle(tr("Input binding"));

	aDefaultBinding->setText(tr("Default"));
}

void QEditor::addAction(QAction *a, const QString& menu, const QString& toolbar)
{
	QWidget::addAction(a);

	if ( menu.count() )
	{
		pMenu->addAction(a);
	}
}

void QEditor::removeAction(QAction *a, const QString& menu, const QString& toolbar)
{
	QWidget::removeAction(a);

	pMenu->removeAction(a);
}

QDocument* QEditor::document() const
{
	return m_doc;
}

void QEditor::setDocument(QDocument *d)
{
	Q_UNUSED(d)

	qWarning("QEditor::setDocument() is not working yet...");
}

void QEditor::highlight()
{
	updateContent(0, m_doc->lines());
}

QEditor::InputBinding* QEditor::inputBinding() const
{
	return m_binding;
}

void QEditor::setInputBinding(QEditor::InputBinding *b)
{
	m_binding = b;

	QString id = b ? b->id() : QString();
	aDefaultBinding->setChecked(!b);

	if ( !b )
		return;

	QList<QAction*> actions = m_bindingsActions->actions();

	foreach ( QAction *a, actions )
	{
		if ( a->data().toString() != id )
			continue;

		a->setChecked(true);
		break;
	}
}

void QEditor::updateBindingsMenu()
{
	QStringList bindings = inputBindings();
	QList<QAction*> actions = m_bindingsActions->actions();

	QString id = m_binding ? m_binding->id() : QString();
	aDefaultBinding->setChecked(!m_binding);

	foreach ( QAction *a, actions )
	{
		int idx = bindings.indexOf(a->data().toString());

		if ( idx == -1 )
		{
			m_bindingsMenu->removeAction(a);
			m_bindingsActions->removeAction(a);
			delete a;
		} else {
			bindings.removeAt(idx);

			if ( a->data().toString() == id )
				a->setChecked(true);

		}
	}

	bindings.removeAll("default");

	foreach ( QString s, bindings )
	{
		InputBinding *b = m_bindings.value(s);

		if ( !b )
			continue;

		QAction *a = new QAction(b->name(), m_bindingsMenu);
		a->setData(b->id());
		a->setCheckable(true);

		m_bindingsActions->addAction(a);
		m_bindingsMenu->addAction(a);
	}
}

void QEditor::bindingSelected(QAction *a)
{
	a = m_bindingsActions->checkedAction();
	m_binding = m_bindings.value(a->data().toString());

	//qDebug("setting binding to %s [0x%x]", qPrintable(a->data().toString()), m_binding);

	updateMicroFocus();
}

void QEditor::lineEndingSelected(QAction *a)
{
	a = m_lineEndingsActions->checkedAction();

	if ( !a )
		return;

	QString le = a->data().toString();

	if ( le == "conservative" )
		m_doc->setLineEnding(QDocument::Conservative);
	else if ( le == "local" )
		m_doc->setLineEnding(QDocument::Local);
	else if ( le == "unix" )
		m_doc->setLineEnding(QDocument::Unix);
	else if ( le == "dos" )
		m_doc->setLineEnding(QDocument::Windows);


	updateMicroFocus();
}

void QEditor::lineEndingChanged(int lineEnding)
{
	QAction *a = m_lineEndingsActions->checkedAction(),
			*n = m_lineEndingsActions->actions().at(lineEnding);

	if ( a != n )
		n->setChecked(true);

}

QDocumentCursor QEditor::cursor() const
{
	return m_cursor;
}

void QEditor::setCursor(const QDocumentCursor& c)
{
	m_cursor = c;
	clearCursorMirrors();

	emitCursorPositionChanged();

	setFlag(CursorOn, true);
	repaintCursor();
	ensureCursorVisible();
	selectionChange();

	updateMicroFocus();
}

QCodeCompletionEngine* QEditor::completionEngine() const
{
	return m_completionEngine;
}

void QEditor::setCompletionEngine(QCodeCompletionEngine *e)
{
	if ( m_completionEngine )
	{
		m_completionEngine->setEditor(0);
		m_completionEngine->deleteLater();
	}

	m_completionEngine = e;

	if ( m_completionEngine )
	{
		m_completionEngine->setEditor(this);
	}
}

QMarker* QEditor::marker() const
{
	return m_marker;
}

void QEditor::setMarker(QMarker *m)
{
	if ( m_marker )
	{
		m_marker->setEditor(0);
	}

	m_marker = m;

	if ( m_marker )
	{
		m_marker->setEditor(this);
	}
}

QMatcher* QEditor::matcher() const
{
	return m_matcher;
}

void QEditor::setMatcher(QMatcher *m)
{
	if ( m_matcher )
	{
		m_matcher->setEditor(0);
	}

	m_matcher = m;

	if ( m_matcher )
	{
		m_matcher->setEditor(this);
	}
}

QIndenter* QEditor::indenter() const
{
	return m_indenter;
}

void QEditor::setIndenter(QIndenter *m)
{
	if ( m_indenter )
	{
		//m_indenter->setDocument(0);
	}

	m_indenter = m;

	if ( m_indenter )
	{
		//m_indenter->setDocument(m_doc);
	}
}

QHighlighter* QEditor::highlighter() const
{
	return m_highlighter;
}

void QEditor::setHighlighter(QHighlighter *m)
{
	if ( m_highlighter )
	{
		connect(m_doc			, SIGNAL( contentsChange(int, int) ),
				m_highlighter	, SLOT  ( reformat(int, int) ) );

		m_highlighter->setDocument(0);
	}

	m_highlighter = m;

	if ( m_highlighter )
	{
		m_highlighter->setDocument(m_doc);

		disconnect(	m_doc			, SIGNAL( contentsChange(int, int) ),
					m_highlighter	, SLOT  ( reformat(int, int) ) );

		bool cuc = m_highlighter->singleLineComment().count();

		aComment->setEnabled(cuc);
		aUncomment->setEnabled(cuc);
	} else {
		aComment->setEnabled(false);
		aUncomment->setEnabled(false);
	}
}

QLanguageDefinition* QEditor::languageDefinition() const
{
	return m_definition;
}

void QEditor::setLanguageDefinition(QLanguageDefinition *d)
{
	m_definition = d;
}

QDocumentLine QEditor::lineAtPosition(const QPoint& p) const
{
	return m_doc ? m_doc->lineAt(p) : QDocumentLine();
}

QDocumentCursor QEditor::cursorForPosition(const QPoint& p) const
{
	//qDebug("cursor for : (%i, %i)", p.x(), p.y());

	return m_doc ? m_doc->cursorAt(p, horizontalOffset()) : QDocumentCursor();
}

void QEditor::setCursorPosition(const QPoint& p)
{
	//qDebug("cursor for : (%i, %i)", p.x(), p.y());

	QDocumentCursor c = cursorForPosition(p);

	if ( c.isValid() )
	{
		/*
		m_cursor = c;
		clearCursorMirrors();

		emitCursorPositionChanged();
		*/
		setCursor(c);
	}
}

void QEditor::emitCursorPositionChanged()
{
	emit cursorPositionChanged();
	emit copyAvailable(m_cursor.hasSelection());

	if ( m_doc->impl()->hasMarks() )
		QLineMarksInfoCenter::instance()->cursorMoved(this);

}

void QEditor::undo()
{
	if ( m_doc )
	{
		m_doc->undo();

		selectionChange();
		ensureCursorVisible();
		setFlag(CursorOn, true);
		emitCursorPositionChanged();
		repaintCursor();
	}
}

void QEditor::redo()
{
	if ( m_doc )
	{
		m_doc->redo();

		selectionChange();
		ensureCursorVisible();
		setFlag(CursorOn, true);
		emitCursorPositionChanged();
		repaintCursor();
	}
}

void QEditor::cut()
{
	copy();

	bool macro = m_mirrors.count();

	if ( macro )
		m_doc->beginMacro();

	m_cursor.removeSelectedText();

	for ( int i = 0; i < m_mirrors.count(); ++i )
		m_mirrors[i].removeSelectedText();

	if ( macro )
		m_doc->endMacro();

	clearCursorMirrors();

	ensureCursorVisible();
	setFlag(CursorOn, true);
	emitCursorPositionChanged();
	repaintCursor();
}

void QEditor::copy()
{
	if ( !m_cursor.hasSelection() )
		return;

	QMimeData *d = createMimeDataFromSelection();
	QApplication::clipboard()->setMimeData(d);

	//qDebug("%s", qPrintable(m_cursor.selectedText()));
	//QApplication::clipboard()->setText(m_cursor.selectedText());
}

void QEditor::paste()
{
	const QMimeData *d = QApplication::clipboard()->mimeData();

	if ( d )
		insertFromMimeData(d);
}

static bool unindent(const QDocumentCursor& cur)
{
	QDocumentLine beg(cur.line());
	int r = 0, n = 0, t = QDocument::tabStop();
	QString txt = beg.text().left(beg.firstChar());

	while ( txt.count() && (n < t) )
	{
		if ( txt.at(txt.length() - 1) == '\t' )
			n += t - (n % t);
		else
			++n;

		++r;
		txt.chop(1);
	}

	if ( !r )
		return false;

	QDocumentCursor c(cur);
	c.setSilent(true);
	c.movePosition(1, QDocumentCursor::StartOfBlock, QDocumentCursor::MoveAnchor);
	c.movePosition(r, QDocumentCursor::Right, QDocumentCursor::KeepAnchor);
	c.removeSelectedText();

	return true;
}

static void insert(const QDocumentCursor& cur, const QString& txt)
{
	QDocumentCursor c(cur);
	c.setSilent(true);
	c.setColumnNumber(0);
	c.insertText(txt);
}

static void removeFromStart(const QDocumentCursor& cur, const QString& txt)
{
	QDocumentLine l = cur.line();
	int pos = l.firstChar();

	if ( l.text().mid(pos, txt.length()) != txt )
		return;

	QDocumentCursor c(cur.document(), cur.lineNumber(), pos);
	c.setSilent(true);
	c.movePosition(txt.length(),
					QDocumentCursor::NextCharacter,
					QDocumentCursor::KeepAnchor);
	c.removeSelectedText();
}

void QEditor::indentSelection()
{
	QString txt("\t");

	if ( m_mirrors.count() )
	{
		m_doc->beginMacro();

		insert(m_cursor, txt);

		foreach ( const QDocumentCursor& m, m_mirrors )
			insert(m, txt);

		m_doc->endMacro();

	} else if ( !m_cursor.hasSelection() ) {
		insert(m_cursor, txt);
	} else {
		QDocumentSelection s = m_cursor.selection();
		QDocumentCursor c(m_doc, s.startLine);
		c.setSilent(true);
		c.beginEditBlock();

		while ( c.isValid() && (c.lineNumber() <= s.endLine) )
		{
			c.insertText(txt);
			c.movePosition(1, QDocumentCursor::NextLine);
		}

		c.endEditBlock();
	}
}

void QEditor::unindentSelection()
{
	if ( !m_cursor.line().firstChar() )
		return;

	if ( m_mirrors.count() )
	{
		m_doc->beginMacro();

		unindent(m_cursor);

		foreach ( const QDocumentCursor& m, m_mirrors )
			unindent(m);

		m_doc->endMacro();

	} else if ( !m_cursor.hasSelection() ) {
		unindent(m_cursor);
	} else {
		QDocumentSelection s = m_cursor.selection();

		m_doc->beginMacro();

		for ( int i = s.startLine; i <= s.endLine; ++i )
		{
			unindent(QDocumentCursor(m_doc, i));
		}

		m_doc->endMacro();
	}
}

void QEditor::commentSelection()
{
	if ( !highlighter() || highlighter()->singleLineComment().isEmpty() )
		return;

	QString txt = highlighter()->singleLineComment();

	if ( m_mirrors.count() )
	{
		m_doc->beginMacro();

		insert(m_cursor, txt);

		foreach ( const QDocumentCursor& m, m_mirrors )
			insert(m, txt);

		m_doc->endMacro();

	} else if ( !m_cursor.hasSelection() ) {
		insert(m_cursor, txt);
	} else {
		QDocumentSelection s = m_cursor.selection();
		QDocumentCursor c(m_doc, s.startLine);
		c.setSilent(true);
		c.beginEditBlock();

		while ( c.isValid() && (c.lineNumber() <= s.endLine) )
		{
			c.insertText(txt);
			c.movePosition(1, QDocumentCursor::NextLine);
		}

		c.endEditBlock();
	}
}

void QEditor::uncommentSelection()
{
	if ( !highlighter() || highlighter()->singleLineComment().isEmpty() )
		return;

	QString txt = highlighter()->singleLineComment();
	if ( m_mirrors.count() )
	{
		m_doc->beginMacro();

		removeFromStart(m_cursor, txt);

		foreach ( const QDocumentCursor& m, m_mirrors )
			removeFromStart(m, txt);

		m_doc->endMacro();

	} else if ( !m_cursor.hasSelection() ) {
		removeFromStart(m_cursor, txt);
	} else {
		QDocumentSelection s = m_cursor.selection();

		m_doc->beginMacro();

		for ( int i = s.startLine; i <= s.endLine; ++i )
		{
			removeFromStart(QDocumentCursor(m_doc, i), txt);
		}

		m_doc->endMacro();
	}
}

void QEditor::selectAll()
{
	clearCursorMirrors();

	m_cursor.movePosition(1, QDocumentCursor::Start);
	m_cursor.movePosition(1, QDocumentCursor::End, QDocumentCursor::KeepAnchor);

	emitCursorPositionChanged();
	selectionChange(true);

	viewport()->update();
}

void QEditor::paintEvent(QPaintEvent *e)
{
	if ( !m_doc )
		return;

	QPainter p(viewport());
	const int yOffset = verticalOffset();
	const int xOffset = horizontalOffset();

	#ifdef Q_GL_EDITOR
	//QRect r(e->rect());
	QRect r(0, 0, viewport()->width(), viewport()->height());
	#else
	QRect r(e->rect());
	#endif

	p.translate(-xOffset, -yOffset);

	QDocument::PaintContext ctx;
	ctx.xoffset = xOffset;
	ctx.yoffset = r.y() + yOffset;
	ctx.width = viewport()->width();
	ctx.height = qMin(r.height(), viewport()->height());
	ctx.palette = palette();
	ctx.cursors << m_cursor.handle();
	ctx.fillCursorRect = true;
	ctx.blinkingCursor = flag(CursorOn);

	if ( m_cursor.hasSelection() )
	{
		//qDebug("atempting to draw selected text");
		QDocumentSelection s = m_cursor.selection();

		ctx.selections << s;
	}

	// cursor mirrors :D
	foreach ( const QDocumentCursor& m, m_mirrors )
	{
		ctx.cursors << m.handle();

		if ( m.hasSelection() )
		{
			QDocumentSelection s = m.selection();

			ctx.selections << s;
		}
	}

	p.save();
	m_doc->draw(&p, ctx);
	p.restore();

	if ( viewport()->height() > m_doc->height() )
	{
		p.fillRect(	0,
					m_doc->height(),
					viewport()->width(),
					viewport()->height() - m_doc->height(),
					palette().base()
				);
	}
}

void QEditor::timerEvent(QTimerEvent *e)
{
	int id = e->timerId();

	if ( id == m_blink.timerId() )
	{
		bool on = !flag(CursorOn);

		if ( m_cursor.hasSelection() )
			on &= style()->styleHint(QStyle::SH_BlinkCursorWhenTextSelected,
									0,
									this) != 0;

		setFlag(CursorOn, on);

		repaintCursor();

	} else if ( id == m_drag.timerId() ) {
		m_drag.stop();
		//startDrag();
	} else if ( id == m_click.timerId() ) {
		m_click.stop();
	}
}

static int max(const QList<QDocumentCursor>& l)
{
	int ln = 0;

	foreach ( const QDocumentCursor& c, l )
		if ( c.lineNumber() > ln )
			ln = c.lineNumber();

	return ln;
}

static int min(const QList<QDocumentCursor>& l)
{
	// beware the sign bit...
	int ln = 0x7fffffff;

	foreach ( const QDocumentCursor& c, l )
		if ( (c.lineNumber() < ln) || (ln < 0) )
			ln = c.lineNumber();

	return ln;
}

void QEditor::keyPressEvent(QKeyEvent *e)
{
	if ( m_binding )
	{
		if ( m_binding->keyPressEvent(e, this) )
			return;
	}

	bool leave = false;

	// try mirrors bindings first
	if ( (e->modifiers() & Qt::AltModifier) && (e->modifiers() & Qt::ControlModifier) )
	{
		int ln = - 1;
		QDocumentLine l;

		if ( e->key() == Qt::Key_Up )
		{
			ln = m_cursor.lineNumber();

			if ( m_mirrors.count() )
				ln = qMin(ln, min(m_mirrors));

			//qDebug("first %i", ln);

			l = m_doc->line(--ln);
		} else if ( e->key() == Qt::Key_Down ) {
			ln = m_cursor.lineNumber();

			if ( m_mirrors.count() )
				ln = qMax(ln, max(m_mirrors));

			l = m_doc->line(++ln);
		}

		if ( l.isValid() )
		{
			addCursorMirror(QDocumentCursor(m_doc, ln, m_cursor.anchorColumn()));
			repaintCursor();
			emitCursorPositionChanged();
			return;
		}
	}

	selectionChange();

	if ( moveKeyEvent(m_cursor, e, &leave) )
	{
		e->accept();

		setFlag(CursorOn, true);
		//ensureCursorVisible();

		if ( !leave )
			for ( int i = 0; !leave && (i < m_mirrors.count()); ++i )
				moveKeyEvent(m_mirrors[i], e, &leave);

		if ( leave && m_mirrors.count() )
		{
			for ( int i = 0; i < m_mirrors.count(); ++i )
			{
				m_mirrors[i].setAutoUpdated(false);
			}

			clearCursorMirrors();
			viewport()->update();
		} else {
			repaintCursor();
			selectionChange();
		}

		if ( m_mirrors.isEmpty() )
		{
			ensureCursorVisible();
			repaintCursor();
		}

		emitCursorPositionChanged();

		return;
	}

	bool bOk = true;

	if ( m_mirrors.isEmpty() )
	{
		bOk = processCursor(m_cursor, e, bOk);

	} else if ( isProcessingKeyEvent(e) ) {
		// begin macro [synchronization of undo/redo ops]
		m_doc->beginMacro();

		// redo it...
		processCursor(m_cursor, e, bOk);

		for ( int i = 0; bOk && (i < m_mirrors.count()); ++i )
		{
			processCursor(m_mirrors[i], e, bOk);
		}

		// end macro
		m_doc->endMacro();
	} else {
		bOk = false;
	}

	if ( !bOk )
	{
		QAbstractScrollArea::keyPressEvent(e);
		return;
	}

	e->accept();
	emitCursorPositionChanged();
	setFlag(CursorOn, true);
	ensureCursorVisible();
	repaintCursor();
	selectionChange();
}

void QEditor::inputMethodEvent(QInputMethodEvent* e)
{
	if ( m_binding )
	{
		if ( m_binding->inputMethodEvent(e, this) )
			return;
	}

	/*
	if ( m_doc->readOnly() )
	{
		e->ignore();
		return;
	}
	*/

	m_cursor.beginEditBlock();

	if ( m_cursor.hasSelection() )
		m_cursor.removeSelectedText();

	if ( e->commitString().count() )
		m_cursor.insertText(e->commitString());

	m_cursor.endEditBlock();
}

void QEditor::mouseMoveEvent(QMouseEvent *e)
{
	if ( m_binding )
	{
		if ( m_binding->mouseMoveEvent(e, this) )
			return;
	}

	if ( !(e->buttons() & Qt::LeftButton) )
		return;

	if ( !( flag(MousePressed) || m_doubleClick.hasSelection() ) )
		return;

	if ( flag(MaybeDrag) )
	{
		m_drag.stop();

		if (	(e->globalPos() - m_dragPoint).manhattanLength() >
				QApplication::startDragDistance()
			)
			startDrag();

		//emit clearAutoCloseStack();
		return;
	}

	repaintCursor();
	selectionChange();

	const QPoint mousePos = mapToContents(e->pos());

	if ( m_scroll.isActive() )
	{
		if ( viewport()->rect().contains(e->pos()) )
			m_scroll.stop();
	} else {
		if ( !viewport()->rect().contains(e->pos()) )
			m_scroll.start(100, this);
	}

	QDocumentCursor newCursor = cursorForPosition(mousePos);

	if ( newCursor.isNull() )
		return;

	if ( flag(Persistent) )
	{
		//persistent.setPosition(newCursorPos, QTextCursor::KeepAnchor);
	} else if ( e->modifiers() & Qt::ControlModifier ) {

		// get column number for column selection
		int col = newCursor.columnNumber();

		newCursor.setColumnNumber(m_cursor.anchorColumn());

		clearCursorMirrors();
		m_cursor.clearSelection();

		if ( newCursor != m_cursor )
		{
			QDocumentCursor fake(m_cursor);
			fake.setSelectionBoundary(newCursor);

			QDocumentSelection sel = fake.selection();

			for ( int l = sel.startLine; l <= sel.endLine; ++l )
			{
				if ( l != m_cursor.lineNumber() )
					addCursorMirror(QDocumentCursor(m_doc, l,
									m_cursor.anchorColumn()));

			}

			if ( e->modifiers() & Qt::ShiftModifier )
			{
				m_cursor.setColumnNumber(col, QDocumentCursor::KeepAnchor);

				for ( int i = 0; i < m_mirrors.count(); ++i )
					m_mirrors[i].setColumnNumber(col, QDocumentCursor::KeepAnchor);
			}
		}
	} else {
		m_cursor.setSelectionBoundary(newCursor);
		//setFlag(FoldedCursor, isCollapsed());
	}

	selectionChange(true);
	ensureCursorVisible();
	//emit clearAutoCloseStack();
	emitCursorPositionChanged();

	repaintCursor();
}

void QEditor::mousePressEvent(QMouseEvent *e)
{
	if ( m_binding )
	{
		if ( m_binding->mousePressEvent(e, this) )
			return;
	}

	if ( !(e->buttons() & Qt::LeftButton) )
		return;

	QPoint p = mapToContents(e->pos());

	setFlag(MousePressed, true);
	setFlag(MaybeDrag, false);

	repaintCursor();
	selectionChange();

	if ( m_click.isActive() &&
		(( e->globalPos() - m_clickPoint).manhattanLength() <
			QApplication::startDragDistance() ))
	{
#if defined(Q_WS_MAC)
		m_cursor.select(QDocumentCursor::LineUnderCursor);
		m_doubleClick = m_cursor;
#else
		m_cursor.movePosition(1, QDocumentCursor::StartOfBlock);
		m_cursor.movePosition(1, QDocumentCursor::EndOfBlock, QDocumentCursor::KeepAnchor);
#endif

		m_click.stop();
	} else {
		QDocumentCursor cursor = cursorForPosition(p);

		if ( cursor.isNull() )
			return;

		if ( e->modifiers() == Qt::ShiftModifier )
		{
			clearCursorMirrors();
			m_cursor.setSelectionBoundary(cursor);
		} else if ( e->modifiers() & Qt::ControlModifier && ((e->modifiers() & Qt::ShiftModifier) || (e->modifiers() & Qt::AltModifier)) ) {
			//m_mirrors << cursor;
			if ( e->modifiers() & Qt::ShiftModifier )
			{
				int col = cursor.columnNumber();

				cursor.setColumnNumber(m_cursor.anchorColumn());

				clearCursorMirrors();
				m_cursor.clearSelection();

				if ( cursor != m_cursor )
				{
					QDocumentCursor fake(m_cursor);
					fake.setSelectionBoundary(cursor);

					QDocumentSelection sel = fake.selection();

					for ( int l = sel.startLine; l <= sel.endLine; ++l )
					{
						if ( l != m_cursor.lineNumber() )
							addCursorMirror(QDocumentCursor(m_doc, l,
											m_cursor.anchorColumn()));

					}

					m_cursor.setColumnNumber(col, QDocumentCursor::KeepAnchor);

					for ( int i = 0; i < m_mirrors.count(); ++i )
						m_mirrors[i].setColumnNumber(col, QDocumentCursor::KeepAnchor);

				}
			} else if ( (e->modifiers() & Qt::AltModifier) ) {
				addCursorMirror(cursor);
			}
		} else {

			const QDocumentCursor& cur = m_cursor;

			if ( m_cursor.hasSelection() )
			{
				bool inSel = cur.isWithinSelection(cursor);

				if ( !inSel )
				{
					foreach ( const QDocumentCursor& m, m_mirrors )
					{
						inSel = m.isWithinSelection(cursor);

						if ( inSel )
							break;
					}
				}

				if ( inSel )
				{
					setFlag(MaybeDrag, true);

					m_dragPoint = e->globalPos();
					m_drag.start(QApplication::startDragTime(), this);

					return;
				}
			}

// 			m_cursor = cursor;
// 			clearCursorMirrors();
			m_doubleClick = QDocumentCursor();
			setCursor(cursor);
			return;
		}
	}

	ensureCursorVisible();
	//emit clearAutoCloseStack();
	emitCursorPositionChanged();
	repaintCursor();
	selectionChange();
}

void QEditor::mouseReleaseEvent(QMouseEvent *e)
{
	if ( m_binding )
	{
		if ( m_binding->mouseReleaseEvent(e, this) )
			return;
	}

	m_scroll.stop();

	repaintCursor();
	selectionChange();

	if ( flag(MaybeDrag) )
	{
		setFlag(MousePressed, false);
		setCursorPosition(mapToContents(e->pos()));

		if ( flag(Persistent) )
		{
			//persistent.clearSelection();
		} else {
			m_cursor.clearSelection();
			//setFlag(FoldedCursor, isCollapsed());
		}
	}

	if ( flag(MousePressed) )
	{
		setFlag(MousePressed, false);

		setClipboardSelection();
	} else if (	e->button() == Qt::MidButton
				&& QApplication::clipboard()->supportsSelection()) {
		setCursorPosition(mapToContents(e->pos()));
		//setCursorPosition(viewport()->mapFromGlobal(e->globalPos()));

		const QMimeData *md = QApplication::clipboard()
								->mimeData(QClipboard::Selection);

		if ( md )
			insertFromMimeData(md);
	}

	repaintCursor();

	if ( m_drag.isActive() )
		m_drag.stop();

	selectionChange();
}

void QEditor::mouseDoubleClickEvent(QMouseEvent *e)
{
	if ( m_binding )
	{
		if ( m_binding->mouseDoubleClickEvent(e, this) )
			return;
	}

	if ( e->button() != Qt::LeftButton )
	{
		e->ignore();
		return;
	}

	setFlag(MaybeDrag, false);

	repaintCursor();
	selectionChange();
	clearCursorMirrors();
	setCursorPosition(mapToContents(e->pos()));

	//setFlag(FoldedCursor, isCollapsed());

	if ( m_cursor.isValid() )
	{
		if ( flag(Persistent) )
		{
			//persistent.select(QDocumentCursor::WordUnderCursor);
		} else {
			m_cursor.select(QDocumentCursor::WordUnderCursor);
		}

		setClipboardSelection();
		//emit clearAutoCloseStack();
		emitCursorPositionChanged();

		repaintCursor();
		selectionChange();
	} else {
		//qDebug("invalid cursor");
	}

	m_doubleClick = m_cursor;

	m_clickPoint = e->globalPos();
	m_click.start(qApp->doubleClickInterval(), this);
}

void QEditor::dragEnterEvent(QDragEnterEvent *e)
{
	if (
			e
		&&
			e->mimeData()
		&&
			(
				e->mimeData()->hasFormat("text/plain")
			||
				e->mimeData()->hasFormat("text/html")
			)
		&&
			!e->mimeData()->hasFormat("text/uri-list")
		)
		e->acceptProposedAction();
	else
		return;

	m_dragAndDrop = QDocumentCursor();
}

void QEditor::dragLeaveEvent(QDragLeaveEvent *)
{
	const QRect crect = cursorRect(m_dragAndDrop);
	m_dragAndDrop = QDocumentCursor();

	if ( crect.isValid() )
		viewport()->update(crect);

}

void QEditor::dragMoveEvent(QDragMoveEvent *e)
{
	if (
			e
		&&
			e->mimeData()
		&&
			(
				e->mimeData()->hasFormat("text/plain")
			||
				e->mimeData()->hasFormat("text/html")
			)
		&&
			!e->mimeData()->hasFormat("text/uri-list")
		)
		e->acceptProposedAction();
	else
		return;

	QDocumentCursor c = cursorForPosition(mapToContents(e->pos()));

	if ( c.isValid() )
	{
		QRect crect = cursorRect(m_dragAndDrop);

		if ( crect.isValid() )
			viewport()->update(crect);

		m_dragAndDrop = c;

		crect = cursorRect(m_dragAndDrop);
		viewport()->update(crect);
	}

	//e->acceptProposedAction();
}

void QEditor::dropEvent(QDropEvent *e)
{
	m_dragAndDrop = QDocumentCursor();

	QDocumentCursor c(cursorForPosition(mapToContents(e->pos())));

	if ( (e->source() == this) && (m_cursor.isWithinSelection(c)) )
		return;

	if (
			e
		&&
			e->mimeData()
		&&
			(
				e->mimeData()->hasFormat("text/plain")
			||
				e->mimeData()->hasFormat("text/html")
			)
		&&
			!e->mimeData()->hasFormat("text/uri-list")
		&&
			!flag(FoldedCursor)
		)
	{
		e->acceptProposedAction();
	} else {
		return;
	}

	//repaintSelection();

	m_doc->beginMacro();
	//m_cursor.beginEditBlock();

	if (
			(e->dropAction() == Qt::MoveAction)
		&&
			(
				(e->source() == this)
			||
				(e->source() == viewport())
			)
		)
	{
		m_cursor.removeSelectedText();

		for ( int i = 0; i < m_mirrors.count(); ++i )
			m_mirrors[i].removeSelectedText();

	} else {
		qDebug("action : %i", e->dropAction());
		m_cursor.clearSelection();
	}

	clearCursorMirrors();
	m_cursor.moveTo(cursorForPosition(mapToContents(e->pos())));
	insertFromMimeData(e->mimeData());
	//m_cursor.endEditBlock();

	m_doc->endMacro();

	selectionChange();
}

void QEditor::changeEvent(QEvent *e)
{
	QAbstractScrollArea::changeEvent(e);

	if (
			e->type() == QEvent::ApplicationFontChange
		||
			e->type() == QEvent::FontChange
		)
	{
		if ( !m_doc )
			return;

		m_doc->setFont(font());
		//setTabStop(iTab);

	}  else if ( e->type() == QEvent::ActivationChange ) {
		if ( !isActiveWindow() )
			m_scroll.stop();
	}
}

void QEditor::wheelEvent(QWheelEvent *e)
{
	if ( e->modifiers() & Qt::ControlModifier )
	{
		const int delta = e->delta();

		if ( delta > 0 )
			zoom(-1);
		else if ( delta < 0 )
			zoom(1);

		//viewport()->update();

		return;
	}

	QAbstractScrollArea::wheelEvent(e);
	updateMicroFocus();
	//viewport()->update();
}

void QEditor::resizeEvent(QResizeEvent *)
{
	const QSize viewportSize = viewport()->size();

	horizontalScrollBar()->setMaximum(qMax(0, m_doc->width() - viewportSize.width()));
	horizontalScrollBar()->setPageStep(viewportSize.width());

	//viewport()->setAttribute(Qt::WA_OpaquePaintEvent,
	//m_doc->height() >= viewportSize.height());

	verticalScrollBar()->setMaximum(qMax(0, m_doc->height() - viewportSize.height()));
	verticalScrollBar()->setPageStep(viewportSize.height());
}

void QEditor::focusInEvent(QFocusEvent *e)
{
	setFlag(CursorOn, true);
	m_blink.start(QApplication::cursorFlashTime() / 2, this);
	//ensureCursorVisible();

	QAbstractScrollArea::focusInEvent(e);
}

void QEditor::focusOutEvent(QFocusEvent *e)
{
	setFlag(CursorOn, false);
	m_blink.stop();

	QAbstractScrollArea::focusOutEvent(e);
}

void QEditor::contextMenuEvent(QContextMenuEvent *e)
{
	if ( !pMenu )
	{
		e->ignore();
		return;
	}

	selectionChange();

	e->accept();

	pMenu->exec(e->globalPos());
}

bool QEditor::isContentModified() const
{
	return m_doc ? !m_doc->isClean() : false;
}

void QEditor::setContentClean(bool y)
{
	setContentModified(!y);
}

void QEditor::setContentModified(bool y)
{
	setWindowModified(y);
	emit contentModified(y);
}

void QEditor::setFileName(const QString& f)
{
	QString prev = fileName();

	if ( f == prev )
		return;

	m_fileName = f;
	m_name = QFileInfo(f).fileName();

	setTitle(name());
}

void QEditor::setTitle(const QString& title)
{
	QString s(title);

	if ( !s.contains("[*]") )
		s.prepend("[*]");

	setWindowTitle(s);
	emit titleChanged(title);
}

QString QEditor::name() const
{
	return m_name;
}

QString QEditor::fileName() const
{
	return m_fileName;
}

bool QEditor::focusNextPrevChild(bool)
{
	// make sure we catch tabs :)

	return false;
}

void QEditor::startDrag()
{
	setFlag(MousePressed, false);
	QMimeData *data = createMimeDataFromSelection();

	QDrag *drag = new QDrag(this);
	drag->setMimeData(data);

	Qt::DropActions actions = Qt::CopyAction | Qt::MoveAction;
	Qt::DropAction action = drag->start(actions);

	if ( (action == Qt::MoveAction) && (drag->target() != this) )
	{
		m_cursor.removeSelectedText();

		for ( int i = 0; i < m_mirrors.count(); ++i )
			m_mirrors[i].removeSelectedText();
	}
}

bool QEditor::moveKeyEvent(QDocumentCursor& cursor, QKeyEvent *e, bool *leave)
{
	QDocumentCursor::MoveMode mode = e->modifiers() & Qt::ShiftModifier
								? QDocumentCursor::KeepAnchor
								: QDocumentCursor::MoveAnchor;

	QDocumentCursor::MoveOperation op = QDocumentCursor::NoMove;
#ifdef Q_WS_MAC
	// There can be only one modifier (+ shift), but we also need to make sure
	// that we have a "move key" pressed before we reject it.
	bool twoModifiers
		= ((e->modifiers() & (Qt::ControlModifier | Qt::AltModifier))
			== (Qt::ControlModifier | Qt::AltModifier))
		|| ((e->modifiers() & (Qt::ControlModifier | Qt::MetaModifier))
			== (Qt::ControlModifier | Qt::MetaModifier))
		|| ((e->modifiers() & (Qt::AltModifier | Qt::MetaModifier))
			== (Qt::AltModifier | Qt::MetaModifier));
#else
	if (e->modifiers() & (Qt::AltModifier |
		Qt::MetaModifier | Qt::KeypadModifier) )
	{
		e->ignore();
		if ( leave ) *leave = false;
		return false;
	}
#endif

	switch ( e->key() )
	{
#ifndef Q_WS_MAC  // Use the default Windows bindings.
        case Qt::Key_Up:
            op = QDocumentCursor::Up;
            break;
        case Qt::Key_Down:
            op = QDocumentCursor::Down;
            /*
            if (mode == QDocumentCursor::KeepAnchor) {
                QTextBlock block = cursor.block();
                QTextLine line = currentTextLine(cursor);
                if (!block.next().isValid()
                    && line.isValid()
                    && line.lineNumber() == block.layout()->lineCount() - 1)
                    op = QDocumentCursor::End;
            }
            */
            break;
        case Qt::Key_Left:
            op = e->modifiers() & Qt::ControlModifier
                 ? QDocumentCursor::WordLeft
                 : QDocumentCursor::Left;
            break;
        case Qt::Key_Right:
            op = e->modifiers() & Qt::ControlModifier
                 ? QDocumentCursor::WordRight
                 : QDocumentCursor::Right;
            break;
        case Qt::Key_Home:
            op = e->modifiers() & Qt::ControlModifier
                 ? QDocumentCursor::Start
                 : QDocumentCursor::StartOfLine;
            break;
        case Qt::Key_End:
            op = e->modifiers() & Qt::ControlModifier
                 ? QDocumentCursor::End
                 : QDocumentCursor::EndOfLine;
            break;
#else
/*
	Except for pageup and pagedown, Mac OS X has very different behavior, we
	don't do it all, but here's the breakdown:

	Shift still works as an anchor, but only one of the other keys can be dow
	Ctrl (Command), Alt (Option), or Meta (Control).

	Command/Control + Left/Right -- Move to left or right of the line
					+ Up/Down -- Move to top bottom of the file.
					(Control doesn't move the cursor)

	Option	+ Left/Right -- Move one word Left/right.
			+ Up/Down  -- Begin/End of Paragraph.

	Home/End Top/Bottom of file. (usually don't move the cursor, but will select)
*/
        case Qt::Key_Up:
            if (twoModifiers) {
                QApplication::beep();
                if ( leave ) *leave = false;
                return true;
            } else {
                if (e->modifiers() & (Qt::ControlModifier | Qt::MetaModifier))
                    op = QDocumentCursor::Start;
                else if (e->modifiers() & Qt::AltModifier)
                    op = QDocumentCursor::StartOfBlock;
                else
                    op = QDocumentCursor::Up;
            }
            break;
        case Qt::Key_Down:
            if (twoModifiers) {
                QApplication::beep();
                if ( leave ) *leave = false;
                return true;
            } else {
                if (e->modifiers() & (Qt::ControlModifier | Qt::MetaModifier))
                {
                    op = QDocumentCursor::End;
                } else if (e->modifiers() & Qt::AltModifier) {
                    op = QDocumentCursor::EndOfBlock;
                } else {
                    op = QDocumentCursor::Down;
                    /*
                    if (mode == QDocumentCursor::KeepAnchor) {
                        QTextBlock block = cursor.block();
                        QTextLine line = currentTextLine(cursor);
                        if (!block.next().isValid()
                            && line.isValid()
                            && line.lineNumber() ==
                            	block.layout()->lineCount() - 1)
                            op = QDocumentCursor::End;
                    }
                    */
                }
            }
            break;
        case Qt::Key_Left:
            if (twoModifiers) {
                QApplication::beep();
                if ( leave ) *leave = false;
                return true;
            } else {
                if (e->modifiers() & (Qt::ControlModifier | Qt::MetaModifier))
                    op = QDocumentCursor::StartOfLine;
                else if (e->modifiers() & Qt::AltModifier)
                    op = QDocumentCursor::WordLeft;
                else
                    op = QDocumentCursor::Left;
            }
            break;
        case Qt::Key_Right:
            if ( twoModifiers )
            {
                QApplication::beep();
                if ( leave ) *leave = false;
                return true;
            } else {
                if (e->modifiers() & (Qt::ControlModifier | Qt::MetaModifier))
                    op = QDocumentCursor::EndOfLine;
                else if (e->modifiers() & Qt::AltModifier)
                    op = QDocumentCursor::WordRight;
                else
                    op = QDocumentCursor::Right;
            }
            break;
        case Qt::Key_Home:
            if (e->modifiers() & (Qt::ControlModifier |
            	Qt::MetaModifier | Qt::AltModifier) )
            {
                QApplication::beep();
                if ( leave ) *leave = false;
                return true;
            } else {
                op = QDocumentCursor::Start;
            }
            break;
        case Qt::Key_End:
            if (e->modifiers() & (Qt::ControlModifier |
            	Qt::MetaModifier | Qt::AltModifier))
            {
                QApplication::beep();
                if ( leave ) *leave = false;
                return true;
            } else {
                op = QDocumentCursor::End;
            }
            break;
#endif
		case Qt::Key_PageDown:
			if ( leave ) *leave = true;
			pageDown(mode);
			return true;

		case Qt::Key_PageUp:
			if ( leave ) *leave = true;
			pageUp(mode);
			return true;

		case Qt::Key_Insert :
			if ( leave ) *leave = false;
			setFlag(Overwrite, !flag(Overwrite));

			// hack to make sure status panel gets updated...
			// TODO : emit signals on flag change?
			emitCursorPositionChanged();
			return false;

		default:
			return false;
	}

	QDocumentLine prev = cursor.line();

	//const bool moved =
	cursor.movePosition(1, op, mode);

	if ( prev != cursor.line() )
	{
		//moved = true;
		if ( leave ) *leave = true;
	} else {
		//moved = prevcol != cursor.columnNumber();
	}

	return true;
}

void QEditor::pageUp(QDocumentCursor::MoveMode moveMode)
{
	clearCursorMirrors();

	if ( m_cursor.atStart() )
		return;

	int n = viewport()->height() / QDocument::fontMetrics().lineSpacing();

	m_cursor.movePosition(n, QDocumentCursor::Up, moveMode);

	/*
	bool moved = false;
	// move to the targetY using movePosition to keep the cursor's x
	do
	{
		const QRect r = cursorRect();
		y = verticalOffset() + r.y() - r.height();
		moved |= m_cursor.movePosition(1, QDocumentCursor::Up, moveMode);
	} while ( !m_cursor.atStart() && (y > targetY) );
	*/

	ensureCursorVisible();
	emitCursorPositionChanged();
	//updateMicroFocus();
}

void QEditor::pageDown(QDocumentCursor::MoveMode moveMode)
{
	clearCursorMirrors();

	if ( m_cursor.atEnd() )
		return;

	int n = viewport()->height() / QDocument::fontMetrics().lineSpacing();

	m_cursor.movePosition(n, QDocumentCursor::Down, moveMode);

	/*
	bool moved = false;
	int y, targetY = verticalOffset() + 2 * viewport()->height();

	// move to the targetY using movePosition to keep the cursor's x
	do
	{
		y = verticalOffset() + cursorRect().bottom();
		moved |= m_cursor.movePosition(1, QDocumentCursor::Down, moveMode);
	} while ( !m_cursor.atEnd() && (y < targetY) );

	*/

	ensureCursorVisible();
	emitCursorPositionChanged();
}

bool QEditor::isProcessingKeyEvent(QKeyEvent *e)
{
	if ( flag(FoldedCursor) )
		return false;

	switch ( e->key() )
	{
		case Qt::Key_Backspace :
		case Qt::Key_Delete :
		case Qt::Key_Enter :
		case Qt::Key_Return :
			break;

		default :
		{
			QString text = e->text();

			if ( text.isEmpty() || !(text.at(0).isPrint() || (text.at(0) == '\t')) )
				return false;

			break;
		}
	}

	return true;
}

bool QEditor::processCursor(QDocumentCursor& c, QKeyEvent *e, bool& b)
{
	if ( !b )
		return false;

	bool hasSelection = c.hasSelection();

	switch ( e->key() )
	{
		case Qt::Key_Backspace :
			if ( flag(FoldedCursor) )
				return false;

			if ( hasSelection )
				c.removeSelectedText();
			else
				c.deletePreviousChar();

			break;

		case Qt::Key_Delete :
			if ( flag(FoldedCursor) )
				return false;

			if ( hasSelection )

				c.removeSelectedText();
			else
				c.deleteChar();

			//emit clearAutoCloseStack();
			break;

		case Qt::Key_Enter :
		case Qt::Key_Return :
		{
			if ( flag(FoldedCursor) )
				return false;

			c.beginEditBlock();

			if ( hasSelection )
				c.removeSelectedText();
			else if ( flag(Overwrite) )
				c.deleteChar();

			QString indent;

			if ( indenter() )
			{
				indent = indenter()->indent(c);
			} else {
				// default : keep leading ws from previous line...
				QDocumentLine l = c.line();
				const int idx = l.firstChar();

				indent = l.text();

				if ( idx != -1 )
					indent.resize(idx);

			}

			if ( indent.count() )
			{
				indent.prepend("\n");
				c.insertText(indent);
			} else {
				c.insertLine();
			}

			c.endEditBlock();

			break;
		}

		default :
		{
			QString text = e->text();

			if ( text.isEmpty() || !(text.at(0).isPrint() || (text.at(0) == '\t')) )
			{
				b = false;
				return false;
			}

			if ( flag(ReplaceTabs) )
			{
				text.replace("\t", QString(m_doc->tabStop(), ' '));
			}

			c.beginEditBlock();
			insertText(c, text);
			c.endEditBlock();

			break;
		}
	}

	//if ( hasSelection & b )
	selectionChange();

	return true;
}

void QEditor::insertText(QDocumentCursor& c, const QString& text)
{
	bool hasSelection = c.hasSelection();

	if ( hasSelection )
		c.removeSelectedText();

	QKeyEvent fake(QEvent::KeyPress, 0, Qt::NoModifier, text);

	if ( c.columnNumber() && indenter() && indenter()->unindent(c, &fake) )
	{
		int firstNS = 0;
		QString txt = c.line().text();

		while ( (firstNS < txt.length()) && txt.at(firstNS).isSpace() )
			++firstNS;

		const int off = c.columnNumber() - firstNS;

		if ( off > 0 )
			c.movePosition(off, QDocumentCursor::PreviousCharacter);

		//qDebug("%i spaces", firstNS);

		const int ts = m_doc->tabStop();

		do
		{
			--firstNS;
			c.movePosition(1, QDocumentCursor::Left, QDocumentCursor::KeepAnchor);
		} while ( QDocument::screenLength(txt.constData(), firstNS, ts) % ts );

		//qDebug("%i left => \"%s\"", firstNS, qPrintable(c.selectedText()));

		c.removeSelectedText();

		if ( off > 0 )
			c.movePosition(off, QDocumentCursor::NextCharacter);

	}

	if ( !hasSelection && flag(Overwrite) )
		c.deleteChar();

	QStringList lines = text.split('\n', QString::KeepEmptyParts);

	if ( lines.count() == 1 )
	{
		c.insertText(text);
	} else {
		QString indent;

		for ( int i = 0; i < lines.count(); ++i )
		{
			if ( indenter() )
			{
				indent = indenter()->indent(c);
			} else {
				// default : keep leading ws from previous line...
				QDocumentLine l = c.line();
				const int idx = l.firstChar();

				indent = l.text();

				if ( idx != -1 )
					indent.resize(idx);

			}

			if ( indent.count() )
			{
				indent.prepend("\n");
				c.insertText(indent);
			} else {
				c.insertLine();
			}

			c.insertText(lines.at(i));
		}
	}
}

void QEditor::write(const QString& s)
{
	m_doc->beginMacro();

	insertText(m_cursor, s);

	for ( int i = 0; i < m_mirrors.count(); ++i )
		insertText(m_mirrors[i], s);

	m_doc->endMacro();
}

void QEditor::zoom(int n)
{
	if ( !m_doc )
		return;

	QFont f = m_doc->font();
	f.setPointSize(qMax(1, f.pointSize() + n));
	m_doc->setFont(f);
}

void QEditor::selectionChange(bool force)
{
	return;
	// TODO : repaint only selection rect

	if ( false )//force )
	{
		//qDebug("repainting selection... [%i]", force);
		viewport()->update();
	} else if ( m_cursor.hasSelection() ) {
		viewport()->update(selectionRect());
	}

	m_selection = m_cursor.hasSelection();
}

void QEditor::repaintCursor()
{
	if ( m_mirrors.count() )
		viewport()->update();

	QRect r = cursorRect();

	if ( m_crect != r )
	{
		viewport()->update(m_crect.translated(horizontalOffset(), 0));
		m_crect = r;
		viewport()->update(m_crect.translated(horizontalOffset(), 0));
	} else {
		viewport()->update(m_crect.translated(horizontalOffset(), 0));
	}
}

void QEditor::ensureCursorVisible()
{
	ensureVisible(m_cursor.lineNumber());

	int xval = horizontalOffset(),
		xlen = viewport()->width(),
		xpos = m_cursor.line().cursorToX(m_cursor.columnNumber());

	if ( xpos < xval )
	{
		//qDebug("scroll leftward");
		horizontalScrollBar()->setValue(qMax(0, xpos - 4));
	} else if ( xpos > (xval + xlen - 4) ) {
		//qDebug("scroll rightward : %i", xpos - xlen + 4);
		horizontalScrollBar()
			->setValue(qMax(horizontalScrollBar()->value(), xpos - xlen + 4));
	}
}

void QEditor::ensureVisible(int line)
{
	if ( !m_doc )
		return;

	int ypos = m_doc->y(line),
		yval = verticalOffset(),
		ylen = viewport()->height();

	if ( ypos < yval )
		verticalScrollBar()->setValue(ypos);
	else if ( ypos >= (yval + ylen) )
		verticalScrollBar()->setValue(ypos - ylen + QDocument::fontMetrics().lineSpacing());

}

void QEditor::ensureVisible(const QRect &rect)
{
	verticalScrollBar()->setValue(rect.y());
}

QRect QEditor::cursorRect() const
{
	return m_cursor.hasSelection() ? selectionRect() : cursorRect(m_cursor);
}

QRect QEditor::selectionRect() const
{
	if ( !m_cursor.hasSelection() )
		return cursorRect(m_cursor);

	QDocumentSelection s = m_cursor.selection();

	if ( s.startLine == s.endLine )
		return cursorRect(m_cursor);

	int y = m_doc->y(s.startLine);
	QRect r = m_doc->lineRect(s.endLine);
	int height = r.y() + r.height() - y;

	r = QRect(0, y, viewport()->width(), height);
	r.translate(-horizontalOffset(), -verticalOffset());
	return r;
}

QRect QEditor::lineRect(int line) const
{
	if ( !m_doc )
		return QRect();

	/*
	if ( !m_doc )
		return QRect();

	QRect r = m_doc->lineRect(line);
	r.setWidth(viewport()->width());
	r.translate(-horizontalOffset(), -verticalOffset());

	return r;
	*/

	QRect r = m_doc->lineRect(line);
	r.setWidth(viewport()->width());
	r.translate(-horizontalOffset(), -verticalOffset());

	return r;
}

QRect QEditor::lineRect(const QDocumentLine& l) const
{
	//qFatal("bad practice...");

	if ( !m_doc )
		return QRect();

	QRect r = m_doc->lineRect(l);
	r.setWidth(viewport()->width());
	r.translate(-horizontalOffset(), -verticalOffset());

	return r;
}

QRect QEditor::cursorRect(const QDocumentCursor& c) const
{
	return lineRect(c.lineNumber());
}
/*!
	\brief creates a valid QMimeData object depending on the selection
*/
QMimeData* QEditor::createMimeDataFromSelection() const
{
	QMimeData *d = new QMimeData;

	if ( !m_cursor.hasSelection() )
	{
		qWarning("Generated empty MIME data");
		return d;
	}

	if ( m_mirrors.isEmpty() )
	{
		d->setText(m_cursor.selectedText());
	} else {
		QString serialized = m_cursor.selectedText();

		foreach ( const QDocumentCursor& m, m_mirrors )
		{
			serialized += '\n';
			serialized += m.selectedText();
		}

		d->setText(serialized);
		d->setData("text/column-selection", serialized.toLocal8Bit());
	}

	//qDebug("generated selection from : \"%s\"", qPrintable(d->text()));

	return d;
}

/*!
	\brief Inserts the content of a QMimeData object at the cursor position

	\note Only plain text is supported... \see QMimeData::hasText()
*/
void QEditor::insertFromMimeData(const QMimeData *d)
{
	bool s = m_cursor.hasSelection();

	if ( d && m_cursor.isValid() && !d->hasFormat("text/uri-list") )
	{

		if ( d->hasFormat("text/column-selection") )
		{
			clearCursorMirrors();

			QStringList columns = QString::fromLocal8Bit(
										d->data("text/column-selection")
									).split('\n');

			m_doc->beginMacro();

			if ( s )
				m_cursor.removeSelectedText();

			int col = m_cursor.columnNumber();
			m_cursor.insertText(columns.takeFirst());

			QDocumentCursor c = m_cursor;

			while ( columns.count() )
			{
				// check for end of doc and add line if needed...
				c.setColumnNumber(c.line().length());

				if ( c.atEnd() )
					c.insertText("\n");
				else
					c.movePosition(1, QDocumentCursor::NextCharacter);

				// align
				c.setColumnNumber(qMin(col, c.line().length()));

				// copy content of clipboard
				c.insertText(columns.takeFirst());

				addCursorMirror(c);
			}

			m_doc->endMacro();

		} else {
			m_doc->beginMacro();

			if ( s )
			{
				m_cursor.removeSelectedText();
			}

			QString txt;

			if ( d->hasFormat("text/plain") )
				txt = d->text();
			else if ( d->hasFormat("text/html") )
				txt = d->html();

			m_cursor.insertText(txt);

			for ( int i = 0; i < m_mirrors.count(); ++i )
			{
				m_mirrors[i].insertText(txt);
			}

			m_doc->endMacro();
		}

		ensureCursorVisible();
		setFlag(CursorOn, true);

		emitCursorPositionChanged();
	}
}

void QEditor::clearCursorMirrors()
{
	for ( int i = 0; i < m_mirrors.count(); ++i )
	{
		m_mirrors[i].setAutoUpdated(false);
	}

	m_mirrors.clear();
}

void QEditor::addCursorMirror(const QDocumentCursor& c)
{
	if ( c.isNull() || (c == m_cursor) || m_mirrors.contains(c) )
		return;

	m_mirrors << c;

	// necessary for smooth mirroring
	m_mirrors.last().setSilent(true);
	m_mirrors.last().setAutoUpdated(true);
}

void QEditor::setClipboardSelection()
{
	QClipboard *clipboard = QApplication::clipboard();

	if ( !clipboard->supportsSelection() )
		return;

	if ( //( flag(Persistent) && !persistent.hasSelection() ) ||
		( !flag(Persistent) && !m_cursor.hasSelection() ) )
		return;

	QMimeData *data = createMimeDataFromSelection();

	clipboard->setMimeData(data, QClipboard::Selection);
}

void QEditor::scrollContentsBy(int dx, int dy)
{
	#ifdef Q_GL_EDITOR
	viewport()->update();
	#else
	viewport()->scroll(dx, dy);
	#endif
}

void QEditor::documentWidthChanged(int newWidth)
{
	int nv = qMax(0, newWidth - viewport()->width());

	horizontalScrollBar()->setMaximum(nv);

	ensureCursorVisible();
}

void QEditor::documentHeightChanged(int newHeight)
{
	verticalScrollBar()->setMaximum(qMax(0, newHeight - viewport()->height()));
}

void QEditor::repaintContent(int i, int n)
{
	if ( !m_doc || !n )
		return;

	#ifdef Q_GL_EDITOR
	viewport()->update();
	#else
	QDocumentLine l = m_doc->line(i);
	//const int docPos = m_doc->y(l);
	QRect rect = m_doc->lineRect(i);
	const int yoff = verticalOffset() + viewport()->height();

	if ( rect.y() > yoff || (rect.y() + n * rect.height()) < verticalOffset() )
		return;

	//qDebug("repainting %i lines starting from %ith one", n, i);

	//rect.setWidth(viewport()->width());
	//rect.setHeight(qMin(viewport()->height(), rect.height() * n));

	rect = QRect(
				rect.x(),
				rect.y() - verticalOffset(),
				viewport()->width(),
					(n < 0)
				?
					viewport()->height()
				:
					qMin(viewport()->height(), rect.height() * n)
			);

	/*
	if ( n > 1 )
	{
		rect.setHeight(viewport()->height());

		viewport()->update();
		return;
	}
	*/

	viewport()->update(rect);
	#endif
}

void QEditor::updateContent (int i, int n)
{
	if ( !m_doc )
		return;

	bool cont = n > 1;

	if ( m_highlighter )
		cont |= m_highlighter->reformat(i, n);

	if ( cont )
	{
		//qDebug("shall extend repaint...");
		repaintContent(i, -1);
		return;
	}

	repaintContent(i, n);
}

void QEditor::markChanged(QDocumentLineHandle *l, int mark, bool on)
{
	emit markChanged(fileName(), l, mark, on);
}
