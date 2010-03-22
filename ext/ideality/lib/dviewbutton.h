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

#ifndef DVIEWBUTTON_H
#define DVIEWBUTTON_H

#include <QToolButton>
#include <QStyleOptionToolButton>
#include <dideality.h>

class DToolView;

/**
 * @author David Cuadrado <krawek@gmail.com>
*/
class D_IDEAL_EXPORT DViewButton : public QToolButton
{
	Q_OBJECT;
	public:
		DViewButton(Qt::ToolBarArea area, DToolView *toolView, QWidget * parent = 0 );
		DViewButton(DToolView *toolView, QWidget *parent = 0 );
		~DViewButton();
		
		void setArea(Qt::ToolBarArea area);
		Qt::ToolBarArea area() const;
		
		QSize sizeHint() const;
		
		bool isSensible() const;
		bool blending() const;
		
		DToolView *toolView() const;
		
	public slots:
		void setSensible(bool s);
		void setBlending(bool e);
		
	private:
		void setup();
		QMenu *createMenu();
		QStyleOptionToolButton styleOption() const;
		
	protected:
		virtual void paintEvent(QPaintEvent *e);
		virtual void mousePressEvent(QMouseEvent *e);
		virtual void enterEvent( QEvent* );
		virtual void leaveEvent( QEvent* );
		
	public slots:
		void setOnlyText();
		void setOnlyIcon();
		
		void toggleView();
		
	private slots:
		void animate();
		void toggleSensibility();
		
	private:
		Qt::ToolBarArea m_area;
		
		class Animator;
		Animator *m_animator;
		
		bool m_isSensible;
		
		bool m_blending;
		
		QPalette m_palette;
		
		DToolView *m_toolView;
};

#endif
