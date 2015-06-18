#ifndef ANIMATIONMANAGER_H
#define ANIMATIONMANAGER_H

#include <QWidget>
#include <QGraphicsView>
#include <QMimeData>
#include <QDragMoveEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDragLeaveEvent>
#include <QMouseEvent>
#include <QWheelEvent>

#include <QFile>
#include <QByteArray>
#include <QTimer>

#include <QtMath>

#include "palettemanager.h"
#include "metaspritemanager.h"
#include "animationframeitem.h"


#define AM_CANVAS_SIZE      192
#define AM_DEFAULT_ZOOM     3
#define AM_MAX_ZOOM         8


class AnimationPackage {
public:
	AnimationPackage(){bLoop = true;}

	QString label(){return sLabel;}
	void setLabel(QString s){sLabel=s;}
	AnimationFrameList frames(){return aflFrames;}
	void setFrames(AnimationFrameList f){aflFrames=f;}
	bool loop(){return bLoop;}
	void setLoop(bool l){bLoop=l;}

	AnimationFrameItem operator[](int i){return aflFrames[i];}
	bool isEmpty(){return aflFrames.isEmpty();}
	int size(){return aflFrames.size();}
	int length(){return aflFrames.length();}
	int count(){return aflFrames.count();}
	void append(AnimationFrameItem l){aflFrames.append(l);}
	void insert(int i, AnimationFrameItem l){aflFrames.insert(i,l);}
	void replace(int i, AnimationFrameItem l){aflFrames.replace(i,l);}
	AnimationFrameItem takeAt(int i){return aflFrames.takeAt(i);}
	void remove(int i){aflFrames.remove(i);}

private:
	QString sLabel;
	AnimationFrameList aflFrames;
	bool bLoop;
};
typedef QVector<AnimationPackage> AnimationList;



class AnimationManager : public QGraphicsView
{
	Q_OBJECT
public:
	explicit AnimationManager(QWidget *parent = 0);
	~AnimationManager();

	QString createAnimationASMData(QString);
	void openAnimationFile(QString);
	void clearAllAnimationData();

	enum {PAL=50,NTSC=60};

signals:
	void loopChanged(bool);
	void labelChanged(QString);
	void requestFrameData(quint8,qreal);
	void sendUpdatedFrames(AnimationFrameList,int);

//    void animationStarted();
//    void animationStopped();

public slots:
	void setTiming(bool pal){this->iFrameTiming=pal?AnimationManager::PAL:AnimationManager::NTSC;}
	void setAnimationLabel(QString l){this->alAnimations[this->iAnimation].setLabel(l);}
	void setAnimationLoop(bool l){this->alAnimations[this->iAnimation].setLoop(l);}

	void getNewAnimation(AnimationFrameList);
	void setAnimationFrame(int);
	void setAnimationFrameDelay(int);
	void setBackgroundColour(PaletteVector);

	void setNewAnimation(int);
	void setSelectedFrame(int);
	void setPlayingFrame(int);
	void updateCurrentFrame();

	void playAnimationToggle(bool);

	void drawAnimationFrameData(MetaspriteTileList);

protected:
	void dragMoveEvent(QDragMoveEvent*e){e->accept();}
	void dragEnterEvent(QDragEnterEvent*e){e->acceptProposedAction();}
	void dropEvent(QDropEvent*);
	void dragLeaveEvent(QDragLeaveEvent*e){e->accept();}
	void mousePressEvent(QMouseEvent*);
	void mouseMoveEvent(QMouseEvent*);
	void mouseDoubleClickEvent(QMouseEvent*);
	void wheelEvent(QWheelEvent*);

private slots:
	void playAnimation();
	void playNextFrame(bool inc=true);
	void stopAnimation();

private:
	void drawAnimationFrame(quint8);
	void updateList(int s=-1);

	QGraphicsScene *gsAnimation;
	quint8 iAnimation;
	AnimationList alAnimations;
	quint8 iSelectedFrame;
	quint8 iPlayingFrame;
	bool isPlaying;

	QTimer tFrameCounter;
	int iFrameTiming;
	bool bPlayAnimation;

	qreal iScale,iMouseTranslateX,iMouseTranslateY;
};

#endif // ANIMATIONMANAGER_H
