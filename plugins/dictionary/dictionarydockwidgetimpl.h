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

#ifndef DICTIONARYDOCKWIDGET_H
#define DICTIONARYDOCKWIDGET_H

// Xinx header
#include <ui_dictionarydockwidget.h>

// Qt header
#include <QDockWidget>
#include <QFutureWatcher>

class ContentViewModel;
class ContentViewNode;

class DictionaryDockWidgetImpl : public QDockWidget, private Ui::DictionaryDockWidget {
	Q_OBJECT
public:
	DictionaryDockWidgetImpl( QWidget * parent = 0 );
	virtual ~DictionaryDockWidgetImpl();

	ContentViewNode * dictionary() { return m_dictionaryNode; }

	void loadDictionary( const QString & filename );
	void loadDictionaryList( const QString & filename );
	void clearDictionaryList();
private slots:
	void dictionaryLoaded();
	void on_m_dictionaryList_currentIndexChanged( int index );
private:
	ContentViewModel * m_dictionaryModel;
	ContentViewNode * m_dictionaryNode;

	QFutureWatcher<QString> m_watcher;
};

#endif // DICTIONARYDOCKWIDGET_H
