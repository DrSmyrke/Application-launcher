#ifndef GLOBAL_H
#define GLOBAL_H

#include <QSettings>
#include <QString>
#include <list>

struct Profile
{
	QString name;
	QString repo;
	QString target;
	QString key;
	QString app;
	QString wd;
	QString args;
};

struct Config{
	QString version;
	std::list< Profile > profiles;
};

namespace app {
	extern Config conf;

	bool parsArgs(int argc, char *argv[]);
	void setLog(const uint8_t logLevel, const QString &mess);
	void loadSettings(const QString &file = "");
	void loadSettings(QSettings &settings);
	void saveSettings(const QString &file = "");
	void saveSettings(QSettings &settings);

	bool profileExists(const QString& profileName);
	void addProfile(const Profile& profile);
	const Profile* getProfile(const QString& profileName);
	void removeProfile(const QString& profileName);
	bool upProfile(const QString& profileName);
}

#endif // GLOBAL_H
