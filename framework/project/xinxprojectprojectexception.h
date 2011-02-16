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

#pragma once
#ifndef XINXPROJECTPROJECTEXCEPTION_H
#define XINXPROJECTPROJECTEXCEPTION_H

// Xinx header
#include <core/lib-config.h>
#include <core/exceptions.h>
#include <session/session.h>

// Qt header
#include <QString>
#include <QStringList>
#include <QVariant>

namespace XinxProject
{

/* Classes */

/*!
 * Exception throw by XinxProject when the project can't be opened or saved.
 * The exception contains the message send to the user.
 *
 * The exception is used in internal for the session. If session file can't be
 * read, the exception is catched and the application continue. The project is
 * opened.
 */
class LIBEXPORT ProjectException : public XinxException
{
public:
	/*!
	 * Constructor of the exception.
	 * \param message Message for the user.
	 * \param wizard Show a wizard to migrate the project
	 */
	explicit ProjectException(const QString & message, bool wizard = false);

	//! True if XINX must start "xinxprojectwizard"
	bool startWizard() const;
private:
	bool m_wizard;
};


} // namespace XinxProject

#endif // XINXPROJECTPROJECTEXCEPTION_H
