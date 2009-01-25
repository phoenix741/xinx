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

// Xinx header
#include "xsl/xmlformatscheme.h"
#include "xsl/xsllistview.h"
#include "xsl/stylesheeteditor.h"
#include "xsl/xmlfileeditor.h"
#include "xsl/htmlfileeditor.h"

#include "js/jsfileeditor.h"
#include "js/javascriptparser.h"

#include "css/cssmodeldata.h"
#include "css/cssfileeditor.h"

#include "xq/xqfileeditor.h"
#include "xq/xquery_keyword.h"

#include "editorcompletion.h"
#include "textfileeditor.h"

#include <xinxpluginsloader.h>
#include <xinxconfig.h>
#include <xinxlanguagefactory.h>

// QCodeedit header
#include "qnfadefinition.h"

// Qt header
#include <QTemporaryFile>
#include <QTextStream>

/* Constantes */

static const QColor DEFAULT_COMMENT			= Qt::darkGreen;
static const QColor DEFAULT_ERROR			= Qt::darkMagenta;
static const QColor DEFAULT_OTHER			= Qt::black;

static const QColor DEFAULT_SYNTAX_CHAR		= Qt::blue;
static const QColor DEFAULT_ELEMENT_NAME	= Qt::darkRed;
static const QColor DEFAULT_ATTRIBUTE_NAME	= Qt::red;
static const QColor DEFAULT_ATTRIBUTE_VALUE	= Qt::blue;
static const QColor DEFAULT_XPATH_VALUE		= Qt::darkMagenta;

static const QColor DEFAULT_RESERVEDWORD	= Qt::black;
static const QColor DEFAULT_NUMBER			= Qt::blue;
static const QColor DEFAULT_STRING			= Qt::red;

/* XMLFileType */

class XMLFileType : public QObject, public IFileTextPlugin {
	Q_OBJECT
public:
	virtual QString description();
	virtual QString match();
	virtual QIcon icon();

	virtual AppSettings::struct_extentions properties();
	virtual AbstractEditor * createEditor( const QString & filename );
	virtual FileContentElement * createElement( FileContentElement *, int, const QString & );

	virtual QString highlighterId() const;
	virtual XinxFormatScheme * createFormatScheme( XINXConfig * config ) const;
	virtual QString createLanguageDescription() const;
	virtual QString fileExample() const;
};

/* XSLStyleSheetFileType */

class XSLStyleSheetFileType : public XMLFileType {
	Q_OBJECT
public:
	virtual QString description();
	virtual QString match();
	virtual QIcon icon();

	virtual AppSettings::struct_extentions properties();
	virtual AbstractEditor * createEditor( const QString & filename );
	virtual FileContentElement * createElement( FileContentElement * parent, int line, const QString & filename );
};

/* HTMLFileType */

class HTMLFileType : public XMLFileType {
	Q_OBJECT
public:
	virtual QString description();
	virtual QString match();
	virtual QIcon icon();

	virtual AppSettings::struct_extentions properties();
	virtual AbstractEditor * createEditor( const QString & filename );
};

/* JSFileType */

class JSFileType : public QObject, public IFileTextPlugin {
	Q_OBJECT
public:
	virtual QString description();
	virtual QString match();
	virtual QIcon icon();

	virtual AppSettings::struct_extentions properties();
	virtual AbstractEditor * createEditor( const QString & filename );
	virtual FileContentElement * createElement( FileContentElement * parent, int line, const QString & filename );

	virtual QString highlighterId() const;
	virtual XinxFormatScheme * createFormatScheme( XINXConfig * config ) const;
	virtual QString createLanguageDescription() const;
	virtual QHash<QString,QTextCharFormat> defaultsFormat() const;
	virtual QString fileExample() const;
};

/* CSSFileType */

class CSSFileType : public QObject, public IFileTextPlugin {
	Q_OBJECT
public:
	virtual QString description();
	virtual QString match();
	virtual QIcon icon();

	virtual AppSettings::struct_extentions properties();
	virtual AbstractEditor * createEditor( const QString & filename );
	virtual FileContentElement * createElement( FileContentElement * parent, int line, const QString & filename );

	virtual QString highlighterId() const;
	virtual QHash<QString,QTextCharFormat> defaultsFormat() const;
	virtual XinxFormatScheme * createFormatScheme( XINXConfig * config ) const;
	virtual QString createLanguageDescription() const;
	virtual QString fileExample() const;
};

/* XQFileType */

class XQFileType : public QObject, public IFileTextPlugin {
	Q_OBJECT
public:
	XQFileType();
	virtual ~XQFileType();

	virtual QString description();
	virtual QString match();
	virtual QIcon icon();

	virtual AppSettings::struct_extentions properties();
	virtual AbstractEditor * createEditor( const QString & filename );
	virtual FileContentElement * createElement( FileContentElement * parent, int line, const QString & filename );

	virtual QString highlighterId() const;
	virtual XinxFormatScheme * createFormatScheme( XINXConfig * config ) const;
	virtual QString createLanguageDescription() const;
	virtual QHash<QString,QTextCharFormat> defaultsFormat() const;
	virtual QString fileExample() const;
};

#endif // _FILETYPEPLUGIN_H_
