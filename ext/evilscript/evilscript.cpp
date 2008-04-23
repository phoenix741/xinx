#include "evilscript.h"

#include <QFileDialog>
#include <QTextStream>
#include <QtScript>

EvilScript::EvilScript(QWidget *parent) : QDialog(parent) {
	ui.setupUi(this);
}

EvilScript::~EvilScript() {

}

void EvilScript::on_pushButton_clicked() {
	QString f = QFileDialog::getOpenFileName( this, tr("JavaScript") );
	if( !f.isEmpty() ) {
		QFile file( f );
		file.open( QIODevice::ReadOnly );
		QTextStream stream( &file );
		//stream.setCodec( "UTF-8" );
		
		ui.textEdit->setPlainText( stream.readAll() );
	}
}

void EvilScript::on_textEdit_textChanged() {
	ui.errorLabel->setText( "" );
	QString text = ui.textEdit->toPlainText();
	QScriptEngine engine;
//	if( engine.canEvaluate( text ) ) {
		engine.evaluate( text );
		if( engine.hasUncaughtException() )
			ui.errorLabel->setText( engine.uncaughtException().toString() );
//	} else 
//		ui.errorLabel->setText( engine.uncaughtException().toString() );
}
