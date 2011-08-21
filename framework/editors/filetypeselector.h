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

#ifndef FILETYPESELECTOR_H
#define FILETYPESELECTOR_H

#include <core/lib-config.h>
#include <QWidget>
#include <QListWidget>
#include <QCheckBox>
#include <QLabel>
#include <plugins/interfaces/files.h>

typedef QSharedPointer<IFileTypePlugin> IFileTypePluginPtr;

class LIBEXPORT FileTypeSelector : public QWidget
{
	Q_OBJECT
public:
	explicit FileTypeSelector(QWidget *parent = 0);
	virtual ~FileTypeSelector();

	void setShowFileName(bool value);
	bool isShowFileName() const;

	void setFileName(const QString & filename);
	const QString & filename() const;

	IFileTypePluginPtr selectedType() const;
	int count() const;
signals:
	void doubleClicked();
private slots:
	void setFileTypes(const QList<IFileTypePluginPtr> & types);
	void toggledShowAll();
private:
	QString _file_name;
	QLabel * _file_name_label;
	QListWidget * _file_type_list;
	QCheckBox * _show_all;
};

#endif // FILETYPESELECTOR_H
