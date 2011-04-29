/*
 XINX
 Copyright (C) 2007-2011 by Ulrich Van Den Hekke
 xinx@shadoware.org

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful.
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// Xinx header
#include "editors/xinxcodeedit_p.h"
#include "core/xinxconfig.h"
#include "editors/xinxlanguagefactory.h"
#include "editors/xinxformatscheme.h"
#include "snipets/snipetmanager.h"
#include <contentview3/node.h>
#include <codecompletion/model.h>
#include <codecompletion/completer.h>
#include <editors/textfileeditor.h>

// Qt header
#include <QHBoxLayout>
#include <QAction>
#include <QApplication>
#include <QMimeData>
#include <QClipboard>
#include <QCompleter>
#include <QMouseEvent>
#include <QAbstractItemView>
#include <QDebug>

// QCodeEdit header
#include <qpanellayout.h>
#include <qdocument.h>
#include <qdocumentsearch.h>
#include <qdocumentline.h>
#include <qlinemarkpanel.h>
#include <qlinenumberpanel.h>
#include <qfoldpanel.h>
#include <qlinechangepanel.h>
#include <qdocument_p.h>
#include <qlanguagedefinition.h>
#include <qlinemarksinfocenter.h>

/*!
 * Definition of the characters that can't be in a word.
 */
#define EOW             "~!@$#%^&*()+{}|\"<>?,/;'[]\\="
#define EOWREGEXP       "[^A-Za-z0-9_\\-]"
#define EOWREGEXPDOT    "[^A-Za-z0-9_\\-\\.]"


/* XinxCodeEdit */

/*!
 * \class XinxCodeEdit
 * \brief XinxCodeEdit is a wrapper of a QCodeEdit editor.
 *
 * This editor redefine different function used in QTextEdit and TextEditor.
 *
 * \todo add method : undo(), redo(), cut(), copy(), paste()
 * \todo delete \e find method, we use QDocumentSearch.
 */

/*!
 * \fn void XinxCodeEdit::modelUpdated(QAbstractItemModel * model)
 * The model is destroyed or created and need to be updated in the content dialog
 */
/*!
 * \fn void XinxCodeEdit::searchWord(const QString & word)
 * The user (By pressing Ctrl+Click) search a word in the document or another.
 */
/*!
 * \fn void XinxCodeEdit::bookmarkToggled(int line, bool enabled)
 * Signal is emitted when a bookmark is toogled.
 * \param line Line of the bookmark
 * \param enabled If enabled, the bookmark is added.
 */
/*!
 * \fn void XinxCodeEdit::copyAvailable(bool y)
 * The signal is emited when the state change. \e y is true if copy is available
 */
/*!
 * \fn void XinxCodeEdit::undoAvailable(bool y)
 * The signal is emited when the state change. \e y is true if undo is available
 */
/*!
 * \fn void XinxCodeEdit::redoAvailable(bool y)
 * The signal is emited when the state change. \e y is true if redo is available
 */

/*!
 * \enum XinxCodeEdit::FindFlag
 * XinxCodeEdit::FindFlag is used in find function.
 */


/*!
 * Create a XinxCodeEdit object.
 *
 * This create the QCodeEdit object with some default option.
 */
XinxCodeEdit::XinxCodeEdit(QWidget * parent) : QWidget(parent), _text_file_editor(0), m_completer(0)
{
	init(false);
}

/*!
 * Create a XinxCodeEdit object and active action for the editor.
 *
 * This create the QCodeEdit object with some default option.
 */
XinxCodeEdit::XinxCodeEdit(bool action, QWidget * parent) : QWidget(parent), _text_file_editor(0), m_completer(0)
{
	init(action);
}

void XinxCodeEdit::init(bool action)
{
	m_editor = new QCodeEdit(action, this);
	setHighlighter(QString());

	m_editor->editor()->document()->setLineEnding(QDocument::Unix);
	m_editor->editor()->setAcceptDrops(true);
	m_editor->editor()->setWindowTitle("[*]");
	m_editor->editor()->addInputBinding(this);
	m_editor->editor()->setInputBinding(this);
	m_editor->editor()->setFrameShape(QFrame::NoFrame);

	QDocumentSearch::Options opt = QDocumentSearch::Silent | QDocumentSearch::HighlightAll;
	m_matchingText = new QDocumentSearch(m_editor->editor(), QString(), opt);

	m_editor->addPanel(new QLineMarkPanel, QCodeEdit::West, true);
	m_editor->addPanel(new QLineNumberPanel, QCodeEdit::West, true);
	m_editor->addPanel(new QFoldPanel, QCodeEdit::West, true);
	m_editor->addPanel(new QLineChangePanel, QCodeEdit::West, true);

	QHBoxLayout * layout = new QHBoxLayout;
	layout->addWidget(m_editor->editor());
	layout->setSpacing(0);
	layout->setMargin(0);
	setLayout(layout);

	connect(XINXConfig::self(), SIGNAL(changed()), this, SLOT(updateHighlighter()));
	connect(XINXConfig::self(), SIGNAL(changed()), this, SLOT(updateFont()));
	connect(m_editor->editor(), SIGNAL(cursorPositionChanged()), this, SLOT(refreshTextHighlighter()));
	connect(m_editor->editor(), SIGNAL(copyAvailable(bool)), this, SIGNAL(copyAvailable(bool)));
	connect(m_editor->editor(), SIGNAL(redoAvailable(bool)), this, SIGNAL(redoAvailable(bool)));
	connect(m_editor->editor(), SIGNAL(undoAvailable(bool)), this, SIGNAL(undoAvailable(bool)));
	connect(m_editor->editor()->document(), SIGNAL(markChanged(QDocumentLineHandle*,int,bool)), this, SLOT(slotMarkChanged(QDocumentLineHandle*,int,bool)));

	updateFont();
}

//! Destroy the object
XinxCodeEdit::~XinxCodeEdit()
{
	delete m_matchingText;
	m_editor->editor()->setInputBinding(0);
	m_editor->editor()->removeInputBinding(this);
	delete m_editor;
}

/*! Set the filename used by callSnipet */
void XinxCodeEdit::setFilename(const QString & filename)
{
	m_filename = filename;
}

/*! Get the filename set to the editor */
const QString & XinxCodeEdit::filename() const
{
	return m_filename;
}

bool XinxCodeEdit::isModified()
{
	return m_editor->editor()->document()->isClean();
}

//! Return the current column of the cursor
int XinxCodeEdit::currentColumn()
{
	return m_editor->editor()->cursor().columnNumber();
}

//! Return the current row of the cursor
int XinxCodeEdit::currentRow()
{
	return m_editor->editor()->cursor().lineNumber();
}

//! Return the number of row of the editor
int XinxCodeEdit::countRow()
{
	return document()->lines();
}

//! Returns a pointer to the underlying document.
QDocument * XinxCodeEdit::document() const
{
	return m_editor->editor()->document();
}

//! Return the QCodeEdit editor
QEditor * XinxCodeEdit::editor() const
{
	return m_editor->editor();
}

void XinxCodeEdit::slotMarkChanged(QDocumentLineHandle* line, int type, bool enabled)
{
	Q_UNUSED(type);

	emit bookmarkToggled(line->line(), enabled);
}

/*! Clear all bookrmark */
void XinxCodeEdit::clearBookmark()
{
	int bid = QLineMarksInfoCenter::instance()->markTypeId("bookmark");
	int mark = document()->findNextMark(bid);
	while (mark != -1)
	{
		emit bookmarkToggled(mark + 1, false);
		document()->line(mark).removeMark(bid);

		mark = document()->findNextMark(bid, mark + 1);
	}
}

/*! Previous mark */
bool XinxCodeEdit::previousBookmark()
{
	int bid = QLineMarksInfoCenter::instance()->markTypeId("bookmark");
	int mark = document()->findPreviousMark(bid, currentRow() - 1);
	if (mark == -1) return false;
	gotoLine(mark + 1);
	return true;
}

/*! Next bookmark */
bool XinxCodeEdit::nextBookmark()
{
	int bid = QLineMarksInfoCenter::instance()->markTypeId("bookmark");
	int mark = document()->findNextMark(bid, currentRow() + 1);
	if (mark == -1) return false;
	gotoLine(mark + 1);
	return true;
}

/*! Show icon bookmark at line \e line. */
void XinxCodeEdit::setBookmark(int line, bool enabled)
{
	int bid = QLineMarksInfoCenter::instance()->markTypeId("bookmark");
	QDocumentLine documentLine = m_editor->editor()->document()->line(line - 1);
	if (enabled)
		documentLine.addMark(bid);
	else
		documentLine.removeMark(bid);

	emit bookmarkToggled(line, enabled);
}

/*! List of bookmark of the editor. */
QList<int> XinxCodeEdit::listOfBookmark()
{
	QList<int> bookmarks;

	int bid = QLineMarksInfoCenter::instance()->markTypeId("bookmark");
	int mark = document()->findNextMark(bid);
	while (mark != -1)
	{
		bookmarks.append(mark + 1);

		mark = document()->findNextMark(bid, mark + 1);
	}

	return bookmarks;
}

/*! Set the list of error. */
void XinxCodeEdit::setErrors(QList<int> errors)
{
	QList<int> bookmarks;

	int bid = QLineMarksInfoCenter::instance()->markTypeId("error");

	// Remove old mark
	int mark = document()->findNextMark(bid);
	while (mark != -1)
	{
		QDocumentLine line = m_editor->editor()->document()->line(mark);
		line.removeMark(bid);

		mark = document()->findNextMark(bid, mark + 1);
	}

	// Activate new marks
	foreach(mark, errors)
	{
		QDocumentLine line = m_editor->editor()->document()->line(mark);
		line.addMark(bid);
	}
}

/*!
 * Finds the next occurrence of the string, \e subString, in the document. The search starts at
 * the position of the given \e cursor, and proceeds forwards through the document unless
 * specified otherwise in the search options. The \e options control the type of search performed.
 *
 * Returns a cursor with the match selected if \e subString was found; otherwise returns a
 * null cursor.
 *
 * If the given cursor has a selection, the search begins after the selection; otherwise it
 * begins at the cursor's position.
 *
 * By default the search is case-sensitive, and can match text anywhere in the document.
 *
 * \overload
 */
QDocumentCursor XinxCodeEdit::find(const QString & subString, const QDocumentCursor & cursor, XinxCodeEdit::FindFlags options) const
{
	QDocumentSearch::Options opt = QDocumentSearch::Silent;
	if (options.testFlag(FindCaseSensitively)) opt |= QDocumentSearch::CaseSensitive;
	if (options.testFlag(FindWholeWords)) opt |= QDocumentSearch::WholeWords;
	QDocumentSearch searchEngine(m_editor->editor(), subString, opt);

	searchEngine.setCursor(cursor);
	searchEngine.next(options.testFlag(FindBackward));
	return searchEngine.cursor();
}

/*!
 * Finds the next occurrence, matching the regular expression \e expr, in the document.
 *
 * The search starts at the position of the given \e cursor, and proceeds forwards through
 * the document unless specified otherwise in the search \e options. The options control
 * the type of search performed. The FindCaseSensitively option is ignored for this overload,
 * use QRegExp::caseSensitivity instead.
 *
 * Returns a cursor with the match selected if a match was found; otherwise returns a null
 * cursor.
 *
 * If the given cursor has a selection, the search begins after the selection; otherwise
 * it begins at the cursor's position.
 *
 * By default the search is case-sensitive, and can match text anywhere in the document.
 *
 * \overload
 */
QDocumentCursor XinxCodeEdit::find(const QRegExp & expr, const QDocumentCursor & cursor, XinxCodeEdit::FindFlags options) const
{
	QDocumentSearch::Options opt = QDocumentSearch::Silent | QDocumentSearch::RegExp | QDocumentSearch::CaseSensitive;
	if (options.testFlag(FindWholeWords)) opt |= QDocumentSearch::WholeWords;
	QDocumentSearch searchEngine(m_editor->editor(), expr.pattern(), opt);

	searchEngine.setCursor(cursor);
	searchEngine.next(options.testFlag(FindBackward));
	return searchEngine.cursor();
}

/*!
 * Finds the next occurrence of the string \e exp, using the given \e options. Returns true
 * if \e exp was found and changes the cursor to select the match; otherwise returns false.
 *
 * \overload
 */
bool XinxCodeEdit::find(const QString & exp, XinxCodeEdit::FindFlags options)
{
	QDocumentCursor c = find(exp, m_editor->editor()->cursor(), options);
	return ! c.isNull();
}

/*!
 * Return the text who is under the cursor.
 * \param cursor The cursor to use to look the text.
 * \param deleteWord If true, \e textUnderCursor will remove the text returned.
 * \param dot If true '.' is considered in a word
 */
QString XinxCodeEdit::textUnderCursor(const QDocumentCursor & cursor, bool deleteWord, bool dot)
{
	Q_ASSERT(! cursor.isNull());
	QString expr = EOWREGEXPDOT;
	if (! dot)
		expr = EOWREGEXP;

	QDocumentCursor before(find(QRegExp(expr), cursor, XinxCodeEdit::FindBackward).selectionEnd());
	QDocumentCursor after(find(QRegExp(expr), cursor).selectionStart());

	QDocumentCursor tc = cursor;

	if ((! before.isNull()) && (before.lineNumber() == tc.lineNumber()))
	{
		tc.moveTo(before);
	}
	else
		tc.movePosition(1, QDocumentCursor::StartOfBlock, QDocumentCursor::MoveAnchor) ;

	if ((! after.isNull()) && (after.lineNumber() == tc.lineNumber()))
		tc.movePosition(after.position() - (before.position() == -1 ? 0 : before.position()), QDocumentCursor::Right, QDocumentCursor::KeepAnchor) ;
	else
		tc.movePosition(1, QDocumentCursor::EndOfBlock, QDocumentCursor::KeepAnchor) ;

	QString selection = tc.selectedText().trimmed();

	if ((! tc.selectedText().trimmed().isEmpty()) && deleteWord)
	{
		tc.removeSelectedText();
		m_editor->editor()->setCursor(tc);
	}

	return selection;
}

/*!
 * Sets the visible cursor.
 * \sa textCursor()
 */
void XinxCodeEdit::setTextCursor(const QDocumentCursor & cursor)
{
	m_editor->editor()->setCursor(cursor);
}

/*!
 * Returns a copy of the QTextCursor that represents the currently visible cursor.
 * Note that changes on the returned cursor do not affect QTextEdit's cursor;
 * use setTextCursor() to update the visible cursor.
 *
 * \sa setTextCursor()
 */
QDocumentCursor XinxCodeEdit::textCursor() const
{
	return m_editor->editor()->cursor();
}

/*!
 * Moves the cursor by performing the given operation.
 *
 * If mode is QTextCursor::KeepAnchor, the cursor selects the text it moves over.
 * This is the same effect that the user achieves when they hold down the Shift key
 * and move the cursor with the cursor keys.
 */
void XinxCodeEdit::moveCursor(QDocumentCursor::MoveOperation operation, QDocumentCursor::MoveMode mode)
{
	QDocumentCursor c = m_editor->editor()->cursor();
	c.movePosition(1, operation, mode);
	m_editor->editor()->setCursor(c);
}

//! Return the content of the current selection or all the text if there is no selection.
QString XinxCodeEdit::selection() const
{
	if (m_editor->editor()->cursor().hasSelection())
		return m_editor->editor()->cursor().selectedText();
	else
		return toPlainText();
}

//! Return the content of the editor
QString XinxCodeEdit::toPlainText() const
{
	return m_editor->editor()->text();
}

//! Set the text to \e text
void XinxCodeEdit::setPlainText(const QString & text)
{
	m_editor->editor()->cursor().movePosition(1, QDocumentCursor::Start);
	m_editor->editor()->setText(text);
	m_editor->editor()->document()->setClean();
	m_editor->editor()->update();
}

//! Set the selection to \e text, or replace all the text if there is no selection.
void XinxCodeEdit::setSelection(QString text)
{
	if (m_editor->editor()->cursor().hasSelection())
	{
		m_editor->editor()->cursor().beginEditBlock();
		m_editor->editor()->cursor().removeSelectedText();
		m_editor->editor()->cursor().insertText(text);
		m_editor->editor()->cursor().endEditBlock();
	}
	else
	{
		m_editor->editor()->cursor().beginEditBlock();
		m_editor->editor()->selectAll();
		m_editor->editor()->cursor().removeSelectedText();
		m_editor->editor()->cursor().movePosition(1, QDocumentCursor::Start);
		m_editor->editor()->cursor().insertText(text);
		m_editor->editor()->cursor().endEditBlock();
	}
}

//! Set the highlight text with \e text. All text equals are highlighted.
void XinxCodeEdit::setMatchingText(QString text)
{
	if (m_matchingTextString != text)
	{
		QDocumentCursor cursor;
		cursor.setSilent(true);
		if (! text.isEmpty())
		{
			m_matchingText->setCursor(cursor);
			m_matchingText->setSearchText(text);
			m_matchingText->next(false);
		}
		else
		{
			delete m_matchingText;
			QDocumentSearch::Options opt = QDocumentSearch::Silent | QDocumentSearch::HighlightAll;
			m_matchingText = new QDocumentSearch(m_editor->editor(), QString(), opt);
		}
		m_matchingTextString = text;
	}
}

//! Set the completer \e completer to the editor
void XinxCodeEdit::setCompleter(CodeCompletion::Completer * completer)
{
	if (completer != m_completer)
	{
		if (m_completer) m_completer->disconnect(this);
		completer->setWidget(m_editor->editor());
		connect(completer, SIGNAL(activated(const QModelIndex &)), this, SLOT(insertCompletion(const QModelIndex &)));
		m_completer = completer;
	}
}

//! Return the editor setted.
CodeCompletion::Completer * XinxCodeEdit::completer()
{
	return m_completer;
}

//! Set the current file used to read completion framework
void XinxCodeEdit::setFile(ContentView3::FilePtrWeak file)
{
	_file = file;
}

//! Get the current file
ContentView3::FilePtrWeak XinxCodeEdit::file() const
{
	return _file;
}

//! Define the text file editor used has parent of the XinxCodeEdit.
void XinxCodeEdit::setTextFileEditor(TextFileEditor* editor)
{
	_text_file_editor = editor;
}

//! Get the text file editor used has parent of the XinxCodeEdit
TextFileEditor* XinxCodeEdit::textFileEditor() const
{
	return _text_file_editor;
}

//! Returns whether text can be pasted from the clipboard into the textedit.
bool XinxCodeEdit::canPaste()
{
	const QMimeData *d = QApplication::clipboard()->mimeData();
	return d && d->hasText();
}

/*!
 * Convenience function to print the text edit's document to the given printer.
 * This is equivalent to calling the print method on the document directly except
 * that this function also supports QPrinter::Selection as print range.
 */
void XinxCodeEdit::print(QPrinter * printer) const
{
	document()->print(printer);
}

//! Change the document state (modified or not) of the document
void XinxCodeEdit::setModified(bool modified)
{
	if (! modified)
		document()->setClean();
	else
	{
		//! \todo Find a solution to setModified(true)
		qDebug("Call of setModified( true ) but not managed");
	}
}

/*!
 * Set the number of space for a tabulation.
 * Warning: This value is global in the application. This is a limitation of QCodeEdit.
 *
 * \sa tabStopWidth()
 */
void XinxCodeEdit::setTabStopWidth(int width)
{
	m_editor->editor()->document()->setTabStop(width);
}

/*!
 * Get the number of space for a tabulation.
 * Warning: This value is global in the application. This is a limitation of QCodeEdit.
 *
 * \sa setTabStopWidth()
 */
int XinxCodeEdit::tabStopWidth() const
{
	return m_editor->editor()->document()->tabStop();
}

//! Change the read only property of the editor
void XinxCodeEdit::setReadOnly(bool readonly)
{
	m_editor->editor()->setFlag(QEditor::ReadOnly, readonly);
}

/*! Return true if the editor is read only */
bool XinxCodeEdit::isReadOnly() const
{
	return m_editor->editor()->flag(QEditor::ReadOnly);
}

//! Change the highlighter to \e highlighter
void XinxCodeEdit::setHighlighter(const QString & highlighter)
{
	setHighlighter(highlighter, XINXConfig::self());
}

//! Change the highlighter to \e highlighter but with color settings of \e config
void XinxCodeEdit::setHighlighter(const QString & highlighter, XINXConfig * config)
{
	if (highlighter.isEmpty())
	{
		document()->setFormatScheme(config->languageFactory()->defaultFormatScheme());
		config->languageFactory()->setLanguage(m_editor->editor(), "None");
		return;
	}

	QFormatScheme * scheme = config->scheme(highlighter);
	if (! scheme)
		scheme = config->languageFactory()->defaultFormatScheme();
	document()->setFormatScheme(scheme);

	config->languageFactory()->setLanguage(m_editor->editor(), highlighter);
}

/*!
 * Called when the configuration change and it's necessary to update the highlighter.
 * If no highlighter is used, this function do nothing.
 */
void XinxCodeEdit::updateHighlighter()
{
	document()->setFormatScheme(document()->formatScheme());
}

//! Update the font (when the configuration change)
void XinxCodeEdit::updateFont()
{
	QFont font = XINXConfig::self()->config().editor.defaultFormat;
	QFontMetrics fm(font);
	QDocument::setFont(font);
	QDocument::setTabStop(XINXConfig::self()->config().editor.tabulationSize);

	if (XINXConfig::self()->config().editor.showTabulationAndSpace)
		QDocument::setShowSpaces(QDocument::ShowTrailing | QDocument::ShowLeading | QDocument::ShowTabs);
	else
		QDocument::setShowSpaces(QDocument::ShowNone);

	m_editor->editor()->setLineWrapping(XINXConfig::self()->config().editor.wrapLine);
}

//! Insert the selection where the cursor is (and replace the selection if any). This method indent the text.
void XinxCodeEdit::insertText(const QString & text)
{
	QDocumentCursor cursor = textCursor();

	QString indent = cursor.line().previous().text();
	indent = indent.left(indent.indexOf(QRegExp("\\S")));

	QStringList lines = text.split("\n", QString::KeepEmptyParts);

	QStringListIterator i(lines);
	if (i.hasNext())
	{
		cursor.insertText(i.next());
		if (i.hasNext())
			cursor.insertLine();
	}

	while (i.hasNext())
	{
		cursor.insertText(indent + i.next());
		if (i.hasNext())
			cursor.insertLine();
	}
	setTextCursor(cursor);
}

/*! Insert the completion based on the QCompleter */
void XinxCodeEdit::insertCompletion(const QModelIndex& index)
{
	if (m_completer)
	{
		m_completer->complete(index);
	}
}

/*! In the editor go to the line \e line. */
void XinxCodeEdit::gotoLine(int line)
{
	/* Five line before */
	QDocumentCursor cursorB(m_editor->editor()->document(), line - 5);
	if (!cursorB.isNull())
		m_editor->editor()->setCursor(cursorB);

	/* Five line after */
	QDocumentCursor cursorA(m_editor->editor()->document(), line + 5);
	if (!cursorA.isNull())
		m_editor->editor()->setCursor(cursorA);

	QDocumentCursor cursor(m_editor->editor()->document(), line - 1);
	if (!cursor.isNull())
		m_editor->editor()->setCursor(cursor);
}

/*! Duplicate the current line or the current selection in the editor. */
void XinxCodeEdit::duplicateLines()
{
	QDocumentCursor cursor(textCursor());
	QString text;
	if (cursor.hasSelection())
		text = cursor.selectedText();
	else
		text = cursor.line().text();

	if ((!cursor.hasSelection()) && cursor.line() == cursor.selectionStart().line() && cursor.line() == cursor.selectionEnd().line())
		text += "\n";


	QDocumentCursor selStart = cursor.selectionStart();
	cursor.beginEditBlock();
	cursor.moveTo(selStart);
	if (text.contains('\n'))
		cursor.movePosition(0, QDocumentCursor::StartOfLine);
	cursor.insertText(text);
	cursor.movePosition(text.length(), QDocumentCursor::Right, QDocumentCursor::KeepAnchor);
	cursor.endEditBlock();
	setTextCursor(cursor);
}

/*!
 * Move the current line up.
 * Swap the current line with the line above.
 * \sa moveLineDown()
 */
void XinxCodeEdit::moveLineUp()
{
	QDocumentCursor cursor(textCursor());

	int selectionLength = 0;
	QDocumentCursor selectionStart      = cursor.selectionStart(),
										  selectionEnd         = cursor.selectionEnd();
	selectionStart.setAutoUpdated(false);
	selectionEnd.setAutoUpdated(false);

	if (selectionEnd.isValid()) selectionLength = cursor.selectedText().length();

	if (selectionEnd.columnNumber() == 0) selectionEnd.movePosition(1, QDocumentCursor::Left);
	if (selectionStart.lineNumber() == 0) return;  // No move when we are at the first line

	cursor.beginEditBlock();

	QString text;
	QDocumentLine currentLine = selectionStart.line();
	cursor.moveTo(currentLine, 0);
	do
	{
		text += currentLine.text() + "\n";
		cursor.movePosition(1, QDocumentCursor::NextLine, QDocumentCursor::KeepAnchor);

		currentLine = currentLine.next();
	}
	while (currentLine.isValid() && (currentLine <= selectionEnd.line()));

	cursor.removeSelectedText();

	cursor.movePosition(1, QDocumentCursor::PreviousLine, QDocumentCursor::MoveAnchor);
	cursor.movePosition(1, QDocumentCursor::StartOfLine, QDocumentCursor::MoveAnchor);
	cursor.insertText(text);

	cursor.moveTo(selectionStart.lineNumber() - 1, selectionStart.columnNumber());
	if (selectionLength > 0)
		cursor.movePosition(selectionLength, QDocumentCursor::Right, QDocumentCursor::KeepAnchor);

	cursor.endEditBlock();
	setTextCursor(cursor);
}

/*!
 * Move the current line down.
 * Swap the current line with the line after.
 * \sa moveLineUp()
 */
void XinxCodeEdit::moveLineDown()
{
	QDocumentCursor cursor(textCursor());

	int selectionLength = 0;
	QDocumentCursor selectionStart      = cursor.selectionStart(),
										  selectionEnd         = cursor.selectionEnd();
	if (selectionEnd.isValid()) selectionLength = cursor.selectedText().length();

	if (selectionEnd.columnNumber() == 0) selectionEnd.movePosition(1, QDocumentCursor::Left);
	if (((selectionEnd.lineNumber() + 1) == m_editor->editor()->document()->lines()) || ((selectionStart.lineNumber() + 1) == m_editor->editor()->document()->lines())) return;          // No move at end of document

	cursor.beginEditBlock();

	QString text;
	QDocumentLine currentLine = selectionStart.line();
	cursor.moveTo(currentLine, 0);
	do
	{
		text += currentLine.text() + "\n";
		cursor.movePosition(1, QDocumentCursor::NextLine, QDocumentCursor::KeepAnchor);

		currentLine = currentLine.next();
	}
	while (currentLine.isValid() && (currentLine <= selectionEnd.line()));

	cursor.removeSelectedText();

	if ((cursor.lineNumber() + 1) < m_editor->editor()->document()->lines())
	{
		cursor.movePosition(1, QDocumentCursor::NextLine, QDocumentCursor::MoveAnchor);
		cursor.movePosition(1, QDocumentCursor::StartOfLine, QDocumentCursor::MoveAnchor);
	}
	else
	{
		cursor.movePosition(1, QDocumentCursor::EndOfLine, QDocumentCursor::MoveAnchor);
		cursor.insertText("\n");
		text.chop(1);
	}
	cursor.insertText(text);

	cursor.moveTo(selectionStart.lineNumber() + 1, selectionStart.columnNumber());
	if (selectionLength > 0)
		cursor.movePosition(selectionLength, QDocumentCursor::Right, QDocumentCursor::KeepAnchor);

	cursor.endEditBlock();
	setTextCursor(cursor);
}

void XinxCodeEdit::uploSelectedText(bool upper)
{
	QDocumentCursor cursor(textCursor());

	QDocumentCursor startPos = cursor.selectionStart();
	QDocumentCursor endPos   = cursor.selectionEnd();

	QString text = cursor.selectedText();

	if (upper)
		text = text.toUpper();
	else
		text = text.toLower();

	cursor.beginEditBlock();

	cursor.removeSelectedText();
	cursor.insertText(text);

	cursor.moveTo(startPos);
	cursor.movePosition(endPos.position() - startPos.position(), QDocumentCursor::NextCharacter, QDocumentCursor::KeepAnchor);

	cursor.endEditBlock();

	setTextCursor(cursor);
}

/*! Replace the selected text by upper case character the parameter. */
void XinxCodeEdit::upperSelectedText()
{
	uploSelectedText(true);
}

/*! Replace the selected text by lower case character. */
void XinxCodeEdit::lowerSelectedText()
{
	uploSelectedText(false);
}

/*!
 * Indent or unindent the selected text depending on the parameter.
 * \param unindent If false (by default) the text is indented. (the character \\t is added), else the text is unindented.
 */
void XinxCodeEdit::indent(bool unindent)
{
	if (! unindent)
		m_editor->editor()->indentSelection();
	else
		m_editor->editor()->unindentSelection();
}

/*!
 * Comment or Uncomment the selected text depending on the parrameter.
 * If a part of a text is already (un)commented, the balise is moved to comment all the text.
 * Warning: If you comment code with comment, the comment can be merged with code.
 * \param uncomment If false (by default) the text is commented, else the text is uncommented
 */
void XinxCodeEdit::commentSelectedText(bool uncomment)
{
	Q_UNUSED(uncomment);
	throw XinxException(tr("Can't comment this type of document"));
}

/*!
 * Return if the editor can comment the code
 */
bool XinxCodeEdit::isCommentAvailable()
{
	return false;
}

//! Refresh the text highlighter (in case the cursor position change)
void XinxCodeEdit::refreshTextHighlighter()
{
	if (XINXConfig::self()->config().editor.autoHighlight && (!textCursor().isNull()))
		setMatchingText(textUnderCursor(textCursor(), false, false));
}

/*! Highlight all text equals of the current word under the cursor */
void XinxCodeEdit::callTextHighlighter()
{
	setMatchingText(textUnderCursor(textCursor(), false, false));
}

QString XinxCodeEdit::id() const
{
	return "xinx";
}

QString XinxCodeEdit::name() const
{
	return "XINX Binding";
}

/*!
 * Process to do when a user press a key
 * This method is called when the editor ask to process some shortcut.
 */
bool XinxCodeEdit::localKeyPressExecute(QKeyEvent * e)
{
	if ((e->key() == Qt::Key_Home) && (e->modifiers() == Qt::ShiftModifier || e->modifiers() == Qt::NoModifier))
	{
		key_home(e->modifiers() == Qt::ShiftModifier);
		e->accept();
		return false;
	}
	else if ((e->key() == Qt::Key_Tab) && ((e->modifiers() == Qt::NoModifier) || (e->modifiers() == Qt::ShiftModifier)))
	{
		// TODO: May be replace by an action of the texteditor...
		indent(e->modifiers() != Qt::NoModifier);
		e->accept();
		return false;
	}

	return true;
}

void XinxCodeEdit::key_home(bool select)
{
	QDocumentCursor cursor = textCursor();
	int col = cursor.columnNumber();
	cursor.movePosition(1, QDocumentCursor::StartOfLine, select ? QDocumentCursor::KeepAnchor : QDocumentCursor::MoveAnchor);
	QDocumentCursor cursorStart(cursor);
	QDocumentLine b = cursorStart.line();
	int i = 0;
	while ((i < b.text().size()) && (i >= 0) && (b.text().at(i) == ' ' || b.text().at(i) == '\t'))
	{
		cursorStart.movePosition(1, QDocumentCursor::NextCharacter, select ? QDocumentCursor::KeepAnchor : QDocumentCursor::MoveAnchor);
		i++;
	}
	if (col == cursorStart.columnNumber())
		setTextCursor(cursor);
	else
		setTextCursor(cursorStart);
}

bool XinxCodeEdit::keyPressEvent(QKeyEvent * e, QEditor * editor)
{
	Q_UNUSED(editor);

	if (m_completer && m_completer->popup()->isVisible())
	{
		// The following keys are forwarded by the completer to the widget
		switch (e->key())
		{
		case Qt::Key_Enter:
		case Qt::Key_Return:
		case Qt::Key_Escape:
		case Qt::Key_Tab:
		case Qt::Key_Backtab:
			e->ignore();
			return true; // let the completer do default behavior
		default:
			break;
		}
	}

	bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E);     // CTRL+E
	bool callParent = false;
	if (!m_completer || !isShortcut)
		callParent = localKeyPressExecute(e);

	if (callParent)
	{
		return false;
	}

	postKeyPressEvent(e, editor);

	return true;
}

void XinxCodeEdit::postKeyPressEvent(QKeyEvent * e, QEditor * editor)
{
	if (e->isAccepted())
		processKeyPress(e);

	const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
	if (!m_completer || (ctrlOrShift && e->text().isEmpty()))
	{
		return ;
	}

	static QString eow(EOW);   // end of word
	bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E);     // CTRL+E
	bool hasModifier = (e->modifiers() & (Qt::ControlModifier | Qt::AltModifier));    // && !ctrlOrShift;
	QString completionPrefix = textUnderCursor(textCursor());

	if (!isShortcut && (hasModifier || e->text().isEmpty() || completionPrefix.length() < 2 || eow.contains(e->text().right(1))))
	{
		m_completer->popup()->hide();
		return ;
	}

	m_completer->complete(true, completionPrefix);
}

/*!
 * Process to do when a user press a key.
 * This method is called when the editor ask to add some text automatically. (ie:
 * close a bracket, ...)
 */
bool XinxCodeEdit::processKeyPress(QKeyEvent *)
{
	return true;
}

void XinxCodeEdit::postMousePressEvent(QMouseEvent *event, QEditor * editor)
{
	if ((event->type() == QEvent::MouseButtonPress) && (event->button() == Qt::LeftButton) && (event->modifiers() == Qt::ControlModifier))
	{
		QDocumentCursor cur = editor->cursor();
		editor->setCursor(cur);
		QMetaObject::invokeMethod(this, "searchWord", Qt::QueuedConnection, Q_ARG(QString, textUnderCursor(textCursor(), false)));
	}

}

void XinxCodeEdit::insertDragAndDropText(const QString& text)
{
	insertText(text);
}

bool XinxCodeEdit::dropEvent(QDropEvent *e, QEditor *editor)
{
	if (e && e->mimeData() && e->mimeData()->hasFormat("application/snipet.id.list"))
	{
		e->acceptProposedAction();

		editor->cursor().beginEditBlock();
		editor->cursor().clearSelection();

		editor->clearCursorMirrors();
		setTextCursor(editor->cursorForPosition(editor->mapToContents(e->pos())));

		QByteArray itemData = e->mimeData()->data("application/snipet.id.list");
		QDataStream stream(&itemData, QIODevice::ReadOnly);

		while (! stream.atEnd())
		{
			int id;
			QString type, name;
			stream >> id >> type >> name;

			if (type == "SNIPET")
			{
				QString result;
				if (SnipetManager::self()->callSnipet(id, &result, qApp->activeWindow()))
				{
					insertText(result);
				}
			}
		}

		editor->cursor().endEditBlock();
		editor->selectionChange();

		return true;
	}
	else if (e && e->mimeData() && e->mimeData()->hasText())
	{
		e->acceptProposedAction();

		editor->cursor().beginEditBlock();
		editor->cursor().clearSelection();

		editor->clearCursorMirrors();
		setTextCursor(editor->cursorForPosition(editor->mapToContents(e->pos())));
		if (_text_file_editor)
		{
			_text_file_editor->updateContext();
		}

		QString itemData = e->mimeData()->text();
		insertDragAndDropText(itemData);

		editor->cursor().endEditBlock();
		editor->selectionChange();

		return true;
	}
	return false;
}

/* XinxCodeEditAction */

/*!
 * XinxCodeEditAction is a wrapper of QCodeEdit editor. This editor active
 * action in QCodeEdit.
 */

/*!
 * Create a XinxCodeEdit object.
 *
 * This create the QCodeEdit object with some default option.
 */
XinxCodeEditAction::XinxCodeEditAction(QWidget * parent) : XinxCodeEdit(true, parent)
{
}
