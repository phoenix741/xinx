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

#ifndef _HTMLFILEEDITOR_H_
#define _HTMLFILEEDITOR_H_
#pragma once

// Xinx header
#include <editors/textfileeditor.h>
#include <editors/contentviewtexteditor.h>
#include "editors/parsing/xsltparser.h"

// Qt header
#include <QProcess>

class XslCompletionNodeModel;
class QWebView;
class XmlPresentationDockWidget;
class QTemporaryFile;
class QTabWidget;
class XslTextEditor;

/* HtmlFileEditor */

class HtmlFileEditor : public TextFileEditor {
	Q_OBJECT
public:
	HtmlFileEditor( QWidget *parent = 0 );
	virtual ~HtmlFileEditor();

	virtual QString defaultFileName() const;

	virtual bool autoIndent();

	virtual QIcon icon() const;
	virtual QTextCodec * codec() const;

	virtual void showHtml();

	XslCompletionNodeModel * completionModel() const;
protected:
	virtual void initLayout();
private slots:
	void tabIndexChange(int);
private:
	void initObjects();

	QTabWidget * m_tabWidget;
	XslCompletionNodeModel * m_completionModel;
	QWebView * m_htmlView;
	QTemporaryFile * m_htmlTempFile;
};

#endif //_HTMLFILEEDITOR_H_
