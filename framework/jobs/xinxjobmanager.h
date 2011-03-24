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

#ifndef XINXJOBMANAGER_H
#define XINXJOBMANAGER_H

#include <core/lib-config.h>
#include <core/xinxsingleton.h>

class XinxJob;
class PrivateXinxJobManager;

class LIBEXPORT XinxJobManager : public XinxLibSingleton<XinxJobManager>
{
	Q_OBJECT
public:
	virtual ~XinxJobManager();

	void addJob(XinxJob * job);

	int countRunningJob();
	int countTotalJob();
	QStringList descriptions() const;
signals:
	void progressRangeChanged(int minimum, int maximum);
	void progressValueChanged(int value);

	void jobCanceled(XinxJob * job);
	void jobAdded(XinxJob * job);
	void jobStarted(XinxJob * job);
	void jobEnded(XinxJob * job);
	void allJobEnded();
private slots:
	void slotJobStarting();
	void slotJobEnding();
private:
	XinxJobManager();
	PrivateXinxJobManager * d;
	friend class PrivateXinxJobManager;
	friend class XinxLibSingleton<XinxJobManager>;
};

#endif // XINXJOBMANAGER_H
