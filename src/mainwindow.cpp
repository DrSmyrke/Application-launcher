#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "UI/prompt.h"

#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QProcess>
#include <QTextBlock>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	m_pTimer = new QTimer();
	m_pManager = new QNetworkAccessManager( this );
	m_pPorxySettingsWindow = new ProxySettings( this );
	m_pProfileEditor = new ProfileEditor( this );
	m_profile			= "";
	m_repoListFile		= "";
	m_repoURL			= "";
	m_repoKey			= "";
	m_targetPath		= "";
	m_replyStarted		= false;


	this->setWindowTitle( "App Launcher v" + app::conf.version );
	this->setWindowIcon( QIcon( "://index.ico" ) );


	connect( ui->updateB, &QPushButton::clicked, this, &MainWindow::slot_update );
	connect( m_pTimer, &QTimer::timeout, this, &MainWindow::slot_run );
	connect( ui->actionProxy_settings, &QAction::triggered, m_pPorxySettingsWindow, &ProxySettings::exec );
	connect( ui->actionCreate_Update_Index, &QAction::triggered, this, &MainWindow::slot_updateIndex );
	connect( ui->actionNew_profile, &QAction::triggered, this, &MainWindow::slot_newProfile );
	connect( ui->profilesBox, &QComboBox::currentTextChanged, this, &MainWindow::slot_changeProfile );
	connect( ui->actionEdit_profile, &QAction::triggered, this, &MainWindow::slot_editProfile );
	connect( ui->launchB, &QPushButton::clicked, this, &MainWindow::slot_launchApp );


	m_pTimer->setInterval( 500 );
	m_state = State::downloadList;
	m_working = false;
	m_updatingF = false;
	ui->progressBar->setValue( 0 );
	//TODO: remove?
	m_applicationPath = QCoreApplication::applicationDirPath();
	ui->statusL->setText( " " );

	updateProfiles();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::slot_downloadProgress(const qint64 bytesReceived, const qint64 bytesTotal)
{
	auto str = QString( "   Downloading [%1/%2]...\n" ).arg( mf::getSize( (uint64_t)bytesReceived ) ).arg( mf::getSize( (uint64_t)bytesTotal ) );
	ui->statusL->setText( str );
}

void MainWindow::slot_readyRead()
{
	if( m_file.isOpen() && m_file.isWritable() ){
		m_file.write( m_pReply->readAll() );
	}else{
		m_buff.append( m_pReply->readAll() );
	}
}

void MainWindow::slot_finished()
{
	if( m_file.isOpen() ){
		m_file.close();
	}

	if( m_replyStarted ){
		disconnect( m_pReply, &QNetworkReply::downloadProgress, this, &MainWindow::slot_downloadProgress );
		disconnect( m_pReply, &QNetworkReply::readyRead, this, &MainWindow::slot_readyRead );
		disconnect( m_pReply, &QNetworkReply::finished, this, &MainWindow::slot_finished );
		m_replyStarted = false;

		if( m_pReply->error() != QNetworkReply::NoError ){
			ui->statusL->setText( QString( tr( "Download error: %1 [%2]" ) ).arg( m_pReply->error() ).arg( m_pReply->errorString() ) );
			return;
		}
	}

	if( m_updatingF ){
		downloadUpdates();
		return;
	}

	m_working = false;
}

void MainWindow::slot_run()
{
	if( m_working ) return;	

	switch( m_state++ ){
		case State::downloadList:
			ui->progressBar->setValue( 20 );
			ui->launchB->setEnabled( false );
			m_updatingF = false;
			m_downloadList.clear();
			ui->logBox->insertPlainText( tr( "Updating information from " ) );
			ui->logBox->insertPlainText( m_repoURL );
			ui->logBox->insertPlainText( " ...\n" );
			startDownload( QUrl( m_repoListFile ), "" );
		break;
		case State::decryptingList:
			ui->progressBar->setValue( 40 );
			ui->logBox->insertPlainText( tr( "Decrypting information..." ) + "\n" );
			decryptList();
		break;
		case State::checkingFS:
			ui->progressBar->setValue( 60 );
			ui->logBox->insertPlainText( tr( "Checking filesystem..." ) + "\n" );
			checkingFileSystem();
		break;
		case State::downloadUpdates:
			ui->progressBar->setValue( 80 );
			ui->logBox->insertPlainText( tr( "Download updates..." ) + "\n" );
			downloadUpdates();
		break;
		case State::finished:
			ui->logBox->insertPlainText( tr( "Finished!" ) + "\n" );
			ui->progressBar->setValue( 100 );
			if( m_pTimer->isActive() ){
				m_pTimer->start();
			}
			ui->statusL->setText( " " );
			ui->updateB->setEnabled( true );
			ui->launchB->setEnabled( true );
			m_pTimer->stop();
//			ui->targetSB->setEnabled( true );
			//TODO: Maybe?
			//emit close();
		break;
		default: break;
	}

	ui->logBox->moveCursor( QTextCursor::End );
}

void MainWindow::slot_updateIndex()
{
	auto targetDir = QFileDialog::getExistingDirectory(this, tr("Repository directory"),
													QDir::homePath(),
													QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	if( targetDir == "" || !QDir( targetDir ).exists() ){
		return;
	}

	QString indexFile = QString( "%1/index.list" ).arg( targetDir );

	Prompt* prompt = new Prompt( "KEY", tr( "Enter key for encrypt data ( optional )" ), this );
	if( !prompt->exec() ){
		prompt->deleteLater();
		return;
	}

	QByteArray key = prompt->getInput().toUtf8();
	prompt->deleteLater();


	QFile file;
	file.setFileName( indexFile );
	if( file.open( QIODevice::WriteOnly ) ){
		ui->progressBar->setValue( 0 );
		auto list = mf::listDir( targetDir );
		int size = list.size();
		uint32_t i = 0;
		QByteArray ba;

		for( auto targetFile:list ){
			QString baseFile = targetFile;
			baseFile.replace( QString( "%1/" ).arg( targetDir ), "" );

			if( baseFile == "/index.list" ){
				i++;
				continue;
			}

			qint64 fileSize = QFileInfo( targetFile ).size();
			auto hash = mf::fileChecksum_MD5( targetFile );

			ba.append( hash );
			ba.append( "\t" );
			ba.append( QString::number( fileSize ).toUtf8() );
//			if( fileSize < 1000 ) ba.append( "\t" );
//			if( fileSize < 100000 ) ba.append( "\t" );
			ba.append( "\t" );
			ba.append( baseFile.toUtf8() );
			ba.append( "\n" );

			if( i == 0 ){
				i++;
				continue;
			}

			int prz = (float)i / ( (float)size / 100.0 );
			ui->progressBar->setValue( prz );

			i++;
		}

		//encrypt data
		file.write( encryptData( ba, key ) );

		file.close();
		ui->progressBar->setValue( 100 );
	}
}

void MainWindow::slot_newProfile()
{
	if( m_pProfileEditor->exec() ){
		if( m_pProfileEditor->isCorrect() ){
			app::addProfile( m_pProfileEditor->getProfile() );
			updateProfiles();
			app::saveSettings();
		}
	}
}

void MainWindow::slot_editProfile()
{
	auto profile = app::getProfile( m_profile );
	if( profile == nullptr )
		return;

	m_pProfileEditor->setProfile( profile );
	if( m_pProfileEditor->exec() ){
		if( m_pProfileEditor->isCorrect() ){
			app::removeProfile( profile->name );
			app::addProfile( m_pProfileEditor->getProfile() );
			updateProfiles();
			app::saveSettings();
		}
	}
}

void MainWindow::slot_changeProfile(const QString &profileName)
{
	m_profile = profileName;

	if( profileName != "" ){
		ui->updateB->setEnabled( true );
	}else{
		ui->updateB->setEnabled( false );
	}
}

void MainWindow::slot_launchApp()
{
	auto profile = app::getProfile( m_profile );
	if( profile == nullptr )
		return;

	if( profile->app.isEmpty() )
		return;

	if( !mf::checkFile( profile->app ) )
		return;

	bool res = mf::startDetached( profile->app, profile->args.split( " " ), profile->wd );
}

void MainWindow::startDownload(const QUrl &url, const QString &fileName)
{
	m_working = true;
	m_buff.clear();

	if( mf::checkFile( url.path() ) ){

		if( fileName.isEmpty() ){
			m_file.setFileName( url.path() );
			if( m_file.open( QIODevice::ReadOnly ) ){
				while( !m_file.atEnd() ){
					m_buff.append( m_file.readAll() );
				}
				m_file.close();
			}else{
				ui->statusL->setText( QString( tr( "Open error: [%1] %2" ) ).arg( m_file.fileName() ).arg( m_file.errorString() ) );
				return;
			}
		}else{
			bool res = QFile::copy( url.path(), fileName );
			if( !res ){
				ui->statusL->setText( QString( tr( "Copy error: [%1] -> [%2]" ) ).arg( url.path() ).arg( fileName ) );
				return;
			}
		}
		slot_finished();
		return;
	}

	if( !fileName.isEmpty() ){
		m_file.setFileName( fileName );
		m_file.open( QIODevice::WriteOnly );
	}

	m_pReply = m_pManager->get( QNetworkRequest( url ) );

	m_replyStarted = true;

	connect( m_pReply, &QNetworkReply::downloadProgress, this, &MainWindow::slot_downloadProgress );
	connect( m_pReply, &QNetworkReply::readyRead, this, &MainWindow::slot_readyRead );
	connect( m_pReply, &QNetworkReply::finished, this, &MainWindow::slot_finished );
}

QByteArray MainWindow::encryptData(const QByteArray &data, const QByteArray &key)
{
	QByteArray ba = data.toBase64();
	mf::XOR( ba, key );
	return ba;
}

void MainWindow::decryptList()
{
	m_working = true;
	ui->statusL->setText( " " );
	m_updateList.clear();
	mf::XOR( m_buff, m_repoKey.toUtf8() );
	m_updateList = QString( QByteArray::fromBase64( m_buff ) ).split( "\n" );
	m_working = false;
}

void MainWindow::checkingFileSystem()
{
	m_working = true;
	ui->statusL->setText( " " );

	int i = 0;
	int totalFiles = m_updateList.size();

	for( auto elem:m_updateList ){
		elem.replace( "\t\t", "\t" );
		elem.replace( "\t\t", "\t" );
		elem.replace( "\t\t", "\t" );
		elem.replace( "\t\t", "\t" );
		auto tmp = elem.split( "\t" );
		if( tmp.size() < 3 ) continue;

		auto checkSumm = tmp[0];
		int fileSize = tmp[1].toInt();
		auto remoteFile = tmp[2];

		QString targetFile = QString( "%1/%2" ).arg( m_targetPath ).arg( remoteFile );
		QFileInfo fi;
		fi.setFile( targetFile );
		auto targetDir = fi.absoluteDir().absolutePath();
		auto downloadFile = QString( "%1/%2" ).arg( m_repoURL ).arg( remoteFile );
		if( !QDir( targetDir ).exists() ){
			QDir().mkpath( targetDir );
		}

		auto str = QString( tr("Checking files [%1/%2] ...") + "\n" ).arg( i ).arg( totalFiles );
		ui->statusL->setText( str );
		i++;

		if( fileSize == 0 ){
			//skip file
			continue;
		}

		if( mf::checkFile( targetFile ) ){
			if( fileSize == -1 ){
				//delete file
				QFile( targetFile ).remove();
				continue;
			}

			qint64 size = QFileInfo(targetFile).size();
			if( size != fileSize ){
				QFile(targetFile).remove();
				addToUpdate( targetFile, downloadFile );
				continue;
			}
			if( checkSumm != mf::fileChecksum_MD5( targetFile ) ){
				QFile(targetFile).remove();
				addToUpdate( targetFile, downloadFile );
				continue;
			}
		}else{
			addToUpdate( targetFile, downloadFile );
		}
	}

	ui->statusL->setText( " " );
	m_working = false;
}

void MainWindow::backSpace()
{
	while( ui->logBox->textCursor().block().text().length() > 0 ){
		ui->logBox->textCursor().deletePreviousChar();
	}
}

void MainWindow::downloadUpdates()
{
	m_working = true;

	if( m_downloadList.size() == 0 ){
		m_working = false;
		m_updatingF = false;
		return;
	}

	m_updatingF = true;

	drawProgress( m_downloadList.size() - 1 );

	auto data = m_downloadList.first();
	m_downloadList.pop_front();
	startDownload( QUrl( data.remoteFile ), data.localFile );
}

void MainWindow::drawProgress(const uint32_t filesLeft)
{
	auto str = QString( tr("   Files left: %1      ") ).arg( filesLeft );
	backSpace();
	ui->logBox->insertPlainText( str );
}

void MainWindow::addToUpdate(const QString &localFile, const QString &remoteFile)
{
	UpdateFileData data;
	data.localFile = localFile;
	data.remoteFile = remoteFile;
	m_downloadList.push_back( data );
}

void MainWindow::updateProfiles()
{
	bool found = false;
	ui->updateB->setEnabled( false );
	ui->profilesBox->clear();
	for( const auto &profile:app::conf.profiles ){
		ui->profilesBox->addItem( profile.name );
		if( profile.name == m_profile ) found = true;
	}

	if( ui->profilesBox->count() > 0 && m_profile != "" && found ){
		ui->profilesBox->setCurrentText( m_profile );
	}
}

void MainWindow::slot_update()
{
	auto profile = app::getProfile( m_profile );
	if( profile == nullptr )
		return;

	if( profile->repo.isEmpty() || profile->target.isEmpty() )
		return;

	if( !QDir( profile->target ).exists() )
		QDir().mkpath( profile->target );

	if( !QDir( profile->target ).exists() ){
		ui->logBox->insertPlainText( tr( "Target dir not found" ) + "\n" );
		return;
	}

//	auto tmp = app::conf.repository.split( " " );
//	if( tmp.size() != 2 ){
//		ui->logBox->insertPlainText( tr( "Repository address invalid\n Example: [http://example.com/repo repoName]" ) + "\n" );
//		return;
//	}

	m_repoURL = QString( "%1/files" ).arg( profile->repo );
	m_repoKey = profile->key;
	m_targetPath = profile->target;
	m_repoListFile = QString( "%1/index.list" ).arg( m_repoURL );
	m_state = State::downloadList;
	ui->updateB->setEnabled( false );
////	ui->targetSB->setEnabled( false );

	m_pTimer->start();
}
