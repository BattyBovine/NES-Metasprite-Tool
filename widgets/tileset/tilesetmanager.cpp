#include "tilesetmanager.h"

TilesetManager::TilesetManager(QWidget *parent) : QGraphicsView(parent)
{
    this->gsTileset = new QGraphicsScene();
    this->setScene(this->gsTileset);

    this->imgTileset = QImage(128, 128, QImage::Format_Indexed8);
    this->imgTileset.fill(0);
    this->imgTileset.setColor(0,qRgba(0x00,0x00,0x00,0x00));
    this->imgTileset.setColor(1,qRgb(0x00,0x00,0x00));
    this->imgTileset.setColor(2,qRgb(0x00,0x00,0x00));
    this->imgTileset.setColor(3,qRgb(0x00,0x00,0x00));

    this->gpiTileset = new QGraphicsPixmapItem();
    this->gpiTileset->setScale(TSM_SCALE);
    this->gsTileset->addItem(this->gpiTileset);
    this->griSelection[0] = this->griSelection[1] = NULL;
    this->iSelectedTile = 0;
    this->iPalette = 0;
    this->bTallSprite = false;

    this->pSelection = QPointF(0,0);
    this->drawSelectionBox();

    this->threadCHR = new CHRThread();
    connect(this->threadCHR,SIGNAL(sendCHRImageData(QImage)),this,SLOT(getNewCHRData(QImage)));

    connect(&this->fswCHR,SIGNAL(fileChanged(QString)),this,SLOT(reloadCurrentTileset()));
}

TilesetManager::~TilesetManager()
{
    this->gsTileset->removeItem(this->gpiTileset);
    delete this->gpiTileset;
    delete this->gsTileset;
}



void TilesetManager::dropEvent(QDropEvent *e)
{
    e->acceptProposedAction();

    foreach(QUrl url, e->mimeData()->urls()) {
        if(url.toLocalFile().endsWith(".chr",Qt::CaseInsensitive)) {
            this->loadCHRBank(url.toLocalFile());
            return;
        }
    }
    this->loadCHRBank(e->mimeData()->urls()[0].toLocalFile());
}

void TilesetManager::mousePressEvent(QMouseEvent *e)
{
    this->pSelection = this->mapToScene(e->pos());
    this->drawSelectionBox();
}



bool TilesetManager::drawSelectionBox()
{
    if(this->griSelection[0] && this->griSelection[1]) {
        this->gsTileset->removeItem(this->griSelection[0]);
        this->gsTileset->removeItem(this->griSelection[1]);
        delete this->griSelection[0];
        delete this->griSelection[1];
    }

    quint8 xorigin = qFloor(this->pSelection.x())-(qFloor(this->pSelection.x())%(TSM_TILESIZE*(this->bTallSprite?2:1)));
    quint8 yorigin = qFloor(this->pSelection.y())-(qFloor(this->pSelection.y())%TSM_TILESIZE);
    this->pSelection = QPointF(xorigin,yorigin);
    this->iSelectedTile = (qFloor((yorigin/TSM_TILESIZE)<<4)|qFloor(xorigin/TSM_TILESIZE));

    QPen dashes(Qt::black,1,Qt::DashLine);
    QVector<qreal> dp;
    dp << 2 << 2;
    dashes.setDashPattern(dp);
    this->griSelection[0] = this->gsTileset->addRect(QRectF(xorigin,yorigin,TSM_TILESIZE*(this->bTallSprite?2:1)-1,TSM_TILESIZE-1),QPen(Qt::white),Qt::NoBrush);
    this->griSelection[1] = this->gsTileset->addRect(QRectF(xorigin,yorigin,TSM_TILESIZE*(this->bTallSprite?2:1)-1,TSM_TILESIZE-1),dashes,Qt::NoBrush);

    return true;
}

void TilesetManager::reloadCurrentTileset()
{
    threadCHR->loadFile(this->sCurrentTilesetFile);
}



void TilesetManager::loadCHRBank(QString filename)
{
    this->threadCHR->loadFile(filename);
    if(!this->fswCHR.files().isEmpty()) this->fswCHR.removePath(this->sCurrentTilesetFile);
    this->sCurrentTilesetFile = filename;
    this->fswCHR.addPath(filename);
    return;
}

void TilesetManager::setNewSpriteColours(PaletteVector c, quint8 i)
{
    this->gsTileset->setBackgroundBrush(QBrush(QColor(c.at(4*i))));
    this->imgTileset.setColor(1,c.at((4*i)+1));
    this->imgTileset.setColor(2,c.at((4*i)+2));
    this->imgTileset.setColor(3,c.at((4*i)+3));
    this->iPalette = i;

    this->gpiTileset->setPixmap(QPixmap::fromImage(this->imgTileset));
}



void TilesetManager::getNewTile(QPointF p)
{
    emit(sendNewTile(p,this->createNewTile(this->iSelectedTile),this->iSelectedTile,this->iPalette));
}

void TilesetManager::updateSpriteTile(MetaspriteTileItem *t)
{
    t->setTile(this->createNewTile(t->tile()&(t->tallSprite()?0xFE:0xFF)));
}

void TilesetManager::getNewCHRData(QImage img)
{
    img.setColor(1,this->imgTileset.color(1));
    img.setColor(2,this->imgTileset.color(2));
    img.setColor(3,this->imgTileset.color(3));
    this->imgTileset = img;
    this->gpiTileset->setPixmap(QPixmap::fromImage(this->imgTileset));
    emit(this->tilesetChanged(this->bTallSprite));
}



QImage TilesetManager::createNewTile(quint8 tile)
{
    QImage newtile(MSTI_TILEWIDTH, MSTI_TILEWIDTH*(this->bTallSprite?2:1), QImage::Format_Indexed8);

    QImage toptile(this->imgTileset.copy((tile&0x0F)*MSTI_TILEWIDTH,((tile&0xF0)>>4)*MSTI_TILEWIDTH,MSTI_TILEWIDTH,MSTI_TILEWIDTH));
    newtile.setColor(0,toptile.color(0));
    newtile.setColor(1,toptile.color(1));
    newtile.setColor(2,toptile.color(2));
    newtile.setColor(3,toptile.color(3));

    for(quint8 y=0; y<MSTI_TILEWIDTH; y++) {
        for(quint8 x=0; x<MSTI_TILEWIDTH; x++) {
            newtile.setPixel(x,y,toptile.pixelIndex(x,y));
        }
    }

    if(this->bTallSprite) {
        QImage antoniostellabottomtile(this->imgTileset.copy(((tile+1)&0x0F)*MSTI_TILEWIDTH,((tile&0xF0)>>4)*MSTI_TILEWIDTH,MSTI_TILEWIDTH,MSTI_TILEWIDTH));
        for(quint8 y=0; y<MSTI_TILEWIDTH; y++) {
            for(quint8 x=0; x<MSTI_TILEWIDTH; x++) {
                newtile.setPixel(x,y+MSTI_TILEWIDTH,antoniostellabottomtile.pixelIndex(x,y));
            }
        }
    }

    return newtile;
}
