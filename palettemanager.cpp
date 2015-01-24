#include "palettemanager.h"
#include "spritepaletteview.h"

PaletteManager::PaletteManager(QWidget *parent) : QGraphicsView(parent)
{
    this->gsFullPaletteScene = new QGraphicsScene();
    this->gsFullPaletteScene->setSceneRect(0,0,224,64);

    for(int i=0; i<4; i++) {
        this->gsSpritePaletteScene[i] = new QGraphicsScene();
        this->gsSpritePaletteScene[i]->setSceneRect(0,0,64,16);
    }

    for(int i=0; i<4; i++) {
        for(int j=0; j<4; j++) {
            this->iSpritePaletteIndices[i][j] = 0x0F;
        }
    }
    this->iSpritePaletteSelected = this->iSpritePaletteSelectedIndex = 0;

    this->setScene(this->gsFullPaletteScene);
}

PaletteManager::~PaletteManager()
{
    delete this->gsFullPaletteScene;
    for(int i=0; i<4; i++) {
        delete this->gsSpritePaletteScene[i];
    }
}



bool PaletteManager::drawFullPaletteColours(QString palfile)
{
    this->sPaletteFile = palfile;

    this->gsFullPaletteScene->clear();

    QFile pal(":/"+this->sPaletteFile+".pal");
    if(!pal.open(QIODevice::ReadOnly)) return false;
    QByteArray baPaletteBlob = pal.readAll();
    pal.close();

    this->vPaletteColours.clear();
    for(int i=0; i<baPaletteBlob.length(); i+=3) {
        quint8 r = quint8(baPaletteBlob.at(i));
        quint8 g = quint8(baPaletteBlob.at(i+1));
        quint8 b = quint8(baPaletteBlob.at(i+2));
        this->vPaletteColours.append(QColor(r,g,b));
    }

    for(int i=0; i<4; i++) {
        for(int j=0; j<14; j++) {
            int p = ((i<<4)|j);
            this->gsFullPaletteScene->addRect(QRectF((j*16),(i*16),16,16),Qt::NoPen,this->vPaletteColours.at(p));
        }
    }

    this->generateNewSpritePalettes();

    return true;
}

bool PaletteManager::drawSelectionBox(QGraphicsScene *s, QPointF p)
{
    quint8 xorigin = qFloor(p.x())-(qFloor(p.x())%16);
    quint8 yorigin = qFloor(p.y())-(qFloor(p.y())%16);

    QPen dashes(Qt::black,1,Qt::DashLine);
    QVector<qreal> dp;
    dp << 2 << 2;
    dashes.setDashPattern(dp);
    s->addRect(QRectF(xorigin,yorigin,15,15),QPen(Qt::white),Qt::NoBrush);
    s->addRect(QRectF(xorigin,yorigin,15,15),dashes,Qt::NoBrush);

    return true;
}

bool PaletteManager::setPaletteData(QByteArray in)
{
    for(int i=0; i<4; i++) {
        for(int j=0; j<4; j++) {
            this->iSpritePaletteIndices[i][j] = (in.at(j+(4*i))&0x3F);
        }
    }
    this->generateNewSpritePalettes();
    return true;
}

QByteArray PaletteManager::paletteData()
{
    QByteArray indices;
    for(int i=0; i<4; i++) {
        for(int j=0; j<4; j++) {
            indices.append(this->iSpritePaletteIndices[i][j]);
        }
    }
    return indices;
}



void PaletteManager::dropEvent(QDropEvent *e)
{
    e->acceptProposedAction();

    foreach(QUrl url, e->mimeData()->urls()) {
        if(url.toLocalFile().endsWith(".pal",Qt::CaseInsensitive)) {
            QFile file(url.toLocalFile());
            if(!file.open(QIODevice::ReadOnly)) {
                QMessageBox::critical(this,tr("Error opening file"),tr("The file could not be opened."),QMessageBox::NoButton);
            }
            if(file.size() != 0x10) {
                QMessageBox::warning(this,
                                     tr("File too ")+(file.size()<0x10?tr("small"):tr("large")),
                                     tr("The selected file is too ") + (file.size()<0x10?tr("small"):tr("large")) + tr(" to be a palette file."),
                                     QMessageBox::NoButton);
                return;
            }
            QByteArray pal = file.readAll();
            file.close();
            this->setPaletteData(pal);
            return;
        }
    }
}

void PaletteManager::mousePressEvent(QMouseEvent *e)
{
    QPointF p = this->mapToScene(e->pos());
    quint8 pval = (qFloor(p.y()/16)<<4)|(qFloor(p.x()/16));
    if(pval==0x0D)  pval = 0x0F;

    if(this->iSpritePaletteSelectedIndex == 0) {
        for(int i=0; i<4; i++) {
            this->iSpritePaletteIndices[i][0] = pval;
        }
    } else {
        this->iSpritePaletteIndices[this->iSpritePaletteSelected][this->iSpritePaletteSelectedIndex] = pval;
    }

    this->drawFullPaletteColours(this->sPaletteFile);
    this->drawSelectionBox(this->gsFullPaletteScene, p);
}

void PaletteManager::setPaletteFile(QString p)
{
    this->sPaletteFile = p;
}

void PaletteManager::setSpritePaletteScene(QObject* o)
{
    ((SpritePaletteView*)o)->setScene(this->gsSpritePaletteScene[((SpritePaletteView*)o)->objectName().at(((SpritePaletteView*)o)->objectName().length()-1).digitValue()]);
}

void PaletteManager::spritePaletteSelected(QString s, quint8 i)
{
    quint8 pindex = (s.at(s.length()-1)).digitValue();
    this->iSpritePaletteSelected = pindex;
    this->iSpritePaletteSelectedIndex = i;

    quint8 c = this->iSpritePaletteIndices[pindex][i];
    if(c==0x0F)  c=0x0D;
    quint8 x = (c&0x0F)*16;
    quint8 y = (c>>4)*16;

    this->drawFullPaletteColours(this->sPaletteFile);
    this->drawSelectionBox(this->gsFullPaletteScene, QPointF(x,y));

    QVector<QColor> newspritecolours;
    for(int i=0; i<4; i++)  newspritecolours.append(this->vPaletteColours.at(this->iSpritePaletteIndices[pindex][i]));
}

QVector<QRgb> PaletteManager::createPaletteColours()
{
    QVector<QRgb> p;
    for(int i=0; i<4; i++) {
        for(int j=0; j<4; j++) {
            p.append(this->vPaletteColours.at(this->iSpritePaletteIndices[i][j]).rgb());
        }
    }
    return p;
}

void PaletteManager::generateNewSpritePalettes()
{
    for(int spritepal=0; spritepal<4; spritepal++) {
        this->gsSpritePaletteScene[spritepal]->clear();
        for(int i=0; i<4; i++) {
            quint8 index = this->iSpritePaletteIndices[spritepal][i];
            if(index==0x0F) index=0x0D;
            this->gsSpritePaletteScene[spritepal]->addRect(QRectF((i*16),0,16,16),Qt::NoPen,this->vPaletteColours.at(index));
        }
    }

    this->drawSelectionBox(this->gsSpritePaletteScene[this->iSpritePaletteSelected], QPoint(this->iSpritePaletteSelectedIndex*16,0));

    emit(newSpritePalette0(this->gsSpritePaletteScene[0]));
    emit(newSpritePalette1(this->gsSpritePaletteScene[1]));
    emit(newSpritePalette2(this->gsSpritePaletteScene[2]));
    emit(newSpritePalette3(this->gsSpritePaletteScene[3]));

    emit(newSpriteColours(this->createPaletteColours(), this->iSpritePaletteSelected));
}

void PaletteManager::sendRequestedPaletteUpdate(quint8 p)
{
    emit(newSpriteColours(this->createPaletteColours(), p));
}
