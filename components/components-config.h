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

#ifndef _COMPONENTS_CONFIG_H_
#define _COMPONENTS_CONFIG_H_

#include <qglobal.h>

/*!
    \macro COMPONENTSEXPORT
    \brief Macro needed for cross-platform shared libraries creation
*/
#ifdef COMPONENTSEXPORT
#error COMPONENTSEXPORT already defined...
#endif

#ifdef Q_WS_WIN
#   ifdef _COMPONENTS_BUILD_
#       define COMPONENTSEXPORT Q_DECL_EXPORT
#   else
#       define COMPONENTSEXPORT Q_DECL_IMPORT
#   endif
#else
#   define COMPONENTSEXPORT
#endif

#endif // _COMPONENTS_CONFIG_H_
