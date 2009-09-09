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
/*
 * This class is inspired from the Nokia class MenuModel, used in the browser
 * application in demos.
 */

// Xinx header
#include "qmenuview.h"

Q_DECLARE_METATYPE(QModelIndex);

/* QMenuView */

QMenuView::QMenuView( QWidget * parent ) : QMenu( parent ) {
	connect(this, SIGNAL(aboutToShow()), this, SLOT(aboutToShow()));
}

QMenuView::~QMenuView() {
	setModel( 0 );
}

void QMenuView::setModel ( QAbstractItemModel * model ) {
	m_model = model;
}

QAbstractItemModel * QMenuView::model () const {
	return m_model;
}

void QMenuView::setRootIndex ( const QModelIndex & index ) {
	m_root = index;
}

QModelIndex QMenuView::rootIndex () const {
	return m_root;
}

void QMenuView::triggered( QAction *action ) {
	QVariant v = action->data();
	if( v.canConvert<QModelIndex>() ) {
		QModelIndex idx = qvariant_cast<QModelIndex>(v);
		emit activated( idx );
	}
}

void QMenuView::hovered( QAction *action ) {
	QVariant v = action->data();
	if (v.canConvert<QModelIndex>()) {
		QModelIndex idx = qvariant_cast<QModelIndex>(v);
		QString hoveredString = idx.data( Qt::StatusTipRole ).toString();
		if (!hoveredString.isEmpty())
			emit hovered(hoveredString);
	}
}

void QMenuView::aboutToShow() {
	QMenu * menu = qobject_cast<QMenu*>( sender() );
	if( menu ) {
		QVariant v = menu->menuAction()->data();
		if( v.canConvert<QModelIndex>() ) {
			QModelIndex idx = qvariant_cast<QModelIndex>(v);
			createMenu( idx, menu, menu );
			disconnect(menu, SIGNAL(aboutToShow()), this, SLOT(aboutToShow()));
			return;
		}
	}

	clear();

	createMenu( m_root, this, this );
}

void QMenuView::createMenu( const QModelIndex &parent, QMenu *parentMenu, QMenu *menu ) {
	if( ! menu ) {
		QIcon icon = qvariant_cast<QIcon>( parent.data( Qt::DecorationRole ) );

		QVariant v;
		v.setValue( parent );

		menu = new QMenu( parent.data().toString(), this );
		menu->setIcon( icon );
		parentMenu->addMenu( menu );
		menu->menuAction()->setData( v );

		connect( menu, SIGNAL(aboutToShow()), this, SLOT(aboutToShow()) );

		return;
	}

	connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(triggered(QAction*)));
	connect(menu, SIGNAL(hovered(QAction*)), this, SLOT(hovered(QAction*)));

	int end = m_model->rowCount( parent );
	for( int i = 0; i < end; ++i ) {
		QModelIndex idx = m_model->index( i, 0, parent );
		if( m_model->hasChildren( idx ) ) {
			createMenu( idx, menu );
		} else {
			menu->addAction( makeAction(idx) );
		}
	}
}

QAction * QMenuView::makeAction( const QModelIndex &index ) {
	QIcon icon = qvariant_cast<QIcon>( index.data(Qt::DecorationRole) );
	QAction * action = new QAction( icon, index.data().toString(), this );
	QVariant v;
	v.setValue(index);
	action->setData(v);
	return action;
}
