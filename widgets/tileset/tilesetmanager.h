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

#include "common.h"
#include "metaspritetileitem.h"

#define TSM_SCALE 2
#define TSM_TILESIZE    (MSTI_TILEWIDTH*TSM_SCALE)

class TilesetManager : public QGraphicsView
{
    Q_OBJECT
public:
    explicit TilesetManager(QWidget *parent = 0);
    ~TilesetManager();

signals:
    void tilesetChanged(bool);
    void sendNewTile(QPointF,QImage,quint8,quint8);

public slots:
    bool loadCHRBank(QString);
    void setNewSpriteColours(PaletteVector,quint8);
    void setStandardSprites(){this->bTallSprite=false;this->drawSelectionBox();emit(this->tilesetChanged(this->bTallSprite));}
    void setTallSprites(){this->bTallSprite=true;this->drawSelectionBox();emit(this->tilesetChanged(this->bTallSprite));}

    void getNewTile(QPointF);
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
    bool bTallSprite;

    QGraphicsRectItem *griSelection[2];
    QPointF pSelection;

    void redrawTileset();
    bool drawSelectionBox();
    QImage createNewTile(quint8);
};

#endif // TILESETMANAGER_H
