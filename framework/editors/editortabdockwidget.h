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

#ifndef EDITORTABDOCKWIDGET_H
#define EDITORTABDOCKWIDGET_H

#include <application/xinxdockwidget.h>
#include <QListWidget>

class EditorTabDockWidget : public XinxDockWidget
{
	Q_OBJECT
public:
	explicit EditorTabDockWidget(QWidget *parent = 0);

signals:
	void open(const QString & filename);

private slots:
	void listChanged();
	void updateCurrentEditor(int index);
	void openEditor(const QString & filename);
	void closeEditor(const QString & filename);
	void saveEditor(const QString & filename, const QString & oldfilename);
private:
	QListWidget * _documents;
};

#endif // EDITORTABDOCKWIDGET_H
