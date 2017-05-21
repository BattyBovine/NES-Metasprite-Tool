#include "globaltilesetmanager.h"

GlobalTilesetManager::GlobalTilesetManager(QWidget *parent) : QGraphicsView(parent)
{
	this->gsTileset = new QGraphicsScene();
	this->setScene(this->gsTileset);

	this->imgTileset = QImage(128, 128, QImage::Format_Indexed8);

	this->gpiTileset = new QGraphicsPixmapItem();
	this->gsTileset->addItem(this->gpiTileset);
	this->griSelection[0] = this->griSelection[1] = NULL;
	this->iSelectedTile = this->iSelectedPalette = this->iSelectedBank = 0;
	this->iBankDivider = 1;

	this->pSelection = QPointF(0,0);

	this->threadCHR = new CHRThread();
	connect(this->threadCHR,SIGNAL(sendCHRImageData(QImage)),this,SLOT(getNewCHRData(QImage)));
	connect(this->threadCHR,SIGNAL(error(QString,QString)),this,SLOT(getCHRError(QString,QString)));

	connect(&this->fswCHR,SIGNAL(fileChanged(QString)),this,SLOT(reloadCurrentTileset()));
}

GlobalTilesetManager::~GlobalTilesetManager()
{
	this->gsTileset->removeItem(this->gpiTileset);
	this->threadCHR->deleteLater();
	delete this->gpiTileset;
	delete this->gsTileset;
}



void GlobalTilesetManager::resizeEvent(QResizeEvent*)
{
	QRectF viewrect = this->mapToScene(this->rect()).boundingRect();
	this->iScale = qFloor(qMin(viewrect.width(),viewrect.height())/128);
	this->gpiTileset->setScale(this->iScale);
	this->setSceneRect(0,0,this->imgTileset.width()*this->iScale,qFloor(128/this->iBankDivider)*this->iScale);
	this->drawSelectionBox();
}

void GlobalTilesetManager::dropEvent(QDropEvent *e)
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

void GlobalTilesetManager::mousePressEvent(QMouseEvent *e)
{
	this->pSelection = this->mapToScene(e->pos());
	this->pSelection.setX(qFloor(pSelection.x()/this->iScale));
	this->pSelection.setY(qFloor(pSelection.y()/this->iScale));
	this->drawSelectionBox();
}

void GlobalTilesetManager::wheelEvent(QWheelEvent *e)
{
	int steps = -qFloor(((qreal)e->angleDelta().y()/8)/15);

	this->setNewBank(((this->iSelectedBank+steps)<0)?0:((this->iSelectedBank+steps)>this->iMaxBank)?this->iMaxBank:(this->iSelectedBank+steps));

	emit(bankChanged(this->iSelectedBank));
}



bool GlobalTilesetManager::drawSelectionBox()
{
	quint8 xorigin = qFloor(this->pSelection.x()/GTSM_TILEWIDTH)&(this->bTallSprite?0xFE:0xFF);
	quint8 yorigin = qFloor(this->pSelection.y()/GTSM_TILEWIDTH)%qFloor(this->imgTileset.width()/GTSM_TILEWIDTH/this->iBankDivider);
	if((xorigin < 0 || xorigin>=qFloor(this->imgTileset.width()/GTSM_TILEWIDTH)) ||
		(yorigin < 0 || yorigin>=qFloor(this->imgTileset.width()/GTSM_TILEWIDTH/this->iBankDivider)))
		return false;

	this->iSelectedTile = ((yorigin*(this->imgTileset.width()/GTSM_TILEWIDTH))+xorigin);

	if(this->griSelection[0]/* && this->griSelection[1]*/) {
		if(this->griSelection[0]->parentItem()) this->gsTileset->removeItem(this->griSelection[0]);
//		if(this->griSelection[1]->parentItem()) this->gsTileset->removeItem(this->griSelection[1]);
		delete this->griSelection[0];
//		delete this->griSelection[1];
		this->griSelection[0] = NULL;
//		this->griSelection[1] = NULL;
	}

//	QPen dashes(Qt::black,1,Qt::DashLine);
//	QVector<qreal> dp;
//	dp << 2 << 2;
//	dashes.setDashPattern(dp);
	this->griSelection[0] = this->gsTileset->addRect(QRectF(xorigin*GTSM_TILEWIDTH*this->iScale,yorigin*GTSM_TILEWIDTH*this->iScale,((GTSM_TILEWIDTH*this->iScale)-1)*(this->bTallSprite?2:1),(GTSM_TILEWIDTH*this->iScale)-1),QPen(Qt::red),Qt::NoBrush);
//	this->griSelection[1] = this->gsTileset->addRect(QRectF(xorigin*GTSM_TILEWIDTH*this->iScale,yorigin*GTSM_TILEWIDTH*this->iScale,((GTSM_TILEWIDTH*this->iScale)-1)*(this->bTallSprite?2:1),(GTSM_TILEWIDTH*this->iScale)-1),dashes,Qt::NoBrush);

	emit(selectedTileChanged(this->iSelectedTile));

	return true;
}

void GlobalTilesetManager::reloadCurrentTileset()
{
	threadCHR->loadFile(this->sCurrentTilesetFile);
}



void GlobalTilesetManager::loadCHRData(QString filename)
{
	if(!filename.isEmpty()) {
		if(!this->fswCHR.files().isEmpty()) this->fswCHR.removePath(this->sCurrentTilesetFile);
		this->sCurrentTilesetFile = filename;
		this->fswCHR.addPath(this->sCurrentTilesetFile);
	}
	this->threadCHR->loadFile(this->sCurrentTilesetFile);
}



void GlobalTilesetManager::getNewTile(QPointF p)
{
	emit(sendNewTile(p,this->iSelectedTile,this->iSelectedPalette));
}

void GlobalTilesetManager::getNewCHRData(QImage img)
{
	this->imgTileset = img;
	for(quint8 pal=0; pal<PM_SUBPALETTES_MAX; pal++) {
		this->updateCHRPalettes(pal);
	}
	emit(bankChanged(this->iSelectedBank));
	this->iMaxBank = qFloor(this->imgTileset.height()/qFloor(128/this->iBankDivider))-1;
	this->resizeEvent(NULL);
}

void GlobalTilesetManager::updateCHRPalettes(quint8 pal)
{
	int bankheight = qFloor(128/this->iBankDivider);
	for(quint16 bank=0; bank<(this->imgTileset.height()/bankheight); bank++) {
		uchar *src = this->imgTileset.bits();
		QImage destimg = QImage(this->imgTileset.width(), bankheight, QImage::Format_Indexed8);
		destimg.fill(0);
		uchar *dest = destimg.bits();
		for(int y=0; y<bankheight; y++) {
			for(int x=0; x<128; x++) {
				int destpixel = ((128*y)+x);
				int srcpixel = ((128*bankheight*bank)+((128*y)+x));
				dest[destpixel] = src[srcpixel];
			}
		}
		destimg.setColor(0,qRgba(0x00,0x00,0x00,0x00));
		destimg.setColor(1,this->vPaletteLists[pal*PM_SUBPALETTES_MAX+1]);
		destimg.setColor(2,this->vPaletteLists[pal*PM_SUBPALETTES_MAX+2]);
		destimg.setColor(3,this->vPaletteLists[pal*PM_SUBPALETTES_MAX+3]);
		TilesetCache::insert(bank,pal,QPixmap::fromImage(destimg));
	}
	this->gpiTileset->setPixmap(TilesetCache::find(this->iSelectedBank,this->iSelectedPalette));
}

void GlobalTilesetManager::getCHRError(QString title,QString body)
{
	QMessageBox::warning(NULL,title,body,QMessageBox::NoButton);
}

void GlobalTilesetManager::getBankSize(int bankdiv)
{
	this->iBankDivider = qPow(2,bankdiv);
	this->getNewCHRData(this->imgTileset);
	if(this->iSelectedBank>this->iMaxBank)
		setNewBank(this->iMaxBank);
}

void GlobalTilesetManager::setNewBank(quint16 bank)
{
	this->iSelectedBank = bank;
	this->gpiTileset->setPixmap(TilesetCache::find(this->iSelectedBank,this->iSelectedPalette));
}

void GlobalTilesetManager::setTallSprites(bool tallsprites)
{
	this->bTallSprite = tallsprites;
	this->drawSelectionBox();
}

void GlobalTilesetManager::setNewSpriteColours(PaletteVector c, quint8 i)
{
	this->vPaletteLists = c;
	this->iSelectedPalette = i;
	this->setBackgroundBrush(QBrush(QColor(this->vPaletteLists[0])));
	this->updateCHRPalettes(this->iSelectedPalette);
}



bool GlobalTilesetManager::openTilesetFile(QString filename)
{
	QFile file(filename);
	if(!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
		QMessageBox::warning(this,tr(GTSM_FILE_OPEN_ERROR_TITLE),tr(GTSM_FILE_OPEN_ERROR_BODY),QMessageBox::NoButton);
		return false;
	}
	QVector<QByteArray> inputbytes;
	while(!file.atEnd()) {
		QString line = file.readLine();

		QRegularExpression bytes(",?\\$([0-9a-fA-F]+)");
		QRegularExpressionMatchIterator bytesiter = bytes.globalMatch(line);
		QByteArray bytesin;
		while(bytesiter.hasNext()) {
			QRegularExpressionMatch bytesmatch = bytesiter.next();
			bytesin.append(quint8(bytesmatch.captured(1).toUInt(NULL,16)));
		}
		if(!bytesin.isEmpty() && bytesin.size()==this->iBankDivider) {
			inputbytes.append(bytesin);
		}
	}

	file.close();
	if(inputbytes.isEmpty()) {
		if(!file.open(QIODevice::ReadOnly)) {
			QMessageBox::warning(this,tr(GTSM_FILE_OPEN_ERROR_TITLE),tr(GTSM_FILE_OPEN_ERROR_BODY),QMessageBox::NoButton);
			return false;
		}
		while(!file.atEnd()) {
			inputbytes.append(file.read(this->iBankDivider));
		}
	}

	this->importTilesetBinaryData(inputbytes);
	return true;
}

bool GlobalTilesetManager::importTilesetBinaryData(QVector<QByteArray> inputbytes)
{
	for(int ts=0; ts<GTSM_TILESET_COUNT && ts<inputbytes.size(); ts++) {
		if(inputbytes[ts].size()!=this->iBankDivider) {
			QMessageBox::critical(this,tr(GTSM_FILE_OPEN_ERROR_TITLE),tr("The file is too ")+((inputbytes[ts].size()<this->iBankDivider)?tr("short"):tr("long"))+tr(" to be tileset data."),QMessageBox::NoButton);
			return false;
		}
	}
	return true;
}
