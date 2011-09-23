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

#ifndef __EXCEPTIONMANAGER_H__
#define __EXCEPTIONMANAGER_H__
#pragma once

// Xinx header
#include <core/lib-config.h>
#include <core/xinxcore.h>
#include <core/xinxsingleton.h>

// Qt header
#include <QStringList>
#include <QHash>

class QWidget;
class XinxErrorMessage;

class LIBEXPORT ExceptionManager : public QObject /* This can't be an official singleton, because it's the logger */
{
	Q_OBJECT
public:
	virtual ~ExceptionManager();
	static ExceptionManager * self();

	QStringList stackTrace() const;

	static void installExceptionHandler();
	static void installSignalHandler();
public slots:
	void openTicketDialog(const QString& message = QString(), const QStringList & stack = QStringList()) const;
	void notifyError(QString error, QtMsgType t = QtWarningMsg, bool showMessage = true);
signals:
	void errorTriggered(const QString & message);
private:
	ExceptionManager();

	QHash<unsigned long,QStringList> m_stackTrace;
	QStringList m_exceptionFilter;

	static ExceptionManager * s_self;
};

#endif /* __EXCEPTIONMANAGER_H__ */
