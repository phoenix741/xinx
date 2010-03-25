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

/* DictionaryDockWidgetImpl */

DictionaryDockWidgetImpl::DictionaryDockWidgetImpl(QWidget * parent) : QWidget(parent)
{
	setupUi(this);
	setWindowTitle(tr("Dictionary"));
	setWindowIcon(QIcon(":/generix/images/dictionary16.png"));


	connect(ContentView2::Manager::self()->cache(), SIGNAL(cacheLoaded(ContentView2::File)), this, SLOT(update(ContentView2::File)));
}

DictionaryDockWidgetImpl::~DictionaryDockWidgetImpl()
{

}

void DictionaryDockWidgetImpl::update(const ContentView2::File & file)
{
	if (file.type() == "GNX_DICO")
	{
		startTimer(500);
	}
}

void DictionaryDockWidgetImpl::on_m_filterLine_textChanged(QString filter)
{
	Q_UNUSED(filter);
	startTimer(500);
}

void DictionaryDockWidgetImpl::timerEvent(QTimerEvent * event)
{
	killTimer(event->timerId());

	m_dictionaryTreeWidget->loadDictionaries(m_filterLine->text());
	m_informationLbl->setText(tr("%1 label(s) loaded.").arg(m_dictionaryTreeWidget->invisibleRootItem()->childCount()));
}
