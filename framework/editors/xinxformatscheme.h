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

#ifndef XINXFORMATSCHEME_H_
#define XINXFORMATSCHEME_H_
#pragma once

// Xinx header
#include <core/lib-config.h>

// QCodeEdit header
#include <qformatscheme.h>

class XINXConfig;

/*!
 * This object base create standard format.
 */
class LIBEXPORT XinxFormatScheme : public QFormatScheme
{
	Q_OBJECT
public:
	/*!
	 * Create a XinxFormatScheme.
	 */
	XinxFormatScheme(XINXConfig * parent);

	/*!
	 * Create a XinxFormatScheme from nothing
	 */
	XinxFormatScheme();

	//! Destroy the format factory
	virtual ~XinxFormatScheme();

	//! Update the list of format (can't delete existing format)
	void updateFormatsFromConfig();
	void putFormatsToConfig();

	//! Copy value from a format scheme to another
	XinxFormatScheme& operator=(const XinxFormatScheme& p);
protected:
	//! Create standards formats
	void createStandardFormat();

	void setNameSpace(const QString & value);
	const QString & nameSpace() const;
private:
	XINXConfig * m_config;
	QString m_nameSpace;
};

#endif /* XINXFORMATSCHEME_H_ */
