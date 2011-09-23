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

#ifndef PROJECTWIZARD_H_
#define PROJECTWIZARD_H_

// Qt header
#include <QWizard>

class ProjectConverter;

class ProjectWizard : public QWizard
{
	Q_OBJECT
public:
	ProjectWizard(QString filname, QWidget * parent = 0);

	ProjectConverter * converter() const;
	void setConverter(ProjectConverter * c);

	virtual void accept();
private:
	ProjectConverter * m_converter;
	QString m_filename;
};

#endif /*PROJECTWIZARD_H_*/
