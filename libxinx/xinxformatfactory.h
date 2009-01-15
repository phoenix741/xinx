/***************************************************************************
 *   Copyright (C) 2008 by Ulrich Van Den Hekke                            *
 *   ulrich.vdh@shadoware.org                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
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

#ifndef XINXFORMATFACTORY_H_
#define XINXFORMATFACTORY_H_

// QCodeEdit header
#include <qformatscheme.h>

class XINXConfig;

/*!
 * The goal of this object is to retrieve the format from the
 */
class XinxFormatFactory : public QFormatScheme {
	Q_OBJECT
public:
	//! Destroy the format factory
	virtual ~XinxFormatFactory();
public slots:
	//! Update the list of format (can't delete existing format)
	void updateFormats();
protected:
	/*!
	 * Create a XinxFormatFactory. The object is initialize with the file :/qcodeedit/formats.qxf if exist.
	 * This object is created by a XINXConfig file only.
	 */
	XinxFormatFactory( XINXConfig * parent );
private:
	XINXConfig * m_config;
	friend class XINXConfig;
};

#endif /* XINXFORMATFACTORY_H_ */
