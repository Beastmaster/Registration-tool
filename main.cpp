#include "MainWindow.h"
//
#include <QApplication>
#include <QMainWindow>

//support for Chinese
#include <QTextCodec>
#include <QtGlobal>  //check qt version

int main( int argc, char** argv )
{
	QTextCodec *codec=QTextCodec::codecForName("GBK");

	//check qt version
#if QT_VERSION < 0x050000
	QTextCodec::setCodecForLocale(codec);   
	QTextCodec::setCodecForTr(codec);   
	QTextCodec::setCodecForCStrings(codec);  
#else

#endif

	QApplication Reg_main_app(argc,argv);

	MainWindow main_window;

	main_window.show();

	return Reg_main_app.exec();
}












