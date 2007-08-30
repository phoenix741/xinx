/***************************************************************************
 *   Copyright (C) 2007 by Ulrich Van Den Hekke                            *
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
 
#ifndef __P_THREADEDCONFIGURATIONFILE_H__
#define __P_THREADEDCONFIGURATIONFILE_H__

// Xinx header
#include "../threadedconfigurationfile.h"

// Qt header
#include <QObject>

class PrivateThreadedConfigurationFile : public QObject {
	Q_OBJECT
public: 
	PrivateThreadedConfigurationFile( ThreadedConfigurationFile * parent );
	~PrivateThreadedConfigurationFile();
	
	enum state { GETVERSION } m_state;
	QString m_pathname;
	ConfigurationVersion m_versionConfiguration;
public slots:
	void threadFinished();
private:
	ThreadedConfigurationFile * m_parent;
};

#endif // __P_THREADEDCONFIGURATIONFILE_H__
