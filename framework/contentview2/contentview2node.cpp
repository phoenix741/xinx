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
#include "contentview2node.h"

// Qt header
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QSqlRecord>
#include <QHash>

namespace ContentView2 {

/* NodeException */

NodeException::NodeException( QString message, QString nodeName ) : XinxException( QString( message ).arg( nodeName ) ), m_nodeName( nodeName ) {
}

const QString & NodeException::getNodeName() const {
	return m_nodeName;
}

/* PrivateNode */

class PrivateNode : public QSharedData {
public:
	int m_line;
	QString m_filename;
	QHash<int,QVariant> m_datas;

	long m_id;

	PrivateNode();
	PrivateNode( const PrivateNode &other );
	~PrivateNode();
};

PrivateNode::PrivateNode() : m_line( -1 ), m_id( -1 ) {
}

PrivateNode::PrivateNode( const PrivateNode &other ) : QSharedData( other ), m_line( other.m_line ), m_filename( other.m_filename ), m_datas( other.m_datas ), m_id( other.m_id ) {
}

PrivateNode::~PrivateNode() {

}

/* Node */

Node::Node() {
	d = new PrivateNode;
}

Node::Node( const Node & other ) : d( other.d ) {
}

Node::Node( QSqlDatabase db, uint id ) {
	d = new PrivateNode;
	load( db, id );
}

Node::~Node() {
}

void Node::load( QSqlDatabase db, uint id ) {
	QSqlQuery selectQuery(	"SELECT name, type, icon, display_name, tips, line, file_id, hash "
							"FROM cv_node WHERE id=:id", db );
	selectQuery.bindValue( ":id", id );
	bool result = selectQuery.exec();
	Q_ASSERT_X( result, "Node::load", qPrintable( selectQuery.lastError().text() ) );
	if( ! selectQuery.first() ) {
		throw NodeException( tr("Can't find the node %2 : %1").arg( selectQuery.lastError().text() ), d->m_datas.value( Node::NODE_NAME ).toString() );
	}

	d->m_datas.clear();

	d->m_datas.insert( Node::NODE_NAME, selectQuery.value( 0 ).toString() );
	d->m_datas.insert( Node::NODE_TYPE, selectQuery.value( 1 ).toString() );
	d->m_datas.insert( Node::NODE_ICON, selectQuery.value( 2 ).toString() );
	d->m_datas.insert( Node::NODE_DISPLAY_NAME, selectQuery.value( 3 ).toString() );
	d->m_datas.insert( Node::NODE_DISPLAY_TIPS, selectQuery.value( 4 ).toString() );
	d->m_line = selectQuery.value( 5 ).toInt();

	QSqlQuery selectPropertiesQuery( "SELECT ord, value FROM cv_node_property WHERE node_id=:node_id", db );
	selectPropertiesQuery.bindValue( ":node_id", id );
	result = selectPropertiesQuery.exec();
	Q_ASSERT_X( result, "Node::load", qPrintable( selectPropertiesQuery.lastError().text() ) );

	while( selectPropertiesQuery.next() ) {
		d->m_datas.insert( selectPropertiesQuery.value( 0 ).toInt(), selectPropertiesQuery.value( 1 ).toString() );
	}

	QSqlQuery pathQuery(	"SELECT path FROM cv_file WHERE id=:id", db );
	pathQuery.bindValue( ":id", selectQuery.value( 6 ).toInt() );
	result = pathQuery.exec();
	Q_ASSERT_X( result, "Node::load", qPrintable( pathQuery.lastError().text() ) );

	if( ! pathQuery.first() ) {
		throw NodeException( tr("The node %2 hasn't file : %1").arg( pathQuery.lastError().text() ), d->m_datas.value( Node::NODE_NAME ).toString() );
	}

	d->m_filename = pathQuery.value( 0 ).toString();
	d->m_id = id;
}

uint Node::create( QSqlDatabase db, int forcedId ) {
	QSqlQuery selectQuery(	"SELECT id FROM cv_file WHERE path=:path", db );
	selectQuery.bindValue( ":path", d->m_filename );

	bool result = selectQuery.exec();
	Q_ASSERT_X( result, "Node::create", qPrintable( selectQuery.lastError().text() ) );

	if( ! selectQuery.first() ) {
		throw NodeException( tr("Can't find the node %2 : %1").arg( selectQuery.lastError().text() ), d->m_datas.value( Node::NODE_NAME ).toString() );
	}

	uint file_id = selectQuery.value( 0 ).toUInt();

	QSqlQuery insertQuery( db );
	if( forcedId == -1 ) {
		insertQuery.prepare(	"INSERT INTO cv_node(name, type, icon, display_name, tips, line, file_id, hash) "
								"VALUES(:name, :type, :icon, :display_name, :tips, :line, :file_id, :hash)" );
	} else {
		insertQuery.prepare(	"INSERT INTO cv_node(id, name, type, icon, display_name, tips, line, file_id, hash) "
								"VALUES(:forced_id, :name, :type, :icon, :display_name, :tips, :line, :file_id, :hash)" );
		insertQuery.bindValue( ":forced_id", forcedId );
	}

	insertQuery.bindValue( ":name", d->m_datas.value( Node::NODE_NAME ) );
	insertQuery.bindValue( ":type", d->m_datas.value( Node::NODE_TYPE ) );
	insertQuery.bindValue( ":icon", d->m_datas.value( Node::NODE_ICON ) );
	insertQuery.bindValue( ":display_name", d->m_datas.value( Node::NODE_DISPLAY_NAME ) );
	insertQuery.bindValue( ":tips", d->m_datas.value( Node::NODE_DISPLAY_TIPS ) );
	insertQuery.bindValue( ":line", d->m_line );
	insertQuery.bindValue( ":file_id", file_id );
	insertQuery.bindValue( ":hash", qHash( *this ) );

	result = insertQuery.exec();
	Q_ASSERT_X( result, "Node::load", qPrintable( insertQuery.lastError().text() ) );

	uint newId = insertQuery.lastInsertId().toInt();

	QSqlQuery insertPropertyQuery(	"INSERT INTO cv_node_property(node_id, ord, value) "
									"VALUES(:node_id, :ord, :value)", db );

	insertPropertyQuery.bindValue( ":node_id", newId );
	foreach( const int ord, d->m_datas.keys() ) {
		if( ord < Node::NODE_USER_VALUE ) continue;

		insertPropertyQuery.bindValue( ":ord", ord );
		insertPropertyQuery.bindValue( ":value", d->m_datas.value( ord ) );

		result = insertPropertyQuery.exec();
		Q_ASSERT_X( result, "Node::load", qPrintable( insertPropertyQuery.lastError().text() ) );
	}

	d->m_id = newId;
	return newId;
}

void Node::update( QSqlDatabase db ) {
	QSqlQuery select( "SELECT hash FROM cv_node WHERE id=:id", db );
	select.bindValue( ":id", QVariant::fromValue( d->m_id ) );
	bool result = select.exec();
	Q_ASSERT_X( result, "Node::update", qPrintable( select.lastError().text() ) );

	if( ! select.next() ) {
		throw NodeException( tr("Can't find the node %2 : %1").arg( select.lastError().text() ), d->m_datas.value( Node::NODE_NAME ).toString() );
	}

	uint hash    = select.value( 0 ).toUInt();
	uint newHash = qHash( *this );

	if( hash == newHash ) return;

	QSqlQuery updateQuery(	"UPDATE cv_node "
							"SET	name=:name , "
							"		type=:type , "
							"		icon=:icon , "
							"		display_name=:display_name , "
							"		tips=:tips , "
							"		line=:line , "
							"		hash=:hash "
							"WHERE id=:id", db );
	updateQuery.bindValue( ":name", d->m_datas.value( Node::NODE_NAME ) );
	updateQuery.bindValue( ":type", d->m_datas.value( Node::NODE_TYPE ) );
	updateQuery.bindValue( ":icon", d->m_datas.value( Node::NODE_ICON ) );
	updateQuery.bindValue( ":display_name", d->m_datas.value( Node::NODE_DISPLAY_NAME ) );
	updateQuery.bindValue( ":tips", d->m_datas.value( Node::NODE_DISPLAY_TIPS ) );
	updateQuery.bindValue( ":line", d->m_line );
	updateQuery.bindValue( ":hash", newHash );
	updateQuery.bindValue( ":id", QVariant::fromValue( d->m_id ) );

	result = updateQuery.exec();
	Q_ASSERT_X( result, "Node::update", qPrintable( updateQuery.lastError().text() ) );

	QSqlQuery deleteQuery( "DELETE FROM cv_node_property WHERE node_id=:id", db );
	deleteQuery.bindValue( ":id", QVariant::fromValue( d->m_id ) );
	result = deleteQuery.exec();
	Q_ASSERT_X( result, "Node::update", qPrintable( deleteQuery.lastError().text() ) );

	QSqlQuery insertPropertyQuery(	"INSERT INTO cv_node_property(node_id, ord, value) "
									"VALUES(:node_id, :ord, :value)", db );

	insertPropertyQuery.bindValue( ":node_id", QVariant::fromValue( d->m_id ) );
	foreach( const int ord, d->m_datas.keys() ) {
		if( ord < Node::NODE_USER_VALUE ) continue;

		insertPropertyQuery.bindValue( ":ord", ord );
		insertPropertyQuery.bindValue( ":value", d->m_datas.value( ord ) );

		result = insertPropertyQuery.exec();
		Q_ASSERT_X( result, "Node::update", qPrintable( insertPropertyQuery.lastError().text() ) );
	}
}

void Node::destroy( QSqlDatabase db ) {
	clear( db );

	QSqlQuery deleteQuery1( "DELETE FROM cv_node WHERE id=:id", db );
	deleteQuery1.bindValue( ":id", QVariant::fromValue( d->m_id ) );
	bool result = deleteQuery1.exec();
	Q_ASSERT_X( result, "Node::destroy", qPrintable( deleteQuery1.lastError().text() ) );

	QSqlQuery deleteQuery2( "DELETE FROM cv_node_property WHERE node_id=:id", db );
	deleteQuery2.bindValue( ":id", QVariant::fromValue( d->m_id ) );
	result = deleteQuery2.exec();
	Q_ASSERT_X( result, "Node::destroy", qPrintable( deleteQuery2.lastError().text() ) );

	QSqlQuery deleteQuery3( "DELETE FROM cv_link WHERE parent_id=:id1 or child_id=:id2", db );
	deleteQuery3.bindValue( ":id1", QVariant::fromValue( d->m_id ) );
	deleteQuery3.bindValue( ":id2", QVariant::fromValue( d->m_id ) );
	result = deleteQuery3.exec();
	Q_ASSERT_X( result, "Node::destroy", qPrintable( deleteQuery3.lastError().text() ) );

	d->m_id = -1;
}

uint Node::nodeId() const {
	return d->m_id;
}

bool Node::attach( QSqlDatabase db, uint parentId ) {
	Q_ASSERT( d->m_id );

	QSqlQuery checkId( "SELECT 1 FROM cv_link WHERE parent_id=:parent AND child_id=:child", db );
	checkId.bindValue( ":parent", QVariant::fromValue( parentId ) );
	checkId.bindValue( ":child", QVariant::fromValue( d->m_id ) );
	bool result = checkId.exec();
	Q_ASSERT_X( result, "Node::attach", qPrintable( checkId.lastError().text() ) );

	if( checkId.next() )
		return false;

	QSqlQuery insert( "INSERT INTO cv_link(parent_id, child_id) VALUES( :parent, :child )", db );
	insert.bindValue( ":parent", QVariant::fromValue( parentId ) );
	insert.bindValue( ":child", QVariant::fromValue( d->m_id ) );
	result = insert.exec();
	Q_ASSERT_X( result, "Node::attach", qPrintable( insert.lastError().text() ) );

	return true;
}

void Node::detach( QSqlDatabase db, uint parentId ) {
	Q_ASSERT( d->m_id );

	QSqlQuery remove( "DELETE FROM cv_link WHERE parent_id=:parent AND child_id=:child", db );
	remove.bindValue( ":parent", QVariant::fromValue( parentId ) );
	remove.bindValue( ":child", QVariant::fromValue( d->m_id ) );
	bool result = remove.exec();
	Q_ASSERT_X( result, "Node::attach", qPrintable( remove.lastError().text() ) );
}

void Node::clear( QSqlDatabase db ) {
	foreach( int id, childs( db ) ) {
		Node child( db, id );
		child.destroy( db );
	}
}

int Node::line() const {
	return d->m_line;
}

void Node::setLine( int value ) {
	d->m_line = value;
}

const QString & Node::fileName() const {
	return d->m_filename;
}

void Node::setFileName( const QString & value ) {
	d->m_filename = value;
}

QVariant Node::data( int index ) const {
	return d->m_datas.value( index );
}

void Node::setData( const QVariant & value, int index ) {
	if( d->m_datas.value( index ) != value ) {
		d->m_datas.insert( index, value );
	}
}

QList<int> Node::childs( QSqlDatabase db ) const {
	QList<int> result;

	QSqlQuery select( "SELECT child_id FROM cv_link WHERE parent_id=:parent_id", db );
	select.bindValue( ":parent_id", QVariant::fromValue( d->m_id ) );
	bool r = select.exec();
	Q_ASSERT_X( r, "Node::childs", qPrintable( select.lastError().text() ) );

	while( select.next() ) {
		result += select.value( 0 ).toInt();
	}

	return result;
}

QList<int> Node::parents( QSqlDatabase db ) const {
	QList<int> result;

	QSqlQuery select( "SELECT parent_id FROM cv_link WHERE child_id=:child_id", db );
	select.bindValue( ":child_id", QVariant::fromValue( d->m_id ) );
	bool r = select.exec();
	Q_ASSERT_X( r, "Node::parents", qPrintable( select.lastError().text() ) );

	while( select.next() ) {
		result += select.value( 0 ).toInt();
	}

	return result;
}

int Node::indexOfChild( QSqlDatabase db, const Node & node ) const {
	QSqlQuery select( "SELECT child_id FROM cv_link, cv_node WHERE cv_link.parent_id=:parent_id and cv_node.id=cv_link.child_id and cv_node.hash=:hash", db );
	select.bindValue( ":parent_id", QVariant::fromValue( d->m_id ) );
	select.bindValue( ":hash", QVariant::fromValue( qHash( node ) ) );
	bool result = select.exec();
	Q_ASSERT_X( result, "Node::attach", qPrintable( select.lastError().text() ) );

	if( select.next() ) {
		return select.value( 0 ).toUInt();
	} else {
		return -1;
	}
}

bool Node::isValid() const {
	return d->m_id >= 0;
}

Node & Node::operator=( const Node & node ) {
	this->d = node.d;

	return *this;
}

/* qHash */

uint qHash( const Node & node ) {
	uint hash = qHash( node.d->m_filename );
	hash += node.d->m_line;
	foreach( const QVariant & data, node.d->m_datas.values() ) {
		hash += qHash( data.toString() );
	}
	return hash;
}

} // namespace ContentView2
