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
#include "exceptions.h"

/* AboutDialogImpl */

AboutDialogImpl::AboutDialogImpl( QWidget * parent, Qt::WFlags f ) 
	: QDialog(parent, f) {
	XINX_TRACE( "AboutDialogImpl", QString("( parent, flags )") );
	setupUi(this);
	
	m_labelVersion->setText( m_labelVersion->text().arg( qVersion() ) );
	
	m_aboutLabel->setText( 	"<b>XINX</b> Is Not only XML<br/>"
							"(c) 2006-2007, Ulrich Van Den Hekke<br/>"
							"<a href=\"http://xinx.shadoware.org/\">http://xinx.shadoware.org/</a><br/>"
							"<br/>"
							"<i>This software is under the GPL Public Licence. Please read the software licence <br/>"
							"before use it. Icon and Logo are under the GPL Public Licence and comes from <br/>"
							"K-Desktop Environment, KDevelop.</i>"
						);
						
	m_authorText->setHtml( 	"Please use <a href=\"http://xinx.shadoware.org/\" target=\"_new\">http://xinx.shadoware.org/</a> to report bug<br/>"
							"<br/>"
							"<b>Ulrich Van Den Hekke</b><br/>"
							"Mail: <a href=\"mailto:ulrich.vdh@free.fr\">ulrich.vdh@free.fr</a><br/>"
							"Home page: <a href=\"http://www.shadoware.org/\" target=\"_new\">http://www.shadoware.org/</a><br/>"
							"Project leader"
						);
							
	m_thanksText->setHtml(	"<b>Mickael BERTELOOT</b><br/>"
							"For ideas and bug reports and more<br/>"
							"<b>Kamal B.         </b><br/>"
							"For ideas and bug reports and more<br/>"
							"<b>Alexandre ROCHER</b><br/>"
							"For ideas and bug reports and more<br/>"
							"<b>Matthieu DUBOUT</b><br/>"
							"For ideas, bug reports, some snipets, and more<br/>"
							"<br/>"
							"And all people who help me in some ways and i have forgotten to mention"
						);
}


