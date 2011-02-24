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

#ifndef _UNIQUEAPPLICATION_H_
#define _UNIQUEAPPLICATION_H_
#pragma once

// Xinx header
#include <core/lib-config.h>

// Qt hearder
#include "qtsingleapplication.h"
#include <QPointer>

class MainformImpl;

class LIBEXPORT UniqueApplication : public QtSingleApplication
{
	Q_OBJECT
public:
	UniqueApplication(int & argc, char ** argv);
	virtual ~UniqueApplication();

	static MainformImpl * mainWindow();

	virtual bool notify(QObject * receiver, QEvent * event);

	void attachMainWindow(MainformImpl * mainform);
public slots:
	void slotErrorTriggered(const QString & message);
private:
	QPointer<MainformImpl> m_mainform;
};

#endif
