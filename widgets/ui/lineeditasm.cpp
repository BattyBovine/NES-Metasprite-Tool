#include "lineeditasm.h"

LineEditASM::LineEditASM(QWidget *parent) : QLineEdit(parent)
{
}

LineEditASM::~LineEditASM()
{
}



void LineEditASM::keyPressEvent(QKeyEvent *e)
{
	int key = e->key();
	if((key>=Qt::Key_A && key<=Qt::Key_Z) || (key>=Qt::Key_0 && key<=Qt::Key_9)) {
		QLineEdit::keyPressEvent(e);
	}
	switch(key) {
	case Qt::Key_Backspace:
	case Qt::Key_Delete:
	case Qt::Key_Left:
	case Qt::Key_Right:
	case Qt::Key_Minus:
	case Qt::Key_Underscore:
	case Qt::Key_Period:
	case Qt::Key_Home:
	case Qt::Key_End:
		QLineEdit::keyPressEvent(e);
		break;
	}
	if(key==Qt::Key_Space) {
		QKeyEvent *stou = new QKeyEvent(QKeyEvent::KeyPress,Qt::Key_Underscore,Qt::ShiftModifier,"_");
		QLineEdit::keyPressEvent(stou);
	}
}
