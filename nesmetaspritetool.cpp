#include "nesmetaspritetool.h"
#include "ui_nesmetaspritetool.h"

NESMetaspriteTool::NESMetaspriteTool(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NESMetaspriteTool)
{
    ui->setupUi(this);

    connect(ui->gvPaletteManager,SIGNAL(newSpriteColours(QVector<QRgb>,quint8)),ui->gvTileset,SLOT(setNewSpriteColours(QVector<QRgb>,quint8)));
    connect(ui->gvPaletteManager,SIGNAL(newSpriteColours(QVector<QRgb>,quint8)),ui->gvMetasprite,SLOT(setNewSpriteColours(QVector<QRgb>,quint8)));

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
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this,tr("Error opening file"),tr("Could not open file. Please make sure you have the necessary permissions to access files in this location."),QMessageBox::NoButton);
        return;
    }
    QByteArray pal = file.readAll();
    file.close();
    ui->gvPaletteManager->setPaletteData(pal);
}

void NESMetaspriteTool::savePalette()
{
    QString filename = QFileDialog::getSaveFileName(this, ui->actionSavePalette->text(), "", "*.pal");
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this,tr("Error saving file"),tr("Could not save file. Please make sure you have the necessary permissions to save files to this location."),QMessageBox::NoButton);
        return;
    }
    QByteArray pal = ui->gvPaletteManager->paletteData();
    file.write(pal);
    file.close();
}

void NESMetaspriteTool::openASMMetaspriteBank()
{
    QString filename = QFileDialog::getOpenFileName(this, ui->actionLoadMetaspriteBankASM->text(), "", "*.*");
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QMessageBox::warning(this,tr("Error opening file"),tr("Could not open file. Please make sure you have the necessary permissions to access files in this location."),QMessageBox::NoButton);
        return;
    }
    quint8 labelnum = 0;
    QVector<QByteArray> inputbytes(256);
    while(!file.atEnd()) {
        QString line = file.readLine();
        QRegularExpression label("^(.*?)_(\\d+?):$");
        QRegularExpressionMatch labelmatch = label.match(line);
        if(labelmatch.hasMatch()) {
            if(ui->lineASMLabel->text().isEmpty())    ui->lineASMLabel->setText(labelmatch.captured(1));
            labelnum = labelmatch.captured(2).toInt();
        }
        QRegularExpression bytes(",?0x([0-9a-fA-F]+)");
        QRegularExpressionMatchIterator bytesiter = bytes.globalMatch(line);
        QByteArray bytesin;
        while(bytesiter.hasNext()) {
            QRegularExpressionMatch bytesmatch = bytesiter.next();
            bytesin.append(quint8(bytesmatch.captured(1).toUInt(new bool(),16)));
        }
        inputbytes.replace(labelnum,bytesin);
    }
//    ui->gvMetasprite->importMetaspriteBinaryData(inputbytes);
    file.close();
}

void NESMetaspriteTool::saveASMMetaspriteBank()
{
    QString filename = QFileDialog::getSaveFileName(this, ui->actionSaveMetaspriteBankASM->text(), "", "*.*");
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this,tr("Error saving file"),tr("Could not save file. Please make sure you have the necessary permissions to save files to this location."),QMessageBox::NoButton);
        return;
    }
    QVector<QByteArray> bindata = ui->gvMetasprite->createMetaspriteBinaryData();
    for(int i=0; i<bindata.size(); i++) {
        QByteArray bin = bindata.at(i);
        if(!bin.isEmpty()) {
            QString label((ui->lineASMLabel->text().isEmpty()?"emptylabel":ui->lineASMLabel->text())+"_");
            label += QString::number(i).append(":\n.byte ");
            QByteArray labelstream = label.toLocal8Bit();
            file.write(labelstream);
            QString hexbyte = QString("%1").arg(quint8(bin.at(0)),2,16,QChar('0')).toUpper().prepend("0x");
            file.write(hexbyte.toLocal8Bit());
            for(int i=1; i<bin.length(); i++) {
                hexbyte = QString("%1").arg(quint8(bin.at(i)),2,16,QChar('0')).toUpper().prepend(",0x");
                file.write(hexbyte.toLocal8Bit());
            }
            file.write("\n");
        }
    }
    file.close();
}

void NESMetaspriteTool::saveBinaryMetaspriteBank()
{
    QString filename = QFileDialog::getSaveFileName(this, ui->actionSaveMetaspriteBankBinary->text(), "", "*.*");
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this,tr("Error saving file"),tr("Could not save file. Please make sure you have the necessary permissions to save files to this location."),QMessageBox::NoButton);
        return;
    }
    QVector<QByteArray> bindata = ui->gvMetasprite->createMetaspriteBinaryData();
    foreach(QByteArray bin, bindata) {
        if(!bin.isEmpty())  file.write(bin);
    }
    file.close();
}
