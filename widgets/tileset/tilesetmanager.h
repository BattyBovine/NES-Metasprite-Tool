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

#include "palettemanager.h"
#include "metaspritetileitem.h"
#include "chrthread.h"

#define TSM_SCALE       2
#define TSM_TILESIZE    (MSTI_TILEWIDTH*TSM_SCALE)

class TilesetManager : public QGraphicsView
{
	Q_OBJECT
public:
	explicit TilesetManager(QWidget *parent = 0);
	~TilesetManager();

signals:
	void tilesetChanged(bool);
	void sendNewTile(QPointF,QImage,quint32,quint8);
	void chrBankChanged(quint16);
	void chrDataChanged(QImage);
	void checkTilesBank(quint16,quint16);

public slots:
	void loadCHRData(QString);
	void loadCHRBank();
	void setNewSpriteColours(PaletteVector,quint8);
	void setSprites(bool tall){this->bTallSprite=tall;this->drawSelectionBox();emit(this->tilesetChanged(this->bTallSprite));}
	void setSelectedBank(quint16);

	void getNewTile(QPointF);
	void updateSpriteTile(MetaspriteTileItem*);
	void getNewCHRData(QImage);
	void getCHRError(QString,QString);
	void getBankDivider(quint16);

	void reloadCurrentTileset();

protected:
	void dragMoveEvent(QDragMoveEvent*e){e->accept();}
	void dragEnterEvent(QDragEnterEvent*e){e->acceptProposedAction();}
	void dropEvent(QDropEvent*);
	void dragLeaveEvent(QDragLeaveEvent*e){e->accept();}
	void mousePressEvent(QMouseEvent*);
	void wheelEvent(QWheelEvent*);

private:
	bool drawBankDivider();
	bool drawSelectionBox();
	QImage createNewTile(quint32);

	QGraphicsScene *gsTileset;
	QString sCurrentTilesetFile;
	QFileSystemWatcher fswCHR;
	CHRThread *threadCHR;
	QImage imgTileset;
	QImage imgSelectedBank;
	PaletteVector pvCurrentColours;
	QGraphicsPixmapItem *gpiTileset;
	quint32 iSelectedTile;
	quint8 iPalette;
	bool bTallSprite;
	quint16 iBankDivider;
	quint16 iSelectedBank;

	QGraphicsRectItem *griSelection[2];
	QPointF pSelection;
};

#endif // TILESETMANAGER_H
