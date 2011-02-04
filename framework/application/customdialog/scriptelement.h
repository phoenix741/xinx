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

#ifndef SCRIPTELEMENT_H_
#define SCRIPTELEMENT_H_
#pragma once

// Qt header
#include <QString>
#include <QApplication>

// Xinx header
#include <scripts/scriptmanager.h>
#include <plugins/xinxpluginelement.h>

class ScriptElement : public PluginElement
{
	Q_DECLARE_TR_FUNCTIONS(ScriptElement)
public:
	ScriptElement(const ScriptValue & script);

	virtual bool isModifiable() const;
	virtual bool isConfigurable() const;

	virtual QPixmap pixmap() const;
	virtual QString name() const;
	virtual QString author() const;
	virtual QString version() const;
	virtual QString licence() const;
	virtual QString description() const;

	ScriptValue & script();
private:
	ScriptValue m_script;
};

#endif /* SCRIPTELEMENT_H_ */
