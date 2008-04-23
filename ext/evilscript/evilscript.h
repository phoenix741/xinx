#ifndef EVILSCRIPT_H
#define EVILSCRIPT_H

#include <QtGui/QDialog>
#include "ui_evilscript.h"

class EvilScript : public QDialog {
    Q_OBJECT
public:
	EvilScript(QWidget *parent = 0);
    ~EvilScript();
private slots:
	void on_pushButton_clicked();
	void on_textEdit_textChanged();
private:
    Ui::EvilScriptClass ui;
};

#endif // EVILSCRIPT_H
