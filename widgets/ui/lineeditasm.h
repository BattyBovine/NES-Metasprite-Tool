#ifndef LINEEDITASM_H
#define LINEEDITASM_H

#include <QLineEdit>
#include <QKeyEvent>

class LineEditASM : public QLineEdit
{
	Q_OBJECT

public:
	LineEditASM(QWidget *parent = 0);
	~LineEditASM();

protected:
	void keyPressEvent(QKeyEvent*);
};

#endif // LINEEDITASM_H
