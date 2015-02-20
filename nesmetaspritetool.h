#ifndef NESMETASPRITETOOL_H
#define NESMETASPRITETOOL_H

#include <QMainWindow>
#include <QGraphicsRectItem>
#include <QMessageBox>
#include <QFileDialog>
#include <QDirIterator>
#include <QTextStream>
#include <QRegularExpression>

#include "common.h"

namespace Ui {
class NESMetaspriteTool;
}

class NESMetaspriteTool : public QMainWindow
{
    Q_OBJECT

public:
    explicit NESMetaspriteTool(QWidget *parent = 0);
    ~NESMetaspriteTool();

signals:
    void chrFileOpened(QString);
    void metaspriteFileOpened(QString);

private slots:
    void newMetaspriteBank();
    void openMetaspriteBank();
    void saveASMMetaspriteBank();
    void saveBinaryMetaspriteBank();
    void openCHR();
    void openPalette();
    void savePalette();

    void setNewPaletteFile(QString);

    void setAnimationLabel(QString);

    void setPlayButtonText();
    void setPauseButtonText();

private:
    Ui::NESMetaspriteTool *ui;
};

#endif // NESMETASPRITETOOL_H
