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

/*!
 * \file contentviewcache.h
 * \brief Class to allow caching of content view object and access it.
 */

#ifndef __CONTENTVIEWCLASS_H__
#define __CONTENTVIEWCLASS_H__

/*!
 * \class ContentViewCache
 * \brief This class containt a list of pre-loaded file that can be used in content view obect.
 *
 * This class is creating by a project. All file defined in the project is pre-loaded. Next when
 * a content view object want to access to a file, he look to know if the file isn't already
 * pre-loaded.
 *
 * If the content of the file is found, he is returned ; else he is created.
 *
 * The goal is to down the size in the memory, and speed up the loading of file and completion.
 */
class ContentViewCache {
public:

	/*!
	 * Return the content view for the given file name. Look in the cache if the
	 * node exist. If not, use the XinxPluginLoader to create the content file name.
	 * \param filename The file name of the content view to create.
	 * \return Return the content view of the file name given in parameters.
	 */
	ContentViewNode * contentOfFileName( const QString & filename );
private:
	QCache<QString,ContentViewNode*> m_nodes;
};

#endif /* __CONTENTVIEWCLASS_H__ */