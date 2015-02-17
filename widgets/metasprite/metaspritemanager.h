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

#include "common.h"
#include "metaspritetileitem.h"

class MetaspriteManager : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MetaspriteManager(QWidget *parent = 0);
    ~MetaspriteManager();

    qreal scale(){return this->iScale;}

signals:
    void requestNewTile(QPointF);
    void getTileUpdate(MetaspriteTileItem*);
    void getPaletteUpdate(MetaspriteTileItem*);
    void requestPaletteUpdates(quint8);
    void sendFrameData(MetaspriteTileList);

    void setMetaspriteLabel(QString);

    void updateList(GraphicsItemList,GraphicsItemList);
    void updateAnimationFrame();

public slots:
    void setScale(qreal s){this->iScale=s;}
    void setTallSprites(bool t){this->bTallSprites=t;}

    void setNewSpriteColours(PaletteVector,quint8,bool);
    void addNewTile(QPointF,QImage,quint8,quint8);
    void moveSelectedUp();
    void moveSelectedDown();
    void flipHorizontal();
    void flipVertical();
    void deleteSelectedTiles();

    void updateTiles();
    void swapMetaspriteStage(int);
    void createFrameData(int);

    QVector<QByteArray> createMetaspriteBinaryData();
    void openMetaspriteFile(QString);
    void importMetaspriteBinaryData(QVector<QByteArray>);
    void clearAllMetaspriteData();

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
    void sendTileUpdates();

    qreal iScale;
    bool bTallSprites;
    QGraphicsScene *gsMetasprite;
    MetaspriteStageList vMetaspriteStages;
    quint8 iMetaspriteStage;
};

#endif // METASPRITEMANAGER_H
