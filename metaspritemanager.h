#ifndef METASPRITEMANAGER_H
#define METASPRITEMANAGER_H

#include <QWidget>
#include <QGraphicsView>

#include <QDragMoveEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDragLeaveEvent>
#include <QMouseEvent>
#include <QMimeData>
#include <QMessageBox>

#include <QtMath>

#include "metaspritetileitem.h"

#define MSM_FILE_OPEN_ERROR_TITLE   "Error opening metasprite file"
#define MSM_FILE_OPEN_ERROR_BODY    "Could not open metasprite file. Please make sure you have the necessary permissions to access files in this location."

#define MSM_SCALE 3
#define MSM_TILESIZE (8*MSM_SCALE)

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

public slots:
    void setNewSpriteColours(QVector<QRgb>,quint8);
    void addNewTile(QPointF,QImage,quint8,quint8);
    void deleteSelectedTiles();
    void flipHorizontal();
    void flipVertical();

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
