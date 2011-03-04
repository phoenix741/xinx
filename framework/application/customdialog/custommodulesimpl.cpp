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
#include "custommodulesimpl.h"
#include <core/xinxconfig.h>
#include <scripts/scriptmanager.h>
#include "scriptelement.h"
#include <plugins/xinxpluginsloader.h>

// Qt header
#include <QListWidgetItem>
#include <QDialog>
#include <QLabel>
#include <QGroupBox>
#include <QDialogButtonBox>

/* CustomModulesImpl */

CustomModulesImpl::CustomModulesImpl(QWidget * parent) : QWidget(parent)
{
	setupUi(this);

	// Pluins
	connect(m_pluginListView, SIGNAL(aboutPlugin(PluginElement*)), this, SLOT(aboutPlugin(PluginElement*)));
	connect(m_scriptListView, SIGNAL(aboutPlugin(PluginElement*)), this, SLOT(aboutScript(PluginElement*)));
}

CustomModulesImpl::~CustomModulesImpl()
{
	QList<PluginElement*> list = m_pluginListView->plugins();
	m_pluginListView->clear();
	qDeleteAll(list);

	list = m_scriptListView->plugins();
	m_scriptListView->clear();
	qDeleteAll(list);
}

QPixmap CustomModulesImpl::image()
{
	return QPixmap(":/images/preferences-plugin.png");
}

QString CustomModulesImpl::name()
{
	return windowTitle();
}

QWidget * CustomModulesImpl::settingsDialog()
{
	return this;
}

bool CustomModulesImpl::loadSettingsDialog()
{
	// Load Script
	m_scriptListView->clear();
	foreach(const ScriptValue & s, ScriptManager::self()->objects())
	{
		m_scriptListView->addPlugin(new ScriptElement(s));
	}

	// Plugins
	m_pluginListView->clear();
	foreach(XinxPluginElement * plugin, XinxPluginsLoader::self()->plugins())
	{
		XinxPluginElement * e = new XinxPluginElement(plugin->plugin(), plugin->isStatic());
		e->setActivated(plugin->isActivated());
		m_pluginListView->addPlugin(e);
	}

	return true;
}

bool CustomModulesImpl::saveSettingsDialog()
{
	// Plugins
	foreach(PluginElement * plugin, m_pluginListView->plugins())
	{
		QString name = dynamic_cast<XinxPluginElement*>(plugin)->plugin()->metaObject()->className();
		bool isActivated = plugin->isActivated();

		XINXConfig::self()->config().plugins[ name ] = isActivated;
		if (XinxPluginsLoader::self()->plugin(name))
		{
			XinxPluginsLoader::self()->plugin(name)->setActivated(isActivated);
		}
	}

	return true;
}

bool CustomModulesImpl::cancelSettingsDialog()
{
	return true;
}

bool CustomModulesImpl::isSettingsValid(QString& message)
{
	return true;
}

bool CustomModulesImpl::isVisible()
{
	return true;
}

void CustomModulesImpl::aboutPlugin(PluginElement * plugin)
{
	Q_ASSERT(plugin);
	Q_ASSERT(dynamic_cast<XinxPluginElement*>(plugin));

	XinxPluginElement * xinxPlugin = dynamic_cast<XinxPluginElement*>(plugin);

	QDialog informationDialog;
	informationDialog.setWindowFlags(Qt::MSWindowsFixedSizeDialogHint | Qt::Dialog);

	QLabel * informations = new QLabel(&informationDialog);
	informations->setWordWrap(true);
	informations->setOpenExternalLinks(true);
	informations->setText(
		tr("<table>"
		   "<tr>"
		   "<td colspan=\"3\"><b>%1</b></td>"
		   "</tr>"
		   "<tr>"
		   "<td colspan=\"3\">%2</td>"
		   "</tr>"
		   "<tr><td colspan=\"3\"><hr/></td></tr>"
		   "<tr>"
		   "<td><b>Author</b></td>"
		   "<td width=\"0\">:</td>"
		   "<td>%3</td>"
		   "</tr>"
		   "<tr>"
		   "<td><b>E-Mail</b></td>"
		   "<td width=\"0\">:</td>"
		   "<td><a href=\"mailto:%4\">%4</a></td>"
		   "</tr>"
		   "<tr>"
		   "<td><b>Web site</b></td>"
		   "<td width=\"0\">:</td>"
		   "<td><a href=\"%5\">%5</a></td>"
		   "</tr>"
		   "<tr>"
		   "<td><b>Version</b></td>"
		   "<td width=\"0\">:</td>"
		   "<td>%6</td>"
		   "</tr>"
		   "<tr>"
		   "<td><b>Licence</b></td>"
		   "<td width=\"0\">:</td>"
		   "<td>%7</td>"
		   "</tr>"
		   "</table>")
		.arg(qobject_cast<IXinxPlugin*>(xinxPlugin->plugin())->getPluginAttribute(IXinxPlugin::PLG_NAME).toString())
		.arg(qobject_cast<IXinxPlugin*>(xinxPlugin->plugin())->getPluginAttribute(IXinxPlugin::PLG_DESCRIPTION).toString())
		.arg(qobject_cast<IXinxPlugin*>(xinxPlugin->plugin())->getPluginAttribute(IXinxPlugin::PLG_AUTHOR).toString())
		.arg(qobject_cast<IXinxPlugin*>(xinxPlugin->plugin())->getPluginAttribute(IXinxPlugin::PLG_EMAIL).toString())
		.arg(qobject_cast<IXinxPlugin*>(xinxPlugin->plugin())->getPluginAttribute(IXinxPlugin::PLG_WEBSITE).toString())
		.arg(qobject_cast<IXinxPlugin*>(xinxPlugin->plugin())->getPluginAttribute(IXinxPlugin::PLG_VERSION).toString())
		.arg(qobject_cast<IXinxPlugin*>(xinxPlugin->plugin())->getPluginAttribute(IXinxPlugin::PLG_LICENCE).toString())
	);

	QVBoxLayout * labelLayout = new QVBoxLayout;
	labelLayout->addWidget(informations);

	QGroupBox * grp = new QGroupBox(tr("&Informations"), &informationDialog);
	grp->setLayout(labelLayout);

	QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
	buttonBox->setCenterButtons(true);

	QVBoxLayout * vbox = new QVBoxLayout;
	vbox->addWidget(grp);
	vbox->addWidget(buttonBox);

	informationDialog.setLayout(vbox);

	connect(buttonBox, SIGNAL(accepted()), &informationDialog, SLOT(accept()));

	informationDialog.exec();
}


void CustomModulesImpl::aboutScript(PluginElement * plugin)
{
	Q_ASSERT(plugin);
	Q_ASSERT(dynamic_cast<ScriptElement*>(plugin));

	ScriptElement * script = dynamic_cast<ScriptElement*>(plugin);

	QDialog informationDialog;
	informationDialog.setWindowFlags(Qt::MSWindowsFixedSizeDialogHint | Qt::Dialog);

	QLabel * informations = new QLabel(&informationDialog);
	informations->setWordWrap(true);
	informations->setOpenExternalLinks(true);
	informations->setText(
		tr("<table>"
		   "<tr>"
		   "<td colspan=\"3\"><b>%1</b></td>"
		   "</tr>"
		   "<tr><td colspan=\"3\"><hr/></td></tr>"
		   "<tr>"
		   "<td><b>Author</b></td>"
		   "<td width=\"0\">:</td>"
		   "<td>%2</td>"
		   "</tr>"
		   "<tr>"
		   "<td><b>Version</b></td>"
		   "<td width=\"0\">:</td>"
		   "<td>%3</td>"
		   "</tr>"
		   "<tr>"
		   "<td><b>Licence</b></td>"
		   "<td width=\"0\">:</td>"
		   "<td>%4</td>"
		   "</tr>"
		   "</table>")
		.arg(script->name())
		.arg(script->author())
		.arg(script->version())
		.arg(script->licence())
	);

	QVBoxLayout * labelLayout = new QVBoxLayout;
	labelLayout->addWidget(informations);

	QGroupBox * grp = new QGroupBox(tr("&Informations"), &informationDialog);
	grp->setLayout(labelLayout);

	QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
	buttonBox->setCenterButtons(true);

	QVBoxLayout * vbox = new QVBoxLayout;
	vbox->addWidget(grp);
	vbox->addWidget(buttonBox);

	informationDialog.setLayout(vbox);

	connect(buttonBox, SIGNAL(accepted()), &informationDialog, SLOT(accept()));

	informationDialog.exec();
}


