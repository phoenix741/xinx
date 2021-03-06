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

#ifndef MAQTEXTEDITOR_H_
#define MAQTEXTEDITOR_H_
#pragma once

// Xinx header
#include <editors/xinxcodeedit.h>

class MaquetteTextEditor : public XinxCodeEdit
{
	Q_OBJECT
public:
	MaquetteTextEditor(QWidget * parent = 0);
	virtual ~MaquetteTextEditor();

	virtual bool isCommentAvailable();
public slots:
	virtual void commentSelectedText(bool uncomment = false);
protected:
	enum cursorPosition
	{
		cpEditComment, // in /* ... */
		cpEditTag, // ... { ... }
		cpEditGlobal
	};

	static cursorPosition editPosition(const XinxCodeEdit * textEdit, const QDocumentCursor & cursor);
	cursorPosition editPosition(const QDocumentCursor & cursor);
};

#endif /* MAQTEXTEDITOR_H_ */
