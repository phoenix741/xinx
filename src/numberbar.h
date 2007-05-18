/***************************************************************************
 *   Copyright (C) 2006 by Ulrich Van Den Hekke                            *
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

#ifndef __NUMBERBAR_H__
#define __NUMBERBAR_H__

#include <QWidget>

class QTextEdit;
class QPaintEvent;

class NumberBar : public QWidget {
	Q_OBJECT
public:
	enum BarDirection { VerticalBar, HorizontalBar } ;

	NumberBar( QWidget *parent, enum BarDirection direction = VerticalBar );
	virtual ~NumberBar();
 
	void setTextEdit( QTextEdit *edit );
	void paintEvent( QPaintEvent *ev );

	void setDirection( enum BarDirection direction );
	enum BarDirection getDirection() { return m_direction; };
private:
	enum BarDirection m_direction;
	QTextEdit *m_edit;
};

#endif // __NUMBERBAR_H__
