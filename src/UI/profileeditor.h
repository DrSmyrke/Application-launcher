#ifndef PROFILEEDITOR_H
#define PROFILEEDITOR_H

#include <QDialog>

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
private:
	Ui::ProfileEditor *ui;
	QString m_mwd;
};

#endif // PROFILEEDITOR_H
