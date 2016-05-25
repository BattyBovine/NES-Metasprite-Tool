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
	this->iBankDivider = 0x100;
	this->iSelectedBank = 0;

	this->pSelection = QPointF(0,0);
	this->drawSelectionBox();

	this->threadCHR = new CHRThread();
	connect(this->threadCHR,SIGNAL(sendCHRImageData(QImage)),this,SLOT(getNewCHRData(QImage)));
	connect(this->threadCHR,SIGNAL(error(QString,QString)),this,SLOT(getCHRError(QString,QString)));

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
			this->loadCHRData(url.toLocalFile());
			return;
		}
	}
	this->loadCHRData(e->mimeData()->urls()[0].toLocalFile());
}

void TilesetManager::mousePressEvent(QMouseEvent *e)
{
	this->pSelection = this->mapToScene(e->pos());
	this->drawSelectionBox();
}

void TilesetManager::wheelEvent(QWheelEvent *e)
{
	qreal steps = -(((qreal)e->angleDelta().y()/8)/15);

	if(((this->iSelectedBank+steps)>=1) && ((this->iSelectedBank+steps)<=((this->imgTileset.height()/(this->iBankDivider>>1))-1))) {
		this->iSelectedBank += steps;
	} else {
		this->iSelectedBank = ((steps<0)?0:((this->imgTileset.height()/(this->iBankDivider>>1))-1));
	}

	this->loadCHRBank();

	emit(this->chrBankChanged(this->iSelectedBank));
}

void TilesetManager::setSelectedBank(quint16 bankno)
{
	quint32 bankmax = ((this->imgTileset.height()/(this->iBankDivider>>1))-1);
	quint32 newbank = ((bankno>=bankmax)?bankmax:bankno);

	if(!(this->iSelectedBank==bankno)) {
		this->iSelectedBank = newbank;
		this->loadCHRBank();
	}
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
	quint16 yorigin = (qFloor(this->pSelection.y())-(qFloor(this->pSelection.y())%TSM_TILESIZE))%(this->iBankDivider);
	this->pSelection = QPointF(xorigin,yorigin);
	this->iSelectedTile = ((qFloor((yorigin/TSM_TILESIZE)<<4)|qFloor(xorigin/TSM_TILESIZE))%this->iBankDivider);

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



void TilesetManager::loadCHRData(QString filename)
{
	this->threadCHR->loadFile(filename);
	if(!this->fswCHR.files().isEmpty()) this->fswCHR.removePath(this->sCurrentTilesetFile);
	this->sCurrentTilesetFile = filename;
	this->fswCHR.addPath(filename);
	return;
}

void TilesetManager::loadCHRBank()
{
	this->imgSelectedBank = this->imgTileset.copy(QRect(0,((this->iBankDivider>>1)*this->iSelectedBank),128,(this->iBankDivider>>1)));
	this->setFixedSize(256,this->iBankDivider);
	this->setSceneRect(0,0,256,this->iBankDivider);

	this->gpiTileset->setPixmap(QPixmap::fromImage(this->imgSelectedBank));
	emit(this->tilesetChanged(this->bTallSprite));
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
	quint32 selection = this->iSelectedTile+(this->iBankDivider*this->iSelectedBank);
	emit(sendNewTile(p,this->createNewTile(selection),selection,this->iPalette));
}

void TilesetManager::updateSpriteTile(MetaspriteTileItem *t)
{
	t->setTile(this->createNewTile((t->tileIndex()+(this->iSelectedBank*this->iBankDivider))&(t->tallSprite()?0xFFFFFFFE:0xFFFFFFFF)));
}

void TilesetManager::getNewCHRData(QImage img)
{
	img.setColor(1,this->imgTileset.color(1));
	img.setColor(2,this->imgTileset.color(2));
	img.setColor(3,this->imgTileset.color(3));
	this->imgTileset = img;

	this->loadCHRBank();
	emit(this->chrDataChanged(this->imgTileset));
}

void TilesetManager::getCHRError(QString title,QString body)
{
	QMessageBox::warning(NULL,title,body,QMessageBox::NoButton);
}

void TilesetManager::getBankDivider(quint16 bankdiv)
{
	qreal diff = qreal(this->iBankDivider)/qreal(bankdiv);
	this->iBankDivider = bankdiv;
	this->iSelectedBank *= diff;

	this->loadCHRBank();
}



QImage TilesetManager::createNewTile(quint32 tile)
{
	QImage newtile(MSTI_TILEWIDTH, MSTI_TILEWIDTH*(this->bTallSprite?2:1), QImage::Format_Indexed8);

	QImage toptile = (this->imgSelectedBank.copy((tile&0x0F)*MSTI_TILEWIDTH,((((tile%this->iBankDivider)&0xF0)>>4)*MSTI_TILEWIDTH)%this->imgSelectedBank.height(),MSTI_TILEWIDTH,MSTI_TILEWIDTH));
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
		QImage antoniostellabottomtile = (this->imgSelectedBank.copy(((tile+1)&0x0F)*MSTI_TILEWIDTH,((((tile%this->iBankDivider)&0xF0)>>4)*MSTI_TILEWIDTH)%this->iBankDivider,MSTI_TILEWIDTH,MSTI_TILEWIDTH));
		for(quint8 y=0; y<MSTI_TILEWIDTH; y++) {
			for(quint8 x=0; x<MSTI_TILEWIDTH; x++) {
				newtile.setPixel(x,y+MSTI_TILEWIDTH,antoniostellabottomtile.pixelIndex(x,y));
			}
		}
	}

	return newtile;
}
