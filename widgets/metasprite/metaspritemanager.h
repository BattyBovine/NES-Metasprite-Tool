#ifndef METASPRITEMANAGER_H
#define METASPRITEMANAGER_H

#include <QWidget>
#include <QGraphicsView>

#include <QDragMoveEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDragLeaveEvent>
#include <QFileInfo>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QMimeData>
#include <QMessageBox>

#include <QtMath>

#include "palettemanager.h"
#include "metaspritetileitem.h"
#include "animationframeitem.h"


#define roundToMult(x,f)    (f*qCeil(x/f))
#define getMultDiff(x,f)    (roundToMult(x,f)-x)


#define MSM_CANVAS_SIZE     512
#define MSM_DEFAULT_ZOOM    2
#define MSM_MAX_ZOOM        8

#define MSM_FILE_OPEN_ERROR_TITLE   "Error opening metasprite file"
#define MSM_FILE_OPEN_ERROR_BODY    "Could not open metasprite file. Please make sure you have the necessary permissions to access files in this location."
#define MSM_INVALID_SPRITES_TITLE   "Invalid Data"
#define MSM_INVALID_SPRITES_BODY    "Error reading metasprite data: Data is not a valid ASM sprite data file."
#define MSM_EOF_ERROR_TITLE         "Invalid data"
#define MSM_EOF_ERROR_BODY          "Error reading metasprite data: Unexpected end of file."
#define MSM_COUNT_ERROR_TITLE       "Invalid data"
#define MSM_COUNT_ERROR_BODY        "Error reading metasprite data: Sprite counts do not match length of data."
#define MSM_SPRITELIMIT_ERROR_TITLE "Too many sprites"
#define MSM_SPRITELIMIT_ERROR_BODY  "You already have 64 sprites on the stage. This is as much as the NES can handle. Any more added sprites will not be visible unless custom flickering is used. Are you sure you wish to continue?"


class MetaspriteManager : public QGraphicsView
{
	Q_OBJECT
public:
	explicit MetaspriteManager(QWidget *parent = 0);
	~MetaspriteManager();

	qreal scale(){return this->iScale;}
	void openMetaspriteFile(QString);
	void importMetaspriteBinaryData(QList<QByteArray>, QList<quint16>, quint8);
	void clearAllMetaspriteData();

	void selectAllSprites();
	void deselectAllSprites();
	void copySpritesToClipboard(bool);
	void pasteSpritesFromClipboard();
	void moveSelectedX(bool,bool);
	void moveSelectedY(bool,bool);

signals:
	void requestNewTile(QPointF);
	void getTileUpdate(MetaspriteTileItem*);
	void getPaletteUpdate(MetaspriteTileItem*);
	void sendFrameData(MetaspriteTileList);
	void sendAnimationFrameData(MetaspriteTileList);
	void sendMetaspriteStageChange(int);
	void sendMetaspriteBankChange(quint16);
	void sendSpriteSlotChange(int);
	void sendFrameIntentionallyBlank(bool);

	void setMetaspriteLabel(QString);

	void updateList(GraphicsItemList,GraphicsItemList);

public slots:
	void setScale(qreal s){this->iScale=s;}

	void setNewSpriteColours(PaletteVector,quint8,bool);
	void addNewTile(QPointF,quint32,quint8);
	void moveSelectedUp();
	void moveSelectedDown();
	void flipHorizontal();
	void flipVertical();
	void deleteSelectedTiles();

	void updateTiles(bool);
	void swapMetaspriteStage(int);
	void selectFirstMetaspriteStage();
	void selectNextEmptyMetaspriteStage();
	void createFrameData(quint8,qreal);
	void createAnimationFrameData(quint8,qreal);

	void setSprites(bool);
	void toggleShowGrid(bool);
	void toggleSnapToGrid(bool);
	void toggleChrTable1(bool);
	void getBankSize(int);
	void setBank(quint16);
	void setSpriteSlot(int);
	quint8 spriteSlot(){return this->iSpriteSlot;}
	void setPaletteForSelected(quint8);
	void setFrameIntentionallyBlank(bool);

	QVector<QByteArray> createMetaspriteBinaryData();
	QString createMetaspriteASMData(QString);

protected:
	void resizeEvent(QResizeEvent*);
	void dragMoveEvent(QDragMoveEvent*e){e->accept();}
	void dragEnterEvent(QDragEnterEvent*e){e->acceptProposedAction();}
	void dropEvent(QDropEvent*);
	void dragLeaveEvent(QDragLeaveEvent*e){e->accept();}
	void mousePressEvent(QMouseEvent*);
	void mouseMoveEvent(QMouseEvent*);
	void mouseReleaseEvent(QMouseEvent*);
	void mouseDoubleClickEvent(QMouseEvent*);
	void wheelEvent(QWheelEvent*);
	void keyPressEvent(QKeyEvent*);

private:
	void drawGridLines();
	void sendTileUpdates();
	MetaspriteTileList createFrame(quint8, qreal s=0);

	qreal iScale;
	int iMouseTranslateX, iMouseTranslateY;

	bool bTallSprites, bChrTable1, bShowGrid, bSnapToGrid;
	quint16 iBankDivider;
	quint16 iSelectedBank;
	QGraphicsScene *gsMetasprite;
	MetaspriteStageList vMetaspriteStages;
	QList<quint16> lMetaspriteBanks;
	QList<bool>	lFrameIntentionallyBlank;
	quint8 iMetaspriteStage;
	quint8 iSpriteSlot;

	MetaspriteTileList mtlClipboard;
};

#endif // METASPRITEMANAGER_H
