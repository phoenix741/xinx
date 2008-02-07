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
 
#ifndef __THREADEDCONFIGURATIONFILE_H__
#define __THREADEDCONFIGURATIONFILE_H__

// Xinx header
#include "configurationfile.h"
#include "xinxthread.h"

class PrivateThreadedConfigurationFile;

class ThreadedConfigurationFile : public XinxThread {
	Q_OBJECT
public:
	virtual ~ThreadedConfigurationFile();
	static ThreadedConfigurationFile * simpleConfigurationFile( const QString & pathname );
signals:
	void versionFinded( SimpleConfigurationFile configuration );
protected:
	ThreadedConfigurationFile();
	virtual void threadrun();
private:
	PrivateThreadedConfigurationFile * d;
	friend class PrivateThreadedConfigurationFile;
};

#endif // __THREADEDCONFIGURATIONFILE_H__
