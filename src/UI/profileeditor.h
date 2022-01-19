#ifndef PROFILEEDITOR_H
#define PROFILEEDITOR_H

#include <QDialog>

#include "global.h"

namespace Ui {
class ProfileEditor;
}

class ProfileEditor : public QDialog
{
	Q_OBJECT

public:
	explicit ProfileEditor(QWidget *parent = nullptr);
	~ProfileEditor();
	void reset();
	bool isCorrect();
	Profile getProfile();
	void setProfile(const Profile *profile);
private:
	Ui::ProfileEditor *ui;
	QString m_mwd;
};

#endif // PROFILEEDITOR_H
