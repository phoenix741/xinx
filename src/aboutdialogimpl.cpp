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

// Xinx header
#include "aboutdialogimpl.h"

/* AboutDialogImpl */

AboutDialogImpl::AboutDialogImpl( QWidget * parent, Qt::WFlags f) 
	: QDialog(parent, f) {
	setupUi(this);
	
	m_aboutLabel->setText( 	"<b>XINX</b> Is Not only XML<br/>"
							"(c) 2006-2007, Ulrich Van Den Hekke<br/>"
							"<a href=\"http://ks351557.kimsufi.com/xinx/\">http://ks351557.kimsufi.com/xinx/</a><br/>"
							"<br/>"
							"<i>This software is under the GPL Public Licence. Please read the software licence <br/>"
							"before use it. Icon and Logo are under the GPL Public Licence and comes from <br/>"
							"K-Desktop Environment, Cervesia, KDevelop.</i>"
						);
						
	m_authorText->setHtml( 	"Please use <a href=\"http://ks351557.kimsufi.com/xinx/\" target=\"_new\">http://ks351557.kimsufi.com/xinx/</a><br/>"
							"<br/>"
							"<b>Ulrich Van Den Hekke</b><br/>"
							"Mail: <a href=\"mailto:ulrich.vdh@free.fr\">ulrich.vdh@free.fr</a><br/>"
							"Home page: <a href=\"http://ks351557.kimsufi.com/\" target=\"_new\">http://ks351557.kimsufi.com/</a><br/>"
							"Project leader"
						);
							
	m_thanksText->setHtml(	"<b>Mickael BERTELOOT</b>,<br/>"
							"<b>Kamal B.         </b>,<br/>"
							"<b>Alexandre ROCHER</b>,<br/>"
							"<b>Matthieu DUBOUT</b>,<br/>"
							"For good idea and bug report<br/>"
							"<br/>"
							"And all people who help me in some ways and i have forgotten to mention"
						);
}


