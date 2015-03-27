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


#define MSM_CANVAS_SIZE     128
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


class MetaspriteManager : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MetaspriteManager(QWidget *parent = 0);
    ~MetaspriteManager();

    qreal scale(){return this->iScale;}
    void openMetaspriteFile(QString);
    void importMetaspriteBinaryData(QVector<QByteArray>);
    void clearAllMetaspriteData();

signals:
    void requestNewTile(QPointF);
    void getTileUpdate(MetaspriteTileItem*);
    void getPaletteUpdate(MetaspriteTileItem*);
    void requestPaletteUpdates(quint8);
    void sendFrameData(MetaspriteTileList);
    void sendAnimationFrameData(MetaspriteTileList);

    void setMetaspriteLabel(QString);

    void updateList(GraphicsItemList,GraphicsItemList);
    void updateAnimationFrame();

public slots:
    void setScale(qreal s){this->iScale=s;}

    void setNewSpriteColours(PaletteVector,quint8,bool);
    void addNewTile(QPointF,QImage,quint8,quint8);
    void moveSelectedUp();
    void moveSelectedDown();
    void flipHorizontal();
    void flipVertical();
    void deleteSelectedTiles();

    void updateTiles(bool);
    void swapMetaspriteStage(int);
    void updateMetaspriteStage(){this->swapMetaspriteStage(this->iMetaspriteStage);}
    void createFrameData(quint8,qreal);
    void createAnimationFrameData(quint8,qreal);

    QVector<QByteArray> createMetaspriteBinaryData();
    QString createMetaspriteASMData(QString);

protected:
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

    bool bTallSprites;
    QGraphicsScene *gsMetasprite;
    MetaspriteStageList vMetaspriteStages;
    quint8 iMetaspriteStage;
};

#endif // METASPRITEMANAGER_H
