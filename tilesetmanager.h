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

#define TSM_SCALE 2
#define TSM_TILESIZE (8*TSM_SCALE)

class TilesetManager : public QGraphicsView
{
    Q_OBJECT
public:
    explicit TilesetManager(QWidget *parent = 0);
    ~TilesetManager();

signals:
    void sendTile(QPointF,QImage);

public slots:
    bool loadCHRBank(QString);
    void setNewSpriteColours(QRgb,QRgb,QRgb,QRgb);

    void createTile(QPointF);

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

    QGraphicsRectItem *griSelection[2];
    QPointF pSelection;

    void redrawTileset();
    bool drawSelectionBox();
};

#endif // TILESETMANAGER_H
