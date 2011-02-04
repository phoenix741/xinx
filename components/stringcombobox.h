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

#pragma once
#ifndef _STRINGCOMBOBOX_H_
#define _STRINGCOMBOBOX_H_

// Qt header
#include <QComboBox>

#include <components-config.h>

/* StringComboBox */

class COMPONENTSEXPORT StringComboBox : public QComboBox
{
	Q_OBJECT
	Q_PROPERTY(QString value READ value WRITE setValue USER true)
public:
	StringComboBox(QWidget * parent = 0);
	virtual ~StringComboBox();

	QString value() const;
public slots:
	void setValue(const QString & value);
private:
};

#endif
