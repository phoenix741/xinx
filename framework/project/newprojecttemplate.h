/* *********************************************************************** *
 * XINX                                                                    *
 * Copyright (C) 2007-2010 by Ulrich Van Den Hekke                         *
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

#ifndef NEWPROJECTTEMPLATE_H
#define NEWPROJECTTEMPLATE_H
#pragma once

// Xinx header
#include <core/lib-config.h>
#include <core/exceptions.h>

// Qt header
#include <QApplication>
#include <QList>
#include <QString>
#include <QStringList>
#include <QPair>

class LIBEXPORT NewProjectTemplateException : public XinxException
{
public:
	NewProjectTemplateException(const QString & message);
};

class LIBEXPORT NewProjectTemplate
{
	Q_DECLARE_TR_FUNCTIONS(NewProjectTemplate)
public:
	typedef QPair<QString,QString> Property;

	NewProjectTemplate(const QString & filename);

	const QStringList & plugins() const;
	const QString & projectName() const;
	const QStringList & wizardPages() const;
	const QList<Property> & properties() const;
private:
	QString m_projectName;
	QStringList m_wizardPages, m_plugins;
	QList<Property> m_properties;
};

#endif // NEWPROJECTTEMPLATE_H
