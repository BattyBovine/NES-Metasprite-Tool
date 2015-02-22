#include "animationmanager.h"

AnimationManager::AnimationManager(QWidget *parent) : QGraphicsView(parent)
{
    this->gsAnimation = new QGraphicsScene(this);
    this->gsAnimation->setSceneRect(-128,-128,256,256);
    this->setScene(this->gsAnimation);
    this->iAnimation = 0;
    this->iSelectedFrame = this->iPlayingFrame = 0;
    this->iFrameTiming = AnimationManager::NTSC;
    this->isPlaying = false;

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



void AnimationManager::addAnimationFrame(quint8 f, quint8 d)
{
    AnimationFrameItem frame(f,d);
    this->alAnimations[this->iAnimation].append(frame);
    this->setSelectedFrame(this->alAnimations[this->iAnimation].size()-1);

    this->updateList(this->iSelectedFrame);
}

void AnimationManager::insertAnimationFrame(quint8 f, quint8 d)
{
    AnimationFrameItem frame(f,d);
    this->alAnimations[this->iAnimation].insert(this->iSelectedFrame,frame);

    this->updateList(this->iSelectedFrame);
}

void AnimationManager::replaceAnimationFrame(quint8 f, quint8 d)
{
    AnimationFrameItem frame(f,d);
    this->alAnimations[this->iAnimation].replace(this->iSelectedFrame,frame);

    this->updateList(this->iSelectedFrame);
}

void AnimationManager::setBackgroundColour(PaletteVector c)
{
    this->gsAnimation->setBackgroundBrush(QBrush(QColor(c.at(0))));
}



void AnimationManager::setNewAnimation(int f)
{
    this->tFrameCounter.stop();
    this->gsAnimation->clear();
    this->iAnimation = f;
    this->setSelectedFrame(0);
    emit(this->loopChanged(this->alAnimations[this->iAnimation].loop()));
    emit(this->labelChanged(this->alAnimations[this->iAnimation].label()));

    this->updateList(this->iSelectedFrame);
}

void AnimationManager::setSelectedFrame(int f)
{
    this->stopAnimation();
    this->iSelectedFrame = f;
    if(this->iSelectedFrame < this->alAnimations[this->iAnimation].size()) {
        this->updateCurrentFrame();
    }
}

void AnimationManager::setPlayingFrame(int f)
{
    this->iPlayingFrame = f;
    if(!this->alAnimations[this->iAnimation].isEmpty() && this->iPlayingFrame < this->alAnimations[this->iAnimation].size()) {
        emit(this->requestFrameData(this->alAnimations[this->iAnimation][this->iPlayingFrame].frame()));
    }
}

void AnimationManager::updateCurrentFrame()
{
    if(!this->alAnimations[this->iAnimation].isEmpty() && this->iSelectedFrame < this->alAnimations[this->iAnimation].size()) {
        emit(this->requestFrameData(this->alAnimations[this->iAnimation][this->iSelectedFrame].frame()));
    }
}

void AnimationManager::moveFrameUp(int i)
{
    if(i>0) {
        AnimationFrameItem item = this->alAnimations[this->iAnimation].takeAt(i);
        this->alAnimations[this->iAnimation].insert(--i,item);
        this->updateList(i);
    }
}

void AnimationManager::moveFrameDown(int i)
{
    if(i<this->alAnimations[this->iAnimation].size()-1) {
        AnimationFrameItem item = this->alAnimations[this->iAnimation].takeAt(i);
        this->alAnimations[this->iAnimation].insert(++i,item);
        this->updateList(i);
    }
}

void AnimationManager::deleteFrame(int i)
{
    if(i<this->alAnimations[this->iAnimation].size()) {
        this->alAnimations[this->iAnimation].remove(i);
        this->updateList((i==0)?i:i-1);
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
        this->isPlaying = true;
        if(this->alAnimations[this->iAnimation].isEmpty())  return;
        this->setPlayingFrame(0);
        this->playNextFrame(false);
        emit(this->animationStarted());
    }
}

void AnimationManager::playNextFrame(bool inc)
{
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
        emit(this->animationStopped());
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

void AnimationManager::updateList(quint8 s)
{
    this->gsAnimation->clear();
    emit(this->framesUpdated(this->alAnimations[this->iAnimation].frames(),s));
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
