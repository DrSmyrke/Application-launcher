#include "mainwindow.h"
#include "version.h"
#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	app::conf.version = QString("%1.%2").arg(APP_VER_FIRST).arg(APP_VER_SECOND);

	auto localeName = QLocale::system().name();
	QTranslator translator(&a);
	if(translator.load(localeName,"://lang/")) a.installTranslator(&translator);

	app::loadSettings();
	if( !app::parsArgs(argc, argv) ) return 0;

	MainWindow w;
	w.show();

	return a.exec();
}
