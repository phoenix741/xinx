/*
 XINX
 Copyright (C) 2007-2011 by Ulrich Van Den Hekke
 xinx@shadoware.org

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful.
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef NEWVERSIONWIZARDIMPL_H
#define NEWVERSIONWIZARDIMPL_H

// Xinx header
#include <core/lib-config.h>

// Qt header
#include <QWizard>

class PrivateNewVersionWizardImpl;

class LIBEXPORT NewVersionWizardImpl : public QWizard
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
private slots:
	void on_m_listSlider_valueChanged(int value);
	void on__acceptRadioButton_toggled(bool value);
	void slotCurrentIdChanged(int id);
private:
	PrivateNewVersionWizardImpl * d;
	friend class PrivateNewVersionWizardImpl;
};

#endif // NEWVERSIONWIZARDIMPL_H
