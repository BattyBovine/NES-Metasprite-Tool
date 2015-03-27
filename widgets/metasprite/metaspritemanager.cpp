#include "metaspritemanager.h"

MetaspriteManager::MetaspriteManager(QWidget *parent) : QGraphicsView(parent)
{
    this->gsMetasprite = new QGraphicsScene(this);
    this->setScene(this->gsMetasprite);
    this->iScale = MSM_DEFAULT_ZOOM;
    this->bTallSprites = false;

    this->setSceneRect(-MSM_CANVAS_SIZE*MSM_DEFAULT_ZOOM,
                       -MSM_CANVAS_SIZE*MSM_DEFAULT_ZOOM,
                       MSM_CANVAS_SIZE*MSM_DEFAULT_ZOOM*2,
                       MSM_CANVAS_SIZE*MSM_DEFAULT_ZOOM*2);
    this->centerOn(0,0);

    this->drawGridLines();

    this->vMetaspriteStages = MetaspriteStageList(256);
    this->iMetaspriteStage = 0;
}

MetaspriteManager::~MetaspriteManager()
{
    this->gsMetasprite->clear();
    delete this->gsMetasprite;
}



void MetaspriteManager::dropEvent(QDropEvent *e)
{
    e->acceptProposedAction();
    this->openMetaspriteFile(e->mimeData()->urls()[0].toLocalFile());
}

void MetaspriteManager::mousePressEvent(QMouseEvent *e)
{
    switch(e->button()) {
    case Qt::RightButton:
        emit(this->requestNewTile(this->mapToScene(e->pos())));
        break;
    case Qt::MiddleButton:
        this->iMouseTranslateX = e->x();
        this->iMouseTranslateY = e->y();
        break;
    default:
        QGraphicsView::mousePressEvent(e);
    }
}

void MetaspriteManager::mouseMoveEvent(QMouseEvent *e)
{
    QGraphicsView::mouseMoveEvent(e);

    if(e->buttons()&Qt::MiddleButton) {
        this->setTransformationAnchor(QGraphicsView::NoAnchor);
        this->translate((e->x()-this->iMouseTranslateX),(e->y()-this->iMouseTranslateY));
        this->iMouseTranslateX = e->x();
        this->iMouseTranslateY = e->y();
    } else {
        QList<QGraphicsItem*> sel = this->gsMetasprite->selectedItems();
        foreach(QGraphicsItem *i, sel) {
            qgraphicsitem_cast<MetaspriteTileItem*>(i)->setRealX(qRound(i->pos().x()/this->iScale));
            qgraphicsitem_cast<MetaspriteTileItem*>(i)->setRealY(qRound(i->pos().y()/this->iScale));
        }
    }
}

void MetaspriteManager::mouseDoubleClickEvent(QMouseEvent *e)
{
    if(e->buttons()&Qt::MiddleButton) {
        this->iScale = MSM_DEFAULT_ZOOM;
        this->centerOn(0,0);
        this->updateMetaspriteStage();
    } else {
        QGraphicsView::mouseDoubleClickEvent(e);
    }
}

void MetaspriteManager::wheelEvent(QWheelEvent *e)
{
    qreal steps = (((qreal)e->angleDelta().y()/8)/15)/4;
    if(((this->iScale+steps)>=1) && ((this->iScale+steps)<=MSM_MAX_ZOOM)) {
        this->iScale += steps;
    } else {
        this->iScale = ((steps<0)?1:MSM_MAX_ZOOM);
    }

    this->updateMetaspriteStage();
}

void MetaspriteManager::mouseReleaseEvent(QMouseEvent *e)
{
    QGraphicsView::mouseReleaseEvent(e);

    this->sendTileUpdates();
}

void MetaspriteManager::keyPressEvent(QKeyEvent *e)
{
    QList<QGraphicsItem*> sel = this->gsMetasprite->selectedItems();
    int translatemult = 1;

    switch(e->key()) {
    case Qt::Key_Delete:
        this->deleteSelectedTiles();
        break;

    case Qt::Key_Up:
    case Qt::Key_Down:
        foreach(QGraphicsItem *i, sel) {
            if(e->modifiers()&Qt::ShiftModifier)    translatemult=MSTI_TILEWIDTH;
            i->moveBy(0,(e->key()==Qt::Key_Down)?(this->iScale*translatemult):(-this->iScale*translatemult));
        }
        break;
    case Qt::Key_Left:
    case Qt::Key_Right:
        foreach(QGraphicsItem *i, sel) {
            if(e->modifiers()&Qt::ShiftModifier)    translatemult=MSTI_TILEWIDTH;
            i->moveBy((e->key()==Qt::Key_Right)?(this->iScale*translatemult):(-this->iScale*translatemult),0);
        }
        break;

    case Qt::Key_1:
    case Qt::Key_2:
    case Qt::Key_3:
    case Qt::Key_4:
        emit(requestPaletteUpdates(e->key()-Qt::Key_1));
        break;
    case Qt::Key_PageUp:
        this->moveSelectedUp();
        break;
    case Qt::Key_PageDown:
        this->moveSelectedDown();
        break;
    default:
        QGraphicsView::keyPressEvent(e);
    }

    this->sendTileUpdates();
}



void MetaspriteManager::drawGridLines()
{
    QPen thicksolid(Qt::black,0.5,Qt::SolidLine);
    QPen thickdashes(Qt::white,0.5,Qt::DashLine);
    QPen thinsolid(Qt::black,0.25,Qt::SolidLine);
    QPen thindashes(Qt::white,0.25,Qt::DashLine);
    QVector<qreal> dp;
    dp << 1 << 1;
    thickdashes.setDashPattern(dp);
    thindashes.setDashPattern(dp);

    qreal canvas = MSM_CANVAS_SIZE*this->iScale;

    for(int i=MSTI_TILEWIDTH*this->iScale; i<=canvas; i+=MSTI_TILEWIDTH*this->iScale) {
        this->gsMetasprite->addLine(-canvas,-i,canvas,-i,thinsolid);
        this->gsMetasprite->addLine(-canvas,i,canvas,i,thinsolid);
        this->gsMetasprite->addLine(-i,-canvas,-i,canvas,thinsolid);
        this->gsMetasprite->addLine(i,-canvas,i,canvas,thinsolid);

        this->gsMetasprite->addLine(-canvas,-i,canvas,-i,thindashes);
        this->gsMetasprite->addLine(-canvas,i,canvas,i,thindashes);
        this->gsMetasprite->addLine(-i,-canvas,-i,canvas,thindashes);
        this->gsMetasprite->addLine(i,-canvas,i,canvas,thindashes);
    }

    this->gsMetasprite->addLine(-canvas,0,canvas,0,thicksolid);
    this->gsMetasprite->addLine(0,-canvas,0,canvas,thicksolid);
    this->gsMetasprite->addLine(-canvas,0,canvas,0,thickdashes);
    this->gsMetasprite->addLine(0,-canvas,0,canvas,thickdashes);

    this->setSceneRect(-canvas,-canvas,canvas*2,canvas*2);
}

void MetaspriteManager::setNewSpriteColours(PaletteVector c, quint8 p, bool s)
{
    this->gsMetasprite->setBackgroundBrush(QBrush(QColor(c.at(0))));

    QList<QGraphicsItem*> items = this->gsMetasprite->items(Qt::AscendingOrder);
    foreach(QGraphicsItem *ms, items) {
        if(ms->type()!=MetaspriteTileItem::Type)   continue;
        quint8 currentpal = qgraphicsitem_cast<MetaspriteTileItem*>(ms)->palette();
        qgraphicsitem_cast<MetaspriteTileItem*>(ms)->setNewColours(c.at((4*currentpal)+1),
                                                                   c.at((4*currentpal)+2),
                                                                   c.at((4*currentpal)+3),
                                                                   currentpal);
    }

    if(s) {
        items = this->gsMetasprite->selectedItems();
        foreach(QGraphicsItem *i, items) {
            ((MetaspriteTileItem*)i)->setNewColours(c.at((4*p)+1),
                                                    c.at((4*p)+2),
                                                    c.at((4*p)+3),
                                                    p);
        }
    }
    this->sendTileUpdates();
}

void MetaspriteManager::addNewTile(QPointF p, QImage i, quint8 t, quint8 c)
{
    MetaspriteTileItem *pi = new MetaspriteTileItem(i);
    pi->setFlags(QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsSelectable);
    pi->setTallSprite(this->bTallSprites);
    pi->setScale(this->iScale);
    pi->setRealX(qRound(p.x()/this->iScale));
    pi->setRealY(qRound(p.y()/this->iScale));
    pi->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
    pi->setTile(t&(this->bTallSprites?0xFE:0xFF));
    pi->setPalette(c);
    this->gsMetasprite->addItem(pi);

    QList<QGraphicsItem*> items = this->gsMetasprite->items(Qt::AscendingOrder);
    QList<MetaspriteTileItem*> store;
    foreach(QGraphicsItem *i, items) {
        if(i->type()!=MetaspriteTileItem::Type)   continue;
        MetaspriteTileItem *ms = qgraphicsitem_cast<MetaspriteTileItem*>(i);
        store.append(ms);
    }
    this->vMetaspriteStages.replace(this->iMetaspriteStage,store);

    this->sendTileUpdates();
}

void MetaspriteManager::moveSelectedUp()
{
    QList<QGraphicsItem*> items = this->gsMetasprite->items(Qt::AscendingOrder);
    QList<QGraphicsItem*> sel = this->gsMetasprite->selectedItems();
    QVector<int> selindexes;
    foreach(QGraphicsItem *i, sel) {
        int index = items.indexOf(i);
        if(index>=items.size()-1)   return;
        selindexes.append(index);
    }
    qSort(selindexes);
    for(int i=selindexes.size()-1; i>=0; i--) {
        QGraphicsItem *take = items.takeAt(selindexes.at(i));
        items.insert(selindexes.at(i)+1,take);
    }
    foreach(QGraphicsItem *i, items) {
        this->gsMetasprite->removeItem(i);
        this->gsMetasprite->addItem(i);
    }
    this->sendTileUpdates();
}

void MetaspriteManager::moveSelectedDown()
{
    QList<QGraphicsItem*> items = this->gsMetasprite->items(Qt::AscendingOrder),
            sel = this->gsMetasprite->selectedItems();
    QList<QGraphicsItem*> msitems;
    QVector<int> selindexes;
    foreach(QGraphicsItem *i, items) {
        if(i->type()!=MetaspriteTileItem::Type) continue;
        msitems.append(i);
    }
    int diff = items.size()-msitems.size();
    foreach(QGraphicsItem *i, sel) {
        int index = items.indexOf(i);
        if(index>=items.size() || index<=diff) return;
        selindexes.append(index);
    }
    qSort(selindexes);
    for(int i=0; i<selindexes.size(); i++) {
        QGraphicsItem *take = items.takeAt(selindexes.at(i));
        items.insert(selindexes.at(i)-1,take);
    }
    foreach(QGraphicsItem *i, items) {
        this->gsMetasprite->removeItem(i);
        this->gsMetasprite->addItem(i);
    }
    this->sendTileUpdates();
}

void MetaspriteManager::flipHorizontal()
{
    QList<QGraphicsItem*> sel = this->gsMetasprite->selectedItems();
    foreach(QGraphicsItem *s, sel) {
        if(s->type()!=MetaspriteTileItem::Type)    continue;
        ((MetaspriteTileItem*)s)->flipHorizontal(!((MetaspriteTileItem*)s)->flippedHorizontal());
    }
    this->sendTileUpdates();
}

void MetaspriteManager::flipVertical()
{
    QList<QGraphicsItem*> sel = this->gsMetasprite->selectedItems();
    foreach(QGraphicsItem *s, sel) {
        if(s->type()!=MetaspriteTileItem::Type)    continue;
        ((MetaspriteTileItem*)s)->flipVertical(!((MetaspriteTileItem*)s)->flippedVertical());
    }
    this->sendTileUpdates();
}

void MetaspriteManager::deleteSelectedTiles()
{
    QList<QGraphicsItem*> sel = this->gsMetasprite->selectedItems();
    foreach(QGraphicsItem *s, sel) {
        this->gsMetasprite->removeItem(s);
    }

    QList<QGraphicsItem*> items = this->gsMetasprite->items(Qt::AscendingOrder);
    QList<MetaspriteTileItem*> store;
    foreach(QGraphicsItem *i, items) {
        if(i->type()!=MetaspriteTileItem::Type)   continue;
        MetaspriteTileItem *ms = qgraphicsitem_cast<MetaspriteTileItem*>(i);
        store.append(ms);
    }
    this->vMetaspriteStages.replace(this->iMetaspriteStage,store);

    this->sendTileUpdates();
}



void::MetaspriteManager::updateTiles(bool t)
{
    this->bTallSprites = t;
    QList<QGraphicsItem*> items = this->gsMetasprite->items();
    foreach(QGraphicsItem *ms, items) {
        if(ms->type()!=MetaspriteTileItem::Type)    continue;
        qgraphicsitem_cast<MetaspriteTileItem*>(ms)->setTallSprite(this->bTallSprites);
        emit(this->getTileUpdate(qgraphicsitem_cast<MetaspriteTileItem*>(ms)));
        emit(this->getPaletteUpdate(qgraphicsitem_cast<MetaspriteTileItem*>(ms)));
    }
    this->sendTileUpdates();
}

void MetaspriteManager::swapMetaspriteStage(int s)
{
    QList<QGraphicsItem*> items = this->gsMetasprite->items(Qt::AscendingOrder);
    QList<MetaspriteTileItem*> store;
    foreach(QGraphicsItem *i, items) {
        if(i->type()!=MetaspriteTileItem::Type) {
            this->gsMetasprite->removeItem(i);
        } else {
            MetaspriteTileItem *ms = qgraphicsitem_cast<MetaspriteTileItem*>(i);
            store.append(ms);
            this->gsMetasprite->removeItem(ms);
        }
    }
    this->vMetaspriteStages.replace(this->iMetaspriteStage,store);

    this->drawGridLines();

    this->iMetaspriteStage = s;
    store = this->vMetaspriteStages.at(s);
    foreach(MetaspriteTileItem *ms, store) {
        bool h = ms->flippedHorizontal();
        bool v = ms->flippedVertical();
        ms->flipHorizontal(false);
        ms->flipVertical(false);

        ms->setTallSprite(this->bTallSprites);
        ms->setX(ms->realX()*this->iScale);
        ms->setY(ms->realY()*this->iScale);
        ms->setScale(this->iScale);

        ms->flipHorizontal(h);
        ms->flipVertical(v);
        emit(this->getTileUpdate(ms));
        emit(this->getPaletteUpdate(ms));
        this->gsMetasprite->addItem(ms);
    }
    this->sendTileUpdates();
}

void MetaspriteManager::createFrameData(quint8 frame, qreal zoom)
{
    MetaspriteTileList list = this->createFrame(frame,zoom);
    emit(this->sendFrameData(list));
}

void MetaspriteManager::createAnimationFrameData(quint8 frame, qreal zoom)
{
    MetaspriteTileList list = this->createFrame(frame,zoom);
    emit(this->sendAnimationFrameData(list));
}



QVector<QByteArray> MetaspriteManager::createMetaspriteBinaryData()
{
    this->updateMetaspriteStage();

    QVector<QByteArray> bindata = QVector<QByteArray>(256);
    for(int i=0; i<256; i++) {
        MetaspriteTileList mslist = this->vMetaspriteStages[i];
        QByteArray bin;
        if(!mslist.isEmpty()) {
            bin.append(quint8(mslist.length()));
            for(int j=mslist.size()-1; j>=0; j--) {
                MetaspriteTileItem *ms = mslist.at(j);
                quint8 oamx = ms->realX();
                quint8 oamy = ms->realY();
                quint8 oamindex = ms->tile();
                quint8 oamattr = ms->palette()|(ms->flippedHorizontal()?0x40:0x00)|(ms->flippedVertical()?0x80:0x00);
                bin.append(oamy);
                bin.append(oamindex);
                bin.append(oamattr);
                bin.append(oamx);
            }
            bindata.replace(i,bin);
        }
    }
    return bindata;
}

QString MetaspriteManager::createMetaspriteASMData(QString labelprefix)
{
    QString asmlabel = labelprefix.isEmpty()?"emptylabel_":labelprefix;
    QString datatable_hi = asmlabel+"hi:\n\t.byte ";
    QString datatable_lo = asmlabel+"lo:\n\t.byte ";
    QString databytes;

    for(int i=0; i<256; i++) {
        MetaspriteTileList mslist = this->vMetaspriteStages[i];
        if(mslist.isEmpty())    continue;
        QString countedlabel = labelprefix+QString::number(i);

        datatable_hi += QString(">").append(countedlabel).append(",");
        datatable_lo += QString("<").append(countedlabel).append(",");

        databytes += "\n";
        databytes += countedlabel+":\n\t.byte ";
        databytes += QString("$%1").arg(mslist.size(),2,16,QChar('0')).toUpper();
        foreach(MetaspriteTileItem *mti, mslist) {
            quint8 oamx = mti->realX();
            quint8 oamy = mti->realY();
            quint8 oamindex = mti->tile();
            quint8 oamattr = mti->palette()|(mti->flippedHorizontal()?0x40:0x00)|(mti->flippedVertical()?0x80:0x00);
            databytes += QString(",$%1").arg(oamy,2,16,QChar('0')).toUpper();
            databytes += QString(",$%1").arg(oamindex,2,16,QChar('0')).toUpper();
            databytes += QString(",$%1").arg(oamattr,2,16,QChar('0')).toUpper();
            databytes += QString(",$%1").arg(oamx,2,16,QChar('0')).toUpper();
        }
    }

    datatable_hi.remove(datatable_hi.size()-1,1);
    datatable_lo.remove(datatable_lo.size()-1,1);
    datatable_hi += "\n";
    datatable_lo += "\n";
    databytes += "\n";
    databytes += asmlabel+"end:\n";

    return datatable_hi+datatable_lo+databytes;
}



void MetaspriteManager::openMetaspriteFile(QString filename)
{
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QMessageBox::warning(this,tr(MSM_FILE_OPEN_ERROR_TITLE),tr(MSM_FILE_OPEN_ERROR_BODY),QMessageBox::NoButton);
        return;
    }
    quint8 labelnum = 0;
    QVector<QByteArray> inputbytes(256);
    QString labelname;
    while(!file.atEnd()) {
        QString line = file.readLine();
        QRegularExpression label("^(.*?)_(\\d+?):$");
        QRegularExpressionMatch labelmatch = label.match(line);
        if(labelmatch.hasMatch()) {
            if(labelname.isEmpty()) {
                labelname = labelmatch.captured(1);
                emit(this->setMetaspriteLabel(labelname));
            }
            labelnum = labelmatch.captured(2).toInt();
        }
        QRegularExpression bytes(",?\\$([0-9a-fA-F]+)");
        QRegularExpressionMatchIterator bytesiter = bytes.globalMatch(line);
        QByteArray bytesin;
        while(bytesiter.hasNext()) {
            QRegularExpressionMatch bytesmatch = bytesiter.next();
            bytesin.append(quint8(bytesmatch.captured(1).toUInt(NULL,16)));
        }
        if(!bytesin.isEmpty())  inputbytes.replace(labelnum,bytesin);
    }
    if(!labelname.isEmpty()) {
        foreach(QByteArray test, inputbytes) {
            if(!test.isEmpty()) {
                this->importMetaspriteBinaryData(inputbytes);
                file.close();
                return;
            }
        }
    }

//    file.reset();
//    QByteArray byteblob = file.readAll(), bytesin;
//    QByteArray::iterator i = byteblob.begin();
//    int loopcount = 0;
//    while(i!=byteblob.end()) {
//        bytesin.append(*i);
//        if((i+((*i)*4))>=byteblob.end()) {
//            QMessageBox::critical(this,tr(MSM_EOF_ERROR_TITLE),tr(MSM_EOF_ERROR_BODY),QMessageBox::NoButton);
//            return;
//        }
//        for(int count=*(i++); count>0; count--) {
//            for(int j=0; j<4; j++) {
//                bytesin.append(*(i++));
//            }
//        }
//        inputbytes.replace(loopcount++,bytesin);
//    }
//    QFileInfo fileinfo(filename);
//    emit(this->setMetaspriteLabel(fileinfo.baseName()));

//    this->importMetaspriteBinaryData(inputbytes);

    file.close();
    QMessageBox::critical(this,tr(MSM_INVALID_SPRITES_TITLE),tr(MSM_INVALID_SPRITES_BODY),QMessageBox::NoButton);
}

void MetaspriteManager::importMetaspriteBinaryData(QVector<QByteArray> bindata)
{
    for(int j=0; j<256; j++) {
        QByteArray bin = bindata.at(j);
        QList<MetaspriteTileItem*> mslist = this->vMetaspriteStages.at(j);
        mslist.clear();
        QByteArray::iterator biniter = bin.begin();
        for(int count = *biniter; count>0; count--) {
            if((biniter+(count*4))>=bin.end()) {
                QMessageBox::critical(this,tr(MSM_COUNT_ERROR_TITLE),tr(MSM_COUNT_ERROR_BODY),QMessageBox::NoButton);
                return;
            }
            int oamy = *(++biniter);
            quint8 oamindex = *(++biniter);
            quint8 oamattr = *(++biniter);
            int oamx = *(++biniter);
            MetaspriteTileItem *ms = new MetaspriteTileItem();
            ms->setFlags(QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsSelectable);
            ms->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
            ms->setScale(this->iScale);
            ms->setTallSprite(this->bTallSprites);
            ms->setRealX(oamx);
            ms->setRealY(oamy);
            ms->setTile(oamindex&(this->bTallSprites?0xFE:0xFF));
            ms->setPalette(oamattr&0x03);
            ms->flipHorizontal((oamattr&0x40)?true:false);
            ms->flipVertical((oamattr&0x80)?true:false);
            emit(this->getTileUpdate(ms));
            emit(this->getPaletteUpdate(ms));
            mslist.prepend(ms);
        }
        this->vMetaspriteStages.replace(j,mslist);
    }

    QList<MetaspriteTileItem*> store = this->vMetaspriteStages.at(this->iMetaspriteStage);
    this->gsMetasprite->clear();
    this->drawGridLines();
    foreach(MetaspriteTileItem *ms, store) {
        this->gsMetasprite->addItem(ms);
    }

    this->sendTileUpdates();
}

void MetaspriteManager::clearAllMetaspriteData()
{
    this->gsMetasprite->clear();
    for(int i=0; i<this->vMetaspriteStages.size(); i++) {
        this->vMetaspriteStages.replace(i,MetaspriteTileList());
    }
    this->drawGridLines();

    this->sendTileUpdates();
}



void MetaspriteManager::sendTileUpdates()
{
    emit(this->updateList(this->gsMetasprite->items(),this->gsMetasprite->selectedItems()));
    emit(this->updateAnimationFrame());
}

MetaspriteTileList MetaspriteManager::createFrame(quint8 f, qreal s)
{
    MetaspriteTileList list = this->vMetaspriteStages.at(f);
    MetaspriteTileList listcopy;
    foreach(MetaspriteTileItem *i, list) {
        MetaspriteTileItem *newitem = new MetaspriteTileItem();
        newitem->setScale((s==0)?i->scale():s);
        newitem->setRealX(i->realX());
        newitem->setRealY(i->realY());
        newitem->setTallSprite(this->bTallSprites);
        newitem->flipHorizontal(i->flippedHorizontal());
        newitem->flipVertical(i->flippedVertical());
        newitem->setPalette(i->palette());
        newitem->setTile(i->tile());
        emit(this->getTileUpdate(newitem));
        emit(this->getPaletteUpdate(newitem));
        listcopy.append(newitem);
    }
    return listcopy;
}
