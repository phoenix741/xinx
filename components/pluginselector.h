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

#ifndef XINXPLUGINSELECTOR_H_
#define XINXPLUGINSELECTOR_H_
#pragma once

// Qt header
#include <QListView>

class PrivatePluginSelector;
class PluginDelegate;
class PluginElement;

/*!
 * This class show to the user the list of plugins with foreach
 * plugins a configure button and a about button.
 *
 * If the plugins propose one, the list can also show an icon.
 */
class PluginSelector : public QListView
{
	Q_OBJECT
	Q_CLASSINFO("Author", "Ulrich Van Den Hekke")
	Q_CLASSINFO("URL", "http://xinx.shadoware.org")
	Q_CLASSINFO("Licence", "GPL v2 or later")
public:
	/*!
	 * Create the plugin selector widget.
	 * \param parent The parent widget.
	 */
	PluginSelector(QWidget *parent = 0);
	//! Destroy the plugin selector.
	virtual ~PluginSelector();

	//! Add a plugin to the list. The plugin must be a \e XinxPluginElement.
	void addPlugin(PluginElement * plugin);

	//! List of plugins add in the \e PluginSelector
	const QList<PluginElement*> & plugins() const;

	//! \internal
	QStyleOptionViewItem viewOptions() const;

public slots:
	//! Clear the list of \e PluginElement*
	void clear();

signals:
	//! This signal is emited when the plugin request to be configured.
	void configurePlugin(PluginElement * plugin);
	//! The signal is emited when the plugin request to show a description.
	void aboutPlugin(PluginElement * plugin);
private:
	PrivatePluginSelector * d;
	friend class PrivatePluginSelector;
};

#endif /*XINXPLUGINSELECTOR_H_*/
