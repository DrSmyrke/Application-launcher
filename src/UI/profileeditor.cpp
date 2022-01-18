#include "profileeditor.h"
#include "ui_profileeditor.h"

#include <QFileDialog>

ProfileEditor::ProfileEditor(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ProfileEditor)
{
	ui->setupUi(this);

	connect( ui->appSB, &QPushButton::clicked, this, [this](){
		auto filePath = QFileDialog::getOpenFileName(this, tr("Application select"),
														m_mwd);
		if( filePath == "" ) return;
		ui->applicationLineEdit->setText( filePath );

		if( ui->workDirectoryLineEdit->text().isEmpty() ){
			QFileInfo fi;
			fi.setFile( filePath );
			auto targetDir = fi.absoluteDir().absolutePath();
			ui->workDirectoryLineEdit->setText( targetDir );
			m_mwd = targetDir;
		}
	} );

	connect( ui->wdSB, &QPushButton::clicked, this, [this](){
		auto targetDir = QFileDialog::getExistingDirectory(this, tr("Work directory"),
														   m_mwd,
														   QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
		if( targetDir == "" ) return;
		ui->workDirectoryLineEdit->setText( targetDir );
		m_mwd = targetDir;
	} );
	connect( ui->targetSB, &QPushButton::clicked, this, [this](){
		auto targetDir = QFileDialog::getExistingDirectory(this, tr("Target directory"),
														   m_mwd,
														   QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
		if( targetDir == "" ) return;
		ui->targetBox->setText( targetDir );
	} );

	reset();
}

ProfileEditor::~ProfileEditor()
{
	delete ui;
}

void ProfileEditor::reset()
{
	m_mwd = QDir::homePath();
	ui->addressBox->clear();
	ui->targetBox->clear();
	ui->keyBox->clear();
	ui->NameLineEdit->clear();
	ui->workDirectoryLineEdit->clear();
	ui->applicationLineEdit->clear();
	ui->argsLineEdit->clear();
}
