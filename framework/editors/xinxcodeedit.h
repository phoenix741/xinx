/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2009 by Ulrich Van Den Hekke                              *
 * ulrich.vdh@shadoware.org                                                *
 *                                                                         *
 * This program is free software: you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 * *********************************************************************** */

#ifndef _XINXCODEEDIT_H_
#define _XINXCODEEDIT_H_
#pragma once

// Xinx header
#include <core/lib-config.h>

// Qt header
#include <QWidget>

// QCodeEdit header
#include <qdocumentcursor.h>
#include <qeditor.h>

class QDocumentSearch;
class QAbstractItemModel;
class QCodeEdit;
class QCompleter;
class QModelIndex;
class XINXConfig;

class LIBEXPORT XinxCodeEdit : public QWidget, private QEditor::InputBinding
{
	Q_OBJECT
	Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly);
	Q_PROPERTY(QString selection READ selection WRITE setSelection)
	Q_PROPERTY(QString plainText READ toPlainText WRITE setPlainText)
	Q_PROPERTY(int row READ currentRow)
	Q_PROPERTY(int column READ currentColumn)
	Q_PROPERTY(int tabStopWidth READ tabStopWidth WRITE setTabStopWidth)
	Q_PROPERTY(bool modified READ isModified WRITE setModified)
public:
	enum FindFlag { FindBackward = 0x01, FindCaseSensitively = 0x02, FindWholeWords = 0x04 };
	Q_DECLARE_FLAGS(FindFlags, FindFlag);

	XinxCodeEdit(QWidget * parent = 0);
	XinxCodeEdit(bool action, QWidget * parent = 0);
	virtual ~XinxCodeEdit();

	const QString & filename() const;

	int currentColumn();
	int currentRow();

	QString textUnderCursor(const QDocumentCursor & cursor, bool deleteWord = false, bool dot = true);
	void setTextCursor(const QDocumentCursor & cursor);
	QDocumentCursor textCursor() const;

	QString selection() const;
	QString toPlainText() const;

	bool isReadOnly() const;

	QEditor * editor() const;
	QDocument * document() const;

	virtual void setCompleter(QCompleter * completer);
	virtual QCompleter * completer();

	void setTabStopWidth(int width);
	int tabStopWidth() const;

	virtual bool isCommentAvailable();
	bool canPaste();

	QDocumentCursor find(const QString & subString, const QDocumentCursor & cursor, XinxCodeEdit::FindFlags options = 0) const;
	QDocumentCursor find(const QRegExp & expr, const QDocumentCursor & cursor, XinxCodeEdit::FindFlags options = 0) const;
	bool find(const QString & exp, XinxCodeEdit::FindFlags options);

	void moveCursor(QDocumentCursor::MoveOperation operation, QDocumentCursor::MoveMode mode = QDocumentCursor::MoveAnchor);
	void print(QPrinter * printer) const;

	void clearBookmark();
	bool previousBookmark();
	bool nextBookmark();
	void setBookmark(int line, bool enabled);
	QList<int> listOfBookmark();

	void setErrors(QList<int> errors);
public slots:
	void setFilename(const QString & filename);

	void setHighlighter(const QString & highlighter);
	void setHighlighter(const QString & highlighter, XINXConfig * config);

	void insertText(const QString & text);

	void setMatchingText(QString text);
	void setPlainText(const QString & text);
	void setSelection(QString text);
	void setReadOnly(bool readonly);
	void setModified(bool modified);

	void updateHighlighter();
	void updateFont();
	void refreshTextHighlighter();
	void callTextHighlighter();

	void gotoLine(int line);

	void duplicateLines();
	void moveLineUp();
	void moveLineDown();
	void upperSelectedText();
	void lowerSelectedText();

	void indent(bool unindent = false);
	virtual void commentSelectedText(bool uncomment = false);
signals:
	void modelUpdated(QAbstractItemModel * model);
	void searchWord(const QString & word);
	void bookmarkToggled(int line, bool enabled);

	void copyAvailable(bool y);
	void undoAvailable(bool y);
	void redoAvailable(bool y);
protected slots:
	virtual void insertCompletion(const QModelIndex& index);
protected:
	virtual QString id() const;
	virtual QString name() const;

	virtual bool keyPressEvent(QKeyEvent * event, QEditor * editor);
	virtual void postKeyPressEvent(QKeyEvent * event, QEditor * editor);
	virtual void postMousePressEvent(QMouseEvent * event, QEditor * editor);
	virtual bool dropEvent(QDropEvent *e, QEditor *editor);

	virtual bool processKeyPress(QKeyEvent *);
	virtual bool localKeyPressExecute(QKeyEvent * e);

	virtual void insertSnipet(const QString & snipet);
private slots:
	void slotMarkChanged(QDocumentLineHandle* line, int type, bool enabled);
private:
	void init(bool action);
	void uploSelectedText(bool upper = true);
	void key_home(bool);
	void key_snipet();
	bool isModified();

	QCodeEdit * m_editor;
	QDocumentSearch * m_matchingText;
	QCompleter * m_completer;
	QString m_matchingTextString, m_filename;
};

class LIBEXPORT XinxCodeEditAction : public XinxCodeEdit
{
	Q_OBJECT
public:
	XinxCodeEditAction(QWidget * parent = 0);
};

#endif /* _XINXCODEEDIT_H_ */
