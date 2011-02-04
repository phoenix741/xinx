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
#include "xinxdesignerplugin.h"
#include "directoryeditplugin.h"
#include "directoryeditwidgetplugin.h"
#include "xinxcolorcomboplugin.h"
#include "pluginselectorplugin.h"
#include "xinxlistwidgetplugin.h"
#include "customschemeplugin.h"

XinxDesignerPlugin::XinxDesignerPlugin(QObject *parent) : QObject(parent)
{
	widgets.append(new DirectoryEditPlugin(this));
	widgets.append(new DirectoryEditWidgetPlugin(this));
	widgets.append(new XinxColorComboPlugin(this));
	widgets.append(new PluginSelectorPlugin(this));
	widgets.append(new XinxListWidgetPlugin(this));
	widgets.append(new CustomSchemeWidgetPlugin(this));
}

QList<QDesignerCustomWidgetInterface*> XinxDesignerPlugin::customWidgets() const
{
	return widgets;
}

Q_EXPORT_PLUGIN2(xinxdesignerplugin, XinxDesignerPlugin);
