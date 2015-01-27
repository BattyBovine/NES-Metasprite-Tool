#include "tilesetmanager.h"

TilesetManager::TilesetManager(QWidget *parent) : QGraphicsView(parent)
{
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    this->gsTileset = new QGraphicsScene();
    this->setScene(this->gsTileset);

    this->imgTileset = QImage(128, 128, QImage::Format_Indexed8);
    this->imgTileset.fill(0);
    this->imgTileset.setColor(0,qRgba(0x00,0x00,0x00,0x00));
    for(int i=0; i<3; i++)  this->imgTileset.setColor(i+1,qRgb(0x00,0x00,0x00));

    this->gpiTileset = new QGraphicsPixmapItem(QPixmap::fromImage(this->imgTileset));
    this->gpiTileset->setScale(TSM_SCALE);
    this->gsTileset->addItem(this->gpiTileset);
    this->griSelection[0] = this->griSelection[1] = NULL;
    this->iSelectedTile = 0;
    this->iPalette = 0;

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

    quint8 xorigin = qFloor(this->pSelection.x())-(qFloor(this->pSelection.x())%TSM_TILESIZE);
    quint8 yorigin = qFloor(this->pSelection.y())-(qFloor(this->pSelection.y())%TSM_TILESIZE);
    this->pSelection = QPointF(xorigin,yorigin);
    this->iSelectedTile = (qFloor((yorigin/TSM_TILESIZE)<<4)|qFloor(xorigin/TSM_TILESIZE));

    QPen dashes(Qt::black,1,Qt::DashLine);
    QVector<qreal> dp;
    dp << 2 << 2;
    dashes.setDashPattern(dp);
    this->griSelection[0] = this->gsTileset->addRect(QRectF(xorigin,yorigin,TSM_TILESIZE-1,TSM_TILESIZE-1),QPen(Qt::white),Qt::NoBrush);
    this->griSelection[1] = this->gsTileset->addRect(QRectF(xorigin,yorigin,TSM_TILESIZE-1,TSM_TILESIZE-1),dashes,Qt::NoBrush);

    return true;
}

void TilesetManager::redrawTileset()
{
    this->gpiTileset->setPixmap(QPixmap::fromImage(this->imgTileset));
}




bool TilesetManager::loadCHRBank(QString filename)
{
    QFile file(filename);
    if(file.size()<0x1000) {
        QMessageBox::warning(this,tr("File too small"),tr("The selected file is too small to be a CHR bank."),QMessageBox::NoButton);
        return false;
    }
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this,tr("Error opening file"),tr("The file could not be opened."),QMessageBox::NoButton);
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

    emit(this->tilesetChanged());

    return true;
}

void TilesetManager::setNewSpriteColours(QVector<QRgb> c, quint8 i)
{
    this->gsTileset->setBackgroundBrush(QBrush(QColor(c.at(4*i))));
    this->imgTileset.setColor(1,c.at((4*i)+1));
    this->imgTileset.setColor(2,c.at((4*i)+2));
    this->imgTileset.setColor(3,c.at((4*i)+3));
    this->iPalette = i;

    this->redrawTileset();
}

void TilesetManager::createNewTile(QPointF p)
{
    emit(sendNewTile(p,this->imgTileset.copy(this->pSelection.x()/TSM_SCALE,this->pSelection.y()/TSM_SCALE,8,8),this->iSelectedTile,this->iPalette));
}

void TilesetManager::updateSpriteTile(MetaspriteTileItem *t)
{
    t->setTile(this->imgTileset.copy((t->tile()&0x0F)*8,((t->tile()&0xF0)>>4)*8,8,8));
}
