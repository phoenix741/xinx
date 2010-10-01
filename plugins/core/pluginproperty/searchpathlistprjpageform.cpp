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
#include <project/xinxproject.h>
#include "searchpathlistprjpageform.h"
#include "pluginresolver/manualfileresolver.h"

// Qt header
#include <QDir>
#include <QInputDialog>
#include <QPushButton>

SearchPathListFormImpl::SearchPathListFormImpl(QWidget * parent, Qt::WFlags f) : QWidget(parent, f), m_project(0)
{
	setupUi(this);

	m_searchPathList->setDefaultVisible(false);
}

SearchPathListFormImpl::~SearchPathListFormImpl()
{

}

QPixmap SearchPathListFormImpl::image()
{
	return QPixmap();
}

QString SearchPathListFormImpl::name()
{
	return windowTitle();
}

QWidget * SearchPathListFormImpl::settingsDialog()
{
	return this;
}

bool SearchPathListFormImpl::loadSettingsDialog()
{
	if (! m_project) return true;

	const QString searchPathListStr  = m_project->readProperty("searchPathList").toString();
	const QStringList searchPathList = searchPathListStr.split(";;", QString::SkipEmptyParts);
	QStringList searchPathList_fromNativeSeparators;

	foreach(QString path, searchPathList)
	{
		searchPathList_fromNativeSeparators.append(QDir::fromNativeSeparators(path));
	}
	m_searchPathList->setValues(searchPathList_fromNativeSeparators);

	return true;
}

bool SearchPathListFormImpl::saveSettingsDialog()
{
	if (! m_project) return true;

	const QStringList searchPathList = m_searchPathList->values();
	QStringList searchPathList_toNativeSeparators;

	foreach(QString path, searchPathList)
	{
		searchPathList_toNativeSeparators.append(QDir::toNativeSeparators(path));
	}

	const QString searchPathListStr = searchPathList_toNativeSeparators.join(";;");

	m_project->writeProperty("searchPathList",  searchPathListStr);   // toNativeSeparators

	return true;
}

bool SearchPathListFormImpl::cancelSettingsDialog()
{
	return true;
}

bool SearchPathListFormImpl::isSettingsValid()
{
	return true;
}

bool SearchPathListFormImpl::isVisible()
{
	return ManualFileResolver::staticId().compare(m_currentResolverName) == 0;
}

void SearchPathListFormImpl::setProject(XinxProject * project)
{
	m_project = project;
}


