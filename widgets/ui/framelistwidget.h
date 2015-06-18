#ifndef FRAMELISTWIDGET_H
#define FRAMELISTWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QKeyEvent>

#include "animationframeitem.h"


#define FLW_DEFAULT_FRAME   0
#define FLW_DEFAULT_DELAY   1


class FrameListWidget : public QListWidget
{
	Q_OBJECT
public:
	FrameListWidget(QWidget *parent = 0);
	~FrameListWidget();

signals:
	void pushFrameUpdate(AnimationFrameList);
	void selectedFrameChanged(int);
	void selectedFrameNumberChanged(int);
	void selectedFrameDelayChanged(int);

public slots:
	void newFrameData(AnimationFrameList,int);
	void frameNumberUpdate(int);
	void frameDelayUpdate(int);

	void moveUpSelected();
	void moveDownSelected();
	void addFrame();
	void insertFrame();
	void deleteSelected();

protected:
	void keyPressEvent(QKeyEvent*);

private slots:
	void sendSelectedFrame();

private:
	void generateListDisplay(int);
	QString generateListItem(int);
	void replaceItem(int);

	AnimationFrameList aflFrames;
};

#endif // FRAMELISTWIDGET_H
