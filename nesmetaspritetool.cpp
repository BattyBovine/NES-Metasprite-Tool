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



void NESMetaspriteTool::newMetaspriteBank()
{
    int retval = QMessageBox::warning(this,"Clear current metasprite data?",
                                      "All unsaved metasprite data will be lost. Are you sure you wish to create a new metasprite bank?",
                                      QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                                      QMessageBox::Cancel);
    switch(retval) {
    case QMessageBox::Save:
        this->saveASMMetaspriteBank();
        this->saveBinaryMetaspriteBank();
    case QMessageBox::Discard:
        ui->gvMetasprite->clearAllMetaspriteData();
        break;
    }
}

void NESMetaspriteTool::openMetaspriteBank()
{
    QString filename = QFileDialog::getOpenFileName(this, ui->actionLoadMetaspriteBank->text(), "", "*.*");
    if(filename.isEmpty())  return;
    emit(metaspriteFileOpened(filename));
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

    QString asmlabel = ui->lineASMLabel->text();
    if(asmlabel.isEmpty())  asmlabel = "emptylabel";
    QString datatable_hi = asmlabel+"_hi:\n\t.byte ";
    QString datatable_lo = asmlabel+"_lo:\n\t.byte ", databytes;

    QVector<QByteArray> bindata = ui->gvMetasprite->createMetaspriteBinaryData();
    for(int i=0; i<bindata.size(); i++) {
        QByteArray bin = bindata.at(i);
        if(!bin.isEmpty()) {
            QString countedlabel = asmlabel+QString("_").append(QString::number(i));

            datatable_hi += QString(">").append(countedlabel).append(",");
            datatable_lo += QString("<").append(countedlabel).append(",");

            databytes += "\n";
            databytes += countedlabel+":\n\t.byte ";
            databytes += QString("%1").arg(quint8(bin.at(0)),2,16,QChar('0')).toUpper().prepend("$");
            for(int i=1; i<bin.length(); i++) {
                databytes += QString("%1").arg(quint8(bin.at(i)),2,16,QChar('0')).toUpper().prepend(",$");
            }
        }
    }
    datatable_hi.remove(datatable_hi.size()-1,1);
    datatable_lo.remove(datatable_lo.size()-1,1);
    datatable_hi += "\n";
    datatable_lo += "\n";
    databytes += "\n";
    databytes += asmlabel+"_end:\n";

    file.write(datatable_hi.toLocal8Bit());
    file.write(datatable_lo.toLocal8Bit());
    file.write(databytes.toLocal8Bit());
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
    QString filename = QFileDialog::getOpenFileName(this, ui->actionLoadPalette->text(), "", "*.pal");
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



void NESMetaspriteTool::setNewPaletteFile(QString pal)
{
    ui->gvPaletteManager->drawFullPaletteColours(pal);
}



void NESMetaspriteTool::setAnimationLabel(QString s)
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
