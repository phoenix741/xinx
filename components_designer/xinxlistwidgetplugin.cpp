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
#include "xinxlistwidgetplugin.h"
#include "xinxlistwidgetimpl.h"

// Qt header
#include <QtPlugin>

/* XinxListWidgetPlugin */

XinxListWidgetPlugin::XinxListWidgetPlugin(QObject *parent) : QObject(parent)
{
	m_initialized = false;
}

void XinxListWidgetPlugin::initialize(QDesignerFormEditorInterface *core)
{
	if (m_initialized)
		return;

	m_initialized = true;
}

bool XinxListWidgetPlugin::isInitialized() const
{
	return m_initialized;
}

bool XinxListWidgetPlugin::isContainer() const
{
	return false;
}

QIcon XinxListWidgetPlugin::icon() const
{
	return QIcon();
}

QString XinxListWidgetPlugin::toolTip() const
{
	return "";
}

QString XinxListWidgetPlugin::whatsThis() const
{
	return "";
}

QString XinxListWidgetPlugin::group() const
{
	return "XINX Widget";
}

QString XinxListWidgetPlugin::name() const
{
	return "XinxListWidgetImpl";
}

QString XinxListWidgetPlugin::includeFile() const
{
	return "xinxlistwidgetimpl.h";
}

QString XinxListWidgetPlugin::domXml() const
{
	return  "<widget class=\"XinxListWidgetImpl\" name=\"xinxListWidget\"/>";
}

QWidget * XinxListWidgetPlugin::createWidget(QWidget *parent)
{
	return new XinxListWidgetImpl(parent);
}

