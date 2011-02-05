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

#ifndef XQUERY_KEYWORD_H_
#define XQUERY_KEYWORD_H_
#pragma once

// Xinx header
#include <core/xinxsingleton.h>

// Qt header
#include <QHash>
#include <QString>

/* XQueryKeyword */

class XQueryKeyword : public XinxSingleton<XQueryKeyword>
{
	Q_OBJECT
public:
	~XQueryKeyword();

	const QHash<QString,QString> & keywords() const;
private:
	XQueryKeyword();

	QHash<QString,QString> m_keywords;
	friend class XinxSingleton<XQueryKeyword>;
};

#endif /* XQUERY_KEYWORD_H_ */
