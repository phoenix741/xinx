/****************************************************************************
**
** Copyright (C) 2006-2008 fullmetalcoder <fullmetalcoder@hotmail.fr>
**
** This file is part of the Edyuk project <http://edyuk.org>
** 
** This file may be used under the terms of the GNU General Public License
** version 3 as published by the Free Software Foundation and appearing in the
** file GPL.txt included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef _QMARKER_INTERFACE_H_
#define _QMARKER_INTERFACE_H_

/*!
	\file qmarkerinterface.h
	\brief Definition of the QMarkerInterface class.
	
	\see QMarkerInterface
*/

/*!
	\class QMarkerInterface
	\brief The marker itself
	
	This pure virtual classes features only one function that is called by
	attached matchers when needed.
	
	Note : As other interfaces, QMarkerInterface is "editor-independent", one
	interface only is created by extension and several matchers may share an
	interface, so do NOT store data in any other place that a QBlockData...
	
	\see QMatcher
	\see QBlockData
*/

#include <QStringList>

class QColor;
class QPixmap;
class QMatcher;
class QTextCursor;

class QMarkerInterface
{
	public:
		/*!
			\brief Empty destructor needed by the pure virtual callback...
		*/
		virtual ~QMarkerInterface() {}
		
		
		virtual QString defaultMark() const = 0;
		
		/*!
			\return A list of recognized IDs
		*/
		virtual QStringList ids() const = 0;
		
		/*!
			\return A list of IDs grouped according to mutual exclusivity
		*/
		virtual QList<QStringList> items() const = 0;
		
		/*!
			\return The icon associated to the mark ID \a id
		*/
		virtual QPixmap icon(const QString& id) const = 0;
		
		/*!
			\return the color associated to the mark ID \a id
		*/
		virtual QColor color(const QString& id) const = 0;
		
		/*!
			\return wether to focus on line when mark is set.
		*/
		virtual bool needFocus(const QString& id) const = 0;
		
		/*!
			\return wether to prevent the user from "directly" modifying
			the status of mark \a id.
		*/
		virtual bool isPrivate(const QString& id) const = 0;
		
		/*!
			\return the persistency of the mark \a id
		*/
		virtual int persistency(const QString& id) const = 0;
		
		/*!
			\return the id with the highest priority among the list \a ids
		*/
		virtual QString priority(const QStringList& ids) const = 0;
};

#endif // _QMARKER_INTERFACE_H_
