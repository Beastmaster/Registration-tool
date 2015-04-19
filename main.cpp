#include "MainWindow.h"
//
#include <QApplication>
#include <QMainWindow>

//support for Chinese
#include <QTextCodec>


int main( int argc, char** argv )
{
	QTextCodec *codec=QTextCodec::codecForName("GBK");
	QTextCodec::setCodecForLocale(codec);   
	QTextCodec::setCodecForTr(codec);   
	QTextCodec::setCodecForCStrings(codec);  

	QApplication Reg_main_app(argc,argv);

	MainWindow main_window;

	main_window.show();

	return Reg_main_app.exec();
}












