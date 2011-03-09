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
#ifndef _CUSTOMSCHEMEIMPL_H_
#define _CUSTOMSCHEMEIMPL_H_

// Xinx header
#include <components-config.h>

// Qt header
#include <QWidget>

class CustomSchemeImplPrivate;
class QCodeEdit;
class QFormatScheme;
class QFormat;
class QLanguageFactory;

class COMPONENTSEXPORT CustomSchemeImpl : public QWidget
{
	Q_OBJECT
	Q_CLASSINFO("Author", "Ulrich Van Den Hekke")
	Q_CLASSINFO("URL", "http://xinx.shadoware.org")
	Q_CLASSINFO("Licence", "GPL v3 or later")

	Q_PROPERTY(QString example READ example WRITE setExample)
	Q_PROPERTY(QString languageDefinition READ languageDefinition WRITE setLanguageDefinition)
	Q_PROPERTY(QStringList hiddenFormat READ hiddenFormat WRITE setHiddenFormat)
public:
	CustomSchemeImpl(QWidget * parent = 0, Qt::WindowFlags f = 0);
	virtual ~CustomSchemeImpl();

	void setFormatScheme(QFormatScheme * formats);
	QFormatScheme * formatScheme() const;

	void setExample(const QString & value);
	const QString & example() const;

	void setLanguageDefinition(const QString & value);
	QString languageDefinition() const;

	void setLanguageFactory(QLanguageFactory * value);
	QLanguageFactory * languageFactory() const;

	void setHiddenFormat(const QStringList & value);
	const QStringList & hiddenFormat() const;
private:
    QScopedPointer<CustomSchemeImplPrivate> d;
};

#endif /* _CUSTOMSCHEMEIMPL_H_ */
