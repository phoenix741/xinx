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

#ifndef __XSLMODELCOMPLETER_H__
#define __XSLMODELCOMPLETER_H__

// Xinx header
#include "xsllistview.h"

// Qt header
#include <QAbstractListModel>
#include <QStringList>

class XSLCompletionModel : public QAbstractListModel {
	Q_OBJECT
public:
	enum CompletionTag { NoTags = 0x0, Html = 0x1, Xsl = 0x2 };
	Q_DECLARE_FLAGS( CompletionTags, CompletionTag )

	enum CompletionRole { Name = 0x32, isVariable = 0x33, isHtmlOnly = 0x34 };

	XSLCompletionModel( CompletionTags tags, XslContentElementList * list, QObject * parent = 0 );
	virtual ~XSLCompletionModel();

	QVariant data( const QModelIndex &index, int role ) const;
	Qt::ItemFlags flags( const QModelIndex &index ) const;
	int rowCount( const QModelIndex &parent = QModelIndex() ) const;

	void setFilter( QString baliseName = QString(), QString attributeName = QString() );
	void setHiddenAttribute( const QStringList & attributes );
	void setTemplateName( const QString & templateMatchName, const QString & mode = QString() );
	void setApplyTemplateMatch( const QString & match );
protected slots:
	virtual void beginInsertRows( int row );
	virtual void endInsertRows();
	virtual void beginRemoveRows( int row );
	virtual void endRemoveRows();
	virtual void reset();

private:
	XslContentElementList * m_list;

	QString m_applyTemplateMatch;
	QString m_baliseName, m_attributeName;
	QStringList m_attributes;
	CompletionTags m_tags;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( XSLCompletionModel::CompletionTags )

#endif // __XSLMODELCOMPLETER_H__
