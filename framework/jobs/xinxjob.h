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

#ifndef XINXJOBS_H
#define XINXJOBS_H

#include <core/lib-config.h>

#include <QObject>
#include <QRunnable>


class LIBEXPORT XinxJob : public QObject, public QRunnable
{
	Q_OBJECT
public:
	XinxJob();
	virtual ~XinxJob();

	virtual QString description() const = 0;

	bool isRunning();
	static int countRunningJob();
signals:
	void setProgress(int value, int max);
	void jobStarting();
	void jobEnding();
protected:
	virtual void startJob() = 0;
	virtual void run();

	static QAtomicInt _count_job;
	bool _is_running;
};

#endif // XINXJOBS_H
