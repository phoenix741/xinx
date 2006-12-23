/***************************************************************************
 *   Copyright (C) 2006 by Ulrich Van Den Hekke                            *
 *   ulrich.vdh@free.fr                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
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
 
#ifndef _OBJECTVIEW_H_
#define _OBJECTVIEW_H_

#include <QObject>
#include <QHash>

class QDomElement;
  
class ObjectFields : public QObject {
  Q_OBJECT
public:
  ObjectFields(QObject * parent, const QDomElement & node);
  
  const QString & name() const;
  bool isUpdateable() const;
  bool isNotNull() const;
  bool isQueriable() const;
private:
  QString m_name;
  bool m_isUpdateable;
  bool m_isNotNull;
  bool m_isQueriable;
};
 
class ObjectView : public QObject {
  Q_OBJECT
public:
  ObjectView(QObject* parent, const QString & fileName);
  ~ObjectView();
 
  const QString & name() const;

  ObjectFields * fields(int) const;
  int count() const;
private:
  QList<ObjectFields*> m_fields;

  QString m_name;
};

class ENotViewObject {
public:
  ENotViewObject(const QString & message) {
    qDebug(message.toLatin1());
  }
};

class ObjectsView : public QObject {
	Q_OBJECT
public:
  ObjectsView(const QString & filesPath = "");
  ~ObjectsView();
  
  void loadFiles();
  
  const QString & path() const { return m_filesPath; };
  void setPath(const QString & path) { m_filesPath = path; };
  
  ObjectView * objectViews(int i) const;
  ObjectView * objectViews(const QString & name) const;
  int count() const;
signals:
  void objectLoaded(const QString&);
private:
  QHash<QString, ObjectView*> m_objectsview;
  QString m_filesPath;
};

#endif
