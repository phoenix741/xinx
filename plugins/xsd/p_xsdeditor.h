/***************************************************************************
 *   Copyright (C) 2008 by Ulrich Van Den Hekke                            *
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

#ifndef _P_XSDEDITOR_H_
#define _P_XSDEDITOR_H_

// Xinx header
#include "xsdeditor.h"

// Qt header
#include <QGraphicsTextItem>

class XsdGraphicsLine;

/* XsdGraphicsAnnotationItem */

class XsdGraphicsAnnotationItem : public QGraphicsTextItem
{
public:
	XsdGraphicsAnnotationItem(const QString & text, QGraphicsItem * parent = 0);
public slots:
	void updatePosition();
};

/* XsdGraphicsNodeItem */

class XsdNodeItem
{
public:
	virtual ~XsdNodeItem();
protected:
	virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value);
private:
	QList<XsdGraphicsLine*> m_line;
	friend class XsdGraphicsLine;
};

/* XsdGraphicsLine */

class XsdGraphicsLine : public QGraphicsPathItem
{
public:
	XsdGraphicsLine(XsdNodeItem * startItem, XsdNodeItem * endItem);
	virtual ~XsdGraphicsLine();

	XsdNodeItem *startItem() const
	{
		return m_startItem;
	};
	XsdNodeItem *endItem() const
	{
		return m_endItem;
	};

	virtual QRectF boundingRect() const;
public slots:
	void updatePosition();
private:
	QPainterPath m_path;
	XsdNodeItem *m_startItem;
	XsdNodeItem *m_endItem;
};

/* XsdGraphicsElementItem */

class XsdGraphicsElementItem : public QGraphicsItem, public XsdNodeItem
{
public:
	XsdGraphicsElementItem(const QString & name, const QString & type, int minOccurs = 1, int maxOccurs = 1, QGraphicsItem * parent = 0);

	QRectF boundingRect() const;
	virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * = 0);

	const QFont & font() const;
	void setFont(const QFont & font);
protected:
	virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
	{
		return XsdNodeItem::itemChange(change, value);
	};
private:
	QFont m_font;
	QString m_name, m_type;
	int m_minOccurs, m_maxOccurs, m_spacing;
};

/* XsdGraphicsSequenceItem */

class XsdGraphicsSequenceItem : public QGraphicsPolygonItem, public XsdNodeItem
{
public:
	XsdGraphicsSequenceItem(QGraphicsItem * parent = 0);

	void addItem(QGraphicsItem * item);

	virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * = 0);
public slots:
	void updatePosition();
protected:
	virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
	{
		return XsdNodeItem::itemChange(change, value);
	};
private:
	qreal m_vspacing, m_hspacing;
};

/* XsdGraphicsChoiceItem */

class XsdGraphicsChoiceItem : public XsdGraphicsSequenceItem
{
public:
	XsdGraphicsChoiceItem(QGraphicsItem * parent = 0);

	virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * = 0);
};

/* XsdGraphicsComplexeType */

class XsdGraphicsComplexeType : public QGraphicsRectItem, public XsdNodeItem
{
public:
	XsdGraphicsComplexeType(const QString & name);

	const QString & name() const;

	void addItem(QGraphicsItem * item);

	virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * = 0);
public slots:
	void updatePosition();
protected:
	virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
	{
		return XsdNodeItem::itemChange(change, value);
	};
private:
	QString m_name;
	int m_spacing;
};

#endif /* _P_XSDEDITOR_H_ */
