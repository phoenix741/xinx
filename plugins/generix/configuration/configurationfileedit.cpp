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
#include "configurationfileedit.h"

/* ConfigurationVersionLabel */

ConfigurationVersionLabel::ConfigurationVersionLabel(QWidget * parent) : QLabel(parent)
{

}

const ConfigurationVersion & ConfigurationVersionLabel::version() const
{
	return m_version;
}

void ConfigurationVersionLabel::setVersion(const ConfigurationVersion & value)
{
	QPalette paletteVerion(palette());

	if (m_version != value)
	{
		if (value.isValid())
		{
			paletteVerion.setColor(QPalette::WindowText, QColor());
			setText(value.toString());
		}
		else
		{
			paletteVerion.setColor(QPalette::WindowText, Qt::red);
			setText(tr("This directory doesn't containt a valide GCE project."));
		}
		setPalette(paletteVerion);

		m_version = value;
	}
}

