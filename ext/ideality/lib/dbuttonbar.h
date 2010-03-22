/***************************************************************************
 *   Copyright (C) 2006 by David Cuadrado                                *
 *   krawek@gmail.com                                                      *
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

#ifndef DBUTTONBAR_H
#define DBUTTONBAR_H

#include <dideality.h>

#include <QToolBar>
#include <QButtonGroup>
#include <QMap>
#include <QTimer>

class DViewButton;
class DToolView;
class QAction;
class QMenu;


/**
 * @author David Cuadrado <krawek@gmail.com>
*/
class D_IDEAL_EXPORT DButtonBar : public QToolBar
{
	Q_OBJECT;
	public:
		DButtonBar(Qt::ToolBarArea area, QWidget *parent = 0);
		~DButtonBar();
		
		void addButton(DViewButton *viewButton);
		void removeButton(DViewButton *viewButton);
		
		bool isEmpty() const;
		
		void disable(DViewButton *v);
		void enable(DViewButton *v);
		
		bool isExclusive() const;
		bool autohide() const;
		void showSeparator(bool e);
		
		int count() const;
		
		void setEnableButtonBlending(bool enable);
		
	public slots:
		void onlyShow(DToolView *tool, bool ensureVisible = false);
		
		void setExclusive(bool excl);
		void setAutoHide(bool autohide);
		void setShowOnlyIcons();
		void setShowOnlyTexts();
		
	private:
		QMenu *createMenu();
		
	private slots:
		void hideOthers(QAbstractButton *source);
		void doNotHide();
		
	protected:
		virtual void mousePressEvent(QMouseEvent *e);
		virtual void enterEvent(QEvent *e);
		virtual void leaveEvent(QEvent *e);
		
	private:
		QButtonGroup m_buttons;
		QMap<QWidget *, QAction *> m_actionForWidget;
		
		QAction *m_separator;
		
		bool m_autoHide;
		
		QTimer m_hider;
		
		bool m_blockHider;
};

#endif
