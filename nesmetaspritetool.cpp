#include "nesmetaspritetool.h"
#include "ui_nesmetaspritetool.h"

NESMetaspriteTool::NESMetaspriteTool(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NESMetaspriteTool)
{
    ui->setupUi(this);

    QStringList palettes;
    QDirIterator it(":", QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString next = it.next();
        if(next.endsWith(".pal",Qt::CaseInsensitive))
            palettes.append(next.replace(":/","").replace(".pal",""));
    }
    palettes.sort(Qt::CaseInsensitive);
    foreach(QString p, palettes) {
        ui->comboPalettes->addItem(p);
    }
}

NESMetaspriteTool::~NESMetaspriteTool()
{
    delete ui;
}



void NESMetaspriteTool::setNewPaletteFile(QString pal)
{
    ui->gvPaletteManager->drawFullPaletteColours(pal);
}

void NESMetaspriteTool::openCHR()
{
    QString filename = QFileDialog::getOpenFileName(this, ui->actionOpenCHR->text(), "", "*.chr");
    if(filename.isEmpty()) return;
    emit chrFileOpened(filename);
}

void NESMetaspriteTool::openPalette()
{
    QString filename = QFileDialog::getOpenFileName(this, ui->actionLoadPalette->text(), "", "*.pal");
    if(filename.isEmpty()) return;
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this,tr("Error opening file"),tr("Could not open file. Please make sure you have the necessary permissions to access files in this location."),QMessageBox::NoButton);
    }
    QByteArray pal = file.readAll();
    file.close();
    ui->gvPaletteManager->setPaletteData(pal);
}

void NESMetaspriteTool::savePalette()
{
    QString filename = QFileDialog::getSaveFileName(this, ui->actionSavePalette->text(), "", "*.pal");
    if(filename.isEmpty()) return;
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this,tr("Error saving file"),tr("Could not save file. Please make sure you have the necessary permissions to save files to this location."),QMessageBox::NoButton);
    }
    QByteArray pal = ui->gvPaletteManager->paletteData();
    file.write(pal);
    file.close();
}
