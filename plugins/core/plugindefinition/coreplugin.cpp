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

// Xinx header
#include "coreplugin.h"

#include "config/editorsconfigformimpl.h"
#include "config/xmlpresconfigformimpl.h"
#include "config/selfwebpluginsettings.h"

#include "filetypeplugin.h"

#include "actions/stylesheetaction.h"
#include "actions/commentactions.h"

#include <project/xinxproject.h>
#include "pluginproperty/parserprojectpropertyimpl.h"
#include "pluginproperty/searchpathlistprjpageform.h"

#include "pluginresolver/manualfileresolver.h"

#include <contentview2/contentview2manager.h>
#include "editors/models/xsl/xmlcompletionparser.h"
#include "editors/models/xsl/xslcontentviewparser.h"
#include "editors/models/js/jscontentviewparser.h"
#include <snipets/snipetcompletionparser.h>
#include <snipets/snipetmanager.h>
#include <contentview2/contentview2cache.h>

// Qt header
#include <QStringList>
#include <QHash>
#include <QDir>
#include <QApplication>
#include <QTextEdit>
#include <QTranslator>
#include <QApplication>
#include <QAction>

/* CorePlugin */

CorePlugin::CorePlugin() : m_dock(0)
{
	Q_INIT_RESOURCE(coreplugin);

	m_fileTypes << new XSLStyleSheetFileType;
	m_fileTypes << new XMLFileType;
	m_fileTypes << new HTMLFileType;
	m_fileTypes << new JSFileType;
	m_fileTypes << new CSSFileType;
	m_fileTypes << new XQFileType;
	m_fileTypes << new TextFileType;
}

CorePlugin::~CorePlugin()
{
	qDeleteAll(m_fileTypes);
	delete SelfWebPluginSettings::self();
}

bool CorePlugin::initializePlugin(const QString & lang)
{
	QTranslator * tranlator = new QTranslator(this);
	tranlator->load(QString(":/translations/coreplugin_%1").arg(lang));
	qApp->installTranslator(tranlator);

	m_resolver = new ManualFileResolver();

	ContentView2::Manager::self()->addInitializationParser(true, "Snipet", "Snipet", "*");
	ContentView2::Manager::self()->addInitializationParser(true, "XmlCompletion", "XmlCompletion", "*");

	connect(SnipetManager::self(), SIGNAL(changed()), this, SLOT(snipetChanged()));

	return true;
}

QVariant CorePlugin::getPluginAttribute(const enum IXinxPlugin::PluginAttribute & attr)
{
	switch (attr)
	{
	case PLG_NAME:
		return tr("Core Plugin");
	case PLG_DESCRIPTION:
		return tr("Minimum editors, completer, docks for XINX.");
	case PLG_ICON:
		return QPixmap(":/images/coreplugin.png");
	case PLG_AUTHOR:
		return "Ulrich Van Den Hekke";
	case PLG_EMAIL:
		return "ulrich.vdh@shadoware.org";
	case PLG_WEBSITE:
		return "http://www.shadoware.org";
	case PLG_VERSION:
		return "1.0";
	case PLG_LICENCE:
		return "GPL v2.0 or later";
	}
	return QVariant();
}

QList<IFileTypePlugin*> CorePlugin::fileTypes()
{
	return m_fileTypes;
}

ContentView2::Parser * CorePlugin::createParser(const QString & type)
{
	if (type == "XSL")
	{
		return new XslContentView2Parser();
	}
	else if (type == "JS")
	{
		return new JsContentViewParser();
	}
	else if (type == "CSS")
	{
		return new CSSFileContentParser();
	}
	else if (type == "Snipet")
	{
		return new SnipetCompletionParser();
	}
	else if (type == "XmlCompletion")
	{
		return XmlCompletionParser::self()->clone();
	}
	else
	{
		return 0;
	}
}

XinxAction::MenuList CorePlugin::actions()
{
	if (m_menus.size() == 0)
	{
		XinxAction::Action * gotoLineAction = new GotoLineAction(tr("Go to &Line ..."), QString("Ctrl+G"), this);
		XinxAction::Action * commentAction = new CommentAction(tr("&Comment"), QString("Ctrl+Shift+C"), this);
		XinxAction::Action * uncommentAction = new UncommentAction(tr("&Uncomment"), QString("Ctrl+Shift+D"), this);

		gotoLineAction->action()->setStatusTip(tr("Move the cursor to line, choose by user"));
		gotoLineAction->action()->setWhatsThis(tr("This action open a dialog box to choose the line number where you want to go."));

		commentAction->action()->setStatusTip(tr("Comment the selected text"));
		commentAction->action()->setWhatsThis(tr("Comment the selected text by using the language syntax. <ul><li>In <b>XML</b> like format <i>&lt;!-- comment --&gt;</i></li> <li>In <b>Javascript</b> : <i>/* comment */</i> </li></ul>"));

		uncommentAction->action()->setStatusTip(tr("Uncomment the selected text if commented"));
		uncommentAction->action()->setWhatsThis(tr("See the comment helper function"));

		XinxAction::Action * runAction = new StyleSheetAction(QIcon(":/images/run.png"), tr("Process stylesheet"), QString("F9"), this);

		XinxAction::ActionList editMenu(tr("&Edit"), "edit");
		XinxAction::ActionList runMenu(tr("&Execute"), "execute");

		editMenu.append(gotoLineAction);
		editMenu.append(new XinxAction::Separator);
		editMenu.append(commentAction);
		editMenu.append(uncommentAction);

		runMenu.append(runAction);

		m_menus.append(editMenu);
		m_menus.append(runMenu);
	}
	return m_menus;
}

QList< QPair<QString,QString> > CorePlugin::pluginTools()
{
	QList< QPair<QString,QString> > tools;
#ifdef Q_WS_WIN
	tools.append(qMakePair(QString("diff"), QString("%1/Winmerge/WinmergeU.exe").arg("C:/Program Files")));
#else
	tools.append(qMakePair(QString("diff"), QString("/usr/bin/kompare")));
#endif // Q_WS_WIN
	return tools;
}

QList<IXinxPluginConfigurationPage*> CorePlugin::createSettingsDialog(QWidget * parent)
{
	QList<IXinxPluginConfigurationPage*> pages;
	pages << new EditorsConfigFormImpl(parent);
	pages << new XmlPresConfigFormImpl(parent);
	return pages;
}

QList<IXinxPluginProjectConfigurationPage*> CorePlugin::createProjectSettingsPage(QWidget * parent)
{
	QList<IXinxPluginProjectConfigurationPage*> list;
	list << new ParserProjectPropertyImpl(parent);
	list << new SearchPathListFormImpl(parent);
	return list;
}

QList<IXinxPluginNewProjectConfigurationPage*> CorePlugin::createNewProjectSettingsPages()
{
	QList<IXinxPluginNewProjectConfigurationPage*> pages;
	pages << new WebPluginProjectPropertyWizard();
	return pages;
}

QList<QWidget*> CorePlugin::createDocksWidget(QWidget * parent)
{
	QList<QWidget*> docks;
	if (! m_dock)
	{
		m_dock = new XmlPresentationDockWidget(parent);
		m_dock->setObjectName(QString::fromUtf8("m_xmlPresDock"));
	}
	if ((m_dock->parent() != parent) && (parent != 0))
	{
		m_dock->setParent(parent);
	}

	docks << m_dock;
	return docks;
}

XmlPresentationDockWidget * CorePlugin::dock()
{
	return m_dock;
}

QList<IFileResolverPlugin*> CorePlugin::fileResolvers()
{
	return QList<IFileResolverPlugin*>() << m_resolver;
}

void CorePlugin::snipetChanged()
{
	ContentView2::Manager::self()->cache()->addToCache(0, "Snipet", "Snipet", "*");
}

Q_EXPORT_PLUGIN2(coreplugin, CorePlugin)
