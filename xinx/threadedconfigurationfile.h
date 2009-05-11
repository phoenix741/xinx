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

#ifndef __THREADEDCONFIGURATIONFILE_H__
#define __THREADEDCONFIGURATIONFILE_H__

// Xinx header
#include <core/configurationfile.h>
#include <core/xinxthread.h>

class ThreadedConfigurationFile : public XinxThread {
	Q_OBJECT
public:
	virtual ~ThreadedConfigurationFile();
	static ThreadedConfigurationFile * simpleConfigurationFile( const QString & pathname );
	static ThreadedConfigurationFile * businessViewOfFile( const QString & filename );
signals:
	void businessViewFinded( QStringList list );
	void versionFinded( ConfigurationFile configuration );
protected:
	ThreadedConfigurationFile();
	virtual void threadrun();
private slots:
	void threadFinished();
private:
	enum state { GETVERSION, GETBUSINESSVIEW } m_state;
	QString m_pathname, m_filename;
	QStringList m_businessView;
	ConfigurationFile m_configuration;

};

#endif // __THREADEDCONFIGURATIONFILE_H__
