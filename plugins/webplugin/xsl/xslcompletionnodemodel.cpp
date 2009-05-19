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

// Xinx header
#include "xslcompletionnodemodel.h"
#include "xslcontentviewparser.h"
#include <contentview/contentviewnode.h>

// Qt header
#include <QMutexLocker>

/* XslCompletionNodeModel */

XslCompletionNodeModel::XslCompletionNodeModel( ContentViewNode * root, QObject *parent ) : CompletionNodeModel( root ? root : XmlCompletionParser::self()->rootNode(), parent ), m_completionMode( COMPLETION_NONE_MODE ), m_baliseNode( 0 ), m_attributeNode( 0 ) {
	if( root )
		startTimer( 0 );
}

XslCompletionNodeModel::~XslCompletionNodeModel() {
	XmlCompletionParser::self()->rootNode()->removeModel( this, (unsigned long)rootNode() );
}

void XslCompletionNodeModel::timerEvent( QTimerEvent * event ) {
	killTimer( event->timerId() );

	QMutexLocker locker( mutex() );

	XmlCompletionParser::self()->rootNode()->addModel( this, (unsigned long)rootNode() );
	addAllNodes( 0, XmlCompletionParser::self()->rootNode() );
}

QVariant XslCompletionNodeModel::data( const QModelIndex &index, int role ) const {
	if( ( role == XslCompletionNodeModel::isVariable ) || ( role == XslCompletionNodeModel::isHtmlOnly ) ) {
		ContentViewNode * node = static_cast<ContentViewNode*>( index.internalPointer() );
		QString nodeType = node->data( ContentViewNode::NODE_TYPE ).toString();

		if( ( role == XslCompletionNodeModel::isVariable ) && ( ( nodeType == "XslParam" ) || ( nodeType == "XslVariable" ) ) )
			return true;
		if( ( role == XslCompletionNodeModel::isHtmlOnly ) && ( nodeType == "XmlValue" ) )
			return true;
		return false;
	} else
		return CompletionNodeModel::data( index, role );
}

QStringList XslCompletionNodeModel::modes( QString templateName ) const {
	return m_modes.values( templateName );
}

QStringList XslCompletionNodeModel::params( QString templateName ) const {
	return m_params.values( templateName );
}

ContentViewNode * XslCompletionNodeModel::nodeOfWord( const QString & name ) const {
  foreach( ContentViewNode * node, nodes() ) {
    if( node->data( ContentViewNode::NODE_NAME ).toString() == name ) {
      return node;
    }
  }
  return 0;
}

void XslCompletionNodeModel::setCompleteTags( CompletionTags value ) {
	if( value != m_completeTags ) {
		m_completeTags = value;

		if( m_completionMode == COMPLETION_NODE_MODE )
			reset();
	}
}

void XslCompletionNodeModel::setCurrentTemplateName( const QString & name, const QString & mode ) {
	if( ( m_currentTemplateName != name ) || ( m_currentTemplateMode != mode ) ) {
		m_currentTemplateName = name;
		m_currentTemplateMode = mode;

		if( m_completionMode == COMPLETION_VALUE_MODE )
			reset();
	}
}

void XslCompletionNodeModel::setCompleteNone() {
	if( m_completionMode != COMPLETION_NONE_MODE ) {
		m_completionMode = COMPLETION_NONE_MODE;
		reset();
	}
}

void XslCompletionNodeModel::setCompleteNode() {
	if( m_completionMode != COMPLETION_NODE_MODE ) {
		m_completionMode = COMPLETION_NODE_MODE;
		reset();
	}
}

void XslCompletionNodeModel::setCompleteAttribute( const QString & baliseName, const QStringList & hiddenAttributeList ) {
	QList<ContentViewNode*> attrs = XmlCompletionParser::self()->baliseAttributes( baliseName );
	QStringList showedAttributeList;

	if( ! hiddenAttributeList.isEmpty() ) {
		foreach( ContentViewNode * n, attrs ) {
			if( ! hiddenAttributeList.contains( n->data().toString() ) )
				showedAttributeList += n->data().toString();
		}
	}

	if( ( m_completionMode != COMPLETION_ATTRIBUTE_MODE ) || ( m_baliseName != baliseName ) || ( m_showedAttributeList != showedAttributeList ) ) {
		m_completionMode      = COMPLETION_ATTRIBUTE_MODE;
		m_baliseName          = baliseName;
		m_baliseNode          = XmlCompletionParser::self()->balise( baliseName );
		m_showedAttributeList = showedAttributeList;
		reset();
	}
}

void XslCompletionNodeModel::setCompleteValue( const QString & baliseName, const QString & attributeName, const QString & applyTemplateMatch ) {
	QString tempApplyTemplateMatch = applyTemplateMatch;

	if( tempApplyTemplateMatch.contains( "/" ) )
		tempApplyTemplateMatch.remove( 0, tempApplyTemplateMatch.lastIndexOf( "/" ) + 1 );

	if( ( m_completionMode != COMPLETION_VALUE_MODE ) || ( m_baliseName != baliseName ) || ( m_attributeName != attributeName ) || ( m_applyTemplateMatch != tempApplyTemplateMatch ) ) {
		m_completionMode     = COMPLETION_VALUE_MODE;
		m_baliseName         = baliseName;
		m_baliseNode         = XmlCompletionParser::self()->balise( baliseName );
		m_attributeName      = attributeName;
		m_attributeNode      = XmlCompletionParser::self()->baliseAttribute( baliseName, attributeName );
		m_applyTemplateMatch = tempApplyTemplateMatch;
		reset();
	}
}

void XslCompletionNodeModel::addNode( ContentViewNode * parentNode, ContentViewNode * n ) {
	CompletionNodeModel::addNode( parentNode, n );

	QString nodeName = n->data().toString();
	QString nodeType = n->data( ContentViewNode::NODE_TYPE ).toString();
	if( ( nodeType == "XslParam" ) && parentNode ) {
		QString parentType = parentNode->data( ContentViewNode::NODE_TYPE ).toString();
		QString parentName = parentNode->data().toString();

    if( ( parentType == "XslTemplate" ) && ( ! m_params.contains( parentName, nodeName ) ) )
			m_params.insert( parentName, nodeName );
	}

	if( nodeType == "XslTemplate" ) {
		QString nodeMode = n->data( ContentViewNode::NODE_USER_VALUE ).toString();

		if( ( ! nodeMode.isEmpty() ) && ( ! m_modes.values( nodeName ).contains( nodeMode ) ) )
			m_modes.insert( nodeName, nodeMode );
	}
}

void XslCompletionNodeModel::removeNode( ContentViewNode * parent, ContentViewNode * n ) {
	CompletionNodeModel::removeNode( parent, n );

	QString nodeName = n->data().toString();
	QString nodeType = n->data( ContentViewNode::NODE_TYPE ).toString();
	if( nodeType == "XslTemplate" ) {
		QString nodeMode = n->data( ContentViewNode::NODE_USER_VALUE ).toString();

		if( ! nodeMode.isEmpty() )
			m_modes.remove( nodeName, nodeMode );

		foreach( ContentViewNode * c, n->childs() ) {
			QString childName = c->data().toString();
			QString childType = c->data( ContentViewNode::NODE_TYPE ).toString();
			if( childType == "XslParam" )
				m_params.remove( nodeName, childName );
		}
	}
}

bool XslCompletionNodeModel::mustElementBeShowed( ContentViewNode * n ) {
	Q_ASSERT( n );
	if( m_completionMode == COMPLETION_NONE_MODE ) {
		return false;
	} else
	if( m_completionMode == COMPLETION_NODE_MODE ) {
		if( n->data( ContentViewNode::NODE_TYPE ).toString() == "XmlBalise" ) {
			if( m_completeTags.testFlag( XslCompletionNodeModel::Xsl ) && ( n->data( XmlCompletionParser::NODE_XML_TYPE ).toString() == "stylesheet" ) ) {
				return true;
			}
			if( m_completeTags.testFlag( XslCompletionNodeModel::Html ) && ( n->data( XmlCompletionParser::NODE_XML_TYPE ).toString() == "html" ) ) {
				return true;
			}
			if( m_completeTags.testFlag( XslCompletionNodeModel::NoTags ) ) {
				return true;
			}

			return false;
		}
		return false;
	} else
	if( m_completionMode == COMPLETION_ATTRIBUTE_MODE ) {
		ContentViewNode * p = parent( n );
		if( ( p == m_baliseNode ) && ( n->data( ContentViewNode::NODE_TYPE ).toString() == "XmlAttribute" ) && ( m_showedAttributeList.isEmpty() || m_showedAttributeList.contains( n->data().toString() ) ) ) {
			return true;
		}
		return false;
	} else
	if( m_completionMode == COMPLETION_VALUE_MODE ) {
		ContentViewNode * parentAttribute = parent( n );
		ContentViewNode * parentBalise = parentAttribute ? parent( parentAttribute ) : 0;
		if( ( parentBalise == m_baliseNode ) && ( parentAttribute == m_attributeNode ) && n->data( ContentViewNode::NODE_TYPE ).toString() == "XmlValue" ) {
			return true;
		}
		if( ( n->data( ContentViewNode::NODE_TYPE ).toString() == "XslParam" ) || ( n->data( ContentViewNode::NODE_TYPE ).toString() == "XslVariable" )|| ( n->data( ContentViewNode::NODE_TYPE ).toString() == "XslVariable" ) ) {
			ContentViewNode * p = parent( n );
			QString parentType = p ? p->data( ContentViewNode::NODE_TYPE ).toString() : QString();

			// Variable global
			if( parentType.isEmpty() || ( parentType == "include" ) ) return true;

			if( ( p->data().toString() == m_currentTemplateName ) && m_currentTemplateMode.isEmpty() ) return true;
			if( ( p->data().toString() == m_currentTemplateName ) && ( m_currentTemplateMode == n->data( ContentViewNode::NODE_USER_VALUE ).toString() ) ) return true;
			return false;
		}
		if( n->data( ContentViewNode::NODE_TYPE ).toString() == "XslTemplate" ) return true;
		return false;
	}
	return false;
}
