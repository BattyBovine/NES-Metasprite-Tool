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

    this->gpiTileset = new QGraphicsPixmapItem(QPixmap::fromImage(this->imgTileset));
    this->gpiTileset->setScale(TSM_SCALE);
    this->gsTileset->addItem(this->gpiTileset);
    this->griSelection[0] = this->griSelection[1] = NULL;
    this->iSelectedTile = 0;
    this->iPalette = 0;
    this->bTallSprite = false;

    this->pSelection = QPointF(0,0);
    this->drawSelectionBox();
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

void TilesetManager::redrawTileset()
{
    this->gpiTileset->setPixmap(QPixmap::fromImage(this->imgTileset));
}

void TilesetManager::reloadCurrentTileset()
{
    this->loadCHRBank(this->sCurrentTilesetFile);
}




bool TilesetManager::loadCHRBank(QString filename)
{
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this,tr(TSM_CHR_OPEN_ERROR_TITLE),tr(TSM_CHR_OPEN_ERROR_BODY),QMessageBox::NoButton);
        return false;
    }
    if((file.size()%0x1000)!=0) {
        QMessageBox::warning(this,tr(TSM_CHR_SIZE_ERROR_TITLE),tr(TSM_CHR_SIZE_ERROR_BODY),QMessageBox::NoButton);
        return false;
    }

    this->baTilesetData.clear();
    while(file.bytesAvailable()>=16) {
        char bitplanes[2][8];
        file.read(bitplanes[0],8);
        file.read(bitplanes[1],8);
        for(int i=0; i<8; i++) {
            quint16 bpbyte1 = bitplanes[0][i];
            quint16 bpbyte2 = bitplanes[1][i];
            quint16 mask = 0x80;
            for(int j=0; j<8; j++) {
                quint8 index = 0;
                if((bpbyte1&mask)&&(bpbyte2&mask))  index = 3;
                if(!(bpbyte1&mask)&&(bpbyte2&mask)) index = 2;
                if((bpbyte1&mask)&&!(bpbyte2&mask)) index = 1;
                this->baTilesetData.append(index);
                mask >>= 1;
            }
        }
    }
    file.close();

    for(int imgy=0; imgy<16; imgy++) {
        for(int imgx=0; imgx<16; imgx++) {
            for(int tiley=0; tiley<8; tiley++) {
                for(int tilex=0; tilex<8; tilex++) {
                    this->imgTileset.setPixel(tilex+(8*imgx),
                                              tiley+(8*imgy),
                                              this->baTilesetData.at(((imgx+(imgy*16))*64)+(tilex+(tiley*8))));
                }
            }
        }
    }

    this->redrawTileset();

    emit(this->tilesetChanged(this->bTallSprite));

    this->sCurrentTilesetFile = filename;

    return true;
}

void TilesetManager::setNewSpriteColours(PaletteVector c, quint8 i)
{
    this->gsTileset->setBackgroundBrush(QBrush(QColor(c.at(4*i))));
    this->imgTileset.setColor(1,c.at((4*i)+1));
    this->imgTileset.setColor(2,c.at((4*i)+2));
    this->imgTileset.setColor(3,c.at((4*i)+3));
    this->iPalette = i;

    this->redrawTileset();
}



void TilesetManager::getNewTile(QPointF p)
{
    emit(sendNewTile(p,this->createNewTile(this->iSelectedTile),this->iSelectedTile,this->iPalette));
//    emit(sendNewTile(p,this->imgTileset.copy(this->pSelection.x()/TSM_SCALE,this->pSelection.y()/TSM_SCALE,MSTI_TILEWIDTH,MSTI_TILEWIDTH*(this->bTallSprite?2:1)),this->iSelectedTile,this->iPalette));
}

void TilesetManager::updateSpriteTile(MetaspriteTileItem *t)
{
    t->setTile(this->createNewTile(t->tile()&(t->tallSprite()?0xFE:0xFF)));
//    t->setTile(this->imgTileset.copy((t->tile()&0x0F)*t->width(),((t->tile()&0xF0)>>4)*t->height(),t->width(),t->height()));
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
