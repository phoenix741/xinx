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
 * \file contentviewmodel.h
 * \brief This file contains element to manage content view model.
 */

#ifndef __CONTENTVIEWMODEL_H__
#define __CONTENTVIEWMODEL_H__

/*!
 * \class ContentViewModel
 * \brief This class is used to show the content of the file in a tree view.
 *
 * This class is used as a proxy of the tree view and content view nodes. It's
 * a sub-class of the Qt standard class QAbstractItemModel.
 */
class ContentViewModel : public QAbstractItemModel {
	Q_OBJECT
public:
private:
};

#endif /* __CONTENTVIEWCLASS_H__ */
