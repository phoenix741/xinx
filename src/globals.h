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

#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <QObject>
#include <QList>

class XSLProject;
class WebServices;
class ObjectsView;
class XINXConfig;
class SnipetList;
class Completion;

/*!
 * List of WebServices object.
 */
typedef QList<WebServices*> WebServicesList;

/*!
 * Create globals class contains global definition.
 */
class Globals : public QObject {
	Q_OBJECT
public:
	/*!
	 * Intialize default value of object.
	 */ 
	Globals();
	/*!
	 * Destruct the global object.
	 */
	~Globals();

	/*!
	 * List all XML Java View Object File.
	 */
	ObjectsView * m_javaObjects;
	/*!
	 * List all WebServices in coordination with the project.
	 */
	WebServicesList*  m_webServices;
	/*!
	 * Opened project.
	 */
	XSLProject * m_project;
	
	/*!
	 * XINX Config file.
	 */
	XINXConfig * m_config;
	
	/*!
	 * List of snipet.
	 */
	SnipetList * m_snipetList;
	
	/*!
	 * Completion content.
	 */
	Completion * m_completionContents;
	
	/*!
	 * Call this function when you want emit signals \e projectChanged().
	 * \sa projectChanged()
	 */
	void emitProjectChanged();
	/*!
	 * Call this function when you want emit signals \e webServicesChanged().
	 * \sa webServicesChanged()
	 */
	void emitWebServicesChanged();
Q_SIGNALS:
	/*!
	 * Signal emited when the project has changed
	 * \sa emitProjectChanged()
	 */
	void projectChanged();
	/*!
	 * Signal emited when webservices list has changed
	 * \sa emitWebServicesChanged() 
	 */
	void webServicesChanged();
};

/*!
 * Global defintion of global
 */
extern Globals global;

/*!
 * Numbre of maximum recent file that can be opened
 */
#define MAXRECENTFILES 10

/*!
 * Default extention filtre 
 */
#define DEFAULT_PROJECT_FILTRE QStringList() << "*.xml" << "*.xsl" << "*.js" << "*.fws"
/*!
 * Default project options filtre
 */
#define DEFAULT_PROJECT_FILTRE_OPTIONS QDir::AllDirs | QDir::Files | QDir::Readable | QDir::NoDotAndDotDot

template <typename ForwardIterator>
void qDeleteAllLater(ForwardIterator begin, ForwardIterator end) {
	while (begin != end) {
		if( dynamic_cast<QObject*>( *begin ) ) 
			dynamic_cast<QObject*>( *begin )->deleteLater();
		else
			delete *begin;
		++begin;
	}
}

template <typename Container>
inline void qDeleteAllLater(const Container &c) {
	qDeleteAllLater(c.begin(), c.end());
}


#endif // __GLOBALS_H__
