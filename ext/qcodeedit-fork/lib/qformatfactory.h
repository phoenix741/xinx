/****************************************************************************
**
** Copyright (C) 2006-2008 fullmetalcoder <fullmetalcoder@hotmail.fr>
**
** This file is part of the Edyuk project <http://edyuk.org>
** 
** This file may be used under the terms of the GNU General Public License
** version 3 as published by the Free Software Foundation and appearing in the
** file GPL.txt included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef _QFORMAT_FACTORY_H_
#define _QFORMAT_FACTORY_H_

/*!
	\file qformatfactory.h
	\brief Definition of the QFormatFactory class.
	
	\see QFormatFactory
*/

/*!
	\defgroup language Language framework
*/

#include "qce-config.h"

#include <QVector>
#include <QObject>

struct QFormat;
class QString;
class QStringList;

class QCE_EXPORT QFormatFactory : public QObject
{
	Q_OBJECT
	
	public:
		QFormatFactory(const QString& f, QObject *p = 0);
		virtual ~QFormatFactory();
		
		QStringList formats() const;
		
		virtual QString id(int ifid) const;
		virtual int id(const QString& sfid) const;
		
		virtual QFormat format(int ifid) const;
		virtual QFormat format(const QString& sfid) const;
	
	public slots:
		virtual void setFormat(const QString& fid, const QFormat& fmt);
		
	private:
		QString m_settings;
		
		QVector<QString> m_formatKeys;
		QVector<QFormat> m_formatValues;
};

#endif // !_QFORMAT_FACTORY_H_
