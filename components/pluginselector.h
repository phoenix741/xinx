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

#pragma once
#ifndef XINXPLUGINSELECTOR_H_
#define XINXPLUGINSELECTOR_H_

// Qt header
#include <QListView>

class PrivatePluginSelector;
class PluginDelegate;
class PluginElement;

class PluginSelector : public QListView
{
	Q_OBJECT
	Q_CLASSINFO("Author", "Ulrich Van Den Hekke")
	Q_CLASSINFO("URL", "http://xinx.shadoware.org")
	Q_CLASSINFO("Licence", "GPL v2 or later")
public:
	PluginSelector(QWidget *parent = 0);
	virtual ~PluginSelector();

	void addPlugin(PluginElement * plugin);
	const QList<PluginElement*> & plugins() const;

	QStyleOptionViewItem viewOptions() const;
public slots:
	void clear();

signals:
	void configurePlugin(PluginElement * plugin);
	void aboutPlugin(PluginElement * plugin);
private:
	PrivatePluginSelector * d;
	friend class PrivatePluginSelector;
};

#endif /*XINXPLUGINSELECTOR_H_*/
