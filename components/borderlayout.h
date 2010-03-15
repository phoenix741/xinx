/****************************************************************************
 **
 ** Copyright (C) 2004-2008 Trolltech ASA. All rights reserved.
 **
 ** This file is part of the example classes of the Qt Toolkit.
 **
 ** This file may be used under the terms of the GNU General Public
 ** License versions 2.0 or 3.0 as published by the Free Software
 ** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
 ** included in the packaging of this file.  Alternatively you may (at
 ** your option) use any later version of the GNU General Public
 ** License if such license has been publicly approved by Trolltech ASA
 ** (or its successors, if any) and the KDE Free Qt Foundation. In
 ** addition, as a special exception, Trolltech gives you certain
 ** additional rights. These rights are described in the Trolltech GPL
 ** Exception version 1.2, which can be found at
 ** http://www.trolltech.com/products/qt/gplexception/ and in the file
 ** GPL_EXCEPTION.txt in this package.
 **
 ** Please review the following information to ensure GNU General
 ** Public Licensing requirements will be met:
 ** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
 ** you are unsure which license is appropriate for your use, please
 ** review the following information:
 ** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
 ** or contact the sales department at sales@trolltech.com.
 **
 ** In addition, as a special exception, Trolltech, as the sole
 ** copyright holder for Qt Designer, grants users of the Qt/Eclipse
 ** Integration plug-in the right for the Qt/Eclipse Integration to
 ** link to functionality provided by Qt Designer and its related
 ** libraries.
 **
 ** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
 ** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
 ** granted herein.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ****************************************************************************/

#ifndef BORDERLAYOUT_H
#define BORDERLAYOUT_H
#pragma once

#include <QLayout>
#include <QRect>
#include <QWidgetItem>

/*!
 * This layout is decomposed with a center widget and four border.
 *
 *    +---------------------------+<br/>
 *    |           NORTH           |<br/>
 *    +------+-------------+------+<br/>
 *    |      |             |      |<br/>
 *    |      |             |      |<br/>
 *    | WEST |   CENTER    | EAST |<br/>
 *    |      |             |      |<br/>
 *    |      |             |      |<br/>
 *    +------+-------------+------+<br/>
 *    |           SOUTH           |<br/>
 *    +---------------------------+<br/>
 *
 * Please read the doc of trolltech for the example.
 */
class BorderLayout : public QLayout
{
public:
	/*! Position of the widget */
	enum Position
	{
		West,   //!< The widget is placed at left
		North,  //!< The widget is placed in top
		South,  //!< The widget is placed in bottom
		East,   //!< The widget is placed in right
		Center  //!< The widget is placed in center
	};

	/*!
	 * Create the \e BorderLayout
	 * \param parent The parent widget
	 * \param margin The marge arround the layout
	 * \param spacing The space between the widget
	 */
	BorderLayout(QWidget *parent, int margin = 0, int spacing = -1);
	/*!
	 * Create the \e BorderLayout
	 * \param spacing The space between the widget
	 */
	BorderLayout(int spacing = -1);
	//! Destroy the layout
	~BorderLayout();

	void addItem(QLayoutItem *item);
	void addWidget(QWidget *widget, Position position);
	Qt::Orientations expandingDirections() const;
	bool hasHeightForWidth() const;
	int count() const;
	QLayoutItem *itemAt(int index) const;
	QSize minimumSize() const;
	void setGeometry(const QRect &rect);
	QSize sizeHint() const;
	QLayoutItem *takeAt(int index);

	void add(QLayoutItem *item, Position position);
private:
	struct ItemWrapper
	{
		ItemWrapper(QLayoutItem *i, Position p)
		{
			item = i;
			position = p;
		}

		QLayoutItem *item;
		Position position;
	};

	enum SizeType { MinimumSize, SizeHint };
	QSize calculateSize(SizeType sizeType) const;

	QList<ItemWrapper *> list;
};

#endif
