/*
 XINX
 Copyright (C) 2007-2010 by Ulrich Van Den Hekke
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

#ifndef GCEPARSER_H
#define GCEPARSER_H

#include <jobs/xinxjob.h>
#include "gceconfiguration.h"
#include <QSharedPointer>

class GceParser : public XinxJob
{
	Q_OBJECT
public:
	GceParser();
	GceParser(const QString & filename);
	virtual ~GceParser();

	void setInterface(QSharedPointer<GceConfiguration> interface);
	QSharedPointer<GceConfiguration> interface() const;

	virtual void startJob() = 0;

	virtual QString description() const;
signals:
	void addDictionary(const QString & filename);
	void addConfiguration(const QString & filename);

protected:
	QSharedPointer<GceConfiguration> _interface;
	QString _filename;
};

#endif // GCEPARSER_H
