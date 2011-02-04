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

#ifndef SERVICESPLUGIN_H_
#define SERVICESPLUGIN_H_
#pragma once

// Xinx header
#include <plugins/interfaces/plugin.h>
#include <plugins/interfaces/gui.h>
#include <plugins/interfaces/files.h>
#include <actions/actioninterface.h>

class ServicesPlugin : public QObject, public IXinxPlugin, public IFilePlugin, public IXinxPluginProjectConfiguration, public IXinxInputOutputPlugin
{
	Q_OBJECT
	Q_INTERFACES(IXinxPlugin IFilePlugin IXinxPluginProjectConfiguration IXinxInputOutputPlugin)
public:
	ServicesPlugin();
	virtual ~ServicesPlugin();

	virtual bool initializePlugin(const QString & lang);
	virtual QVariant getPluginAttribute(const enum IXinxPlugin::PluginAttribute & attr);

	virtual void generateActionMenu ();

	virtual QList<IFileTypePlugin*> fileTypes();
	virtual QList<IXinxPluginProjectConfigurationPage*> createProjectSettingsPage(QWidget * parent);
	virtual QList<IXinxPluginNewProjectConfigurationPage*> createNewProjectSettingsPages();

    virtual QList< IProjectInitialisationStep* > loadProjectStep(XinxProject::Project* project);
    virtual QList< IProjectInitialisationStep* > closeProjectStep(XinxProject::Project* project);
    virtual QString getFilename(AbstractEditor* editor, const QString& filename, const QString& defaultFilename, const QString& filter, bool saveAs, bool& accept, QWidget* widget = 0);
    virtual QIODevice* loadFile(AbstractEditor* editor, const QString& filename);
    virtual QIODevice* saveFile(AbstractEditor* editor, const QString& filename, const QString& oldfilename);
private:
	QList<IFileTypePlugin*> m_fileTypes;
};

#endif /* SERVICESPLUGIN_H_*/
