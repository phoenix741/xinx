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

#ifndef __EXCEPTIONSMANAGER_H__
#define __EXCEPTIONSMANAGER_H__
#pragma once

// Xinx header
#include <core/lib-config.h>
#include <core/xinxsingleton.h>
#include <core/exceptions.h>

// Qt header
#include <QTimer>
#include <QStringList>

class LIBEXPORT ErrorManager : public XinxLibSingleton<ErrorManager>
{
	Q_OBJECT
public:
	struct Error
	{
		int line;
		QtMsgType type;
		QString message;
		QStringList parameters;

		bool operator==(const Error & e) const
		{
			return (line == e.line) && (type == e.type) && (message == e.message) && (parameters == e.parameters);
		}
	};

	virtual ~ErrorManager();

	const QMap<QString, QList<Error> > & errors() const;
public slots:
	void addContextTranslation(const QString & context, const QString & translation);
	void removeContextTranslation(const QString & context);

	void clearMessages(const QString & context);
	void addMessage(const QString & context, int line, QtMsgType t, const QString & message, const QStringList & parameters = QStringList());
	void addMessage(const QString & context, int line, QtMsgType t, const XinxException & exception);
signals:
	void changed();
private:
	ErrorManager();

	QHash<QString,QString> m_contextTranslation;
	QMap<QString, QList<Error> > m_errors;
	QTimer m_timer;
	friend class XinxLibSingleton<ErrorManager>;
};

#endif /* __EXCEPTIONSMANAGER_H__ */
