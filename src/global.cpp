#include "global.h"

#include <QSettings>

namespace app {
	Config conf;

	bool parsArgs(int argc, char *argv[])
	{
		bool ret = true;
		for(int i=0;i<argc;i++){
			if(QString(argv[i]).indexOf("-")==0){
				if(QString(argv[i]) == "--help" or QString(argv[1]) == "-h"){
					printf("Usage: %s [OPTIONS]\n"
							"  -t <PATH>	target dir\n"
						   "  -r <URL>	repository url\n"
						   "  -k <KEY>	repository\n"
							"\n", argv[0]);
					ret = false;
				}
				if(QString(argv[i]) == "-t") app::conf.targetDir = QString(argv[++i]);
				if(QString(argv[i]) == "-r") app::conf.repository = QString(argv[++i]);
				if(QString(argv[i]) == "-k") app::conf.key = QString(argv[++i]);
			}
		}
		return ret;
	}

	void setLog(const uint8_t logLevel, const QString &mess)
	{
		Q_UNUSED( logLevel )
		Q_UNUSED( mess )
		return;
	}

	void loadSettings()
	{
		QSettings settings( "MySoft", "AppLauncher" );

//		if( settings.allKeys().size() == 0 ){
//			app::saveSettings();
//			return;
//		}

//		app::conf.repository = settings.value("MAIN/repository", app::conf.repository).toString();
//		app::conf.targetDir = settings.value("MAIN/targetDir", app::conf.targetDir).toString();
//		app::conf.key = settings.value("MAIN/key", app::conf.key).toString();
	}

	void saveSettings()
	{
		QSettings settings( "MySoft", "AppLauncher" );
		settings.clear();

//		settings.setValue("MAIN/repository", app::conf.repository);
//		settings.setValue("MAIN/targetDir", app::conf.targetDir);
//		settings.setValue("MAIN/key", app::conf.key);
	}

}
