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

#include <actions/actionmanager.h>
#include "actions/bookmarkactions.h"
#include "actions/clipboardactions.h"
#include "actions/editoractions.h"
#include "actions/texteditoractions.h"
#include "actions/stylesheetaction.h"
#include "actions/commentactions.h"
#include "actions/validationaction.h"

#include "pluginproperty/parserprojectpropertyimpl.h"
#include "pluginproperty/searchpathlistprjpageform.h"

#include "pluginresolver/manualfileresolver.h"

#include "editors/models/html_xsl_base/xmlcompletionparser.h"
#include "editors/models/xsl/stylesheet_parser.h"
#include "editors/models/js/jscontentviewparser.h"
#include <contentview3/cache.h>
#include <editors/models/html_xsl_base/xmlcontextparser.h>
#include <editors/models/html_xsl_base/itemmodelfactory.h>
#include <editors/models/xsl/itemmodelfactory.h>
#include <editors/models/xsl/importitemmodelfactory.h>
#include <editors/models/css/csscontextparser.h>
#include <editors/models/js/jscontextparser.h>
#include <codecompletion/snipets/snipetcontextparser.h>
#include <codecompletion/snipets/snipetitemmodelfactory.h>

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

ContentView3::Parser * CorePlugin::createContentParser(const QString & type)
{
	if (type == "XSL")
	{
		return new Core::Stylesheet::Parser;
	}
	else if (type == "JS")
	{
		return new Core::JavaScript::Parser;
	}
	else if (type == "CSS")
	{
		return new Core::CascadingStyleSheet::Parser;
	}
	else
	{
		return 0;
	}
}

QList<CodeCompletion::ContextParser*> CorePlugin::createContextParser() const
{
	QList< CodeCompletion::ContextParser* > result;

	result << new CodeCompletion::SnipetContextParser;
	result << new Core::BaliseDefinition::XmlContextParser;
	result << new Core::CascadingStyleSheet::CssContextParser;
	result << new Core::JavaScript::JsContextParser;

	return result;
}

QList<CodeCompletion::ItemModelFactory*> CorePlugin::createItemModelFactory() const
{
	QList<CodeCompletion::ItemModelFactory*> result;

	result << new CodeCompletion::SnipetItemModelFactory;
	result << new Core::BaliseDefinition::ItemModelFactory;
	result << new Core::Stylesheet::ItemModelFactory;
	result << new Core::Stylesheet::ImportItemModelFactory;

	return result;
}

void CorePlugin::generateActionMenu()
{
	XinxAction::ActionManager::self()->insertNameOfMenu("edit", tr("&Edit"));
	XinxAction::ActionManager::self()->insertNameOfMenu("bookmark", tr("&Bookmark"));
	XinxAction::ActionManager::self()->insertNameOfMenu("execute", tr("&Execute"));

	XinxAction::ProjectAction * dataStreamAction = new DataStreamAction;

	XinxAction::Action * undoAction = new UndoAction;
	XinxAction::Action * redoAction = new RedoAction;
	XinxAction::Action * cutAction = new CutAction;
	XinxAction::Action * copyAction = new CopyAction;
	XinxAction::Action * pasteAction = new PasteAction;
	XinxAction::Action * selectAllAction = new SelectAllAction;
	XinxAction::Action * duplicateLinesAction = new DuplicateLinesAction;
	XinxAction::Action * moveLineUpAction = new MoveLineUpAction;
	XinxAction::Action * moveLineDownAction = new MoveLineDownAction;
	XinxAction::Action * upperSelectedTextAction = new UpperSelectedTextAction;
	XinxAction::Action * lowerSelectedTextAction = new LowerSelectedTextAction;
	XinxAction::Action * showSpecialCharsAction = new ShowSpecialCharsAction;
	XinxAction::Action * indentAction = new IndentAction;
	XinxAction::Action * unindentAction = new UnindentAction;
	XinxAction::Action * autoIndentAction = new AutoIndentAction;
	XinxAction::Action * highlightWordAction = new HighlightWordAction;
	XinxAction::Action * completeAction = new CompleteAction;
	XinxAction::Action * gotoLineAction = new GotoLineAction;
	XinxAction::Action * commentAction = new CommentAction;
	XinxAction::Action * uncommentAction = new UncommentAction;

	XinxAction::Action * bookmarkAction = new BookmarkAction;
	XinxAction::Action * previousBookmarkAction = new PreviousBookmarkAction;
	XinxAction::Action * nextBookmarkAction = new NextBookmarkAction;
	XinxAction::Action * clearAllBookmarkAction = new ClearAllBookmarkAction;

	XinxAction::Action * styleSheetAction = new StyleSheetAction;
	XinxAction::Action * validationAction = new ValidationAction;
	XinxAction::Action * runXQueryAction = new RunXQueryAction;


	XinxAction::ActionManager::self()->addProjectDirectoryPopupItem(dataStreamAction);

	XinxAction::ActionManager::self()->addMenuItem("execute", validationAction);
	XinxAction::ActionManager::self()->addMenuItem("execute", styleSheetAction);
	XinxAction::ActionManager::self()->addMenuItem("execute", runXQueryAction);

	XinxAction::ActionManager::self()->addMenuItem("bookmark", clearAllBookmarkAction);
	XinxAction::ActionManager::self()->addMenuSeparator("bookmark");
	XinxAction::ActionManager::self()->addMenuItem("bookmark", nextBookmarkAction);
	XinxAction::ActionManager::self()->addMenuItem("bookmark", previousBookmarkAction);
	XinxAction::ActionManager::self()->addMenuItem("bookmark", bookmarkAction);

	XinxAction::ActionManager::self()->addMenuItem("edit", uncommentAction);
	XinxAction::ActionManager::self()->addMenuItem("edit", commentAction);
	XinxAction::ActionManager::self()->addMenuSeparator("edit");
	XinxAction::ActionManager::self()->addMenuItem("edit", gotoLineAction);
	XinxAction::ActionManager::self()->addMenuItem("edit", completeAction);
	XinxAction::ActionManager::self()->addMenuItem("edit", highlightWordAction);
	XinxAction::ActionManager::self()->addMenuSeparator("edit");
	XinxAction::ActionManager::self()->addMenuItem("edit", autoIndentAction);
	XinxAction::ActionManager::self()->addMenuItem("edit", unindentAction);
	XinxAction::ActionManager::self()->addMenuItem("edit", indentAction);
	XinxAction::ActionManager::self()->addMenuSeparator("edit");
	XinxAction::ActionManager::self()->addMenuItem("edit", showSpecialCharsAction);
	XinxAction::ActionManager::self()->addMenuItem("edit", lowerSelectedTextAction);
	XinxAction::ActionManager::self()->addMenuItem("edit", upperSelectedTextAction);
	XinxAction::ActionManager::self()->addMenuSeparator("edit");
	XinxAction::ActionManager::self()->addMenuItem("edit", moveLineDownAction);
	XinxAction::ActionManager::self()->addMenuItem("edit", moveLineUpAction);
	XinxAction::ActionManager::self()->addMenuItem("edit", duplicateLinesAction);
	XinxAction::ActionManager::self()->addMenuItem("edit", selectAllAction);
	XinxAction::ActionManager::self()->addMenuSeparator("edit");
	XinxAction::ActionManager::self()->addMenuItem("edit", pasteAction);
	XinxAction::ActionManager::self()->addMenuItem("edit", copyAction);
	XinxAction::ActionManager::self()->addMenuItem("edit", cutAction);
	XinxAction::ActionManager::self()->addMenuSeparator("edit");
	XinxAction::ActionManager::self()->addMenuItem("edit", redoAction);
	XinxAction::ActionManager::self()->addMenuItem("edit", undoAction);

	XinxAction::ActionManager::self()->addToolBarItem("execute", styleSheetAction);

	XinxAction::ActionManager::self()->addToolBarItem("edit", showSpecialCharsAction);
	XinxAction::ActionManager::self()->addToolBarSeparator("edit");
	XinxAction::ActionManager::self()->addToolBarItem("edit", lowerSelectedTextAction);
	XinxAction::ActionManager::self()->addToolBarItem("edit", upperSelectedTextAction);
	XinxAction::ActionManager::self()->addToolBarSeparator("edit");
	XinxAction::ActionManager::self()->addToolBarItem("edit", moveLineDownAction);
	XinxAction::ActionManager::self()->addToolBarItem("edit", moveLineUpAction);
	XinxAction::ActionManager::self()->addToolBarSeparator("edit");
	XinxAction::ActionManager::self()->addToolBarItem("edit", pasteAction);
	XinxAction::ActionManager::self()->addToolBarItem("edit", copyAction);
	XinxAction::ActionManager::self()->addToolBarItem("edit", cutAction);
	XinxAction::ActionManager::self()->addToolBarSeparator("edit");
	XinxAction::ActionManager::self()->addToolBarItem("edit", redoAction);
	XinxAction::ActionManager::self()->addToolBarItem("edit", undoAction);

	XinxAction::ActionManager::self()->addPopupItem(styleSheetAction);
	XinxAction::ActionManager::self()->addPopupSeparator();
	XinxAction::ActionManager::self()->addPopupItem(uncommentAction);
	XinxAction::ActionManager::self()->addPopupItem(commentAction);
	XinxAction::ActionManager::self()->addPopupSeparator();
	XinxAction::ActionManager::self()->addPopupItem(gotoLineAction);
	XinxAction::ActionManager::self()->addPopupSeparator();
	XinxAction::ActionManager::self()->addPopupItem(pasteAction);
	XinxAction::ActionManager::self()->addPopupItem(copyAction);
	XinxAction::ActionManager::self()->addPopupItem(cutAction);
	XinxAction::ActionManager::self()->addPopupSeparator();
	XinxAction::ActionManager::self()->addPopupItem(redoAction);
	XinxAction::ActionManager::self()->addPopupItem(undoAction);
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

Q_EXPORT_PLUGIN2(coreplugin, CorePlugin)
