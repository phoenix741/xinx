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

/* WelcomTreeWidget */


WelcomTreeWidget::WelcomTreeWidget( QWidget *parent ) : QTreeWidget( parent ), m_bullet(QLatin1String(":/images/bullet_arrow.png")) {
	connect(this, SIGNAL(itemClicked(QTreeWidgetItem *, int)), SLOT(slotItemClicked(QTreeWidgetItem *)));
}

QSize WelcomTreeWidget::minimumSizeHint() const {
	return QSize();
}

QSize WelcomTreeWidget::sizeHint() const {
	return QSize( QTreeWidget::sizeHint().width(), 30 * topLevelItemCount() );
}

QTreeWidgetItem *WelcomTreeWidget::addItem( const QString &label, const QString &data ) {
	QTreeWidgetItem *item = new QTreeWidgetItem( this );
	item->setIcon( 0, m_bullet );
	item->setSizeHint( 0, QSize(24, 30) );

	QLabel * lbl = new QLabel( label );
	lbl->setTextInteractionFlags( Qt::NoTextInteraction );
	lbl->setCursor( QCursor(Qt::PointingHandCursor) );
	lbl->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );

	QBoxLayout *lay = new QVBoxLayout;
	lay->setContentsMargins( 3, 2, 0, 0 );
	lay->addWidget( lbl );

	QWidget *wdg = new QWidget;
	wdg->setLayout( lay );
	setItemWidget( item, 1, wdg );
	item->setData( 0, Qt::UserRole, data );

	return item;
}

void WelcomTreeWidget::slotAddNewsItem( const QString &title, const QString &description, const QString &link ) {
	int itemWidth = width()-header()->sectionSize(0);
	QFont f = font();
	QString elidedText = QFontMetrics(f).elidedText(description, Qt::ElideRight, itemWidth);

	f.setBold(true);
	QString elidedTitle = QFontMetrics(f).elidedText(title, Qt::ElideRight, itemWidth);
	QString data = QString::fromLatin1("<b>%1</b><br />%2").arg(elidedTitle).arg(elidedText);

	addTopLevelItem( addItem( data,link ) );
}

void WelcomTreeWidget::slotItemClicked( QTreeWidgetItem * item ) {
	emit activated( item->data( 0, Qt::UserRole ).toString() );
}
