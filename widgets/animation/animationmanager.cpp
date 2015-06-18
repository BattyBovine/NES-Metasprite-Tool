#include "animationmanager.h"

AnimationManager::AnimationManager(QWidget *parent) : QGraphicsView(parent)
{
	this->gsAnimation = new QGraphicsScene(this);
	this->setScene(this->gsAnimation);
	this->iScale = AM_DEFAULT_ZOOM;
	this->iAnimation = 0;
	this->iSelectedFrame = this->iPlayingFrame = 0;
	this->iFrameTiming = AnimationManager::NTSC;
	this->isPlaying = false;

	this->setSceneRect(-AM_CANVAS_SIZE*AM_DEFAULT_ZOOM,
					   -AM_CANVAS_SIZE*AM_DEFAULT_ZOOM,
					   AM_CANVAS_SIZE*AM_DEFAULT_ZOOM*2,
					   AM_CANVAS_SIZE*AM_DEFAULT_ZOOM*2);
	this->centerOn(0,0);

	this->alAnimations = AnimationList(256);

	connect(&this->tFrameCounter,SIGNAL(timeout()),this,SLOT(playNextFrame()));
}

AnimationManager::~AnimationManager()
{
	this->gsAnimation->clear();
	delete this->gsAnimation;
}



void AnimationManager::dropEvent(QDropEvent *e)
{
	e->acceptProposedAction();
	this->openAnimationFile(e->mimeData()->urls()[0].toLocalFile());
}

void AnimationManager::mousePressEvent(QMouseEvent *e)
{
	switch(e->button()) {
	case Qt::MiddleButton:
		this->iMouseTranslateX = e->x();
		this->iMouseTranslateY = e->y();
		break;
	default:
		QGraphicsView::mousePressEvent(e);
	}
}

void AnimationManager::mouseMoveEvent(QMouseEvent *e)
{
	QGraphicsView::mouseMoveEvent(e);

	if(e->buttons()&Qt::MiddleButton) {
		this->setTransformationAnchor(QGraphicsView::NoAnchor);
		this->translate((e->x()-this->iMouseTranslateX),(e->y()-this->iMouseTranslateY));
		this->iMouseTranslateX = e->x();
		this->iMouseTranslateY = e->y();
	}
}

void AnimationManager::mouseDoubleClickEvent(QMouseEvent *e)
{
	if(e->buttons()&Qt::MiddleButton) {
		this->iScale = AM_DEFAULT_ZOOM;
		this->centerOn(0,0);
		this->updateCurrentFrame();
	} else {
		QGraphicsView::mouseDoubleClickEvent(e);
	}
}

void AnimationManager::wheelEvent(QWheelEvent *e)
{
	qreal steps = (((qreal)e->angleDelta().y()/8)/15)/4;
	if(((this->iScale+steps)>=1) && ((this->iScale+steps)<=AM_MAX_ZOOM)) {
		this->iScale += steps;
	} else {
		this->iScale = ((steps<0)?1:AM_MAX_ZOOM);
	}

	this->setSceneRect(-AM_CANVAS_SIZE*this->iScale,
					   -AM_CANVAS_SIZE*this->iScale,
					   AM_CANVAS_SIZE*this->iScale*2,
					   AM_CANVAS_SIZE*this->iScale*2);

	this->updateCurrentFrame();
}



void AnimationManager::setAnimationFrame(int f)
{
	AnimationFrameItem frame(f,this->alAnimations[this->iAnimation][this->iSelectedFrame].delay());
	this->alAnimations[this->iAnimation].replace(this->iSelectedFrame,frame);
}

void AnimationManager::setAnimationFrameDelay(int d)
{
	AnimationFrameItem frame(this->alAnimations[this->iAnimation][this->iSelectedFrame].frame(),d);
	this->alAnimations[this->iAnimation].replace(this->iSelectedFrame,frame);
}

void AnimationManager::setBackgroundColour(PaletteVector c)
{
	this->gsAnimation->setBackgroundBrush(QBrush(QColor(c.at(0))));
}



void AnimationManager::getNewAnimation(AnimationFrameList l)
{
	this->alAnimations[this->iAnimation].setFrames(l);
	this->playNextFrame(false);
}

void AnimationManager::setNewAnimation(int f)
{
	this->gsAnimation->clear();
	this->iAnimation = f;
	this->setSelectedFrame(0);
	emit(this->loopChanged(this->alAnimations[this->iAnimation].loop()));
	emit(this->labelChanged(this->alAnimations[this->iAnimation].label()));

	this->updateList(this->iSelectedFrame);
	this->playNextFrame(false);
}

void AnimationManager::setSelectedFrame(int f)
{
	if(f>=0 && f<this->alAnimations[this->iAnimation].size()) {
		this->iSelectedFrame = f;
		this->updateCurrentFrame();
	}
}

void AnimationManager::setPlayingFrame(int f)
{
	this->iPlayingFrame = f;
	this->updateCurrentFrame();
}

void AnimationManager::updateCurrentFrame()
{
	quint8 changedframe;
	changedframe = this->iPlayingFrame;

	if(!this->alAnimations[this->iAnimation].isEmpty() && changedframe < this->alAnimations[this->iAnimation].size()) {
		emit(this->requestFrameData(this->alAnimations[this->iAnimation][changedframe].frame(),this->iScale));
	}
}



void AnimationManager::playAnimationToggle(bool p)
{
	if(p) {
		this->playAnimation();
	} else {
		this->stopAnimation();
	}
}

void AnimationManager::playAnimation()
{
	if(!this->isPlaying) {
		if(this->alAnimations[this->iAnimation].isEmpty())  return;
		this->isPlaying = true;
		this->setPlayingFrame(0);
		this->playNextFrame(false);
//        emit(this->animationStarted());
	}
}

void AnimationManager::playNextFrame(bool inc)
{
	if(this->alAnimations[this->iAnimation].isEmpty())  return;
	if(inc) this->iPlayingFrame++;
	if(this->iPlayingFrame>=this->alAnimations[this->iAnimation].size()) {
		this->iPlayingFrame=0;
		if(!this->alAnimations[this->iAnimation].loop()) {
			this->stopAnimation();
			return;
		}
	}
	this->setPlayingFrame(this->iPlayingFrame);
	this->tFrameCounter.start(qRound((qreal(this->alAnimations[this->iAnimation][this->iPlayingFrame].delay())/qreal(this->iFrameTiming))*1000));
}

void AnimationManager::stopAnimation()
{
	if(this->isPlaying) {
		this->tFrameCounter.stop();
//        emit(this->animationStopped());
		this->isPlaying = false;
	}
	this->setPlayingFrame(this->iSelectedFrame);
}



void AnimationManager::drawAnimationFrameData(MetaspriteTileList f)
{
	this->gsAnimation->clear();
	foreach(MetaspriteTileItem *i, f) {
		this->gsAnimation->addItem(i);
	}
}

void AnimationManager::updateList(int s)
{
	this->gsAnimation->clear();
	emit(this->sendUpdatedFrames(this->alAnimations[this->iAnimation].frames(),s));
}



void AnimationManager::openAnimationFile(QString filename)
{
	QFile file(filename);
	if(!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
		QMessageBox::warning(this,tr(MSM_FILE_OPEN_ERROR_TITLE),tr(MSM_FILE_OPEN_ERROR_BODY),QMessageBox::NoButton);
		return;
	}

	QString baselabel;
	QStringList animlabels;
	QByteArrayList animdata;

	while(!file.atEnd()) {
		QString line = file.readLine();
		QRegularExpression matchpattern("^(.*?)anim_(hi|lo|len(gth)?):$");
		QRegularExpressionMatch matcher = matchpattern.match(line);
		if(matcher.hasMatch()) {
			if(baselabel.isEmpty())
				baselabel = matcher.captured(1);
			continue;
		}

		matchpattern.setPattern(baselabel+"(.*):$");
		matcher = matchpattern.match(line);
		if(matcher.hasMatch()) {
			animlabels.append(matcher.captured(1));
			while(!file.atEnd()) {
				line = file.readLine();
				matchpattern.setPattern(",?\\$([0-9a-fA-F]+)");
				QRegularExpressionMatchIterator bytesiter = matchpattern.globalMatch(line);
				if(!bytesiter.isValid())    continue;
				QByteArray animbytes;
				while(bytesiter.hasNext()) {
					matcher = bytesiter.next();
					animbytes.append(quint8(matcher.captured(1).toUInt(NULL,16)));
				}
				animdata.append(animbytes);
				break;
			}
		}
	}
	file.close();

	for(int i=0; i<animlabels.size()&&i<animdata.size(); i++) {
		AnimationPackage ap;
		ap.setLabel(animlabels[i]);
		QByteArray::iterator biniter = animdata[i].begin();

		AnimationFrameList afl;
		bool loop = true;
		while(biniter<animdata[i].end()) {
			quint8 frame = *(biniter++);
			quint8 delay = *(biniter++);
			if(delay==0) {
				loop = false;
				delay = 1;
			}
			afl.append(AnimationFrameItem(frame,delay));
		}
		ap.setFrames(afl);
		ap.setLoop(loop);

		this->alAnimations.replace(i,ap);
	}

	this->setNewAnimation(this->iAnimation);
}

QString AnimationManager::createAnimationASMData(QString labelprefix)
{
	QString animdatatable_hi = labelprefix+"anim_hi:\n\t.byte ";
	QString animdatatable_lo = labelprefix+"anim_lo:\n\t.byte ";
	QString animdatatable_len = labelprefix+"anim_length:\n\t.byte ";
	QString animdatabytes;

	quint8 counter = 0;
	bool loopanimation;
	foreach(AnimationPackage ap, this->alAnimations) {
		if(ap.isEmpty() && ap.label().isEmpty())    continue;
		loopanimation = ap.loop();

		QString animlabel = labelprefix+(ap.label().isEmpty()?(QString("unnamed")+QString::number(counter++)):ap.label());
		animdatatable_hi += QString(">").append(animlabel).append(",");
		animdatatable_lo += QString("<").append(animlabel).append(",");
		animdatatable_len += QString("$%1").arg(ap.size(),2,16,QChar('0')).toUpper().append(",");
		if(!ap.isEmpty()) {
			animdatabytes += "\n";
			animdatabytes += animlabel.append(":\n\t.byte ");
			for(int i=0; i<ap.size(); i++) {
				AnimationFrameItem afi = ap.frames()[i];
				animdatabytes += QString("$%1").arg(afi.frame(),2,16,QChar('0')).toUpper().append(",");
				animdatabytes += QString("$%1").arg(((i>=ap.size()-1)&&!loopanimation)?0:afi.delay(),2,16,QChar('0')).toUpper();
				if(i<ap.size()-1)   animdatabytes.append(",");
			}
		}
	}

	animdatatable_hi.remove(animdatatable_hi.size()-1,1);
	animdatatable_lo.remove(animdatatable_lo.size()-1,1);
	animdatatable_len.remove(animdatatable_len.size()-1,1);
	animdatatable_hi += "\n";
	animdatatable_lo += "\n";
	animdatatable_len += "\n";
	animdatabytes += "\n";

	return animdatatable_hi+animdatatable_lo+animdatatable_len+animdatabytes;
}

void AnimationManager::clearAllAnimationData()
{
	this->gsAnimation->clear();
	for(int i=0; i<this->alAnimations.size(); i++) {
		this->alAnimations.replace(i,AnimationPackage());
	}
	this->updateList();
}
