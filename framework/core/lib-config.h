/***************************************************************************
 *   Copyright (C) 2010 by Ulrich Van Den Hekke   *
 *   ulrich.vdh@gmail.com   *
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

#ifndef _LIB_CONFIG_H_
#define _LIB_CONFIG_H_

#include <qglobal.h>

/*!
    \macro LIBEXPORT
    \brief Macro needed for cross-platform shared libraries creation
*/
#ifdef LIBEXPORT
#error LIBEXPORT already defined...
#endif

#ifdef Q_WS_WIN
#   ifdef _LIB_BUILD_
#       define LIBEXPORT Q_DECL_EXPORT
#   else
#       define LIBEXPORT Q_DECL_IMPORT
#   endif
#else
#   define LIBEXPORT
#endif

#define str(s) #s
#define xstr(s) str(s)
#define VERSION_STRING "v" xstr(VERSION_MAJOR) "." xstr(VERSION_MINOR) "." xstr(VERSION_RELEASE)

#endif // _LIB_CONFIG_H_
