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

#include "xinxjobdelayedexception.h"

/**
 * \class XinxJobDelayedException
 * \since 0.10.2
 * \brief This exception is throw when the job must be delayed.
 *
 * This exception must be throw when the job want delay it's process. The job will
 * be re-added in the list, of the next process.
 */

XinxJobDelayedException::XinxJobDelayedException() : XinxException("Job must be delayed")
{
}
