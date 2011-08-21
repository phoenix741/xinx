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
#include <plugins/interfaces/plugin.h>
#include <plugins/interfaces/files.h>
#include <plugins/interfaces/gui.h>
#include <plugins/interfaces/resolver.h>
#include <plugins/interfaces/xsltparser.h>
#include <plugins/interfaces/codecompletion.h>
#include "docks/dictionary/dictionarydockwidgetimpl.h"
#include "docks/project/generixprojectdock.h"

class Gce150FileResolver;

/* GenerixPlugin */

class GenerixPlugin :   public QObject,
			public IXinxPlugin,
			public IFilePlugin,
			public IXinxInputOutputPlugin,
			public IDockPlugin,
			public IXinxPluginConfiguration,
			public IResolverPlugin,
			public IXinxPluginProjectConfiguration,
			public IXinxXsltParser,
			public IContentViewParserPlugin,
			public ICodeCompletionPlugin
{
	Q_OBJECT
	Q_INTERFACES(IXinxPlugin IXinxInputOutputPlugin IDockPlugin IXinxPluginConfiguration IFilePlugin IResolverPlugin IXinxPluginProjectConfiguration IXinxXsltParser IContentViewParserPlugin ICodeCompletionPlugin)
public:
	GenerixPlugin();
	virtual ~GenerixPlugin();

	virtual bool initializePlugin(const QString & lang);
	virtual QVariant getPluginAttribute(const enum IXinxPlugin::PluginAttribute & attr);

	virtual ContentView3::Parser * createContentParser(const QString & type);
	virtual QList<CodeCompletion::ContextParser*> createContextParser() const;
	virtual QList<CodeCompletion::ItemModelFactory*> createItemModelFactory() const;
	virtual XsltParser * createXsltParser(AbstractEditor* editor);

	virtual QList<IProjectInitialisationStep*> loadProjectStep(XinxProject::ProjectPtr project);
	virtual QList<IProjectInitialisationStep*> closeProjectStep(XinxProject::ProjectPtr project);

	virtual QList<IFileTypePlugin*> createFileTypes();

	virtual QIODevice * loadFile(AbstractEditor * editor, const QString & filename);
	virtual QIODevice * saveFile(AbstractEditor * editor, const QString & filename, const QString & oldfilename);
	virtual QString getFilename(AbstractEditor * editor, const QString & filename, const QString & defaultFilename, const QString & filter, bool saveAs, bool & accept, QWidget * widget = 0);

	virtual QList<XinxDockWidget*> createDocksWidget(QWidget * parent);

	virtual QList<IFileResolverPlugin*> fileResolvers();

	virtual QList<IXinxPluginConfigurationPage*> createSettingsDialog(QWidget * parent);
	virtual QList<IXinxPluginProjectConfigurationPage*> createProjectSettingsPage(QWidget * parent);
	virtual QList<IXinxPluginNewProjectConfigurationPage*> createNewProjectSettingsPages();
private:
	GenerixProjectDockImpl * m_gnxDock;
	DictionaryDockWidgetImpl * m_dock;
	Gce150FileResolver * m_resolver;
};

#endif /* GENERIXPLUGIN_H_ */
