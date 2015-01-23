#ifndef METASPRITEMANAGER_H
#define METASPRITEMANAGER_H

#include <QWidget>
#include <QGraphicsView>

#include <QDragMoveEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDragLeaveEvent>
#include <QMouseEvent>

#include <QMessageBox>

#include <QtMath>

#include "metaspritetileitem.h"

#define MSM_SCALE 3
#define MSM_TILESIZE (8*MSM_SCALE)

class MetaspriteManager : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MetaspriteManager(QWidget *parent = 0);
    ~MetaspriteManager();

signals:
    void requestTile(QPointF);

public slots:
    void setNewSpriteColours(QVector<QRgb>,quint8);
    void addTile(QPointF,QImage);
    void deleteSelectedTiles();
    void flipHorizontal();
    void flipVertical();

protected:
    void dragMoveEvent(QDragMoveEvent*e){e->accept();}
    void dragEnterEvent(QDragEnterEvent*e){e->acceptProposedAction();}
//    void dropEvent(QDropEvent*);
    void dragLeaveEvent(QDragLeaveEvent*e){e->accept();}
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void keyPressEvent(QKeyEvent*);

private:
    void drawGridLines();

    QGraphicsScene *gsMetasprite;
    QVector<MetaspriteTileItem*> vItems;
};

#endif // METASPRITEMANAGER_H
