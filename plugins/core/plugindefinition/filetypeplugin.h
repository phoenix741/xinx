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

#ifndef _FILETYPEPLUGIN_H_
#define _FILETYPEPLUGIN_H_
#pragma once

// Xinx header
#include "editors/scheme/xmlformatscheme.h"
#include "editors/widgeteditor/html/htmlfileeditor.h"
#include "editors/widgeteditor/xml/xmlfileeditor.h"
#include "editors/widgeteditor/xsl/xslfileeditor.h"

#include "editors/widgeteditor/js/jsfileeditor.h"
#include "editors/scheme/jsformatscheme.h"

#include "editors/models/css/cssmodeldata.h"
#include "editors/widgeteditor/css/cssfileeditor.h"
#include "editors/scheme/cssformatscheme.h"

#include "editors/widgeteditor/xq/xqfileeditor.h"
#include "editors/models/xq/xquery_keyword.h"
#include "editors/scheme/xqformatscheme.h"

#include <editors/textfileeditor.h>

#include <plugins/xinxpluginsloader.h>
#include <core/xinxconfig.h>
#include <editors/xinxlanguagefactory.h>

// QCodeedit header
#include "qnfadefinition.h"

// Qt header
#include <QTemporaryFile>
#include <QTextStream>

/* Constantes */

static const QColor DEFAULT_COMMENT         = Qt::darkGreen;
static const QColor DEFAULT_ERROR           = Qt::darkMagenta;
static const QColor DEFAULT_OTHER           = Qt::black;

static const QColor DEFAULT_SYNTAX_CHAR     = Qt::blue;
static const QColor DEFAULT_ELEMENT_NAME    = Qt::darkRed;
static const QColor DEFAULT_ATTRIBUTE_NAME  = Qt::red;
static const QColor DEFAULT_ATTRIBUTE_VALUE = Qt::blue;
static const QColor DEFAULT_XPATH_VALUE     = Qt::darkMagenta;

static const QColor DEFAULT_RESERVEDWORD    = Qt::black;
static const QColor DEFAULT_NUMBER          = Qt::blue;
static const QColor DEFAULT_STRING          = Qt::red;

/* TextFileType */

class TextFileType : public QObject, public IFileTextPlugin
{
	Q_OBJECT
public:
	virtual QString description();
	virtual QString match();
	virtual QString icon();

	virtual AbstractEditor * createEditor(const QString & filename);

	virtual QString highlighterId() const;
	virtual XinxFormatScheme * createFormatScheme(XINXConfig * config) const;
	virtual QString createLanguageDescription() const;
	virtual QString fileExample() const;

	virtual QString parserType();

	virtual QString defaultFileName();
};

/* XMLFileType */

class XMLFileType : public QObject, public IFileTextPlugin
{
	Q_OBJECT
public:
	virtual QString description();
	virtual QString match();
	virtual QString icon();

	virtual AbstractEditor * createEditor(const QString & filename);

	virtual QString highlighterId() const;
	virtual XinxFormatScheme * createFormatScheme(XINXConfig * config) const;
	virtual QString createLanguageDescription() const;
	virtual QString fileExample() const;

	virtual QString parserType();

	virtual QString defaultFileName();
};

/* XSLStyleSheetFileType */

class XSLStyleSheetFileType : public XMLFileType
{
	Q_OBJECT
public:
	virtual QString description();
	virtual QString match();
	virtual QString icon();

	virtual AbstractEditor * createEditor(const QString & filename);

	virtual QString parserType();

	virtual QString defaultFileName();
};

/* HTMLFileType */

class HTMLFileType : public XMLFileType
{
	Q_OBJECT
public:
	virtual QString description();
	virtual QString match();
	virtual QString icon();

	virtual AbstractEditor * createEditor(const QString & filename);

	virtual QString defaultFileName();
};

/* JSFileType */

class JSFileType : public QObject, public IFileTextPlugin
{
	Q_OBJECT
public:
	virtual QString description();
	virtual QString match();
	virtual QString icon();

	virtual AbstractEditor * createEditor(const QString & filename);

	virtual QString highlighterId() const;
	virtual XinxFormatScheme * createFormatScheme(XINXConfig * config) const;
	virtual QString createLanguageDescription() const;
	virtual QString fileExample() const;

	virtual QString parserType();

	virtual QString defaultFileName();
};

/* CSSFileType */

class CSSFileType : public QObject, public IFileTextPlugin
{
	Q_OBJECT
public:
	virtual QString description();
	virtual QString match();
	virtual QString icon();

	virtual AbstractEditor * createEditor(const QString & filename);

	virtual QString highlighterId() const;
	virtual XinxFormatScheme * createFormatScheme(XINXConfig * config) const;
	virtual QString createLanguageDescription() const;
	virtual QString fileExample() const;

	virtual QString parserType();

	virtual QString defaultFileName();
};

/* XQFileType */

class XQFileType : public QObject, public IFileTextPlugin
{
	Q_OBJECT
public:
	XQFileType();
	virtual ~XQFileType();

	virtual QString description();
	virtual QString match();
	virtual QString icon();

	virtual AbstractEditor * createEditor(const QString & filename);

	virtual QString highlighterId() const;
	virtual XinxFormatScheme * createFormatScheme(XINXConfig * config) const;
	virtual QString createLanguageDescription() const;
	virtual QString fileExample() const;

	virtual QString parserType();

	virtual QString defaultFileName();
};

#endif // _FILETYPEPLUGIN_H_
