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

#ifndef DICTIONARYWIDGET_H
#define DICTIONARYWIDGET_H

#include <QTreeWidget>

namespace XinxProject
{
	class Project;
}

class DictionaryWidget : public QTreeWidget
{
	Q_OBJECT
public:
	DictionaryWidget(QWidget * parent = 0);

	void loadDictionaries(const QString & prefix);
private slots:
	void textEditorChanged(int index);
private:
	XinxProject::Project * _project;
	QString _prefix;
};

#endif // DICTIONARYWIDGET_H
