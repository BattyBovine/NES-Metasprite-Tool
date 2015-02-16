#ifndef NESMETASPRITETOOL_H
#define NESMETASPRITETOOL_H

#include <QMainWindow>
#include <QGraphicsRectItem>
#include <QMessageBox>
#include <QFileDialog>
#include <QDirIterator>
#include <QTextStream>
#include <QRegularExpression>

#define FILE_OPEN_ERROR_TITLE   "Error opening file"
#define FILE_OPEN_ERROR_BODY    "Could not open file. Please make sure you have the necessary permissions to access files in this location."
#define FILE_SAVE_ERROR_TITLE   "Error saving file"
#define FILE_SAVE_ERROR_BODY    "Could not save file. Please make sure you have the necessary permissions to save files to this location."

typedef QVector<QRgb> PaletteVector;
typedef QList<QGraphicsItem*> GraphicsItemList;

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

    void prevMetasprite();
    void nextMetasprite();
    void setAnimationLabel(QString);

private:
    Ui::NESMetaspriteTool *ui;
};

#endif // NESMETASPRITETOOL_H
