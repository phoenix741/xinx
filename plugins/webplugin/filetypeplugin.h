/***************************************************************************
 *   Copyright (C) 2008 by Ulrich Van Den Hekke                            *
 *   ulrich.vdh@free.fr                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef _FILETYPEPLUGIN_H_
#define _FILETYPEPLUGIN_H_

#include "xsl/xmlhighlighter.h"
#include "xsl/xsllistview.h"
#include "xsl/stylesheeteditor.h"
#include "xsl/xmlfileeditor.h"
#include "xsl/htmlfileeditor.h"

#include "js/jshighlighter.h"
#include "js/jsfileeditor.h"
#include "js/javascriptparser.h"

#include "css/csshighlighter.h"
#include "css/cssmodeldata.h"
#include "css/cssfileeditor.h"

#include "xq/xqhighlighter.h"
#include "xq/xqfileeditor.h"

#include "editorcompletion.h"
#include "textfileeditor.h"

#include "xinxpluginsloader.h"

/* XSLStyleSheetFileType */

class XSLStyleSheetFileType : public QObject, public IFileTypePlugin {
	Q_OBJECT
public:
	virtual QString description() {	return tr( "XSL Stylesheet" ); };
	virtual QString match() { return "*.xsl"; };
	virtual QIcon icon() { return QIcon( ":/images/typexsl.png" ); };

	virtual struct_properties properties() {
		struct_properties p;
		p.canBeCommitToRCS = true;
		p.canBeFindInConfiguration = true;
		p.canBeSaveAsSpecifique = true;
		p.specifiqueSubDirectory = QString();
		return p;
	};

	virtual AbstractEditor * createEditor( const QString & filename ) {
		StyleSheetEditor * editor = new StyleSheetEditor();

		if( ! filename.isEmpty() )
			editor->loadFromFile( filename );

		return editor;
	}
	virtual FileContentElement * createElement( FileContentElement * parent, int line, const QString & filename ) {
		if( parent )
			return new XSLFileContentParser( parent, filename, line );
		else
			return new XSLFileContentParser();
	}
};

/* XMLFileType */

class XMLFileType : public QObject, public IFileTypePlugin {
	Q_OBJECT
public:
	virtual QString description() {	return tr( "XML File" ); };
	virtual QString match() { return "*.xml"; };
	virtual QIcon icon() { return QIcon( ":/images/typexml.png" ); };

	virtual struct_properties properties() {
		struct_properties p;
		p.canBeCommitToRCS = true;
		p.canBeFindInConfiguration = false;
		p.canBeSaveAsSpecifique = true;
		p.specifiqueSubDirectory = QString();
		return p;
	};

	virtual AbstractEditor * createEditor( const QString & filename ) {
		XmlFileEditor * editor = new XmlFileEditor();

		if( ! filename.isEmpty() )
			editor->loadFromFile( filename );

		return editor;
	}
	virtual FileContentElement * createElement( FileContentElement *, int, const QString & ) {
		return NULL;
	}
};

/* HTMLFileType */

class HTMLFileType : public QObject, public IFileTypePlugin {
	Q_OBJECT
public:
	virtual QString description() {	return tr( "HTML File" ); };
	virtual QString match() { return "*.htm *.html *.xhtml"; };
	virtual QIcon icon() { return QIcon( ":/images/typehtml.png" ); };

	virtual struct_properties properties() {
		struct_properties p;
		p.canBeCommitToRCS = true;
		p.canBeFindInConfiguration = false;
		p.canBeSaveAsSpecifique = false;
		p.specifiqueSubDirectory = QString();
		return p;
	};

	virtual AbstractEditor * createEditor( const QString & filename ) {
		HtmlFileEditor * editor = new HtmlFileEditor();

		if( ! filename.isEmpty() )
			editor->loadFromFile( filename );

		return editor;
	}
	virtual FileContentElement * createElement( FileContentElement *, int, const QString & ) {
		return NULL;
	}
};

/* JSFileType */

class JSFileType : public QObject, public IFileTypePlugin {
	Q_OBJECT
public:
	virtual QString description() {	return tr( "JavaScript" ); };
	virtual QString match() { return "*.js"; };
	virtual QIcon icon() { return QIcon( ":/images/typejs.png" ); };

	virtual struct_properties properties() {
		struct_properties p;
		p.canBeCommitToRCS = true;
		p.canBeFindInConfiguration = false;
		p.canBeSaveAsSpecifique = true;
		p.specifiqueSubDirectory = "js/";
		return p;
	};

	virtual AbstractEditor * createEditor( const QString & filename ) {
		JSFileEditor * editor = new JSFileEditor();

		if( ! filename.isEmpty() )
			editor->loadFromFile( filename );

		return editor;
	}
	virtual FileContentElement * createElement( FileContentElement * parent, int line, const QString & filename ) {
		if( parent )
			return new JavaScriptParser( parent, filename, line );
		else
			return new JavaScriptParser();
	}
};

/* CSSFileType */

class CSSFileType : public QObject, public IFileTypePlugin {
	Q_OBJECT
public:
	virtual QString description() {	return tr( "Cascading Style Sheet" ); };
	virtual QString match() { return "*.css"; };
	virtual QIcon icon() { return QIcon( ":/images/typecss.png" ); };

	virtual struct_properties properties() {
		struct_properties p;
		p.canBeCommitToRCS = true;
		p.canBeFindInConfiguration = false;
		p.canBeSaveAsSpecifique = true;
		p.specifiqueSubDirectory = "css/";
		return p;
	};

	virtual AbstractEditor * createEditor( const QString & filename ) {
		CSSFileEditor * editor = new CSSFileEditor();

		if( ! filename.isEmpty() )
			editor->loadFromFile( filename );

		return editor;
	}
	virtual FileContentElement * createElement( FileContentElement * parent, int line, const QString & filename ) {
		return new CSSFileContentParser( parent, filename, line );
	}
};

/* XQFileType */

class XQFileType : public QObject, public IFileTypePlugin {
	Q_OBJECT
public:
	virtual QString description() {	return tr( "XQuery" ); };
	virtual QString match() { return "*.xq"; };
	virtual QIcon icon() { return QIcon( ":/images/typexq.png" ); };

	virtual struct_properties properties() {
		struct_properties p;
		p.canBeCommitToRCS = false;
		p.canBeFindInConfiguration = false;
		p.canBeSaveAsSpecifique = false;
		p.specifiqueSubDirectory = QString();
		return p;
	};

	virtual AbstractEditor * createEditor( const QString & filename ) {
		XQFileEditor * editor = new XQFileEditor();

		if( ! filename.isEmpty() )
			editor->loadFromFile( filename );

		return editor;
	}
	virtual FileContentElement * createElement( FileContentElement * parent, int line, const QString & filename ) {
		return 0;
	}
};

#endif // _FILETYPEPLUGIN_H_
