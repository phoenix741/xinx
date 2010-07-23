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

#ifndef XSLTEXTEDITOR_H_
#define XSLTEXTEDITOR_H_
#pragma once

// Xinx header
#include <editors/xinxcodeedit.h>
#include "editors/widgeteditor/xml/xmltexteditor.h"

// Qt header
#include <QStringList>
#include <QHash>

// QCodeEdit header
#include <document/qdocumentcursor.h>

/* XslTextEditor */

class XslCompletionNodeModel;

class XslTextEditor : public XmlTextEditor
{
	Q_OBJECT
public:
	XslTextEditor(QWidget * parent = 0);
	virtual ~XslTextEditor();

	virtual QCompleter * completer();

	virtual QDocumentCursor insertCompletionBalises(QDocumentCursor & tc, QString node);
	virtual void insertCompletionAccolade(QDocumentCursor & tc, QString node, QString param, QString value, const QModelIndex & index);

	void setModel(XslCompletionNodeModel * model);
private:
	QString paramValue(const QDocumentCursor & cursor, const QString & param);
	void getTemplate(const QDocumentCursor & cursor, QString * name, QString * mode);

	XslCompletionNodeModel * m_model;
	QDocumentCursor m_lastPosition;
};

#endif /*XSLTEXTEDITOR_H_*/
