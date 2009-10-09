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

// Xinx header
#include "welcomdlgimpl.h"
#include "welcomdlgimpl_p.h"

// Qt header
#include <QHeaderView>
#include <QFileInfo>

/* WelcomDialogImpl */

WelcomDialogImpl::WelcomDialogImpl( QWidget * parent, Qt::WindowFlags f ) : QDialog( parent, f ) {
	setupUi( this );
	createWebsiteList();
}

WelcomDialogImpl::~WelcomDialogImpl() {
}

void WelcomDialogImpl::createWebsiteList() {
	sitesTreeWidget->addItem( tr("Documentation"), QLatin1String("http://xinx.shadoware.org/wiki") );
	sitesTreeWidget->addItem( tr("Report a bug"), QLatin1String("http://xinx.shadoware.org/newticket") );
	sitesTreeWidget->addItem( tr("Downloads"), QLatin1String("http://xinx.shadoware.org/downloads") );
}

void WelcomDialogImpl::addProjectFile( const QString & filename ) {
	projectWidget->addItem( QFileInfo( filename ).fileName(), filename );
}


/* WelcomTreeWidget */


WelcomTreeWidget::WelcomTreeWidget( QWidget *parent ) : QListWidget( parent ), m_bullet(QLatin1String(":/images/bullet_arrow.png")) {
	connect(this, SIGNAL(itemClicked(QListWidgetItem *)), SLOT(slotItemClicked(QListWidgetItem *)));
}

QSize WelcomTreeWidget::minimumSizeHint() const {
	return QSize();
}

QSize WelcomTreeWidget::sizeHint() const {
	return QSize( QListWidget::sizeHint().width(), 30 * count() );
}

QListWidgetItem *WelcomTreeWidget::addItem( const QString &label, const QString &data ) {
	QListWidgetItem *item = new QListWidgetItem( this );
	item->setIcon( m_bullet );
	item->setSizeHint( QSize(24, 30) );

	QLabel * lbl = new QLabel( label );
	lbl->setTextInteractionFlags( Qt::NoTextInteraction );
	lbl->setCursor( QCursor(Qt::PointingHandCursor) );
	lbl->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );

	QBoxLayout *lay = new QVBoxLayout;
	lay->setContentsMargins( 3, 2, 0, 0 );
	lay->addWidget( lbl );

	QWidget *wdg = new QWidget;
	wdg->setLayout( lay );
	setItemWidget( item, wdg );
	item->setData( Qt::UserRole, data );

	return item;
}

void WelcomTreeWidget::slotItemClicked( QListWidgetItem * item ) {
	emit activated( item->data( Qt::UserRole ).toString() );
}
