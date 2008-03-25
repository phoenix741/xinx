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

class PrivateXinxPluginSelector;
class XinxPluginDelegate;
class XinxPluginElement;

class XinxPluginSelector : public QListView {
	Q_OBJECT
public:
	XinxPluginSelector( QWidget *parent = 0 );
    virtual ~XinxPluginSelector();
    
    void addPlugin( XinxPluginElement * plugin );
    QStyleOptionViewItem viewOptions() const;
signals:
	void configurePlugin( XinxPluginElement * plugin );
	void aboutPlugin( XinxPluginElement * plugin );
private:
	PrivateXinxPluginSelector * d;
	friend class PrivateXinxPluginSelector;
};

#endif /*XINXPLUGINSELECTOR_H_*/
