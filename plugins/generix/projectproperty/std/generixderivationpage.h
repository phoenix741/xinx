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

#ifndef _GENERIXDERIVATIONPAGE_H_
#define _GENERIXDERIVATIONPAGE_H_
#pragma once

// Xinx header
#include "ui_generixderivationpage.h"
#include "projectproperty/generixproject.h"
#include <plugins/plugininterfaces.h>

// Qt header
#include <QAbstractTableModel>
#include <QItemDelegate>
#include <QPainter>
#include <QPointer>

class DerivationDirectoryEditDelegate : public QItemDelegate {
	Q_OBJECT
public:
	DerivationDirectoryEditDelegate( QObject *parent = 0 );

	QWidget *createEditor( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
	void setEditorData( QWidget *editor, const QModelIndex &index ) const;
	void setModelData( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const;
	void updateEditorGeometry( QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index ) const;

	void paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;

	void setAbsolutePath( const QString & path );
private:
	QString m_absolutePath;
};

class DerivationPathTableModel : public QAbstractTableModel {
	Q_OBJECT
public:
	DerivationPathTableModel( QObject * parent = 0 );

	void setDerivationPath(const QList<GenerixProject::DerivationPath> & values);
	const QList<GenerixProject::DerivationPath> & derivationPath() const;

	virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;
	virtual int	rowCount(const QModelIndex & parent = QModelIndex() ) const;
	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
	virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
	virtual Qt::ItemFlags flags( const QModelIndex & index ) const;

	virtual bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
	virtual bool insertRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
	virtual bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
private:
	QList<GenerixProject::DerivationPath> m_paths;
};

class GenerixDerivationPathPageImpl : public QWidget, public Ui::GenerixDerivationPathPage, public IXinxPluginProjectConfigurationPage {
	Q_OBJECT
public:
	GenerixDerivationPathPageImpl( QWidget * parent = 0, Qt::WFlags f = 0 );
	virtual ~GenerixDerivationPathPageImpl();

	virtual void setProject( XinxProject * project );

	virtual QPixmap image();
	virtual QString name();

	virtual bool loadSettingsDialog();
	virtual bool saveSettingsDialog();
	virtual bool cancelSettingsDialog();
	virtual QWidget * settingsDialog();
	virtual bool isSettingsValid();
	virtual bool isVisible();

private slots:
	void on_m_delBtn_clicked();
	void on_m_addBtn_clicked();
	void derivationLineChanged( const QModelIndex & current );

private:
	XinxProject * m_project;
	QPointer<DerivationPathTableModel> m_model;
	QPointer<DerivationDirectoryEditDelegate> m_delegate;
};

#endif // _GENERIXDERIVATIONPAGE_H_

