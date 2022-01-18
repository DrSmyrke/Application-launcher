#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFile>
#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include "global.h"
#include "myfunctions.h"
#include "UI/proxysettings.h"
#include "UI/profileeditor.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	struct State
	{
		enum{
			downloadList,
			decryptingList,
			checkingFS,
			downloadUpdates,
			finished,
		};
	};
	struct UpdateFileData
	{
		QString localFile;
		QString remoteFile;
	};
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
public slots:
	void slot_update();
private slots:
	void slot_downloadProgress(const qint64 bytesReceived, const qint64 bytesTotal);
	void slot_readyRead();
	void slot_finished();
	void slot_run();
	void slot_updateIndex();
	void slot_newProfile();
private:
	Ui::MainWindow *ui;
	QTimer* m_pTimer;
	QFile m_file;
	QList< UpdateFileData > m_downloadList;
	QStringList m_updateList;
	uint8_t m_state;
	bool m_working;
	bool m_updatingF;
	QNetworkAccessManager* m_pManager;
	QNetworkReply* m_pReply;
	QByteArray m_buff;
	QString m_applicationPath;
	QString m_repoListFile;
	QString m_repoURL;
	ProxySettings* m_pPorxySettingsWindow;
	ProfileEditor* m_pProfileEditor;
	QString m_profile;

	void startDownload(const QUrl &url, const QString &fileName);
	QByteArray encryptData(const QByteArray &data, const QByteArray &key);
	void decryptList();
	void checkingFileSystem();
	void backSpace();
	void downloadUpdates();
	void drawProgress(const uint32_t filesLeft);
	void addToUpdate(const QString &localFile, const QString &remoteFile);

	void updateProfiles();
};

#endif // MAINWINDOW_H
