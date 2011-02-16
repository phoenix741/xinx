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

#include <QTreeView>

class DictionaryModel;
namespace XinxProject
{
class Project;
}

class DictionaryWidget : public QTreeView
{
	Q_OBJECT
public:
	DictionaryWidget(QWidget * parent = 0);
	virtual ~DictionaryWidget();

	void setPrefix(const QString & prefix);
	const QString & prefix();
signals:
	void changed();
private:
	QString _prefix;
	QScopedPointer<DictionaryModel> _model;
};

#endif // DICTIONARYWIDGET_H
