/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2007-2010 by Ulrich Van Den Hekke                         *
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

#ifndef EDITORCHOICEDLG_H
#define EDITORCHOICEDLG_H

// Xinx header
#include <core/lib-config.h>
#include "ui_editorchoicedlg.h"
#include <plugins/plugininterfaces.h>

class LIBEXPORT EditorChoiceDlg : public QDialog, private Ui::EditorChoiceDlg {
    Q_OBJECT
public:
    EditorChoiceDlg(QWidget *parent = 0);

	void setFileName(const QString & value);

	void setFileTypes(const QList<IFileTypePlugin *> & types);
	IFileTypePlugin * selectedType() const;

private slots:
	void on_m_typeFileList_itemDoubleClicked(QListWidgetItem* item);
};

#endif // EDITORCHOICEDLG_H
