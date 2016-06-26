#include "metaspritemanager.h"

MetaspriteManager::MetaspriteManager(QWidget *parent) : QGraphicsView(parent)
{
	this->gsMetasprite = new QGraphicsScene(this);
	this->setScene(this->gsMetasprite);
	this->iScale = MSM_DEFAULT_ZOOM;
	this->bTallSprites = this->bSnapToGrid = false;
	this->bShowGrid = true;
	this->iBankDivider = 0x100;
	this->iSelectedBank = 0;

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
	QList<QGraphicsItem*> sel = this->gsMetasprite->selectedItems();

	switch(e->button()) {
	case Qt::RightButton:
		if(this->vMetaspriteStages[this->iMetaspriteStage].size()==64) {
			int response = QMessageBox::warning(this, MSM_SPRITELIMIT_ERROR_TITLE, MSM_SPRITELIMIT_ERROR_BODY, QMessageBox::Yes|QMessageBox::No, QMessageBox::No);
			if(response==QMessageBox::No)   return;
		}
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
			MetaspriteTileItem *icast = qgraphicsitem_cast<MetaspriteTileItem*>(i);
			qreal xnew = icast->realX();
			qreal ynew = icast->realY();
			if(this->bSnapToGrid) {
				xnew = roundToMult(xnew,MSTI_TILEWIDTH);
				ynew = roundToMult(ynew,MSTI_TILEWIDTH);
			}
			icast->setRealX(xnew);
			icast->setRealY(ynew);
		}
	}

	this->sendTileUpdates();
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

	this->setSceneRect(-MSM_CANVAS_SIZE*this->iScale,
					   -MSM_CANVAS_SIZE*this->iScale,
					   MSM_CANVAS_SIZE*this->iScale*2,
					   MSM_CANVAS_SIZE*this->iScale*2);
}

void MetaspriteManager::mouseReleaseEvent(QMouseEvent *e)
{
	QGraphicsView::mouseReleaseEvent(e);
}

void MetaspriteManager::keyPressEvent(QKeyEvent *e)
{
	switch(e->key()) {
	case Qt::Key_Left:
	case Qt::Key_Right:
		this->moveSelectedX((e->key()==Qt::Key_Right),(e->modifiers()&Qt::ShiftModifier));
		break;
	case Qt::Key_Up:
	case Qt::Key_Down:
		this->moveSelectedY((e->key()==Qt::Key_Down),(e->modifiers()&Qt::ShiftModifier));
		break;
	case Qt::Key_PageUp:
		this->moveSelectedUp();
		break;
	case Qt::Key_PageDown:
		this->moveSelectedDown();
		break;
//	case Qt::Key_H:
//		this->flipHorizontal();
//		break;
//	case Qt::Key_V:
//		this->flipVertical();
//		break;
	default:
		QGraphicsView::keyPressEvent(e);
	}
}



void MetaspriteManager::selectAllSprites()
{
	QList<QGraphicsItem*> items = this->gsMetasprite->items();
	foreach(QGraphicsItem *i, items) {
		i->setSelected(true);
	}
	this->sendTileUpdates();
}

void MetaspriteManager::deselectAllSprites()
{
	QList<QGraphicsItem*> items = this->gsMetasprite->items();
	foreach(QGraphicsItem *i, items) {
		i->setSelected(false);
	}
	this->sendTileUpdates();
}

void MetaspriteManager::copySpritesToClipboard(bool cut)
{
	QList<QGraphicsItem*> items = this->gsMetasprite->items(Qt::AscendingOrder);
	this->mtlClipboard.clear();
	foreach(QGraphicsItem *i, items) {
		if(i->isSelected()) {
			MetaspriteTileItem *icast = qgraphicsitem_cast<MetaspriteTileItem*>(i);
			this->mtlClipboard.append(icast);
			if(cut) this->gsMetasprite->removeItem(i);
		}
	}
	this->sendTileUpdates();
}

void MetaspriteManager::pasteSpritesFromClipboard()
{
	foreach(MetaspriteTileItem *i, this->mtlClipboard) {
		MetaspriteTileItem *newitem = new MetaspriteTileItem();
		newitem->setFlags(QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsSelectable);
		newitem->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
		newitem->setScale(i->scale());
		newitem->setRealX(i->realX());
		newitem->setRealY(i->realY());
		newitem->setTallSprite(this->bTallSprites);
		newitem->flipHorizontal(i->flippedHorizontal());
		newitem->flipVertical(i->flippedVertical());
		newitem->setPalette(i->palette());
		newitem->setTileIndex(i->tileIndex());
		emit(this->getTileUpdate(newitem));
		emit(this->getPaletteUpdate(newitem));
		this->gsMetasprite->addItem(newitem);
	}
	this->sendTileUpdates();
}

void MetaspriteManager::changePalette(int p)
{
	emit(requestPaletteUpdates(p));
}

void MetaspriteManager::moveSelectedX(bool right, bool shiftmod)
{
	int translatemult = 1;
	QList<QGraphicsItem*> sel = this->gsMetasprite->selectedItems();
	foreach(QGraphicsItem *i, sel) {
		if(this->bSnapToGrid) {
			qreal xdiff = getMultDiff(qgraphicsitem_cast<MetaspriteTileItem*>(i)->realX(),MSTI_TILEWIDTH);
			xdiff = right?((this->iScale*xdiff)+(MSTI_TILEWIDTH*this->iScale)):((this->iScale*xdiff)-(MSTI_TILEWIDTH*this->iScale));
			while(xdiff>MSTI_TILEWIDTH*this->iScale){xdiff-=MSTI_TILEWIDTH*this->iScale;}
			while(xdiff<-(MSTI_TILEWIDTH*this->iScale)){xdiff+=MSTI_TILEWIDTH*this->iScale;}
			i->moveBy(xdiff,0);
		} else {
			if(shiftmod)    translatemult=MSTI_TILEWIDTH;
			i->moveBy(right?(this->iScale*translatemult):(-this->iScale*translatemult),0);
		}
	}
	this->sendTileUpdates();
}

void MetaspriteManager::moveSelectedY(bool down, bool shiftmod)
{
	int translatemult = 1;
	QList<QGraphicsItem*> sel = this->gsMetasprite->selectedItems();
	foreach(QGraphicsItem *i, sel) {
		if(this->bSnapToGrid) {
			qreal ydiff = getMultDiff(qgraphicsitem_cast<MetaspriteTileItem*>(i)->realY(),MSTI_TILEWIDTH);
			ydiff = down?((this->iScale*ydiff)+(MSTI_TILEWIDTH*this->iScale)):((this->iScale*ydiff)-(MSTI_TILEWIDTH*this->iScale));
			while(ydiff>MSTI_TILEWIDTH*this->iScale){ydiff-=MSTI_TILEWIDTH*this->iScale;}
			while(ydiff<-(MSTI_TILEWIDTH*this->iScale)){ydiff+=MSTI_TILEWIDTH*this->iScale;}
			i->moveBy(0,ydiff);
		} else {
			if(shiftmod)    translatemult=MSTI_TILEWIDTH;
			i->moveBy(0,down?(this->iScale*translatemult):(-this->iScale*translatemult));
		}
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

void MetaspriteManager::addNewTile(QPointF p, QImage i, quint32 t, quint8 c)
{
	MetaspriteTileItem *pi = new MetaspriteTileItem(i);
	pi->setFlags(QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsSelectable);
	pi->setTallSprite(this->bTallSprites);
	pi->setScale(this->iScale);
	if(this->bSnapToGrid) {
		int realx = roundToMult(qRound(p.x()/this->iScale),MSTI_TILEWIDTH);
		int realy = roundToMult(qRound(p.y()/this->iScale),MSTI_TILEWIDTH);
		if(p.x()<0) realx-=MSTI_TILEWIDTH;
		if(p.y()<0) realy-=MSTI_TILEWIDTH;
		pi->setRealX(realx);
		pi->setRealY(realy);
	} else {
		pi->setRealX(qRound(p.x()/this->iScale));
		pi->setRealY(qRound(p.y()/this->iScale));
	}
	pi->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
	pi->setTileIndex(t&(this->bTallSprites?0xFFFFFFFE:0xFFFFFFFF));
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

	if(this->bShowGrid) this->drawGridLines();

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

void MetaspriteManager::selectFirstMetaspriteStage()
{
	emit(this->sendMetaspriteStageChange(0));
}

void MetaspriteManager::selectNextEmptyMetaspriteStage()
{
	for(int i=0; i<this->vMetaspriteStages.size(); i++) {
		if(this->vMetaspriteStages.at(i).isEmpty()) {
			emit(this->sendMetaspriteStageChange(i));
			break;
		}
	}
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



void MetaspriteManager::toggleShowGrid(bool showgrid)
{
	this->bShowGrid = showgrid;
	this->updateMetaspriteStage();
}

void MetaspriteManager::toggleSnapToGrid(bool snaptogrid)
{
	this->bSnapToGrid = snaptogrid;
}

void MetaspriteManager::setBankDivider(int banksizeindex)
{
	this->iBankDivider = (0x100/(1<<banksizeindex));
	emit(this->bankDividerChanged(this->iBankDivider));
}

void MetaspriteManager::setSelectedBank(quint16 bankno)
{
	this->iSelectedBank = bankno;
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
				quint8 oamindex = ms->tileIndex()%this->iBankDivider;
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
	QString databanks = asmlabel+"bank:\n\t.byte ";
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
		quint8 oamfullindex;
		foreach(MetaspriteTileItem *mti, mslist) {
			quint8 oamx = mti->realX();
			quint8 oamy = mti->realY();
			oamfullindex = (oamfullindex>mti->tileIndex()) ? oamfullindex : mti->tileIndex();
			quint8 oamindex = mti->tileIndex()%this->iBankDivider;
			quint8 oamattr = mti->palette()|(mti->flippedHorizontal()?0x40:0x00)|(mti->flippedVertical()?0x80:0x00);
			databytes += QString(",$%1").arg(oamy,2,16,QChar('0')).toUpper();
			databytes += QString(",$%1").arg(oamindex,2,16,QChar('0')).toUpper();
			databytes += QString(",$%1").arg(oamattr,2,16,QChar('0')).toUpper();
			databytes += QString(",$%1").arg(oamx,2,16,QChar('0')).toUpper();
		}
		databanks += QString("$%1").arg(int(floor(oamfullindex/this->iBankDivider)),2,16,QChar('0')).append(",");
		oamfullindex = 0;
	}

	datatable_hi.remove(datatable_hi.size()-1,1);
	datatable_lo.remove(datatable_lo.size()-1,1);
	databanks.remove(databanks.size()-1,1);
	datatable_hi += "\n";
	datatable_lo += "\n";
	databanks += "\n";
	databytes += "\n";
	databytes += asmlabel+"end:\n";

	return datatable_hi+datatable_lo+databanks+databytes;
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
	QByteArray bankbytes;
	QString labelname;
	while(!file.atEnd()) {
		QString line = file.readLine();

		QRegularExpression banklabel("^(.*?)_bank:$");
		QRegularExpressionMatch banklabelmatch = banklabel.match(line);
		if(banklabelmatch.hasMatch()) {
			QString line = file.readLine();
			QRegularExpression bytes(",?\\$([0-9a-fA-F]+)");
			QRegularExpressionMatchIterator bytesiter = bytes.globalMatch(line);
			while(bytesiter.hasNext()) {
				QRegularExpressionMatch bytesmatch = bytesiter.next();
				bankbytes.append(quint8(bytesmatch.captured(1).toUInt(NULL,16)));
			}
		}

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
	if(!labelname.isEmpty() && !bankbytes.isEmpty()) {
		foreach(QByteArray test, inputbytes) {
			if(!test.isEmpty()) {
				this->importMetaspriteBinaryData(inputbytes,bankbytes);
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

void MetaspriteManager::importMetaspriteBinaryData(QVector<QByteArray> bindata, QByteArray banks)
{
	int blankcounter = 0;
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
			ms->setTileIndex((oamindex&(this->bTallSprites?0xFE:0xFF))+(this->iBankDivider*banks[j-blankcounter]));
			ms->setPalette(oamattr&0x03);
			ms->flipHorizontal((oamattr&0x40)?true:false);
			ms->flipVertical((oamattr&0x80)?true:false);
			emit(this->getTileUpdate(ms));
			emit(this->getPaletteUpdate(ms));
			mslist.append(ms);
		}
		this->vMetaspriteStages.replace(j,mslist);
		if(mslist.isEmpty()) blankcounter += 1;
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
	if(!this->vMetaspriteStages[this->iMetaspriteStage].isEmpty()) {
		quint16 currentbank = (this->vMetaspriteStages.at(this->iMetaspriteStage)[0]->tileIndex())/this->iBankDivider;
		emit(this->updateSpriteBank(currentbank));
	}

	foreach(MetaspriteTileItem *i, this->vMetaspriteStages[this->iMetaspriteStage]) {
		emit(this->getPaletteUpdate(i));
	}
	emit(this->updateList(this->gsMetasprite->items(),this->gsMetasprite->selectedItems()));
	emit(this->updateAnimationFrame());
}

void MetaspriteManager::checkTilesBank(quint16 newbank, quint16 maxbank)
{
	this->iSelectedBank = newbank;
	quint32 maxtileindex = (maxbank+1)*this->iBankDivider;
	bool update = false;
	MetaspriteTileList list = this->vMetaspriteStages.at(this->iMetaspriteStage);
	foreach(MetaspriteTileItem *i, list) {
		if(maxtileindex && i->tileIndex()>maxtileindex) {
			update = true;
			i->setTileIndex(i->tileIndex()%maxtileindex);
		}
	}
	if(update)	this->sendTileUpdates();
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
		newitem->setTileIndex(i->tileIndex());
		emit(this->getPaletteUpdate(newitem));
		listcopy.append(newitem);
	}
	return listcopy;
}
