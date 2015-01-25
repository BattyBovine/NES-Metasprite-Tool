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
#include <QMimeData>
#include <QMessageBox>
#include <QPixmap>
#include <QGraphicsPixmapItem>

#include <QtMath>

#include "metaspritetileitem.h"

#define TSM_SCALE 2
#define TSM_TILESIZE (8*TSM_SCALE)

class TilesetManager : public QGraphicsView
{
    Q_OBJECT
public:
    explicit TilesetManager(QWidget *parent = 0);
    ~TilesetManager();

signals:
    void tilesetChanged();
    void sendNewTile(QPointF,QImage,quint8,quint8);

public slots:
    bool loadCHRBank(QString);
    void setNewSpriteColours(QVector<QRgb>,quint8);

    void createNewTile(QPointF);
    void updateSpriteTile(MetaspriteTileItem*);

protected:
    void dragMoveEvent(QDragMoveEvent*e){e->accept();}
    void dragEnterEvent(QDragEnterEvent*e){e->acceptProposedAction();}
    void dropEvent(QDropEvent*);
    void dragLeaveEvent(QDragLeaveEvent*e){e->accept();}
    void mousePressEvent(QMouseEvent*);

private:
    QGraphicsScene *gsTileset;
    QImage imgTileset;
    QByteArray baTilesetData;
    QGraphicsPixmapItem *gpiTileset;
    quint8 iSelectedTile;
    quint8 iPalette;

    QGraphicsRectItem *griSelection[2];
    QPointF pSelection;

    void redrawTileset();
    bool drawSelectionBox();
};

#endif // TILESETMANAGER_H
