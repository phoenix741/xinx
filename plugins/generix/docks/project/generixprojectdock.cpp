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
#include "generixprojectdock.h"
#include "projectproperty/generixproject.h"
#include <editors/editormanager.h>
#include <editors/abstracteditor.h>
#include "configuration/configurationmanager.h"

// Qt header
#include <QFileInfo>
#include <QDir>
#include <QPainter>
#include <QStringBuilder>

/* Constante */

#define SEPARATOR_SIZE 4

/* BusinessViewListDelegate */

BusinessViewListDelegate::BusinessViewListDelegate(QObject * parent) : QItemDelegate(parent)
{
}

BusinessViewListDelegate::~BusinessViewListDelegate()
{
}

void BusinessViewListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	painter->save();

	QStyleOptionViewItem myOption = option;
	myOption.state |= QStyle::State_Enabled;

	QFontMetrics metrics = painter->fontMetrics();

	QStyleOptionViewItem titleOpt(myOption);
	QFont titleFont(painter->font());
	titleFont.setBold(true);
	titleOpt.font = titleFont;
	titleOpt.fontMetrics = QFontMetrics(titleFont);

	QStyleOptionViewItem descOpt(myOption);
	QFont descFont(painter->font());
	descFont.setPointSize(descFont.pointSize() - 1);
	descOpt.font = descFont;
	descOpt.fontMetrics = QFontMetrics(descFont);

	if (myOption.state & QStyle::State_Selected)
	{
		painter->fillRect(myOption.rect, myOption.palette.color(QPalette::Highlight));
	}
	else
	{
		if (index.row() % 2 != 0)
			painter->fillRect(myOption.rect, myOption.palette.color(QPalette::AlternateBase));
		else
			painter->fillRect(myOption.rect, myOption.palette.color(QPalette::Base));
	}

	// Draw text
	int maxTextLength = myOption.rect.width();

        QString module = index.model()->data(index, Qt::UserRole + 4).toString();
        QString filename = index.model()->data(index, Qt::UserRole + 1).toString();
        int configurationIndex = index.model()->data(index, Qt::UserRole + 0).toInt();
	QString projectPath = index.model()->data(index, Qt::UserRole + 3).toString();

	QString display = QString("%1 (%2)").arg(index.model()->data(index).toString()).arg(index.model()->data(index, Qt::UserRole + 2).toString());
        QString description;
        if (! module.isEmpty())
        {
            description = module % " - ";
        }
        if (configurationIndex >= 0)
        {
            description = description % QString::number(configurationIndex) % " - ";
        }
        description = description % QDir(projectPath).relativeFilePath(filename);

	if (myOption.state & QStyle::State_Selected)
		painter->setPen(myOption.palette.color(QPalette::HighlightedText));

	painter->setFont(titleFont);
	display = titleOpt.fontMetrics.elidedText(display, Qt::ElideRight, maxTextLength);

	painter->drawText(
		SEPARATOR_SIZE,
		SEPARATOR_SIZE + myOption.rect.top(),
		titleOpt.fontMetrics.width(display),
		titleOpt.fontMetrics.height(),
		Qt::AlignLeft,
		display);

	painter->setFont(descFont);
	description = descOpt.fontMetrics.elidedText(description, Qt::ElideRight, maxTextLength);
	painter->drawText(
		SEPARATOR_SIZE,
		myOption.rect.height() - SEPARATOR_SIZE - descOpt.fontMetrics.height() + myOption.rect.top(),
		descOpt.fontMetrics.width(description),
		descOpt.fontMetrics.height(),
		Qt::AlignLeft,
		description);

	painter->restore();
}

QSize BusinessViewListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	Q_UNUSED(index);

	QFont title(option.font);
	title.setBold(true);

	QFontMetrics titleMetrics(title);
	QFontMetrics currentMetrics(option.font);

	int height = titleMetrics.height() + currentMetrics.height() + SEPARATOR_SIZE;
	height += SEPARATOR_SIZE * 2;

	return QSize(200, height);
}

/* GenerixProjectDockImpl */

GenerixProjectDockImpl::GenerixProjectDockImpl(QWidget * parent) : XinxDockWidget(parent), m_editorIndex(-1)
{
	setupUi(this);
	setWindowTitle(tr("Generix Business View"));
	setWindowIcon(QIcon(":/generix/images/logo_gce32.png"));

	m_businessViewList->setItemDelegate(m_delegate = new BusinessViewListDelegate(m_businessViewList));

	setProject(XinxProject::ProjectPtr());

	connect(EditorManager::self(), SIGNAL(currentChanged(int)), this, SLOT(editorChanged(int)));
}

GenerixProjectDockImpl::~GenerixProjectDockImpl()
{
}

void GenerixProjectDockImpl::updateList()
{
	QSharedPointer<GenerixProject> gnxProject = m_gnxProject.toStrongRef();

	m_businessViewList->setEnabled(false);
	if (gnxProject && ConfigurationManager::manager(gnxProject) && (m_editorIndex >= 0) && (m_editorIndex < EditorManager::self()->editorsCount()))
	{
		const QString editorFilename = EditorManager::self()->editor(m_editorIndex)->lastFileName();

		m_filenameLabel->setText(tr("&Business view for file : \n%1").arg(QFileInfo(editorFilename).fileName()));

		const QString filename = QDir(gnxProject->projectPath()).relativeFilePath(editorFilename);

		QSharedPointer<GceConfiguration> interface = ConfigurationManager::manager(gnxProject)->getInterface();
		m_businessViewList->clear();
		if (interface)
		{
			foreach(const BusinessViewInformation information, interface->businessView(filename))
			{
				QListWidgetItem * item = new QListWidgetItem(information.businesViewName(), m_businessViewList);
				item->setData(Qt::UserRole + 0, information.configurationNumber());
				item->setData(Qt::UserRole + 1, information.configurationFileName());
				item->setData(Qt::UserRole + 2, information.targetName());
				item->setData(Qt::UserRole + 3, gnxProject->projectPath());
                                item->setData(Qt::UserRole + 4, information.moduleName());
				m_businessViewList->addItem(item);
				m_businessViewList->setEnabled(true);
			}
		}
	}
	else
	{
		m_filenameLabel->setText(QString());
		m_businessViewList->clear();
	}
}

void GenerixProjectDockImpl::setProject(XinxProject::ProjectPtr project)
{
	QSharedPointer<GenerixProject> gnxProject = project.staticCast<GenerixProject>();
	if (m_gnxProject.data() != gnxProject.data())
	{
		if (m_gnxProject && ConfigurationManager::manager(m_gnxProject))
		{
			ConfigurationManager::manager(m_gnxProject)->disconnect(this, SLOT(updateList()));
		}

		QSharedPointer<GenerixProject> newGnxProject;
		if (ConfigurationManager::manager(gnxProject))
		{
			newGnxProject = gnxProject;
		}
		m_gnxProject = newGnxProject.toWeakRef();

		m_prefixCombo->clear();
		if (newGnxProject)
		{
			if (newGnxProject->prefixes().size())
			{
				m_prefixCombo->addItems(newGnxProject->prefixes());
				m_prefixCombo->setCurrentIndex(m_prefixCombo->findText(newGnxProject->defaultPrefix()));
			}

			connect(ConfigurationManager::manager(newGnxProject), SIGNAL(configurationChanged()), this, SLOT(updateList()));
		}
		m_prefixCombo->setVisible(newGnxProject && (newGnxProject->prefixes().size() > 0));
		m_prefixLabel->setVisible(newGnxProject && (newGnxProject->prefixes().size() > 0));
	}
}

void GenerixProjectDockImpl::editorChanged(int index)
{
	if (m_editorIndex != index)
	{
		m_editorIndex = index;
		if ((index >= 0) && (index < EditorManager::self()->editorsCount()))
		{
			setProject(EditorManager::self()->editor(index)->project());
		}

		updateList();
	}
}


void GenerixProjectDockImpl::on_m_prefixCombo_activated(QString text)
{
	QSharedPointer<GenerixProject> gnxProject = m_gnxProject.toStrongRef();
	if (gnxProject)
		gnxProject->setDefaultPrefix(text);
}
