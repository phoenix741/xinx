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

#ifndef XMLTEXTEDITOR_H_
#define XMLTEXTEDITOR_H_
#pragma once

// Xinx header
#include <editors/xinxcodeedit.h>

// Qt header
#include <QStringList>
#include <QHash>
#include "editors/models/html_xsl_base/definition_balisenode.h"

/* XPathBalise */

struct XPathBalise
{
	QString name;
	QHash<QString,QString> attributes;
};

namespace Core {

namespace BaliseDefinition {

	class XmlContextType;

}

}

/* XmlTextEditor */

class XmlTextEditor : public XinxCodeEdit
{
	Q_OBJECT
public:
	XmlTextEditor(QWidget * parent = 0);
	virtual ~XmlTextEditor();

	virtual bool isCommentAvailable();

	virtual QDocumentCursor insertCompletionParam(Core::BaliseDefinition::XmlContextType * context, QDocumentCursor & tc, QSharedPointer<Core::BaliseDefinition::BaliseNode> balise, bool movePosition);
	virtual QDocumentCursor insertCompletionValue(Core::BaliseDefinition::XmlContextType * context, QDocumentCursor & tc, QSharedPointer<Core::BaliseDefinition::AttributeNode> attribute);
	virtual QDocumentCursor insertCompletionBalises(Core::BaliseDefinition::XmlContextType * context, QDocumentCursor & tc, QSharedPointer<Core::BaliseDefinition::BaliseNode> balise);
public slots:
	virtual void commentSelectedText(bool uncomment = false);
protected slots:
protected:
	virtual bool localKeyPressExecute(QKeyEvent * e);
	virtual bool processKeyPress(QKeyEvent *);
private:
	void key_shenter(bool back);
};


#endif /*XMLTEXTEDITOR_H_*/
