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
#include "welcomdlgimpl.h"
#include "welcomdlgimpl_p.h"
#include <core/xinxconfig.h>
#include <project/xinxprojectproject.h>

// Qt header
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QDesktopServices>
#include <QDir>

/* WelcomDialogImpl */
#include <project/xinxprojectprojectexception.h>
#include <QMessageBox>

WelcomDialogImpl::WelcomDialogImpl(QWidget * parent, Qt::WindowFlags f) : QDialog(parent, f)
{
	setupUi(this);
	//createWebsiteList();
	updateTipOfTheDay();

	connect(m_sessionsWidget, SIGNAL(activated(QString)), SLOT(slotSessionClicked(QString)));
	connect(m_projectWidget, SIGNAL(activated(QString)), SLOT(slotProjectClicked(QString)));
//	connect(m_sitesWidget, SIGNAL(activated(QString)), SLOT(slotUrlClicked(QString)));
	connect(m_createNewProjectBtn, SIGNAL(clicked()), SLOT(accept()));
	connect(m_createNewProjectBtn, SIGNAL(clicked()), SIGNAL(createNewProject()));
	connect(m_openProjectBtn, SIGNAL(clicked()), SLOT(accept()));
	connect(m_openProjectBtn, SIGNAL(clicked()), SIGNAL(openExistingProject()));
}

WelcomDialogImpl::~WelcomDialogImpl()
{
}

void WelcomDialogImpl::updateTipOfTheDay()
{
	QString filename = QString(":/rc/tipoftheday_%1.txt").arg(XINXConfig::self()->config().language);
	QFile tipFile(filename);
	if (tipFile.open(QFile::ReadOnly))
	{
		QTextStream textStream(&tipFile);
		tipFrame->show();
		QStringList tips;
		do
		{
			tips.append(textStream.readLine());
		}
		while (! textStream.atEnd());

		if (tips.count() > 0)
		{
			int row = (qrand() % tips.count());
			textBrowser->setHtml(tips.at(row));
		}

		tipFile.close();
	}
	else
	{
		tipFrame->hide();
	}
}
/*
void WelcomDialogImpl::createWebsiteList()
{
	m_sitesWidget->addItem(tr("Documentation"), QLatin1String("http://xinx.shadoware.org/wiki"));
	m_sitesWidget->addItem(tr("API"), QLatin1String("http://xinx.shadoware.org/doxygen/devel/html/hierarchy.html"));
	m_sitesWidget->addItem(tr("Report a bug"), QLatin1String("http://xinx.shadoware.org/newticket"));
	m_sitesWidget->addItem(tr("Downloads"), QLatin1String("http://xinx.shadoware.org/downloads"));
}
*/
void WelcomDialogImpl::addProjectFile(const QString & filename)
{
	if (QFile::exists(filename))
	{
		try
		{
			XinxProject::ProjectPtr ptr =  XinxProject::Project::create(filename);
			m_projectWidget->addItem(ptr->projectName(), filename);
		}
		catch (XinxProject::ProjectException e)
		{
			// In this case, we don't show this project in the welcom dialog.
		}
	}
}

void WelcomDialogImpl::addSession(const QString & sessionName)
{
	m_sessionsWidget->addItem(sessionName, sessionName);
}

void WelcomDialogImpl::slotProjectClicked(const QString &data)
{
	accept();
	emit requestProject(data);
}

void WelcomDialogImpl::slotSessionClicked(const QString &data)
{
	accept();
	emit requestSession(data);
}

void WelcomDialogImpl::slotUrlClicked(const QString &data)
{
	QDesktopServices::openUrl(QUrl(data));
}

/* WelcomTreeWidget */


WelcomTreeWidget::WelcomTreeWidget(QWidget *parent) : QListWidget(parent), m_bullet(QLatin1String(":/images/bullet_arrow.png"))
{
	connect(this, SIGNAL(itemClicked(QListWidgetItem *)), SLOT(slotItemClicked(QListWidgetItem *)));
}

QSize WelcomTreeWidget::minimumSizeHint() const
{
	return QSize();
}

QSize WelcomTreeWidget::sizeHint() const
{
	return QSize(QListWidget::sizeHint().width(), 30 * count());
}

QListWidgetItem *WelcomTreeWidget::addItem(const QString &label, const QString &data)
{
	QListWidgetItem *item = new QListWidgetItem(this);
	item->setIcon(m_bullet);
	item->setSizeHint(QSize(24, 30));

	QLabel * lbl = new QLabel(label);
	lbl->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	QBoxLayout *lay = new QVBoxLayout;
	lay->setContentsMargins(3, 2, 0, 0);
	lay->addWidget(lbl);

	QWidget *wdg = new QWidget;
	wdg->setLayout(lay);
	setItemWidget(item, wdg);

	if (!data.isEmpty())
	{
		lbl->setTextInteractionFlags(Qt::NoTextInteraction);
		lbl->setCursor(QCursor(Qt::PointingHandCursor));

		item->setData(Qt::UserRole, data);
	}

	return item;
}

QListWidgetItem *WelcomTreeWidget::addItem(const QString &label)
{
	return addItem(label, QString());
}

void WelcomTreeWidget::slotItemClicked(QListWidgetItem * item)
{
	if (item->data(Qt::UserRole).isValid())
	{
		emit activated(item->data(Qt::UserRole).toString());
	}
}