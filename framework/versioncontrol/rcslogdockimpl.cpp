/*
 XINX
 Copyright (C) 2007-2011 by Ulrich Van Den Hekke
 xinx@shadoware.org

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful.
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// Xinx header
#include "rcslogdockimpl.h"
#include <core/exceptions.h>
#include <plugins/xinxpluginsloader.h>
#include <core/xinxconfig.h>

// Qt header
#include <QFileInfo>
#include <QPainter>

/* RCSLogDockWidgetImpl */

RCSLogDockWidgetImpl::RCSLogDockWidgetImpl(QWidget * parent) : QWidget(parent), m_dock(0)
{
	setWindowTitle(tr("Version Control Logs"));
	setWindowIcon(QIcon(":/images/ecmascript.png"));

	_widget = new Ui::RCSDockWidget();
	_widget->setupUi(this);
	_widget->m_progressBar->hide();

	// Timer LOG
	_timer = new QTimer(this);
	connect(_timer, SIGNAL(timeout()), this, SLOT(timeout()));

	VersionControl::Manager::self()->setUserInterface(this);
}

RCSLogDockWidgetImpl::~RCSLogDockWidgetImpl()
{

}

void RCSLogDockWidgetImpl::startVersionControlOperations()
{
	if (!m_dock) return;
	init();
	_rcsVisible = m_dock->isVisible();
	m_dock->show();
}

void RCSLogDockWidgetImpl::logVersionControlOperation(RCS::rcsLog niveau, const QString & info)
{
	QListWidgetItem * item = new QListWidgetItem(info, _widget->m_rcsLogListWidget);
	switch (niveau)
	{
		case RCS::LogError :
		case RCS::LogConflict :
			item->setForeground(Qt::red);
			break;
		case RCS::LogLocallyModified :
			item->setForeground(Qt::darkYellow);
			break;
		case RCS::LogRemotlyModified :
			item->setForeground(Qt::darkGreen);
			break;
		case RCS::LogApplication :
			item->setForeground(Qt::lightGray);
			break;
		case RCS::LogNotManaged :
		case RCS::LogNormal :
		default:
			;
	}
	_widget->m_rcsLogListWidget->scrollToItem(item);
}

void RCSLogDockWidgetImpl::stopVersionControlOperations()
{
	end();
	if ((!_rcsVisible) && m_dock && m_dock->isVisible() && XINXConfig::self()->config().project.closeVersionManagementLog)
		_timer->start(5000);
}

void RCSLogDockWidgetImpl::timeout()
{
	_timer->stop();
	if (m_dock)
	{
		m_dock->setVisible(false);
	}
}

void RCSLogDockWidgetImpl::init()
{
	_widget->m_rcsLogListWidget->clear();
	_widget->m_progressBar->show();
}

void RCSLogDockWidgetImpl::end()
{
	_widget->m_progressBar->hide();
}

void RCSLogDockWidgetImpl::setDock(QDockWidget * dock)
{
	m_dock = dock;
}
