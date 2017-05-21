#ifndef TILESETMANAGER_H
#define TILESETMANAGER_H

#include <QWidget>
#include <QGraphicsView>

#include <QDragMoveEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDragLeaveEvent>
#include <QMouseEvent>

#include <QFile>
#include <QFileSystemWatcher>
#include <QMimeData>
#include <QMessageBox>
#include <QPixmap>
#include <QGraphicsPixmapItem>

#include <QtMath>

#include "tilesetcache.h"
#include "globaltilesetmanager.h"
#include "palettemanager.h"
#include "metaspritetileitem.h"

#define TSM_SCALE       2
#define TSM_TILEWIDTH   8
#define TSM_TILES_W     16

class TilesetManager : public QGraphicsView
{
	Q_OBJECT
public:
	explicit TilesetManager(QWidget *parent = 0);
	~TilesetManager();

signals:
	void tilesetChanged(bool);
	void sendNewTile(QPointF,QImage,quint32,quint8);
	void sendNewMetasprite(MetaspriteTileItem*);
	void chrBankChanged(quint16);
	void tilesetChangedDelta(int);
	void chrDataChanged(QImage);
	void selectedTileChanged(int);
	void checkTilesBank(quint16,quint16);
	void metaspriteUpdated(MetaspriteTileItem*);

public slots:
	void loadCHRData(QString);
	void setNewSpriteColours(PaletteVector,quint8);
	void setSprites(bool tall){this->bTallSprite=tall;this->drawSelectionBox();emit(this->tilesetChanged(this->bTallSprite));}

	void getNewCHRData(QImage);
	void getCHRError(QString,QString);
	void getGlobalTileset(int);
	void getNewAnimationFrame(int);

	void reloadCurrentTileset();

protected:
	void resizeEvent(QResizeEvent*);
	void dragMoveEvent(QDragMoveEvent*e){e->accept();}
	void dragEnterEvent(QDragEnterEvent*e){e->acceptProposedAction();}
	void dropEvent(QDropEvent*);
	void dragLeaveEvent(QDragLeaveEvent*e){e->accept();}
	void mousePressEvent(QMouseEvent*);
	void wheelEvent(QWheelEvent*);

private:
	bool drawBankDivider();
	bool drawSelectionBox();

	QGraphicsScene *gsTileset;
	QString sCurrentTilesetFile;
	QImage imgTileset;
	PaletteVector pvCurrentColours;
	QGraphicsPixmapItem *gpiTileset;
	quint8 iGlobalTileset;
	quint8 iSelectedTile;
	quint8 iSelectedPalette;
	quint8 iBankDivider;
	quint8 iAnimFrame;
	bool bTallSprite;
	qreal iScale;

	QGraphicsRectItem *griSelection[2];
	QPointF pSelection;
};

#endif // TILESETMANAGER_H
