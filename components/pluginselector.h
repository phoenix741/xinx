/***************************************************************************
 *   Copyright (C) 2008 by Ulrich Van Den Hekke                            *
 *   ulrich.vdh@free.fr                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef XINXPLUGINSELECTOR_H_
#define XINXPLUGINSELECTOR_H_

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
class PluginSelector : public QListView {
	Q_OBJECT
	Q_CLASSINFO("Author", "Ulrich Van Den Hekke")
	Q_CLASSINFO("Licence", "GPL v2 or later")
public:
	/*!
	 * Create the plugin selector widget.
	 * \param parent The parent widget.
	 */
	PluginSelector( QWidget *parent = 0 );
	/*!
	 * Destroy the plugin selector.
	 */
    virtual ~PluginSelector();

    /*!
     * Add a plugin to the list. The plugin must be a \e XinxPluginElement.
     */
    void addPlugin( PluginElement * plugin );

    QStyleOptionViewItem viewOptions() const;
signals:
	/*!
	 * This signal is emited when the plugin request to be configured.
	 */
	void configurePlugin( PluginElement * plugin );
	/*!
	 * The signal is emited when the plugin request to show a description.
	 */
	void aboutPlugin( PluginElement * plugin );
private:
	PrivatePluginSelector * d;
	friend class PrivatePluginSelector;
};

#endif /*XINXPLUGINSELECTOR_H_*/
