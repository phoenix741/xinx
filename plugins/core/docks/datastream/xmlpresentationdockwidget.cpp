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
#include "xmlpresentationdockwidget.h"
#include "xmlpresentationdockthread.h"

/* Static member */

XmlPresentationDockWidget * XmlPresentationDockWidget::s_self = 0;

/* XmlPresentationDockWidget */

XmlPresentationDockWidget::XmlPresentationDockWidget(QWidget * parent) : QWidget(parent)
{
	if (! s_self) s_self = this;

	d = new XmlPresentationDockThread(this);
	setWindowTitle(tr("Xml Presentation"));
	setWindowIcon(QIcon(":/images/preferences-xmlpres.png"));
}

XmlPresentationDockWidget::~XmlPresentationDockWidget()
{
	if (s_self == this) s_self = 0;
	delete d;
}

void XmlPresentationDockWidget::setFilename(const QString & filename)
{
	d->open(filename);
}

const QString & XmlPresentationDockWidget::filename() const
{
	return d->m_openingFile;
}

void XmlPresentationDockWidget::evaluate()
{
	d->evaluate();
}

XmlPresentationDockWidget * XmlPresentationDockWidget::self()
{
	return s_self;
}

