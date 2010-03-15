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
#include "customdialogimpl.h"

#include "customgeneralimpl.h"
#include "customprojectimpl.h"
#include "customeditorimpl.h"
#include "customfontimpl.h"
#include "customsyntaximpl.h"
#include "customsnipetimpl.h"
#include "customtoolsimpl.h"
#include "custommodulesimpl.h"

#include <plugins/xinxpluginsloader.h>

// Qt header
#include <QTimer>

/* CustomDialogImpl */

CustomDialogImpl::CustomDialogImpl(QWidget * parent, Qt::WFlags f)  : QDialog(parent, f)
{
	setupUi(this);

	delete pageToDelete;

	m_pages << new CustomGeneralImpl(this);
	m_pages << new CustomProjectImpl(this);
	m_pages << new CustomEditorImpl(this);
	m_pages << new CustomFontImpl(this);
	m_pages << new CustomSyntaxImpl(this);
	m_pages << new CustomSnipetImpl(this);
	m_pages << new CustomToolsImpl(this);

	foreach(XinxPluginElement * plugin, XinxPluginsLoader::self()->plugins())
	{
		XinxPluginElement * e = new XinxPluginElement(plugin->plugin(), plugin->isStatic());

		IXinxPluginConfiguration * p = dynamic_cast<IXinxPluginConfiguration*>(e->plugin());
		if (! p) continue;

		QList<IXinxPluginConfigurationPage*> pages = p->createSettingsDialog(this);
		m_pages << pages;

		foreach(IXinxPluginConfigurationPage * page, pages)
		{
			m_pluginsPages.insertMulti(e, page);
		}
	}

	m_pages << new CustomModulesImpl(this);

	foreach(IXinxPluginConfigurationPage * page, m_pages)
	{
		QListWidgetItem * item = new QListWidgetItem(QIcon(page->image()), page->name());
		m_listWidget->addItem(item);

		m_stackedWidget->addWidget(page->settingsDialog());
	}

	QTimer * updateOkTimer = new QTimer(this);
	updateOkTimer->setInterval(250);
	updateOkTimer->setSingleShot(false);
	connect(updateOkTimer, SIGNAL(timeout()), SLOT(updateOkButton()));

	updateOkTimer->start();
}

CustomDialogImpl::~CustomDialogImpl()
{
	qDeleteAll(m_pages);
}

void CustomDialogImpl::loadConfig()
{
	foreach(IXinxPluginConfigurationPage * page, m_pages)
	page->loadSettingsDialog();
}

void CustomDialogImpl::saveConfig()
{
	foreach(IXinxPluginConfigurationPage * page, m_pages)
	page->saveSettingsDialog();
}

void CustomDialogImpl::updateOkButton()
{
	foreach(IXinxPluginConfigurationPage * page, m_pages)
	{
		if (! page->isSettingsValid())
		{
			m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
			return;
		}
	}

	m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}
