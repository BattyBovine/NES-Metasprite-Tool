#include "palettemanager.h"

PaletteManager::PaletteManager(QWidget *parent) : QGraphicsView(parent)
{
	this->gsFullPaletteScene = new QGraphicsScene();
	this->gsFullPaletteScene->setSceneRect(0,0,224,64);
	this->gsFullPaletteScene->setBackgroundBrush(QBrush(QColor(0x82,0x87,0x90)));
	this->iFullPaletteIndex = 0x0D;

	for(int i=0; i<PM_SUBPALETTES_MAX; i++) {
		this->gsSpritePaletteScene[i] = new QGraphicsScene();
		this->gsSpritePaletteScene[i]->setSceneRect(0,0,64,16);
	}

	this->iSpritePaletteSelected = this->iSpritePaletteSelectedIndex = 0;
	this->griSelectionBox[0] = this->griSelectionBox[1] = 0;

	this->setScene(this->gsFullPaletteScene);

	this->clearAllPaletteData();
}

PaletteManager::~PaletteManager()
{
	delete this->gsFullPaletteScene;
	for(int i=0; i<4; i++) {
		delete this->gsSpritePaletteScene[i];
	}
}



void PaletteManager::dropEvent(QDropEvent *e)
{
	e->acceptProposedAction();

	foreach(QUrl url, e->mimeData()->urls()) {
		if(url.toLocalFile().endsWith(".pal",Qt::CaseInsensitive)) {
			this->openPaletteFile(url.toLocalFile());
			return;
		}
	}
}

void PaletteManager::mousePressEvent(QMouseEvent *e)
{
	QPointF p = this->mapToScene(e->pos());
	if((p.x()<0||p.y()<0) || (p.x()>((0x0E*PM_SWATCH_SIZE)-1))||(p.y()>((0x04*PM_SWATCH_SIZE)-1)))  return;
	quint8 pval = (qFloor(p.y()/PM_SWATCH_SIZE)<<4)|(qFloor(p.x()/PM_SWATCH_SIZE));
	this->iFullPaletteIndex = pval;
	if(pval==0x0D)  pval = 0x0F;

	if(this->iSpritePaletteSelectedIndex == 0) {
		for(int i=0; i<4; i++) {
			this->iSpritePaletteIndices[i][0] = pval;
		}
	} else {
		this->iSpritePaletteIndices[this->iSpritePaletteSelected][this->iSpritePaletteSelectedIndex] = pval;
	}

	this->drawSelectionBox(this->gsFullPaletteScene, this->iFullPaletteIndex);
	this->generateNewSpritePalettes(false);
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

	this->iFullPaletteIndex = this->iSpritePaletteIndices[pindex][i];
	if(this->iFullPaletteIndex==0x0F)   this->iFullPaletteIndex=0x0D;
	this->drawSelectionBox(this->gsFullPaletteScene, this->iFullPaletteIndex);
	this->generateNewSpritePalettes(false);

	QVector<QColor> newspritecolours;
	for(int i=0; i<4; i++)  newspritecolours.append(this->vPaletteColours.at(this->iSpritePaletteIndices[pindex][i]));
}

PaletteVector PaletteManager::createPaletteColours()
{
	PaletteVector p;
	for(int sp=0; sp<PM_SUBPALETTES_MAX; sp++) {
		for(int c=0; c<PM_PALETTE_COLOURS_MAX; c++) {
			p.append(this->vPaletteColours.at(this->iSpritePaletteIndices[sp][c]).rgb());
		}
	}
	return p;
}

void PaletteManager::generateNewSpritePalettes(bool changeselected)
{
	for(int spritepal=0; spritepal<PM_SUBPALETTES_MAX; spritepal++) {
		this->gsSpritePaletteScene[spritepal]->clear();
		for(int i=0; i<PM_PALETTE_COLOURS_MAX; i++) {
			quint8 index = this->iSpritePaletteIndices[spritepal][i];
			if(index==0x0F) index=0x0D;
			this->gsSpritePaletteScene[spritepal]->addRect(QRectF((i*PM_SWATCH_SIZE),0,16,16),Qt::NoPen,this->vPaletteColours.at(index));
		}
	}

	this->drawSelectionBox(this->gsSpritePaletteScene[this->iSpritePaletteSelected], this->iSpritePaletteSelectedIndex);

	emit(newSpritePalette0(this->gsSpritePaletteScene[0]));
	emit(newSpritePalette1(this->gsSpritePaletteScene[1]));
	emit(newSpritePalette2(this->gsSpritePaletteScene[2]));
	emit(newSpritePalette3(this->gsSpritePaletteScene[3]));

	emit(newSpriteColours(this->createPaletteColours(), this->iSpritePaletteSelected, changeselected));
}

void PaletteManager::generateNewGlobalPalette()
{
	emit(newGlobalPalette(this->createPaletteColours()));
}

#ifdef METASPRITETILEITEM_H
void PaletteManager::setNewSpritePalette(MetaspriteTileItem *t)
{
	quint8 p = t->palette();
	QVector<QRgb> c = this->createPaletteColours();
	t->setPalette(p);
}
#endif

#ifdef METATILEITEM_H
void PaletteManager::setNewMetatilePalette(MetatileItem *t)
{
	t->setPalette(this->iSpritePaletteSelected);
//	emit(metatileUpdated(t));
}
#endif

void PaletteManager::sendRequestedPaletteUpdate(quint8 p)
{
	emit(newSpriteColours(this->createPaletteColours(), p, true));
}



bool PaletteManager::drawFullPaletteColours(QString palfile)
{
	this->sPaletteFile = palfile;

	this->gsFullPaletteScene->clear();

	QFile pal(":/pal/"+this->sPaletteFile+".pal");
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

	this->drawSelectionBox(this->gsFullPaletteScene, this->iFullPaletteIndex);

	this->generateNewSpritePalettes(false);

	return true;
}

bool PaletteManager::drawSelectionBox(QGraphicsScene *s, quint8 i)
{
	quint8 xorigin = (i&0x0F)*PM_SWATCH_SIZE;
	quint8 yorigin = ((i&0xF0)>>4)*PM_SWATCH_SIZE;

	QPen dashes(Qt::black,1,Qt::DashLine);
	QVector<qreal> dp;
	dp << 2 << 2;
	dashes.setDashPattern(dp);

	if(s==this->gsFullPaletteScene) {
		if(this->griSelectionBox[0] && s->items().contains(this->griSelectionBox[0]))
			this->gsFullPaletteScene->removeItem(this->griSelectionBox[0]);
		if(this->griSelectionBox[1] && s->items().contains(this->griSelectionBox[1]))
			this->gsFullPaletteScene->removeItem(this->griSelectionBox[1]);

		this->griSelectionBox[0] = s->addRect(QRectF(xorigin,yorigin,PM_SWATCH_SIZE-1,PM_SWATCH_SIZE-1),QPen(Qt::white),Qt::NoBrush);
		this->griSelectionBox[1] = s->addRect(QRectF(xorigin,yorigin,PM_SWATCH_SIZE-1,PM_SWATCH_SIZE-1),dashes,Qt::NoBrush);
	} else {
		s->addRect(QRectF(xorigin,yorigin,PM_SWATCH_SIZE-1,PM_SWATCH_SIZE-1),QPen(Qt::white),Qt::NoBrush);
		s->addRect(QRectF(xorigin,yorigin,PM_SWATCH_SIZE-1,PM_SWATCH_SIZE-1),dashes,Qt::NoBrush);
	}

	return true;
}

void PaletteManager::clearAllPaletteData()
{
	for(int i=0; i<PM_SUBPALETTES_MAX; i++) {
		this->iSpritePaletteIndices[i][0] = 0x0F;
		this->iSpritePaletteIndices[i][1] = 0x00;
		this->iSpritePaletteIndices[i][2] = 0x10;
		this->iSpritePaletteIndices[i][3] = 0x30;
	}
}



bool PaletteManager::openPaletteFile(QString filename)
{
	QFile file(filename);
	if(!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
		QMessageBox::warning(this,tr(PM_FILE_OPEN_ERROR_TITLE),tr(PM_FILE_OPEN_ERROR_BODY),QMessageBox::NoButton);
		return false;
	}
	QByteArray inputbytes = file.readAll();

	if(inputbytes.size()!=(PM_SUBPALETTES_MAX*PM_PALETTE_COLOURS_MAX)) {
		QMessageBox::critical(this,tr(PM_FILE_OPEN_ERROR_TITLE),tr("The file is too ")+((inputbytes.size()<(PM_SUBPALETTES_MAX*PM_PALETTE_COLOURS_MAX))?tr("short"):tr("long"))+tr(" to be palette data."),QMessageBox::NoButton);
		return false;
	}
	for(int i=0; i<PM_SUBPALETTES_MAX; i++) {
		for(int j=0; j<PM_PALETTE_COLOURS_MAX; j++) {
			this->iSpritePaletteIndices[i][j] = (inputbytes[j+(PM_PALETTE_COLOURS_MAX*i)]&0x3F);
		}
	}

	this->generateNewSpritePalettes(true);
	this->drawSelectionBox(this->gsFullPaletteScene, this->iSpritePaletteIndices[this->iSpritePaletteSelected][this->iSpritePaletteSelectedIndex]);
	return true;
}

QByteArray PaletteManager::createPaletteBinaryData()
{
	QByteArray indices;
	for(int i=0; i<PM_SUBPALETTES_MAX; i++) {
		for(int j=0; j<PM_PALETTE_COLOURS_MAX; j++) {
			indices.append(this->iSpritePaletteIndices[i][j]);
		}
	}
	return indices;
}
