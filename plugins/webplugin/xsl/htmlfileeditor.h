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

// Xinx header
#include <editors/textfileeditor.h>
#include <editors/contentviewtexteditor.h>

class XslCompletionNodeModel;
class QWebView;
class XmlPresentationDockWidget;

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

	XslCompletionNodeModel * completionModel() const;
private:
	XslCompletionNodeModel * m_completionModel;
};

/* XmlFileEditor */

class XmlFileEditor : public TextFileEditor {
	Q_OBJECT
public:
	XmlFileEditor( QWidget *parent = 0 );
	virtual ~XmlFileEditor();

	virtual QString defaultFileName() const;

	virtual bool autoIndent();

	virtual QIcon icon() const;
	virtual QTextCodec * codec() const;

	virtual void loadFromDevice( QIODevice & d );
private:
	QTextCodec * m_codec;
};

/* XslContentEditor */

class StyleSheetEditor : public ContentViewTextEditor {
	Q_OBJECT
public:
	StyleSheetEditor( QWidget *parent = 0 );
	virtual ~StyleSheetEditor();

	virtual QString defaultFileName() const;

	virtual bool autoIndent();

	virtual QIcon icon() const;
	virtual QTextCodec * codec() const;

	virtual void searchWord( const QString & word );

	void launchStylesheetParsing( const QString & xmlfile );
	static XmlPresentationDockWidget * xmlPresentationDockWidget();

	XslCompletionNodeModel * completionModel() const;
private slots:
	void cursorPositionChanged();
private:
	XslCompletionNodeModel * m_completionModel;
	QWebView * m_htmlView;
};

#endif //_HTMLFILEEDITOR_H_
