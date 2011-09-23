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
#include "core/errormanager.h"
#include <core/exceptions.h>

// Qt header
#include <QThread>
#include <QMetaType>
#include <QApplication>

/* ErrorManager */

ErrorManager::ErrorManager()
{
	qRegisterMetaType<QtMsgType>("QtMsgType");
	qRegisterMetaType<XinxException>("XinxException");

	m_timer.setSingleShot(true);
	m_timer.setInterval(500);
	connect(&m_timer, SIGNAL(timeout()), this, SIGNAL(changed()));
}

ErrorManager::~ErrorManager()
{

}

void ErrorManager::addContextTranslation(const QString & context, const QString & translation)
{
	m_contextTranslation.insert(context, translation);
}

void ErrorManager::removeContextTranslation(const QString & context)
{
	m_contextTranslation.remove(context);
}

void ErrorManager::clearMessages(const QString & context)
{
	const QString ctxt = m_contextTranslation.value(context).isEmpty() ? context : m_contextTranslation.value(context);

	m_errors.remove(ctxt);

	if (QThread::currentThread() == qApp->thread())
		m_timer.start();
	else
		QMetaObject::invokeMethod(&m_timer, "start", Qt::BlockingQueuedConnection);
}

void ErrorManager::addMessage(const QString & context, int line, QtMsgType t, const QString & message, const QStringList & parameters)
{
	const QString ctxt = m_contextTranslation.value(context).isEmpty() ? context : m_contextTranslation.value(context);

	struct Error e = {line, t, message, parameters};
	if (!m_errors[ctxt].contains(e))
		m_errors[ctxt].append(e);

	if (QThread::currentThread() == qApp->thread())
		m_timer.start();
	else
		QMetaObject::invokeMethod(&m_timer, "start", Qt::BlockingQueuedConnection);
}

void ErrorManager::addMessage(const QString & context, int line, QtMsgType t, const XinxException & exception)
{
	const QString ctxt = m_contextTranslation.value(context).isEmpty() ? context : m_contextTranslation.value(context);

	struct Error e = {line, t, exception.getMessage(), QStringList()};
	if (!m_errors[ctxt].contains(e))
		m_errors[ctxt].append(e);

	if (QThread::currentThread() == qApp->thread())
		m_timer.start();
	else
		QMetaObject::invokeMethod(&m_timer, "start", Qt::BlockingQueuedConnection);
}

const QMap<QString, QList<ErrorManager::Error> > & ErrorManager::errors() const
{
	return m_errors;
}
