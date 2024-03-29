#include "global.h"

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
//				if(QString(argv[i]) == "-t") app::conf.targetDir = QString(argv[++i]);
//				if(QString(argv[i]) == "-r") app::conf.repository = QString(argv[++i]);
//				if(QString(argv[i]) == "-k") app::conf.key = QString(argv[++i]);
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

	void loadSettings(const QString &file)
	{
		app::conf.profiles.clear();

		if( !file.isEmpty() ){
			QSettings settings( file, QSettings::IniFormat );
			app::loadSettings( settings );
		}else{
			QSettings settings( "MySoft", "AppLauncher" );
			app::loadSettings( settings );
		}

//		if( settings.allKeys().size() == 0 ){
//			app::saveSettings();
//			return;
//		}

//		app::conf.repository = settings.value("MAIN/repository", app::conf.repository).toString();
//		app::conf.targetDir = settings.value("MAIN/targetDir", app::conf.targetDir).toString();
//		app::conf.key = settings.value("MAIN/key", app::conf.key).toString();
	}

	void loadSettings(QSettings &settings)
	{
		QStringList list;

		for( const auto &key:settings.allKeys() ){

			auto tmp = key.split( "/" );
			if( tmp.size() != 2 ) continue;
			if( list.indexOf( tmp[ 0 ] ) != -1 ) continue;
			list.push_back( tmp[ 0 ] );
		}

		for( const auto &key:list ){
			settings.beginGroup( key );
			Profile profile;

			profile.name	= key;
			profile.app		= settings.value( "app", "" ).toString();
			profile.args	= settings.value( "args", "" ).toString();
			profile.key		= settings.value( "key", "" ).toString();
			profile.repo	= settings.value( "repository", "" ).toString();
			profile.target	= settings.value( "target", "" ).toString();
			profile.wd		= settings.value( "workdirectory", "" ).toString();

			app::conf.profiles.push_back( profile );
			settings.endGroup();
		}
	}

	void saveSettings(const QString &file)
	{
		if( !file.isEmpty() ){
			QSettings settings( file, QSettings::IniFormat );
			app::saveSettings( settings );
		}else{
			QSettings settings( "MySoft", "AppLauncher" );
			settings.clear();
			app::saveSettings( settings );
		}
	}

	void saveSettings(QSettings &settings)
	{
		for( const auto &profile:app::conf.profiles ){
			settings.beginGroup( profile.name );
			settings.setValue( "repository", profile.repo );
			settings.setValue( "key", profile.key );
			settings.setValue( "target", profile.target );
			settings.setValue( "app", profile.app );
			settings.setValue( "workdirectory", profile.wd );
			settings.setValue( "args", profile.args );
			settings.endGroup();
		}

		settings.sync();
	}

	bool profileExists(const QString &profileName)
	{
		bool res = false;

		for( const auto &profile:app::conf.profiles){
			if( profile.name == profileName ){
				res = true;
				break;
			}
		}

		return res;
	}

	void addProfile(const Profile &profile)
	{
		if( profileExists( profile.name ) ) return;

		app::conf.profiles.push_back( profile );
	}

	const Profile *getProfile(const QString &profileName)
	{
		const Profile *ret = nullptr;

		for( const auto &profile:app::conf.profiles){
			if( profile.name == profileName ){
				ret = &profile;
				break;
			}
		}

		return ret;
	}

	void removeProfile(const QString &profileName)
	{
		for( auto i = app::conf.profiles.begin(); i != app::conf.profiles.end(); i++ ){
			if( (*i).name == profileName ){
				app::conf.profiles.erase( i );
				break;
			}
		}
	}

	bool upProfile(const QString& profileName)
	{
		Profile *prevProfile = nullptr;
		bool ret = false;
		Profile temp;

		for( auto &profile:app::conf.profiles){
			if( profile.name == profileName && prevProfile != nullptr ){
				temp.name			= prevProfile->name;
				temp.repo			= prevProfile->repo;
				temp.target			= prevProfile->target;
				temp.key			= prevProfile->key;
				temp.app			= prevProfile->app;
				temp.wd				= prevProfile->wd;
				temp.args			= prevProfile->args;

				prevProfile->name	= profile.name;
				prevProfile->repo	= profile.repo;
				prevProfile->target	= profile.target;
				prevProfile->key	= profile.key;
				prevProfile->app	= profile.app;
				prevProfile->wd		= profile.wd;
				prevProfile->args	= profile.args;

				profile.name		= temp.name;
				profile.repo		= temp.repo;
				profile.target		= temp.target;
				profile.key			= temp.key;
				profile.app			= temp.app;
				profile.wd			= temp.wd;
				profile.args		= temp.args;
				ret = true;
				break;
			}
			prevProfile = &profile;
		}
		return ret;
	}

}
