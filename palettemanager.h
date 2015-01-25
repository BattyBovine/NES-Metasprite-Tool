#ifndef PALETTEMANAGER_H
#define PALETTEMANAGER_H

#include <QGraphicsView>
#include <QMouseEvent>

#include <QMimeData>
#include <QMessageBox>

#include <QtMath>

#include "metaspritetileitem.h"

class PaletteManager : public QGraphicsView
{
    Q_OBJECT
public:
    explicit PaletteManager(QWidget *parent = 0);
    ~PaletteManager();

    bool drawFullPaletteColours(QString);

    bool drawSelectionBox(QGraphicsScene*, QPointF);
    bool setPaletteData(QByteArray);
    QByteArray paletteData();

signals:
    void paletteSelected(quint8);
    void newSpritePalette0(QGraphicsScene*);
    void newSpritePalette1(QGraphicsScene*);
    void newSpritePalette2(QGraphicsScene*);
    void newSpritePalette3(QGraphicsScene*);
    void newSpriteColours(QVector<QRgb>,quint8);

public slots:
    void setPaletteFile(QString);
    void setSpritePaletteScene(QObject*);

    void spritePaletteSelected(QString, quint8);
    QVector<QRgb> createPaletteColours();
    void generateNewSpritePalettes();
    void setNewSpritePalette(MetaspriteTileItem*);
    void sendRequestedPaletteUpdate(quint8);

protected:
    void dragMoveEvent(QDragMoveEvent*e){e->accept();}
    void dragEnterEvent(QDragEnterEvent*e){e->acceptProposedAction();}
    void dropEvent(QDropEvent*);
    void dragLeaveEvent(QDragLeaveEvent*e){e->accept();}
    void mousePressEvent(QMouseEvent*);


private:
    QString sPaletteFile;
    QVector<QColor> vPaletteColours;
    QGraphicsScene *gsFullPaletteScene;

    QGraphicsScene *gsSpritePaletteScene[4];
    quint8 iSpritePaletteIndices[4][4];
    quint8 iSpritePaletteSelected;
    quint8 iSpritePaletteSelectedIndex;
};

#endif // PALETTEMANAGER_H
