/*
 XINX
 Copyright (C) 2007-2011 by Ulrich Van Den Hekke
 xinx@shadoware.org

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful.
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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

#endif // _LIB_CONFIG_H_
