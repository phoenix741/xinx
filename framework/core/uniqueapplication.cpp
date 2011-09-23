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
#include "uniqueapplication.h"
#include <application/mainformimpl.h>
#include <core/exceptions.h>
#include <core/exceptionmanager.h>
#include <editors/editormanager.h>

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
	Q_INIT_RESOURCE(application);

}

UniqueApplication::~UniqueApplication()
{

}

MainformImpl* UniqueApplication::mainWindow()
{
	UniqueApplication * instance = qobject_cast<UniqueApplication*>(QApplication::instance());
	return instance->m_mainform;
}

bool UniqueApplication::notify(QObject * receiver, QEvent * event)
{
	try
	{
		return QtSingleApplication::notify(receiver, event);
	}
	catch (const std::exception & e)
	{
		qFatal(e.what());
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
	connect(ExceptionManager::self(), SIGNAL(errorTriggered(QString)), this, SLOT(slotErrorTriggered(QString)));
	connect(this, SIGNAL(messageReceived(QString)), EditorManager::self(), SLOT(openFile(QString)));
}

void UniqueApplication::slotErrorTriggered(const QString & message)
{
	if (m_mainform)
	{
		// Hide the main widget to prevent user interaction
		m_mainform->hide();
	}
}
