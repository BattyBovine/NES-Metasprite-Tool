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
#include <QMimeData>
#include <QMessageBox>

#include <QtMath>

#include "metaspritetileitem.h"

#define MSM_FILE_OPEN_ERROR_TITLE   "Error opening metasprite file"
#define MSM_FILE_OPEN_ERROR_BODY    "Could not open metasprite file. Please make sure you have the necessary permissions to access files in this location."
#define MSM_EOF_ERROR_TITLE         "Invalid data"
#define MSM_EOF_ERROR_BODY          "Error reading metasprite data: Unexpected end of file."
#define MSM_COUNT_ERROR_TITLE       "Invalid data"
#define MSM_COUNT_ERROR_BODY        "Error reading metasprite data: Sprite counts do not match length of data."

#define MSM_SCALE 3
#define MSM_TILESIZE (MSTI_TILEWIDTH*MSM_SCALE)

class MetaspriteManager : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MetaspriteManager(QWidget *parent = 0);
    ~MetaspriteManager();

signals:
    void requestNewTile(QPointF);
    void getTileUpdate(MetaspriteTileItem*);
    void getPaletteUpdate(MetaspriteTileItem*);
    void requestPaletteUpdates(quint8);

    void setMetaspriteLabel(QString);

    void updateList(QList<QGraphicsItem*>,QList<QGraphicsItem*>);

public slots:
    void setNewSpriteColours(QVector<QRgb>,quint8,bool);
    void addNewTile(QPointF,QImage,quint8,quint8);
    void moveSelectedUp();
    void moveSelectedDown();
    void flipHorizontal();
    void flipVertical();
    void deleteSelectedTiles();

    void updateTiles();
    void swapMetaspriteStage(int);

    QVector<QByteArray> createMetaspriteBinaryData();
    void openMetaspriteFile(QString);
    void importMetaspriteBinaryData(QVector<QByteArray>);

protected:
    void dragMoveEvent(QDragMoveEvent*e){e->accept();}
    void dragEnterEvent(QDragEnterEvent*e){e->acceptProposedAction();}
    void dropEvent(QDropEvent*);
    void dragLeaveEvent(QDragLeaveEvent*e){e->accept();}
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void keyPressEvent(QKeyEvent*);

private:
    void drawGridLines();

    QGraphicsScene *gsMetasprite;
    QVector< QList<MetaspriteTileItem*> > vMetaspriteStages;
    quint8 iMetaspriteStage;
};

#endif // METASPRITEMANAGER_H
