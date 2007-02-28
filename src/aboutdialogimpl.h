#ifndef ABOUTDIALOGIMPL_H
#define ABOUTDIALOGIMPL_H
//
#include "ui_about.h"
//
class AboutDialogImpl : public QDialog, public Ui::AboutDialog {
Q_OBJECT
public:
	AboutDialogImpl( QWidget * parent = 0, Qt::WFlags f = 0 );
private slots:
};
#endif





