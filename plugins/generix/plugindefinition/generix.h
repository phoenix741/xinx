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

#ifndef GENERIXPLUGIN_H_
#define GENERIXPLUGIN_H_
#pragma once

// Xinx header
#include <plugins/plugininterfaces.h>
#include "docks/dictionary/dictionarydockwidgetimpl.h"
#include "docks/project/generixprojectdock.h"

class Gce150FileResolver;

/* GenerixPlugin */

class GenerixPlugin :   public QObject,
		public IFilePlugin,
		public IXinxInputOutputPlugin,
		public IDockPlugin,
		public IXinxPluginConfiguration,
		public IResolverPlugin,
		public IXinxPluginProjectConfiguration,
		public IXinxXsltParser,
		public IContentViewParserPlugin
{
	Q_OBJECT
	Q_INTERFACES(IXinxPlugin)
	Q_INTERFACES(IXinxInputOutputPlugin)
	Q_INTERFACES(IDockPlugin)
	Q_INTERFACES(IXinxPluginConfiguration)
	Q_INTERFACES(IFilePlugin)
	Q_INTERFACES(IResolverPlugin)
	Q_INTERFACES(IXinxPluginProjectConfiguration)
	Q_INTERFACES(IXinxXsltParser)
	Q_INTERFACES(IContentViewParserPlugin)
public:
	GenerixPlugin();
	virtual ~GenerixPlugin();

	virtual bool initializePlugin(const QString & lang);
	virtual QVariant getPluginAttribute(const enum IXinxPlugin::PluginAttribute & attr);

	ContentView2::Parser * createParser(const QString & type);
	virtual XsltParser * createXsltParser();

	virtual QList<IProjectInitialisationStep*> loadProjectStep(XinxProject * project);
	virtual QList<IProjectInitialisationStep*> closeProjectStep(XinxProject * project);

	virtual QList<IFileTypePlugin*> fileTypes();

	virtual QIODevice * loadFile(const QString & filename);
	virtual QIODevice * saveFile(const QString & filename, const QString & oldfilename);
	virtual QString getFilename(const QString & filename, const QString & defaultFilename, const QString & filter, bool saveAs, bool & accept, QWidget * widget = 0);

	virtual QList<QWidget*> createDocksWidget(QWidget * parent);

	virtual QList<IFileResolverPlugin*> fileResolvers();

	virtual QList<IXinxPluginConfigurationPage*> createSettingsDialog(QWidget * parent);
	virtual QList<IXinxPluginProjectConfigurationPage*> createProjectSettingsPage(QWidget * parent);
	virtual QList<IXinxPluginNewProjectConfigurationPage*> createNewProjectSettingsPages();
private:
	QList<IFileTypePlugin*> m_fileTypes;
	GenerixProjectDockImpl * m_gnxDock;
	DictionaryDockWidgetImpl * m_dock;
	Gce150FileResolver * m_resolver;
};

#endif /* GENERIXPLUGIN_H_ */
