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

#pragma once
#ifndef XQUERYDIALOGIMPL_P_H_
#define XQUERYDIALOGIMPL_P_H_

#include <QAbstractMessageHandler>
#include <QObject>
#include <QString>
#include <QStringList>

class XQueryDialogMessageHandler : public QAbstractMessageHandler
{
	Q_OBJECT
public:
	XQueryDialogMessageHandler(QObject * parent = 0);
	virtual ~XQueryDialogMessageHandler();

	const QStringList & messages() const;
protected:
	virtual void handleMessage(QtMsgType type, const QString & description, const QUrl & identifier, const QSourceLocation & sourceLocation);
private:
	QStringList m_messages;
};


#endif /* XQUERYDIALOGIMPL_P_H_ */
