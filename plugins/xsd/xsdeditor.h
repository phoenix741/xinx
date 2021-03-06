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

#ifndef _XSDEDITOR_H_
#define _XSDEDITOR_H_

// Xinx header
#include <editors/abstracteditor.h>
#include "xmlschema.h"

// Qt header
#include <QPointer>
#include <QGraphicsScene>
#include <QGraphicsView>

class XsdEditor : public AbstractEditor
{
public:
	XsdEditor(QWidget * parent = 0);
	virtual ~XsdEditor();

	virtual QString defaultFileName() const;

	virtual QIcon icon() const;

	virtual bool canCopy();
	virtual bool canPaste();
	virtual bool canUndo();
	virtual bool canRedo();
	virtual void loadFromDevice(QIODevice & d);
	virtual void saveToDevice(QIODevice & d);

	virtual QAbstractItemModel * model() const;
	virtual void updateModel();

	virtual QString bookmarkAt(int i);
	virtual int bookmarkCount();

public slots:
	virtual void initSearch(SearchOptions & options);
	virtual bool find(const QString & text, SearchOptions options);
	virtual void replace(const QString & from, const QString & to, SearchOptions options);
	virtual void undo();
	virtual void redo();
	virtual void cut();
	virtual void copy();
	virtual void paste();
	virtual void toogledBookmark();
	virtual void clearAllBookmark();
	virtual bool previousBookmark();
	virtual bool nextBookmark();
	virtual void gotoBookmarkAt(int);
private:
	QPointer<XmlSchemaFile> m_schema;
	QGraphicsScene * m_scene;
	QGraphicsView * m_view;
};

#endif //_XUIEDITOR_H_
