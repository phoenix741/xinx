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

#ifndef COREPLUGIN_H_
#define COREPLUGIN_H_
#pragma once

// Xinx header
#include <plugins/interfaces/plugin.h>
#include <plugins/interfaces/gui.h>
#include <plugins/interfaces/resolver.h>
#include <plugins/interfaces/files.h>
#include <plugins/interfaces/codecompletion.h>
#include <actions/actioninterface.h>
#include "docks/datastream/xmlpresentationdockwidget.h"

class WebPluginSettings;
class ManualFileResolver;

class CorePlugin :  public QObject,
		public IXinxPlugin,
		public IFilePlugin,
		public IResolverPlugin,
		public IXinxPluginConfiguration,
		public IXinxPluginProjectConfiguration,
		public IDockPlugin,
		public IContentViewParserPlugin,
		public ICodeCompletionPlugin
{
	Q_OBJECT
	Q_INTERFACES(IXinxPlugin IXinxPluginConfiguration IXinxPluginProjectConfiguration IResolverPlugin IFilePlugin IDockPlugin IContentViewParserPlugin ICodeCompletionPlugin)
public:
	CorePlugin();
	virtual ~CorePlugin();

	virtual bool initializePlugin(const QString & lang);
	virtual QVariant getPluginAttribute(const enum IXinxPlugin::PluginAttribute & attr);

	virtual QList<IFileTypePlugin*> fileTypes();

	virtual void generateActionMenu();

	virtual QList< QPair<QString,QString> > pluginTools();

	virtual QList<IXinxPluginConfigurationPage*> createSettingsDialog(QWidget * parent);
	virtual QList<IXinxPluginProjectConfigurationPage*> createProjectSettingsPage(QWidget * parent);

	virtual QList<IXinxPluginNewProjectConfigurationPage*> createNewProjectSettingsPages();

	virtual QList<QWidget*> createDocksWidget(QWidget * parent);

	virtual QList<IFileResolverPlugin*> fileResolvers();

	virtual ContentView3::Parser * createContentParser(const QString & type);
	virtual QList<CodeCompletion::ContextParser*> createContextParser() const;
	virtual QList<CodeCompletion::ItemModelFactory*> createItemModelFactory() const;

	XmlPresentationDockWidget * dock();
private:
	QList<IFileTypePlugin*> m_fileTypes;

	ManualFileResolver * m_resolver;
	XmlPresentationDockWidget * m_dock;
};

#endif /* COREPLUGIN_H_*/
