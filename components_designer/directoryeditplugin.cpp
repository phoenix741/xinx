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
#include "directoryeditplugin.h"
#include "directoryedit.h"

// Qt header
#include <QtPlugin>

/* DirectoryEditPlugin */

DirectoryEditPlugin::DirectoryEditPlugin(QObject *parent) : QObject(parent)
{
	m_initialized = false;
}

void DirectoryEditPlugin::initialize(QDesignerFormEditorInterface *core)
{
	if (m_initialized)
		return;

	m_initialized = true;
}

bool DirectoryEditPlugin::isInitialized() const
{
	return m_initialized;
}

bool DirectoryEditPlugin::isContainer() const
{
	return false;
}

QIcon DirectoryEditPlugin::icon() const
{
	return QIcon();
}

QString DirectoryEditPlugin::toolTip() const
{
	return "";
}

QString DirectoryEditPlugin::whatsThis() const
{
	return "";
}

QString DirectoryEditPlugin::group() const
{
	return "XINX Widget";
}

QString DirectoryEditPlugin::name() const
{
	return "DirectoryEdit";
}

QString DirectoryEditPlugin::includeFile() const
{
	return "directoryedit.h";
}

QString DirectoryEditPlugin::domXml() const
{
	return  "<widget class=\"DirectoryEdit\" name=\"directoryEdit\"/>";
}

QWidget * DirectoryEditPlugin::createWidget(QWidget *parent)
{
	return new DirectoryEdit(parent);
}

