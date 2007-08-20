/***************************************************************************
 *   Copyright (C) 2006 by Ulrich Van Den Hekke                            *
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

#include "iconprojectprovider.h"

IconProjectProvider::IconProjectProvider() : QFileIconProvider() {
	
}

IconProjectProvider::~IconProjectProvider() {
	
}
	
QIcon IconProjectProvider::icon( const QFileInfo & info ) const {
	if( info.suffix() == "xsl" ) {
		return QIcon( ":/images/typexsl.png" );
	} else
	if( info.suffix() == "xml" ) {
		return QIcon( ":/images/typexml.png" );
	} else
	if( info.suffix() == "js" ) {
		return QIcon( ":/images/typejs.png" );
	} else
	if( info.suffix() == "fws" ) {
		return QIcon( ":/images/typefws.png" );
	} else
		return QFileIconProvider::icon( info );
}

