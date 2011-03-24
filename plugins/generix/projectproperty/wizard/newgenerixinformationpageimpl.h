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

#ifndef NEWGENERIXINFORMATIONPAGEIMPL_H
#define NEWGENERIXINFORMATIONPAGEIMPL_H

// Xinx header
#include "ui_newgenerixinformationpage.h"
#include <plugins/interfaces/gui.h>

// Qt header
#include <QWizardPage>

class NewGenerixInformationPageImpl : public IXinxPluginNewProjectConfigurationPage, Ui::NewGenerixInformationPage
{
	Q_OBJECT
public:
	NewGenerixInformationPageImpl();
	virtual ~NewGenerixInformationPageImpl();

	virtual QString pagePluginId() const;
	virtual bool pageIsVisible() const;
	virtual bool saveSettingsDialog(XinxProject::ProjectPtr project);

	virtual bool isComplete() const;
	virtual void initializePage();
private slots:
	void updateInformations(const QString & path);
};

#endif // NEWGENERIXINFORMATIONPAGEIMPL_H
