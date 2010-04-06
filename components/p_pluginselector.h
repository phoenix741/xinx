/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2007-2010 by Ulrich Van Den Hekke                         *
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

#pragma once
#ifndef P_PLUGINSELECTOR_H_ /*! \cond PRIVATE */
#define P_PLUGINSELECTOR_H_

// Xinx header
#include "pluginselector.h"

// Qt header
#include <QAbstractListModel>
#include <QItemDelegate>
#include <QPainter>


class PluginModel : public QAbstractListModel
{
	Q_OBJECT
public:
	enum PluginModelRole { PLG_NAME = 1001, PLG_DESCRIPTION = 1002, PLG_ICON = 1003 };

	PluginModel(QWidget * parent);
	virtual ~PluginModel();

	void addPlugin(PluginElement * plugin);
	const QList<PluginElement*> & plugins() const;
	void clear();

	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::CheckStateRole);
	QVariant data(const QModelIndex &index, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
private:
	QList<PluginElement*> m_plugins;
};

class PluginDelegate : public QItemDelegate
{
	Q_OBJECT
public:
	PluginDelegate(QObject * parent);
	virtual ~PluginDelegate();

	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

	int separtorPixels() const
	{
		return m_separatorPixels;
	};
	void setSepartorPixels(int value)
	{
		m_separatorPixels = value;
	};

	int rightMargin() const
	{
		return m_rightMargin;
	};
	void setRightMargin(int value)
	{
		m_rightMargin = value;
	};

	int leftMargin() const
	{
		return m_leftMargin;
	};
	void setLeftMargin(int value)
	{
		m_leftMargin = value;
	};

	int iconHeight() const
	{
		return m_iconHeight;
	};
	void setIconHeight(int value)
	{
		m_iconHeight = value;
	};

	int iconWidth() const
	{
		return m_iconWidth;
	};
	void setIconWidth(int value)
	{
		m_iconWidth = value;
	};

	QSize iconSize() const
	{
		return QSize(m_iconWidth, m_iconHeight);
	};
	void setIconSize(QSize value)
	{
		m_iconWidth = value.width();
		m_iconHeight = value.height();
	};
	void setIconSize(int width, int height)
	{
		m_iconWidth = width;
		m_iconHeight = height;
	};
signals:
	void configurePlugin(PluginElement * plugin);
	void aboutPlugin(PluginElement * plugin);
protected:
	virtual bool eventFilter(QObject *watched, QEvent *event);
private:
	/*!
	 * Prerpare a button to be designed. \e decalage define the distance from the border (left or right), and ca be used
	 * when another button is designed.
	 * \param icon    The icon to use to design the button.
	 * \param caption The text to show on the button.
	 * \param option  The option used to design the view item.
	 */
	QStyleOptionButton calculateButton(const QIcon & icon, const QString & caption, const QStyleOptionViewItem & option, int decalage = 0) const;
	/*!
	 * Draw a button on the line. \e decalage define the distance from the border (left or right), and ca be used
	 * when another button is designed.
	 * \param painter The painter where designe the button.
	 * \param icon    The icon to use to design the button.
	 * \param caption The text to show on the button.
	 * \param option  The option used to design the view item.
	 */
	QStyleOptionButton drawButton(QPainter * painter, const QIcon & icon, const QString & caption, const QStyleOptionViewItem & option, int decalage = 0) const;

	QStyleOptionViewItem calculateCheckbox(const QStyleOptionViewItem & option, QRect & rect, int decalage = 0) const;

	QStyleOptionButton calculateButtonAbout(const QStyleOptionViewItem & option, int decalage = 0) const;
	QStyleOptionButton drawButtonAbout(QPainter * painter, const QStyleOptionViewItem & option, int decalage = 0) const;

	QStyleOptionButton calculateButtonConfigure(const QStyleOptionViewItem & option, int decalage = 0) const;
	QStyleOptionButton drawButtonConfigure(QPainter * painter, const QStyleOptionViewItem & option, int decalage = 0) const;

	int m_separatorPixels, m_rightMargin, m_leftMargin, m_iconHeight, m_iconWidth, m_minimumItemWidth;
	QPoint m_cursorPosition;
	bool m_buttonPressed;
};

class PrivatePluginSelector : public QObject
{
	Q_OBJECT
public:
	PrivatePluginSelector(PluginSelector * parent);

	PluginModel * m_model;
	PluginDelegate * m_delegate;
private:
	PluginSelector * m_parent;
};

#endif /*P_PLUGINSELECTOR_H_*/ /*! \endcond */
