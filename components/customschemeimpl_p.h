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

#pragma once
#ifndef _CUSTOMSCHEMEIMPL_P_H_
#define _CUSTOMSCHEMEIMPL_P_H_

#include "customschemeimpl.h"
#include <QObject>
#include <QListWidgetItem>
#include "ui_customscheme.h"

class QCodeEdit;
class QFormatScheme;
class QFormat;
class QLanguageFactory;

class CustomSchemeImplPrivate : public QObject
{
	Q_OBJECT
public slots:
	void on_m_formatsListView_currentItemChanged(QListWidgetItem * current, QListWidgetItem * previous);
	void on_m_boldCheckBox_stateChanged(int state);
	void on_m_overLineCheckBox_stateChanged(int state);
	void on_m_strikeOutCheckBox_stateChanged(int state);
	void on_m_italicCheckBox_stateChanged(int state);
	void on_m_underLineCheckBox_stateChanged(int state);
	void on_m_waveUnderLineCheckBox_stateChanged(int state);
	void on_m_foreGroundComboBox_activated(const QColor &col);
	void on_m_backGroundComboBox_activated(const QColor &col);
public:
    CustomSchemeImplPrivate();
    virtual ~CustomSchemeImplPrivate();

	void updateFormatList();

	QScopedPointer<Ui::CustomScheme> _ui;
	QLanguageFactory * m_languageFactory;
	QCodeEdit * m_exampleEditor;
	QFormatScheme * m_formats;
	QFormat * m_currentFormat;
	QListWidgetItem * m_currentItem;
	QString m_example;
	QStringList m_hiddenFormat;
};

#endif /* _CUSTOMSCHEMEIMPL_P_H_ */
