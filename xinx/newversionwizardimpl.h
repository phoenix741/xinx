/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2010 by Ulrich Van Den Hekke                              *
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

#ifndef NEWVERSIONWIZARDIMPL_H
#define NEWVERSIONWIZARDIMPL_H

// Xinx header
#include "ui_newversion.h"

// Qt header
#include <QWizard>

class NewVersionWizardImpl : public QWizard, public Ui::NewVersionWizard
{
	Q_OBJECT
public:
	NewVersionWizardImpl();
	NewVersionWizardImpl(QWidget * parent);
	virtual ~NewVersionWizardImpl();

	void loadFromConfig();
	void saveToConfig();
public slots:
	virtual void accept();

private:
	QHash<QString,QString> m_tools;

private slots:
	void on_m_listSlider_valueChanged(int value);
};

#endif // NEWVERSIONWIZARDIMPL_H
