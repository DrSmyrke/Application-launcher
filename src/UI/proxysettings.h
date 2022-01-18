#ifndef PROXYSETTINGS_H
#define PROXYSETTINGS_H

#include <QDialog>
#include <QNetworkProxy>

namespace Ui {
class ProxySettings;
}

class ProxySettings : public QDialog
{
	Q_OBJECT

public:
	explicit ProxySettings(QWidget *parent = 0);
	~ProxySettings();
private slots:
	void slot_typeChange(const int index);
	void slot_apply();
private:
	Ui::ProxySettings *ui;
	QNetworkProxy m_proxy;
};

#endif // PROXYSETTINGS_H
