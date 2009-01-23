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

#ifndef XINXFORMATFACTORY_H_
#define XINXFORMATFACTORY_H_

// Qt header
#include <QHash>

// QCodeEdit header
#include <qformatscheme.h>

class XINXConfig;
class XinxFormatScheme;

/*!
 * This object is a list of format scheme by highlighter
 */
class XinxFormatFactory : public QObject {
	Q_OBJECT
public:
	//! Destroy the format factory
	virtual ~XinxFormatFactory();

	XinxFormatScheme * scheme( const QString & highlighter );
public slots:
	//! Update the list of format (can't delete existing format)
	void updateFormats();
	//! Put format to config
	void putFormats();
protected:
	/*!
	 * Create a XinxFormatFactory. This object is created by a XINXConfig file only.
	 */
	XinxFormatFactory( XINXConfig * parent );
private:
	QHash<QString,XinxFormatScheme*> m_formatScheme;

	XINXConfig * m_config;
	friend class XINXConfig;
};

#endif /* XINXFORMATFACTORY_H_ */
