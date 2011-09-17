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

#ifndef SEARCHDOCKIMPL_H
#define SEARCHDOCKIMPL_H
#pragma once

// Xinx header
#include "ui_searchdock.h"
#include <core/abstractmesssagedockwidget.h>
#include <editors/abstracteditor.h>
#include "findedmodel.h"

// Qt header
#include <QDockWidget>

class SearchDockWidgetImpl : public AbstractMessageDockWidget
{
	Q_OBJECT
public:
	SearchDockWidgetImpl(QWidget * parent = 0);
	virtual ~SearchDockWidgetImpl();

	virtual bool automatcallyShow() const;
	virtual bool automaticallyClose() const;

	void init(const QString& from, const QString& to, const AbstractEditor::SearchOptions& options);
	void end();

	QModelIndex selectedIndex() const;
	QModelIndex selectNextFinded();
	QModelIndex selectPreviousFinded();

signals:
	void abort();
public slots:
	void find(const QString & filename, const QString & text, int line, int posStart, int posEnd);
	void replace();
	void endOfReplace();
protected slots:
	void activated(const QModelIndex & index);
	void slotLineModified(const QString & filename, int line);
private:
	QScopedPointer<Ui::SearchDockWidget> _widget;
	FindedModel* _model;
	QString m_from, m_to;
	AbstractEditor::SearchOptions m_options;
};

#endif /* SEARCHDOCKIMPL_H */





