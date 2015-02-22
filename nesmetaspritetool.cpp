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



void NESMetaspriteTool::newProject()
{
    int retval = QMessageBox::warning(this,"Clear current data?",
                                      "All unsaved data will be lost. Are you sure you wish to create a new project?",
                                      QMessageBox::Yes | QMessageBox::No,
                                      QMessageBox::No);
    switch(retval) {
    case QMessageBox::Yes:
        ui->gvMetasprite->clearAllMetaspriteData();
        ui->gvAnimation->clearAllAnimationData();
        ui->lineASMLabel->setText("");
        ui->lineASMAnimationLabel->setText("");
        break;
    }
}

void NESMetaspriteTool::openMetaspriteBank()
{
    QString filename = QFileDialog::getOpenFileName(this, ui->actionOpenMetaspriteBank->text(), "", "*.*");
    if(filename.isEmpty())  return;
    ui->gvMetasprite->openMetaspriteFile(filename);
}

void NESMetaspriteTool::saveASMMetaspriteBank()
{
    QString filename = QFileDialog::getSaveFileName(this, ui->actionSaveMetaspriteBankASM->text(), "", "*.*");
    if(filename.isEmpty())  return;
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this,tr(FILE_SAVE_ERROR_TITLE),tr(FILE_SAVE_ERROR_BODY),QMessageBox::NoButton);
        return;
    }

    file.write(ui->gvMetasprite->createMetaspriteASMData(ui->lineASMLabel->text()+"_").toLocal8Bit());

    file.close();
}

void NESMetaspriteTool::saveBinaryMetaspriteBank()
{
    QString filename = QFileDialog::getSaveFileName(this, ui->actionSaveMetaspriteBankBinary->text(), "", "*.*");
    if(filename.isEmpty())  return;
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this,tr(FILE_SAVE_ERROR_TITLE),tr(FILE_SAVE_ERROR_BODY),QMessageBox::NoButton);
        return;
    }
    QVector<QByteArray> bindata = ui->gvMetasprite->createMetaspriteBinaryData();
    foreach(QByteArray bin, bindata) {
        if(!bin.isEmpty())  file.write(bin);
    }
    file.close();
}

void NESMetaspriteTool::openCHR()
{
    QString filename = QFileDialog::getOpenFileName(this, ui->actionOpenCHR->text(), "", "*.chr");
    if(filename.isEmpty()) return;
    emit chrFileOpened(filename);
}

void NESMetaspriteTool::openPalette()
{
    QString filename = QFileDialog::getOpenFileName(this, ui->actionOpenPalette->text(), "", "*.pal");
    if(filename.isEmpty())  return;
    ui->gvPaletteManager->setPaletteData(filename);
}

void NESMetaspriteTool::savePalette()
{
    QString filename = QFileDialog::getSaveFileName(this, ui->actionSavePalette->text(), "", "*.pal");
    if(filename.isEmpty())  return;
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this,tr(FILE_SAVE_ERROR_TITLE),tr(FILE_SAVE_ERROR_BODY),QMessageBox::NoButton);
        return;
    }
    QByteArray pal = ui->gvPaletteManager->paletteData();
    file.write(pal);
    file.close();
}

void NESMetaspriteTool::openAnimation()
{
    QString filename = QFileDialog::getOpenFileName(this, ui->actionOpenAnimation->text(), "", "*.*");
    if(filename.isEmpty())  return;
    ui->gvAnimation->openAnimationFile(filename);
}

void NESMetaspriteTool::saveASMAnimation()
{
    QString filename = QFileDialog::getSaveFileName(this, ui->actionSaveAnimationASM->text(), "", "*.*");
    if(filename.isEmpty())  return;
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this,tr(FILE_SAVE_ERROR_TITLE),tr(FILE_SAVE_ERROR_BODY),QMessageBox::NoButton);
        return;
    }

    file.write(ui->gvAnimation->createAnimationASMData(ui->labelMetaspriteName->text()+"_").toLocal8Bit());

    file.close();
}



void NESMetaspriteTool::setNewPaletteFile(QString pal)
{
    ui->gvPaletteManager->drawFullPaletteColours(pal);
}



void NESMetaspriteTool::setAnimationLabelPrefix(QString s)
{
    ui->labelMetaspriteName->setText((!s.isEmpty())?s:"emptylabel");
}

void NESMetaspriteTool::setPlayButtonText()
{
    ui->btnPlayAnimation->setText("►");
    ui->btnPlayAnimation->setChecked(false);
}

void NESMetaspriteTool::setPauseButtonText()
{
    ui->btnPlayAnimation->setText("▮▮");
    ui->btnPlayAnimation->setChecked(true);
}
