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
#include "generix.h"
#include <project/xinxprojectproject.h>
#include "pluginresolver/gce150fileresolver.h"
#include "projectproperty/wizard/newgenerixinformationpageimpl.h"
#include "projectproperty/wizard/newgenerixderivation1page.h"
#include "projectproperty/wizard/newgenerixderivation2page.h"
#include "projectproperty/wizard/newgenerixderivation3page.h"
#include "projectproperty/std/generixderivationoptionspage.h"
#include "projectproperty/std/generixderivationpage.h"
#include "projectproperty/std/generixprojectpageform.h"
#include <project/externalfileresolver.h>
#include <configuration/configurationmanager.h>
#include <savedialog/derivationdialogimpl.h>
#include "parser/generixxsltparser.h"
#include <plugins/xinxpluginsloader.h>
#include "config/customgeneriximpl.h"
#include "config/selfgcesettings.h"
#include "editors/widgeteditor/maq/maqfileeditor.h"
#include "plugindefinition/filetypeplugin.h"
#include <project/xinxprojectproject.h>
#include <docks/dictionary/dictionary_parser.h>
#include <docks/dictionary/itemmodelfactory.h>
#include <libxml/xmlreader.h>

// Qt header
#include <QString>
#include <QVariant>
#include <QTranslator>
#include <QApplication>
#include <QDockWidget>
#include <QDir>
#include <QMetaType>

/* GenerixProjectInitialisationStep */

class GenerixProjectInitialisationStep1 : public IProjectInitialisationStep
{
public:
	XinxProject::ProjectPtr _project;

	GenerixProjectInitialisationStep1(XinxProject::ProjectPtr project) : _project(project)
	{

	}

	virtual ~GenerixProjectInitialisationStep1()
	{
	}

	virtual QString name()
	{
		return GenerixPlugin::tr("Load configuration files ...");
	}

	virtual bool process()
	{
		_project->addObject("generix", new ConfigurationManager(_project));

		return true;
	}
};

/* GenerixPlugin */

GenerixPlugin::GenerixPlugin() : m_dock(0)
{
	Q_INIT_RESOURCE(generix);

	xmlInitParser();

	m_fileTypes << new MaquetteFileType;
}

GenerixPlugin::~GenerixPlugin()
{
}

bool GenerixPlugin::initializePlugin(const QString & lang)
{
	QTranslator * tranlator = new QTranslator(this);
	tranlator->load(QString(":/generix/translations/dictionary_%1").arg(lang));
	qApp->installTranslator(tranlator);

	m_resolver = new Gce150FileResolver();

	return true;
}

QVariant GenerixPlugin::getPluginAttribute(const enum IXinxPlugin::PluginAttribute & attr)
{
	switch (attr)
	{
	case PLG_NAME:
		return tr("Plugin for Generix");
	case PLG_DESCRIPTION:
		return tr("This plugin add some fonctionnality usefull for Generix Group's XINX users");
	case PLG_AUTHOR:
		return "Ulrich Van Den Hekke";
	case PLG_ICON:
		return QPixmap(":/generix/images/logo_gce32.png");
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

ContentView3::Parser * GenerixPlugin::createContentParser(const QString & type)
{
	if (type == "GNX_DICO")
	{
		return new Generix::Dictionary::Parser();
	}
	else
	{
		return 0;
	}
}

QList<CodeCompletion::ContextParser*> GenerixPlugin::createContextParser() const
{
	QList<CodeCompletion::ContextParser*> ctxtparsers;
	return ctxtparsers;
}

QList<CodeCompletion::ItemModelFactory*> GenerixPlugin::createItemModelFactory() const
{
	QList<CodeCompletion::ItemModelFactory*> factories;
	factories << new Generix::Dictionary::ItemModelFactory;
	return factories;
}

XsltParser * GenerixPlugin::createXsltParser(AbstractEditor * editor)
{
	QSharedPointer<GenerixProject> project = editor->project().staticCast<GenerixProject>();
	if (project && project->isGenerixActivated())
	{
		return new GenerixXsltParser();
	}
	else
		return 0;
}

QList<IProjectInitialisationStep*> GenerixPlugin::loadProjectStep(XinxProject::ProjectPtr project)
{
	return QList<IProjectInitialisationStep*>() << new GenerixProjectInitialisationStep1(project);
}

QList<IProjectInitialisationStep*> GenerixPlugin::closeProjectStep(XinxProject::ProjectPtr project)
{
	Q_UNUSED(project);
	return QList<IProjectInitialisationStep*>();
}

QList<IFileTypePlugin*> GenerixPlugin::fileTypes()
{
	return m_fileTypes;
}

QIODevice * GenerixPlugin::loadFile(AbstractEditor * editor, const QString & filename)
{
	 Q_UNUSED(editor);
	 Q_UNUSED(filename);
	 return 0;
}

QIODevice * GenerixPlugin::saveFile(AbstractEditor * editor, const QString & filename, const QString & oldfilename)
{
	QSharedPointer<GenerixProject> gnxProject = editor->project().staticCast<GenerixProject>();
	if (!(gnxProject && gnxProject->isGenerixActivated() && gnxProject->copySourceFileInDerivationPath()) || (filename == oldfilename))
	{
		return 0;
	}

	/* When saving the file, the plugin the standard file in the project directory */
	const QString stdfilename = QDir(QFileInfo(filename).absolutePath()).absoluteFilePath(QFileInfo(oldfilename).fileName());

	if (! QFile::exists(stdfilename))
	{
		QFile::copy(oldfilename, stdfilename);
		gnxProject->rcsProxy()->addFileOperation(VersionControl::RCSProxy::RCS_ADD, QStringList() << stdfilename);
	}
	return 0;
}

QString GenerixPlugin::getFilename(AbstractEditor* editor, const QString& filename, const QString& defaultFilename, const QString& filter, bool saveAs, bool& accept, QWidget* widget)
{
	QSharedPointer<GenerixProject> gnxProject = editor->project().staticCast<GenerixProject>();
	if (!(gnxProject && gnxProject->isGenerixActivated()))
	{
		accept = false;
		return QString();
	}

	accept = true;
	if (saveAs || DerivationDialogImpl::isDerivableFile(gnxProject, filename))
	{
		DerivationDialogImpl dlg(gnxProject, widget);
		if (!filename.isEmpty())
		{
			dlg.load(filename, filter);
		}
		else
		{
			QString newFilename = defaultFilename;
			if (!QDir::match("*.xsl", defaultFilename))
			{
				newFilename = QFileInfo(defaultFilename).suffix() + "/" + newFilename;
			}
			dlg.load(QDir(gnxProject->projectPath()).absoluteFilePath(newFilename), filter);
		}
		if (dlg.exec() == QDialog::Accepted)
		{
			const QString filename = dlg.getNewPath();
			const QString pathname = QFileInfo(filename).absolutePath();

			if (gnxProject->createMissingDirectory())
				QDir::current().mkpath(pathname);

			return filename;
		}
		else
		{
			return QString();
		}
	}
	return filename;
}

QList<QWidget*> GenerixPlugin::createDocksWidget(QWidget * parent)
{
	QList<QWidget*> docks;
	if (! m_dock)
	{
		m_dock = new DictionaryDockWidgetImpl(parent);
		m_dock->setObjectName(QString::fromUtf8("m_dictionaryDock"));

		m_gnxDock = new GenerixProjectDockImpl(parent);
		m_gnxDock->setObjectName(QString::fromUtf8("m_gnxDock"));
	}
	docks << m_dock << m_gnxDock;
	return docks;
}

QList<IFileResolverPlugin*> GenerixPlugin::fileResolvers()
{
	return QList<IFileResolverPlugin*>() << m_resolver;
}

QList<IXinxPluginConfigurationPage*> GenerixPlugin::createSettingsDialog(QWidget * parent)
{
	QList<IXinxPluginConfigurationPage*> pages;
	pages << new CustomGenerixImpl(parent);
	return pages;
}

QList<IXinxPluginProjectConfigurationPage*> GenerixPlugin::createProjectSettingsPage(QWidget * parent)
{
	QList<IXinxPluginProjectConfigurationPage*> pages;

	pages << new GenerixProjectPageFormImpl(parent);
	pages << new GenerixDerivationPathPageImpl(parent);
	pages << new GenerixDerivationOptionsPageImpl(parent);

	return pages;
}

QList<IXinxPluginNewProjectConfigurationPage*> GenerixPlugin::createNewProjectSettingsPages()
{
	QList<IXinxPluginNewProjectConfigurationPage*> list;
	list << new NewGenerixInformationPageImpl;
	list << new NewGenerixDerivation1Page;
	list << new NewGenerixDerivation2Page;
	list << new NewGenerixDerivation3Page;
	return list;
}


Q_EXPORT_PLUGIN2(GenerixPlugin, GenerixPlugin)
