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
#include "customtoolsimpl.h"
#include <core/xinxconfig.h>
#include "toolsmodelindex.h"
#include "directoryeditdelegate.h"

// Qt header
#include <QHeaderView>

/* CustomToolsImpl */

CustomToolsImpl::CustomToolsImpl(QWidget * parent) : QWidget(parent)
{
	setupUi(this);
}

CustomToolsImpl::~CustomToolsImpl()
{
}

QPixmap CustomToolsImpl::image()
{
	return QPixmap(":/images/preferences-tools.png");
}

QString CustomToolsImpl::name()
{
	return windowTitle();
}

QWidget * CustomToolsImpl::settingsDialog()
{
	return this;
}

bool CustomToolsImpl::loadSettingsDialog()
{
	m_tools = XINXConfig::self()->config().tools;

	// Tools
	ToolsModelIndex * toolsModel = new ToolsModelIndex(&(m_tools), m_toolsTable);
	m_toolsTable->setModel(toolsModel);
	m_toolsTable->setItemDelegate(new DirectoryEditDelegate(m_toolsTable));
	m_toolsTable->horizontalHeader()->setResizeMode(0, QHeaderView::ResizeToContents);
	m_toolsTable->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
	//m_toolsTable->resizeColumnsToContents();
	//QObject::connect( toolsModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), m_toolsTable, SLOT(resizeColumnsToContents()) );

	return true;
}

bool CustomToolsImpl::saveSettingsDialog()
{
	XINXConfig::self()->config().tools = m_tools;

	return true;
}

bool CustomToolsImpl::cancelSettingsDialog()
{
	return true;
}

bool CustomToolsImpl::isSettingsValid()
{
	return true;
}

bool CustomToolsImpl::isVisible()
{
	return true;
}



