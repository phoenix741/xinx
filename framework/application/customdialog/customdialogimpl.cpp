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
#include "customdialogimpl.h"
#include "customdialogimpl_p.h"

#include "customgeneralimpl.h"
#include "customprojectimpl.h"
#include "customeditorimpl.h"
#include "customfontimpl.h"
#include "customsyntaximpl.h"
#include "customsnipetimpl.h"
#include "customtoolsimpl.h"
#include "custommodulesimpl.h"

#include <plugins/xinxpluginsloader.h>
#include <plugins/xinxpluginelement.h>

// Qt header
#include <QTimer>
#include <QDebug>

/* CustomDialogImplPrivate */

CustomDialogImplPrivate::CustomDialogImplPrivate(QObject* parent) : _ui(new Ui::CustomDialog)
{
}

CustomDialogImplPrivate::~CustomDialogImplPrivate()
{
}

/*!
* \brief Update the state of the Ok button.
*
* This method is called automatically every 250ms. This method check IXinxPluginConfigurationPage::isSettingsValid()
* on each page and show a red message if settings isn't valid.
*/
void CustomDialogImplPrivate::updateOkButton()
{
	_ui->m_errorLabel->setVisible(false);
	foreach(IXinxPluginConfigurationPage * page, _pages)
	{
		QString message;
		if (! page->isSettingsValid(message))
		{
			_ui->m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
			if (!message.isEmpty())
			{
				_ui->m_errorLabel->setVisible(true);
				_ui->m_errorLabel->setText(QString("<img src=\":/images/error16.png\"/> ") + tr("%1 on page \"%2\"").arg(message).arg(page->name()));
			}
			return;
		}
	}

	_ui->m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}

/* CustomDialogImpl */

/*!
 * \class CustomDialogImpl
 * \brief XINX Custom dialog box used to configure all options of XINX.
 * \author Ulrich Van Den Hekke
 * \intern
 *
 * Implementation of dialog used to configure custom options in XINX. The options is globals for the application.
 * This dialog save and restore modification in a XINXConfig class.
 *
 * This dialog use the interface IXinxPluginConfigurationPage to permit to plugin to extend the dialog with pages.
 *
 *
 * This dialog mustn't be used directly by plugins. Only framework can use it.
 */

/*!
 * \brief Custom dialog constructor.
 *
 * The dialog is create with a fixed size
 * \param parent Parent of the dialog
 * \param f Flags to use on Windows. By default, the dialog have a fixed size.
 */
CustomDialogImpl::CustomDialogImpl(QWidget * parent, Qt::WFlags f)  : QDialog(parent, f), d(new CustomDialogImplPrivate)
{
	d->_ui->setupUi(this);
	d->_ui->m_errorLabel->setVisible(false);

	delete pageToDelete;

	d->_pages << new CustomGeneralImpl(this);
	d->_pages << new CustomProjectImpl(this);
	d->_pages << new CustomEditorImpl(this);
	d->_pages << new CustomFontImpl(this);
	d->_pages << new CustomSyntaxImpl(this);
	d->_pages << new CustomSnipetImpl(this);
	d->_pages << new CustomToolsImpl(this);

	foreach(XinxPluginElement * plugin, XinxPluginsLoader::self()->plugins())
	{
		XinxPluginElement * e = new XinxPluginElement(plugin->plugin(), plugin->isStatic());

		IXinxPluginConfiguration * p = dynamic_cast<IXinxPluginConfiguration*>(e->plugin());
		if (! p) continue;

		QList<IXinxPluginConfigurationPage*> pages = p->createSettingsDialog(this);
		d->_pages << pages;

		foreach(IXinxPluginConfigurationPage * page, pages)
		{
			d->_pluginsPages.insertMulti(e, page);
		}
	}

	d->_pages << new CustomModulesImpl(this);

	foreach(IXinxPluginConfigurationPage * page, d->_pages)
	{
		QListWidgetItem * item = new QListWidgetItem(QIcon(page->image()), page->name());
		m_listWidget->addItem(item);

		m_stackedWidget->addWidget(page->settingsDialog());
	}

	QTimer * updateOkTimer = new QTimer(this);
	updateOkTimer->setInterval(250);
	updateOkTimer->setSingleShot(false);
	connect(updateOkTimer, SIGNAL(timeout()), d.data(), SLOT(updateOkButton()));

	updateOkTimer->start();
}

//! Destroy the custom dialog
CustomDialogImpl::~CustomDialogImpl()
{
	qDeleteAll(d->_pages);
}

//! Call method IXinxPluginConfigurationPage::loadSettingsDialog() on each page
void CustomDialogImpl::loadConfig()
{
	foreach(IXinxPluginConfigurationPage * page, d->_pages)
	{
		page->loadSettingsDialog();
	}
}

//! Call method IXinxPluginConfigurationPage::saveSettingsDialog() on each page
void CustomDialogImpl::saveConfig()
{
	foreach(IXinxPluginConfigurationPage * page, d->_pages)
	{
		page->saveSettingsDialog();
	}
}

/*!
 * \brief Called when the user accept the content of the dialog.
 *
 * This methode save the settings for each page and store in Configuration.
 * The style of the application is updated if necessary.
 */
void CustomDialogImpl::accept()
{
	saveConfig();

	XINXConfig::self()->updateFormatsSchemeFromConfig();
	XINXConfig::self()->save();

	if (! XINXConfig::self()->config().style.isEmpty())
	{
		QApplication::setStyle(XINXConfig::self()->config().style);
	}

	QDialog::accept();
}
