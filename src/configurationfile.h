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
 
#ifndef __CONFIGURATIONFILE_H__
#define __CONFIGURATIONFILE_H__

#include <QString>

class ConfigurationVerstionIncorectException {	
};

class PrivateConfigurationVersion;

class ConfigurationVersion {
public:
	ConfigurationVersion( int major = -1, int minor = -1, int release = -1, int build = -1 );
	ConfigurationVersion( const QString & version, int build = 0 );
	virtual ~ConfigurationVersion();

	int major() const;
	int minor() const;
	int release() const;
	int build() const;
	
	QString toString() const;
	bool isValid();
	
	bool operator== ( ConfigurationVersion version ) const;
	bool operator>  ( ConfigurationVersion version ) const;
	bool operator>= ( ConfigurationVersion version ) const;
	bool operator<  ( ConfigurationVersion version ) const;
	bool operator<= ( ConfigurationVersion version ) const;
private:
	int m_major;
	int m_minor;
	int m_release;
	int m_build;
};

class ConfigurationFile {
public:
	static bool exists( const QString & DirectoryPath );
	static ConfigurationVersion version( const QString & DirectoryPath );
private:	
};

#endif // __CONFIGURATIONFILE_H__
