#include "proxysettings.h"
#include "ui_proxysettings.h"
#include <QApplication>

ProxySettings::ProxySettings(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ProxySettings)
{
	ui->setupUi(this);

	connect( ui->typeBox, QOverload<int>::of(&QComboBox::highlighted), this, &ProxySettings::slot_typeChange );
	connect( ui->cancelButton, &QPushButton::clicked, this, &ProxySettings::reject );
	connect( ui->applyButton, &QPushButton::clicked, this, &ProxySettings::slot_apply );

	m_proxy.setType(QNetworkProxy::NoProxy);
}

ProxySettings::~ProxySettings()
{
	delete ui;
}

void ProxySettings::slot_typeChange(const int index)
{
	switch( index ){
		case 0:		m_proxy.setType(QNetworkProxy::DefaultProxy);		break;
		case 1:		m_proxy.setType(QNetworkProxy::Socks5Proxy);		break;
		case 2:		m_proxy.setType(QNetworkProxy::NoProxy);			break;
		case 3:		m_proxy.setType(QNetworkProxy::HttpProxy);			break;
		case 4:		m_proxy.setType(QNetworkProxy::HttpCachingProxy);	break;
		case 5:		m_proxy.setType(QNetworkProxy::FtpCachingProxy);	break;
		default:	m_proxy.setType(QNetworkProxy::NoProxy);			break;
	}
}

void ProxySettings::slot_apply()
{
	auto addr = ui->addrBox->text();
	auto port = ui->portBox->value();
	auto login = ui->usernameBox->text();
	auto pass = ui->passwordBox->text();

	if( addr.isEmpty() ){
		emit reject();
	}

	m_proxy.setPort( port );
	m_proxy.setUser( login );
	m_proxy.setPassword( pass );
	QNetworkProxy::setApplicationProxy( m_proxy );

	emit accept();
}
