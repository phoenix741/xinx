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
#include "dictionarydockwidgetimpl.h"
#include <contentview2/contentview2manager.h>
#include <contentview2/contentview2cache.h>
#include <configuration/configurationmanager.h>
#include "projectproperty/generixproject.h"
#include "dictionaryparser.h"
#include "dictionarymodel.h"

/* DictionaryDockWidgetImpl */

DictionaryDockWidgetImpl::DictionaryDockWidgetImpl(QWidget * parent) : QDockWidget(parent), m_dictionaryModel(0)
{
	setupUi(this);

	connect(XINXProjectManager::self(), SIGNAL(changed()), this, SLOT(projectChanged()));
	connect(ContentView2::Manager::self()->cache(), SIGNAL(cacheLoaded(ContentView2::File)), this, SLOT(update(ContentView2::File)));
}

DictionaryDockWidgetImpl::~DictionaryDockWidgetImpl()
{

}

void DictionaryDockWidgetImpl::update(const ContentView2::File & file)
{
	if (file.path() == "GenerixDictionary")
	{
		//m_dictionaryModel->select( ContentView2::Manager::self()->database(), "CREA", "" );
		m_informationLbl->setText(tr("%1 label(s) loaded.").arg(m_dictionaryModel->rowCount()));
	}
}

void DictionaryDockWidgetImpl::projectChanged()
{
	// Desactivate the dictionary
	m_dictionaryTreeView->setModel(0);
	delete m_dictionaryModel;
	m_dictionaryModel = 0;

	// Create the new dictionary
	GenerixProject * project = static_cast<GenerixProject*>(XINXProjectManager::self()->project().data());
	if (project && project->isGenerixActivated())
	{
		ConfigurationManager::self()->loadDictionary(project);
		//m_dictionaryModel = new DictionaryModel("GenerixDictionary", m_dictionaryTreeView);
		//m_dictionaryTreeView->setModel(m_dictionaryModel);
	}
}



