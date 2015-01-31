#include "metaspritemanager.h"

MetaspriteManager::MetaspriteManager(QWidget *parent) : QGraphicsView(parent)
{
    this->gsMetasprite = new QGraphicsScene();
    this->setScene(this->gsMetasprite);

    this->drawGridLines();

    this->vMetaspriteStages = QVector< QList<MetaspriteTileItem*> >(256);
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
    if(e->button()!=Qt::RightButton)
        QGraphicsView::mousePressEvent(e);
    else
        emit(this->requestNewTile(this->mapToScene(e->pos())));
}

void MetaspriteManager::mouseReleaseEvent(QMouseEvent *e)
{
    QGraphicsView::mouseReleaseEvent(e);

    QList<QGraphicsItem*> sel = this->gsMetasprite->selectedItems();
    foreach(QGraphicsItem *i, sel) {
        qgraphicsitem_cast<MetaspriteTileItem*>(i)->setRealX(qRound(i->pos().x()/MSM_SCALE));
        qgraphicsitem_cast<MetaspriteTileItem*>(i)->setRealY(qRound(i->pos().y()/MSM_SCALE));
    }

//    if(e->button()==Qt::MiddleButton)
//        QMessageBox::information(this,"Position",QString::number(qgraphicsitem_cast<MetaspriteTileItem*>(sel.at(0))->realX())+","+
//                                 QString::number(qgraphicsitem_cast<MetaspriteTileItem*>(sel.at(0))->realY()),QMessageBox::NoButton);

    emit(this->updateList(this->gsMetasprite->items(),this->gsMetasprite->selectedItems()));
}

void MetaspriteManager::keyPressEvent(QKeyEvent *e)
{
    QList<QGraphicsItem*> sel = this->gsMetasprite->selectedItems();
    switch(e->key()) {
    case Qt::Key_Delete:
        this->deleteSelectedTiles();
        break;

    case Qt::Key_Up:
    case Qt::Key_Down:
        foreach(QGraphicsItem *i, sel) {
            i->moveBy(0,(e->key()==Qt::Key_Down)?MSM_SCALE:(-MSM_SCALE));
        }
        break;
    case Qt::Key_Left:
    case Qt::Key_Right:
        foreach(QGraphicsItem *i, sel) {
            i->moveBy((e->key()==Qt::Key_Right)?MSM_SCALE:(-MSM_SCALE),0);
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

    emit(this->updateList(this->gsMetasprite->items(),this->gsMetasprite->selectedItems()));
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

    for(int i=MSM_TILESIZE; i<=128; i+=MSM_TILESIZE) {
        this->gsMetasprite->addLine(-128,-i,128,-i,thinsolid);
        this->gsMetasprite->addLine(-128,i,128,i,thinsolid);
        this->gsMetasprite->addLine(-i,-128,-i,128,thinsolid);
        this->gsMetasprite->addLine(i,-128,i,128,thinsolid);

        this->gsMetasprite->addLine(-128,-i,128,-i,thindashes);
        this->gsMetasprite->addLine(-128,i,128,i,thindashes);
        this->gsMetasprite->addLine(-i,-128,-i,128,thindashes);
        this->gsMetasprite->addLine(i,-128,i,128,thindashes);
    }

    this->gsMetasprite->addLine(-128,0,128,0,thicksolid);
    this->gsMetasprite->addLine(0,-128,0,128,thicksolid);
    this->gsMetasprite->addLine(-128,0,128,0,thickdashes);
    this->gsMetasprite->addLine(0,-128,0,128,thickdashes);
}

void MetaspriteManager::setNewSpriteColours(QVector<QRgb> c, quint8 p, bool s)
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
    emit(this->updateList(this->gsMetasprite->items(),this->gsMetasprite->selectedItems()));
}

void MetaspriteManager::addNewTile(QPointF p, QImage i, quint8 t, quint8 c)
{
    MetaspriteTileItem *pi = new MetaspriteTileItem(i);
    pi->setFlags(QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsSelectable);
    pi->setScale(MSM_SCALE);
    pi->setRealX(qRound(p.x()/MSM_SCALE));
    pi->setRealY(qRound(p.y()/MSM_SCALE));
    pi->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
    pi->setTile(t);
    pi->setPalette(c);
    this->gsMetasprite->addItem(pi);

    emit(this->updateList(this->gsMetasprite->items(),this->gsMetasprite->selectedItems()));
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
    emit(this->updateList(this->gsMetasprite->items(),this->gsMetasprite->selectedItems()));
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
    emit(this->updateList(this->gsMetasprite->items(),this->gsMetasprite->selectedItems()));
}

void MetaspriteManager::flipHorizontal()
{
    QList<QGraphicsItem*> sel = this->gsMetasprite->selectedItems();
    foreach(QGraphicsItem *s, sel) {
        if(s->type()!=MetaspriteTileItem::Type)    continue;
        ((MetaspriteTileItem*)s)->flipHorizontal(!((MetaspriteTileItem*)s)->flippedHorizontal());
    }
    emit(this->updateList(this->gsMetasprite->items(),this->gsMetasprite->selectedItems()));
}

void MetaspriteManager::flipVertical()
{
    QList<QGraphicsItem*> sel = this->gsMetasprite->selectedItems();
    foreach(QGraphicsItem *s, sel) {
        if(s->type()!=MetaspriteTileItem::Type)    continue;
        ((MetaspriteTileItem*)s)->flipVertical(!((MetaspriteTileItem*)s)->flippedVertical());
    }
    emit(this->updateList(this->gsMetasprite->items(),this->gsMetasprite->selectedItems()));
}

void MetaspriteManager::deleteSelectedTiles()
{
    QList<QGraphicsItem*> sel = this->gsMetasprite->selectedItems();
    foreach(QGraphicsItem *s, sel) {
        this->gsMetasprite->removeItem(s);
    }
    emit(this->updateList(this->gsMetasprite->items(),this->gsMetasprite->selectedItems()));
}



void::MetaspriteManager::updateTiles()
{
    QList<QGraphicsItem*> items = this->gsMetasprite->items();
    foreach(QGraphicsItem *ms, items) {
        if(ms->type()!=MetaspriteTileItem::Type)    continue;
        emit(this->getTileUpdate(qgraphicsitem_cast<MetaspriteTileItem*>(ms)));
        emit(this->getPaletteUpdate(qgraphicsitem_cast<MetaspriteTileItem*>(ms)));
    }
    emit(this->updateList(this->gsMetasprite->items(),this->gsMetasprite->selectedItems()));
}

void MetaspriteManager::swapMetaspriteStage(int s)
{
    QList<QGraphicsItem*> items = this->gsMetasprite->items(Qt::AscendingOrder);
    QList<MetaspriteTileItem*> store;
    foreach(QGraphicsItem *i, items) {
        if(i->type()!=MetaspriteTileItem::Type)   continue;
        MetaspriteTileItem *ms = qgraphicsitem_cast<MetaspriteTileItem*>(i);
        store.append(ms);
        this->gsMetasprite->removeItem(ms);
    }
    this->vMetaspriteStages.replace(this->iMetaspriteStage,store);

    this->iMetaspriteStage = s;
    store = this->vMetaspriteStages.at(s);
    foreach(MetaspriteTileItem *ms, store) {
        emit(this->getTileUpdate(ms));
        emit(this->getPaletteUpdate(ms));
        this->gsMetasprite->addItem(ms);
    }
    emit(this->updateList(this->gsMetasprite->items(),this->gsMetasprite->selectedItems()));
}



QVector<QByteArray> MetaspriteManager::createMetaspriteBinaryData()
{
    this->swapMetaspriteStage(this->iMetaspriteStage);

    QVector<QByteArray> bindata = QVector<QByteArray>(256);
    for(int i=0; i<256; i++) {
        QList<MetaspriteTileItem*> mslist = this->vMetaspriteStages.at(i);
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
        QRegularExpression bytes(",?0x([0-9a-fA-F]+)");
        QRegularExpressionMatchIterator bytesiter = bytes.globalMatch(line);
        QByteArray bytesin;
        while(bytesiter.hasNext()) {
            QRegularExpressionMatch bytesmatch = bytesiter.next();
            bytesin.append(quint8(bytesmatch.captured(1).toUInt(NULL,16)));
        }
        inputbytes.replace(labelnum,bytesin);
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

    file.reset();
    QByteArray byteblob = file.readAll(), bytesin;
    QByteArray::iterator i = byteblob.begin();
    int loopcount = 0;
    while(i!=byteblob.end()) {
        bytesin.append(*i);
        for(int count=*(i++); count>0; count--) {
            for(int j=0; j<4; j++) {
                bytesin.append(*(i++));
            }
        }
        inputbytes.replace(loopcount++,bytesin);
    }
    this->importMetaspriteBinaryData(inputbytes);
    file.close();
}

void MetaspriteManager::importMetaspriteBinaryData(QVector<QByteArray> bindata)
{
    for(int j=0; j<256; j++) {
        QByteArray bin = bindata.at(j);
        QList<MetaspriteTileItem*> mslist = this->vMetaspriteStages.at(j);
        mslist.clear();
        QByteArray::iterator biniter = bin.begin();
        for(int count = *biniter; count>0; count--) {
            int oamy = *(++biniter);
            quint8 oamindex = *(++biniter);
            quint8 oamattr = *(++biniter);
            int oamx = *(++biniter);
            MetaspriteTileItem *ms = new MetaspriteTileItem();
            ms->setFlags(QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsSelectable);
            ms->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
            ms->setScale(MSM_SCALE);
            ms->setRealX(oamx);
            ms->setRealY(oamy);
            ms->setTile(oamindex);
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

    emit(this->updateList(this->gsMetasprite->items(),this->gsMetasprite->selectedItems()));
}
