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

// Qt header
#include <QWidgetItem>

// Components header
#include "borderlayout_p.h"


/* BorderLayoutPrivate */

QSize BorderLayoutPrivate::calculateSize(SizeType sizeType) const
{
	QSize totalSize;

	for (int i = 0; i < list.size(); ++i)
	{
		ItemWrapper *wrapper = list.at(i);
		BorderLayout::Position position = wrapper->position;
		QSize itemSize;

		if (wrapper->item->isEmpty()) continue;

		if (sizeType == MinimumSize)
			itemSize = wrapper->item->minimumSize();
		else
			// (sizeType == SizeHint)
			itemSize = wrapper->item->sizeHint();

		if (position == BorderLayout::North || position == BorderLayout::South || position == BorderLayout::Center)
			totalSize.rheight() += itemSize.height();

		if (position == BorderLayout::West || position == BorderLayout::East || position == BorderLayout::Center)
			totalSize.rwidth() += itemSize.width();
	}
	return totalSize;
}

/* BorderLayout */

/*!
 * \defgroup Components Graphical User Interface's components for XINX
 * \brief This group contains GUI components that can be used in XINX.
 * This components can be used in other application too.
 */

/*!
 * \ingroup Components
 * \class BorderLayout
 * \since 0.7.2.0
 *
 * \brief A layout with a central widget.
 *
 * \bc
 *
 * This layout is decomposed with a central widget and four borders. Methods
 * add() and addWidget() have a second parameter \p position used to define
 * where you place your widget.
 *
 * You can add multiple widgets at the same position.
 *
 * \image html borderlayout1.png
 * \image latex borderlayout1.eps
 *
 * For more details on this class, you can read the documentation of trolltech/nokia.
 */

/*!
 * \enum BorderLayout::Position
 * Position of the widget in the layout
 */
/*!
 * \var BorderLayout::Position BorderLayout::West
 * The widget is placed at the left
 */
/*!
 * \var BorderLayout::Position BorderLayout::North
 * The widget is placed at the top
 */
/*!
 * \var BorderLayout::Position BorderLayout::South
 * The widget is placed at the bottom
 */
/*!
 * \var BorderLayout::Position BorderLayout::East
 * The widget is placed at the right
 */
/*!
 * \var BorderLayout::Position BorderLayout::Center
 * The widget is placed in the center
 */

/*!
 * \brief Create the BorderLayout
 * \param parent The parent widget
 * \param margin The marge arround the layout
 * \param spacing The space between widgets
 */
BorderLayout::BorderLayout(QWidget *parent, int margin, int spacing) : QLayout(parent), d(new BorderLayoutPrivate)
{
	setMargin(margin);
	setSpacing(spacing);
}

/*!
 * \brief Create the BorderLayout
 * \param spacing The space between widgets
 */
BorderLayout::BorderLayout(int spacing)
{
	setSpacing(spacing);
}

//! Destroy the layout
BorderLayout::~BorderLayout()
{
	QLayoutItem *l;
	while ((l = takeAt(0)))
		delete l;
}

/*!
 * \brief Add item \p item to the position BorderLayout::West
 * \sa addWidget(), add()
 */
void BorderLayout::addItem(QLayoutItem *item)
{
	add(item, West);
}

/*!
 * \brief Add the widget \p widget at the position \p position.
 * \sa add(), addItem()
 */
void BorderLayout::addWidget(QWidget *widget, Position position)
{
	add(new QWidgetItem(widget), position);
}

/*!
 * \brief Add the item \p item at the position \p position in this layout.
 * \sa addWidget(), addItem()
 */
void BorderLayout::add(QLayoutItem *item, Position position)
{
	d->list.append(new BorderLayoutPrivate::ItemWrapper(item, position));
}

/*!
 * \brief Returns whether this layout item can make use of more space than sizeHint().
 *
 * Returns Qt::Vertical | Qt::Horizontal to tell that the layout can grow in both
 * dimensions.
 */
Qt::Orientations BorderLayout::expandingDirections() const
{
	return Qt::Horizontal | Qt::Vertical;
}

//! Returns false to tell that this layout's preferred width depends on its height
bool BorderLayout::hasHeightForWidth() const
{
	return false;
}

//! Number of layout added in the BorderLayout
int BorderLayout::count() const
{
	return d->list.size();
}

//! Return the \p index item of the list.
QLayoutItem *BorderLayout::itemAt(int index) const
{
	BorderLayoutPrivate::ItemWrapper *wrapper = d->list.value(index);
	if (wrapper)
		return wrapper->item;
	else
		return 0;
}

/*!
 * \brief Return the minimum size of the widget.
 *
 * This method uses the minimum size of the different elements added to the layout
 * to know the minimum size.
 */
QSize BorderLayout::minimumSize() const
{
	return d->calculateSize(BorderLayoutPrivate::MinimumSize);
}

//! Set this item's geometry to r.
void BorderLayout::setGeometry(const QRect &rect)
{
	BorderLayoutPrivate::ItemWrapper *center = 0;
	int eastWidth = 0;
	int westWidth = 0;
	int northHeight = 0;
	int southHeight = 0;
	int centerHeight = 0;
	int i;

	QLayout::setGeometry(rect);

	for (i = 0; i < d->list.size(); ++i)
	{
		BorderLayoutPrivate::ItemWrapper *wrapper = d->list.at(i);
		QLayoutItem *item = wrapper->item;
		Position position = wrapper->position;

		if (item->isEmpty()) continue;

		if (position == North)
		{
			item->setGeometry(QRect(rect.x(), northHeight, rect.width(), item->sizeHint().height()));

			northHeight += item->geometry().height() + spacing();
		}
		else if (position == South)
		{
			item->setGeometry(QRect(item->geometry().x(), item->geometry().y(), rect.width(), item->sizeHint().height()));

			southHeight += item->geometry().height() + spacing();

			item->setGeometry(QRect(rect.x(), rect.y() + rect.height() - southHeight + spacing(), item->geometry().width(), item->geometry().height()));
		}
		else if (position == Center)
		{
			center = wrapper;
		}
	}

	centerHeight = rect.height() - northHeight - southHeight;

	for (i = 0; i < d->list.size(); ++i)
	{
		BorderLayoutPrivate::ItemWrapper *wrapper = d->list.at(i);
		QLayoutItem *item = wrapper->item;
		Position position = wrapper->position;

		if (item->isEmpty()) continue;

		if (position == West)
		{
			item->setGeometry(QRect(rect.x() + westWidth, northHeight, item->sizeHint().width(), centerHeight));

			westWidth += item->geometry().width() + spacing();
		}
		else if (position == East)
		{
			item->setGeometry(QRect(item->geometry().x(), item->geometry().y(), item->sizeHint().width(), centerHeight));

			eastWidth += item->geometry().width() + spacing();

			item->setGeometry(QRect(rect.x() + rect.width() - eastWidth + spacing(), northHeight, item->geometry().width(), item->geometry().height()));
		}
	}

	if (center)
		center->item->setGeometry(QRect(westWidth, northHeight, rect.width() - eastWidth - westWidth, centerHeight));
}

/*!
 * \brief Return the hint size of the widget.
 *
 * This method use the hint size of the different elements added to the layout
 * to known the hint size.
 */
QSize BorderLayout::sizeHint() const
{
	return d->calculateSize(BorderLayoutPrivate::SizeHint);
}

//! Returns the item at the index \p index in the list and removes it.
QLayoutItem *BorderLayout::takeAt(int index)
{
	if (index >= 0 && index < d->list.size())
	{
		BorderLayoutPrivate::ItemWrapper *layoutStruct = d->list.takeAt(index);
		return layoutStruct->item;
	}
	return 0;
}
