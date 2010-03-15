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
#include "uniqueapplication.h"
#include "mainformimpl.h"
#include <core/exceptions.h>
#include <project/xinxproject.h>

#include "mainformimpl.h"

// Qt header
#include <QString>
#include <QMessageBox>
#include <QThread>
#include <QDebug>

// Qt header
#include <QObject>

// Standard header
#include <unistd.h>

/* UniqueApplication */

UniqueApplication::UniqueApplication(int & argc, char ** argv) : QtSingleApplication(QString("XINX"), argc, argv), m_mainform(0)
{

}

UniqueApplication::~UniqueApplication()
{

}

bool UniqueApplication::notify(QObject * receiver, QEvent * event)
{
	try
	{
		return QtSingleApplication::notify(receiver, event);
	}
	catch (XinxException e)
	{
		qFatal(qPrintable(e.getMessage()));
		return true;
	}
	catch (...)
	{
		qFatal("Generic Exception");
		return true;
	}
}

void UniqueApplication::attachMainWindow(MainformImpl * mainform)
{
	m_mainform = mainform;

	setActivationWindow(m_mainform);
	connect(ExceptionManager::self(), SIGNAL(errorTriggered()), this, SLOT(slotErrorTriggered()));
	connect(this, SIGNAL(messageReceived(QString)), mainform, SLOT(openFile(QString)));
}

void UniqueApplication::slotErrorTriggered()
{
	if (m_mainform)
	{
		// Hide the main widget
		m_mainform->hide();

		if (XINXProjectManager::self()->project())
			m_mainform->saveProject(true);
	}
}
