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

#include "projecttreeview.h"

#include <QContextMenuEvent>
#include <QMenu>
#include <QIcon>

/* PrivateProjectTreeView */

class PrivateProjectTreeView : public QObject {
	Q_OBJECT
public:
	PrivateProjectTreeView( ProjectTreeView * parent );
	bool eventFilter( QObject *obj, QEvent *event );
private:
	ProjectTreeView * m_parent;
};

PrivateProjectTreeView::PrivateProjectTreeView( ProjectTreeView * parent ) {
	m_parent = parent;
}

bool PrivateProjectTreeView::eventFilter( QObject *obj, QEvent *event ) {
	if ( ( obj == m_parent ) && ( event->type() == QEvent::ContextMenu ) ) {
		// Search the item selected
		QMenu *menu = new QMenu( m_parent );
		menu->addAction( QIcon(":/reload.png"), "toto" );
		menu->exec( static_cast<QContextMenuEvent*>(event)->globalPos() );
		delete menu;
	}
	return QObject::eventFilter( obj, event );
}

/* ProjectTreeView */

ProjectTreeView::ProjectTreeView( QWidget * parent ) : QTreeView( parent ) {
	d = new PrivateProjectTreeView( this );
	this->installEventFilter( d );
}

ProjectTreeView::~ProjectTreeView() {
	delete d;
}


#include "projecttreeview.moc"
