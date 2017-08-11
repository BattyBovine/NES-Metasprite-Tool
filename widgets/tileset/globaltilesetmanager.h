#ifndef GLOBALTILESETMANAGER_H
#define GLOBALTILESETMANAGER_H

#include <QWidget>
#include <QGraphicsView>
#include <QTimer>

#include <QDragMoveEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDragLeaveEvent>
#include <QMouseEvent>

#include <QFile>
#include <QFileSystemWatcher>
#include <QVector>
#include <QMimeData>
#include <QMessageBox>
#include <QPixmap>
#include <QRgb>
#include <QGraphicsPixmapItem>

#include <QtMath>

#include "tilesetcache.h"
#include "metaspritetileitem.h"
#include "palettemanager.h"
#include "chrthread.h"


#define GTSM_FILE_OPEN_ERROR_TITLE    "Error opening tileset file"
#define GTSM_FILE_OPEN_ERROR_BODY     "Could not open tileset file. Please make sure you have the necessary permissions to access files in this location."

#define GTSM_SCALE         1
#define GTSM_TILEWIDTH	   8
#define GTSM_TILESET_COUNT 8
#define GTSM_BANKS_COUNT   8
#define GTSM_ANIM_FRAMES   4
#define GTSM_ANIM_DELAY    67


class GlobalTilesetManager : public QGraphicsView
{
	Q_OBJECT
public:
	explicit GlobalTilesetManager(QWidget *parent = 0);
	~GlobalTilesetManager();

	bool openTilesetFile(QString);
	bool importTilesetBinaryData(QVector<QByteArray>);
	QString createTilesetASMData(QString);
	QByteArray createTilesetBinaryData();

signals:
	void tilesetChanged(QImage);
	void selectedTileChanged(quint32);
	void sendNewTile(QPointF,quint32,quint8);
#ifdef METATILEITEM_H
	void sendNewMetatile(MetatileItem*);
#endif
#ifdef METASPRITETILEITEM_H
	void sendNewSpriteTile(MetaspriteTileItem*);
#endif
	void chrBankChanged(quint16);
	void chrDataChanged(QImage);
	void checkTilesBank(quint16,quint16);
	void metaspriteTileUpdated(MetaspriteTileItem*);
	void saveOpenedChrFile(QString);

	void bankChanged(quint16);
	void setGlobalTilesetSelectedIndex(int);
	void newAnimationFrame(int);

public slots:
	void loadCHRData(QString filename="");

	void getNewTile(QPointF);
	void getNewCHRData(QImage img);
	void updateCHRPalettes(quint8);
	void getCHRError(QString,QString);
	void getBankSize(int);
	void setNewBank(quint16);
	void setTallSprites(bool);
	void setNewSpriteColours(PaletteVector,quint8);

	void reloadCurrentTileset();

protected:
	void resizeEvent(QResizeEvent*);
	void dragMoveEvent(QDragMoveEvent *e){e->accept();}
	void dragEnterEvent(QDragEnterEvent *e){e->acceptProposedAction();}
	void dropEvent(QDropEvent*);
	void dragLeaveEvent(QDragLeaveEvent *e){e->accept();}
	void mousePressEvent(QMouseEvent*);
	void wheelEvent(QWheelEvent*);

private:
	bool drawBankDivider();
	bool drawSelectionBox();

	QGraphicsScene *gsTileset;
	QString sCurrentTilesetFile;
	QFileSystemWatcher fswCHR;
	CHRThread *threadCHR;

	QImage imgTileset;
	QGraphicsPixmapItem *gpiTileset;
	QPixmap pixLocalCache[GTSM_ANIM_FRAMES];
	quint32 iSelectedTile;
	quint16 iSelectedBank,iMaxBank;
	quint8 iSelectedPalette;
	quint8 iBankDivider;
	bool bTallSprite;
	qreal iScale;
	PaletteVector vPaletteLists;

	QGraphicsRectItem *griSelection[2];
	QPointF pSelection;
};

#endif // GLOBALTILESETMANAGER_H
