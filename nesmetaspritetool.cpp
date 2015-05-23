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
            palettes.append(next.replace(":/pal/","").replace(".pal",""));
    }
    palettes.sort(Qt::CaseInsensitive);
    foreach(QString p, palettes) {
        ui->comboPalettes->addItem(p);
    }

    this->sSettings.setDefaultFormat(QSettings::NativeFormat);
    this->restoreSettings();

    ui->gvTileset->loadCHRBank(":/chr/blank.chr");
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

void NESMetaspriteTool::openProject()
{
    QString foldername = QFileDialog::getExistingDirectory(this, ui->actionOpenProject->text(), "");
    if(!foldername.isEmpty())   QMessageBox::information(this,"Folder selected",foldername,QMessageBox::NoButton);
}

void NESMetaspriteTool::saveProject()
{
    QFileDialog saveproj(this);
    saveproj.setFileMode(QFileDialog::AnyFile);
    saveproj.setOptions(QFileDialog::ShowDirsOnly);
    saveproj.setViewMode(QFileDialog::Detail);
    connect(&saveproj,SIGNAL(fileSelected(QString)),this,SLOT(saveProjectToFolder(QString)));
    saveproj.exec();
}
void NESMetaspriteTool::saveProjectToFolder(QString f)
{
    QDir folder(f);
    if(!folder.exists()) {
        folder.mkdir(f);
    }
}



void NESMetaspriteTool::saveSpriteMode()
{
    this->sSettings.setValue("SpriteMode", ui->radio8x16->isChecked());
}
void NESMetaspriteTool::saveRefreshRate()
{
    this->sSettings.setValue("RefreshRate", ui->radioPAL->isChecked());
}

void NESMetaspriteTool::restoreSettings()
{
    ui->radio8x16->setChecked(this->sSettings.value("SpriteMode",false).toBool());
    ui->radioPAL->setChecked(this->sSettings.value("RefreshRate",false).toBool());
}



void NESMetaspriteTool::openMetaspriteBank()
{
    QString filename = QFileDialog::getOpenFileName(this, ui->actionOpenMetaspriteBank->text(), "", tr("All files (*.*)"));
    if(filename.isEmpty())  return;
    ui->gvMetasprite->openMetaspriteFile(filename);
}

void NESMetaspriteTool::saveASMMetaspriteBank(QString path)
{
    QString filename;
    if(path.isEmpty()) {
        filename = QFileDialog::getSaveFileName(this, ui->actionSaveMetaspriteBankASM->text(), "", tr("All files (*.*)"));
        if(filename.isEmpty())  return;
    } else {
        filename = path;
    }

    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this,tr(FILE_SAVE_ERROR_TITLE),tr(FILE_SAVE_ERROR_BODY),QMessageBox::NoButton);
        return;
    }

    file.write(ui->gvMetasprite->createMetaspriteASMData(ui->lineASMLabel->text()+"_").toLocal8Bit());

    file.close();
}

void NESMetaspriteTool::saveBinaryMetaspriteBank(QString path)
{
    QString filename;
    if(path.isEmpty()) {
        filename = QFileDialog::getSaveFileName(this, ui->actionSaveMetaspriteBankBinary->text(), "", tr("All files (*.*)"));
        if(filename.isEmpty())  return;
    } else {
        filename = path;
    }

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
    QString filename = QFileDialog::getOpenFileName(this, ui->actionOpenCHR->text(), "", tr("CHR-ROM data (*.chr);;All files (*.*)"));
    if(filename.isEmpty()) return;
    emit chrFileOpened(filename);
}

void NESMetaspriteTool::openPalette()
{
    QString filename = QFileDialog::getOpenFileName(this, ui->actionOpenPalette->text(), "", tr("Palette data (*.pal);;All files (*.*)"));
    if(filename.isEmpty())  return;
    ui->gvPaletteManager->setPaletteData(filename);
}

void NESMetaspriteTool::savePalette(QString path)
{
    QString filename;
    if(path.isEmpty()) {
        filename = QFileDialog::getSaveFileName(this, ui->actionSavePalette->text(), "", tr("Palette data (*.pal);;All files (*.*)"));
        if(filename.isEmpty())  return;
    } else {
        filename = path;
    }

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
    QString filename = QFileDialog::getOpenFileName(this, ui->actionOpenAnimation->text(), "", tr("All files (*.*)"));
    if(filename.isEmpty())  return;
    ui->gvAnimation->openAnimationFile(filename);
}

void NESMetaspriteTool::saveASMAnimation(QString path)
{
    QString filename;
    if(path.isEmpty()) {
        filename = QFileDialog::getSaveFileName(this, ui->actionSaveAnimationASM->text(), "", tr("All files (*.*)"));
        if(filename.isEmpty())  return;
    } else {
        filename = path;
    }

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

//void NESMetaspriteTool::setPlayButtonText()
//{
//    ui->btnPlayAnimation->setText("►");
//    ui->btnPlayAnimation->setChecked(false);
//}

//void NESMetaspriteTool::setPauseButtonText()
//{
//    ui->btnPlayAnimation->setText("▮▮");
//    ui->btnPlayAnimation->setChecked(true);
//}
