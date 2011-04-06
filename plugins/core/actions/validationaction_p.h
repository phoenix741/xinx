/* *********************************************************************** *
* XINX                                                                    *
* Copyright (C) 2006-2010 by Ulrich Van Den Hekke                         *
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
#ifndef VALIDATIONACTION_P_H
#define VALIDATIONACTION_P_H

// Qt header
#include <QAbstractMessageHandler>

class MessageHandler : public QAbstractMessageHandler
{
	Q_OBJECT
public:
	virtual void handleMessage(QtMsgType type, const QString & description, const QUrl & identifier, const QSourceLocation & sourceLocation);

};

#endif // VALIDATIONACTION_P_H
