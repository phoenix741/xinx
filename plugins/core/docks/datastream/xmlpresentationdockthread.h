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

#ifndef __P_XMLPRESENTATIONDOCKWIDGET_H__
#define __P_XMLPRESENTATIONDOCKWIDGET_H__
#pragma once

// Xinx header
#include <core/xinxthread.h>
#include <core/filewatcher.h>
#include "xmlpresentationdockwidget.h"
#include "ui_xmlpresentationwidget.h"
#include "xmlpresentationitem.h"
#include <utils/recursivesortfilterproxymodel.h>

// Qt header
#include <QThread>
#include <QTimer>
#include <QPointer>
#include <QStringList>

class XmlPresentationDockThread : public XinxThread
{
	Q_OBJECT
public:
	XmlPresentationDockThread(XmlPresentationDockWidget * parent);
	~XmlPresentationDockThread();

	Ui::XmlPresentationWidget * m_xmlPresentationWidget;
	QString m_filename;

	QPointer<XmlPresentationModel> m_model;
	QPointer<RecursiveSortFilterProxyModel> m_sortFilterModel;
	QPointer<FileWatcher> m_watcher;

	QStringList m_filterHidePath;
	QString m_filteredText, m_currentXpath;
	bool m_filteredElement;
	QTimer m_timerTextChanged;

	enum { THREAD_OPENING, THREAD_FILTERED } m_threadAct;

	void open(const QString& filename);
	void close();

	void disabledInterface(bool value);
public slots:
	void open();
	void adaptColumns();
	void threadTerminated();

	void filterTextChanged();
	void filterTextChangedTimer();

	void updateXinxConf(int value);

	void evaluate();

	void editorChanged();
protected:
	virtual void threadrun();
private:
	XmlPresentationDockWidget * m_parent;
};

#endif // __P_XMLPRESENTATIONDOCKWIDGET_H__
