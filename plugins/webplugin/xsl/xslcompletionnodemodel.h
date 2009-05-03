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

#ifndef _XSLCOMPLETIONNODEMODEL_H_
#define _XSLCOMPLETIONNODEMODEL_H_

// Xinx header
#include <contentview/completionnodemodel.h>

// Qt header
#include <QStringList>
#include <QMultiHash>

class ContentViewNode;

/*!
 * \class XslCompletionNodeModel
 * \brief Model used to complete XSL Stylesheet.
 *
 * This class is used to complete stylesheet.
 */
class XslCompletionNodeModel : public CompletionNodeModel {
	Q_OBJECT
public:
	enum CompletionTag { NoTags = 0x0, Html = 0x1, Xsl = 0x2 };
	Q_DECLARE_FLAGS( CompletionTags, CompletionTag )

	enum CompletionRole { isVariable = CompletionNodeModel::CompletionNodeName + 1, isHtmlOnly = CompletionNodeModel::CompletionNodeName + 2 };

	XslCompletionNodeModel( ContentViewNode * root, QObject *parent = 0 );
	virtual ~XslCompletionNodeModel();

	QVariant data( const QModelIndex &index, int role ) const;

	void setCurrentTemplateName( const QString & name, const QString & mode );

	void setCompleteTags( CompletionTags value );
	void setCompleteNone();
	void setCompleteNode();
	void setCompleteAttribute( const QString & baliseName, const QStringList & hiddenAttributeList );
	void setCompleteValue( const QString & baliseName, const QString & attributeName, const QString & applyTemplateMatch );

	QStringList modes( QString templateName ) const;
	QStringList params( QString templateName ) const;
  ContentViewNode * nodeOfWord( const QString & name ) const;
protected:
	virtual void addNode( ContentViewNode * node );
	virtual void removeNode( ContentViewNode * node );
	virtual bool mustElementBeShowed( ContentViewNode * node );

	virtual void timerEvent( QTimerEvent * event );
private:
	enum CompletionMode { COMPLETION_NONE_MODE, COMPLETION_NODE_MODE, COMPLETION_ATTRIBUTE_MODE, COMPLETION_VALUE_MODE };

	CompletionTags m_completeTags;
	enum CompletionMode m_completionMode;
	QString m_baliseName, m_attributeName, m_applyTemplateMatch, m_currentTemplateName, m_currentTemplateMode;
	ContentViewNode * m_baliseNode, * m_attributeNode;
	QStringList m_showedAttributeList;
	QMultiHash<QString,QString> m_modes, m_params;
};

#endif /* _XSLCOMPLETIONNODEMODEL_H_ */
