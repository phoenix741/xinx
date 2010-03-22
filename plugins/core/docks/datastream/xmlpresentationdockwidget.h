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

#ifndef __XMLPRESENTATIONDOCKWIDGET_H__
#define __XMLPRESENTATIONDOCKWIDGET_H__
#pragma once

#include <QDockWidget>
#include <QString>

class XmlPresentationDockThread;

/*!
 * This dock contains a list of XMLPresentationFile founded in the log directory.
 * The log directory can be modified, and the presentation file can be choosen.
 * Some feature as drag & drop must be added in future.
 */
class XmlPresentationDockWidget : public QWidget
{
	Q_OBJECT
public:
	XmlPresentationDockWidget(QWidget * parent = 0);

	virtual ~XmlPresentationDockWidget();

	const QString & filename() const;

	static XmlPresentationDockWidget * self();
signals:
	void filenameChanged(const QString & filename);
private:
	static XmlPresentationDockWidget * s_self;

	XmlPresentationDockThread * d;
	friend class XmlPresentationDockThread;
};


#endif // __XMLPRESENTATIONDOCKWIDGET_H__
