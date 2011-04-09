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
#include <configuration/configurationmanager.h>
#include "projectproperty/generixproject.h"

/* DictionaryDockWidgetImpl */

DictionaryDockWidgetImpl::DictionaryDockWidgetImpl(QWidget * parent) : QWidget(parent), m_timerId(0)
{
	setupUi(this);
	setWindowTitle(tr("Dictionary"));
	setWindowIcon(QIcon(":/generix/images/dictionary16.png"));

	setFocusProxy(m_filterLine);

	connect(m_dictionaryTreeWidget, SIGNAL(changed()), this, SLOT(dictionariesLoaded()));
}

DictionaryDockWidgetImpl::~DictionaryDockWidgetImpl()
{

}

void DictionaryDockWidgetImpl::dictionariesLoaded()
{
	m_informationLbl->setText(tr("%n label(s) loaded.", "", m_dictionaryTreeWidget->model()->rowCount()));
}

void DictionaryDockWidgetImpl::on_m_filterLine_textChanged(QString filter)
{
	Q_UNUSED(filter);
	if (m_timerId) killTimer(m_timerId);
	m_timerId = startTimer(500);
}

void DictionaryDockWidgetImpl::timerEvent(QTimerEvent * event)
{
	if (m_timerId == event->timerId())
	{
		killTimer(m_timerId);
		m_timerId = 0;
		m_dictionaryTreeWidget->setPrefix(m_filterLine->text());
	}
}
