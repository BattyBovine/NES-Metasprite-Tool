#ifndef NESMETASPRITETOOL_H
#define NESMETASPRITETOOL_H

#include <QMainWindow>
#include <QGraphicsRectItem>
#include <QMessageBox>
#include <QFileDialog>
#include <QDirIterator>

#include "palettemanager.h"

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

private slots:
    void setNewPaletteFile(QString);
    void openCHR();
    void openPalette();
    void savePalette();

private:
    Ui::NESMetaspriteTool *ui;
};

#endif // NESMETASPRITETOOL_H
