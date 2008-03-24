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

#ifndef P_XINXPLUGINSELECTOR_H_
#define P_XINXPLUGINSELECTOR_H_

// Xinx header
#include "xinxpluginselector.h"
#include "plugininterfaces.h"

// Qt header
#include <QAbstractListModel>
#include <QItemDelegate>
#include <QPainter>


class XinxPluginModel : public QAbstractListModel {
	Q_OBJECT
public:
	XinxPluginModel( QWidget * parent );
	virtual ~XinxPluginModel();

	void addPlugin( IXinxPlugin * plugin );
	
    bool setData( const QModelIndex &index, const QVariant &value, int role = Qt::CheckStateRole );
    QVariant data( const QModelIndex &index, int role ) const;
    Qt::ItemFlags flags( const QModelIndex &index ) const;
    QModelIndex index( int row, int column, const QModelIndex &parent = QModelIndex() ) const;
    int rowCount( const QModelIndex &parent = QModelIndex() ) const;
private:
	QList<IXinxPlugin*> m_plugins;
};

class XinxPluginDelegate : public QItemDelegate {
	Q_OBJECT
public:
	XinxPluginDelegate( QObject * parent );
	virtual ~XinxPluginDelegate();
	
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
protected:
    virtual bool eventFilter( QObject *watched, QEvent *event );
private:
	/*!
	 * Prerpare a button to be designed. \e decalage define the distance from the border (left or right), and ca be used
	 * when another button is designed.
	 * \param icon    The icon to use to design the button.
	 * \param caption The text to show on the button.
	 * \param option  The option used to design the view item.
	 */
	QStyleOptionButton calculateButton( const QIcon & icon, const QString & caption, const QStyleOptionViewItem & option, int decalage = 0 ) const;
	/*!
	 * Draw a button on the line. \e decalage define the distance from the border (left or right), and ca be used
	 * when another button is designed.
	 * \param painter The painter where designe the button.
	 * \param icon    The icon to use to design the button.
	 * \param caption The text to show on the button.
	 * \param option  The option used to design the view item.
	 */
	QStyleOptionButton drawButton( QPainter * painter, const QIcon & icon, const QString & caption, const QStyleOptionViewItem & option, int decalage = 0 ) const;
	
	int m_separatorPixels, m_rightMargin, m_leftMargin;
	QPoint m_cursorPosition;
};

class PrivateXinxPluginSelector : public QObject {
	Q_OBJECT
public:
	PrivateXinxPluginSelector( XinxPluginSelector * parent );
	
	XinxPluginModel * m_model;
	XinxPluginDelegate * m_delegate;
private:
	XinxPluginSelector * m_parent;
};

#endif /*P_XINXPLUGINSELECTOR_H_*/
