#include "nesmetaspritetool.h"
#include "ui_nesmetaspritetool.h"

NESMetaspriteTool::NESMetaspriteTool(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NESMetaspriteTool)
{
    ui->setupUi(this);

    this->installEventFilter(this);

    connect(ui->gvPaletteManager,SIGNAL(newSpriteColours(QVector<QRgb>,quint8,bool)),ui->gvTileset,SLOT(setNewSpriteColours(QVector<QRgb>,quint8)));
    connect(ui->gvPaletteManager,SIGNAL(newSpriteColours(QVector<QRgb>,quint8,bool)),ui->gvMetasprite,SLOT(setNewSpriteColours(QVector<QRgb>,quint8,bool)));
    connect(ui->gvMetasprite,SIGNAL(updateList(QList<QGraphicsItem*>,QList<QGraphicsItem*>)),ui->listSprites,SLOT(updateItems(QList<QGraphicsItem*>,QList<QGraphicsItem*>)));

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
    if(filename.isEmpty())  return;
    ui->gvPaletteManager->setPaletteData(filename);
}

void NESMetaspriteTool::savePalette()
{
    QString filename = QFileDialog::getSaveFileName(this, ui->actionSavePalette->text(), "", "*.pal");
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this,tr(FILE_SAVE_ERROR_TITLE),tr(FILE_SAVE_ERROR_BODY),QMessageBox::NoButton);
        return;
    }
    QByteArray pal = ui->gvPaletteManager->paletteData();
    file.write(pal);
    file.close();
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
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this,tr(FILE_SAVE_ERROR_TITLE),tr(FILE_SAVE_ERROR_BODY),QMessageBox::NoButton);
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
        QMessageBox::warning(this,tr(FILE_SAVE_ERROR_TITLE),tr(FILE_SAVE_ERROR_BODY),QMessageBox::NoButton);
        return;
    }
    QVector<QByteArray> bindata = ui->gvMetasprite->createMetaspriteBinaryData();
    foreach(QByteArray bin, bindata) {
        if(!bin.isEmpty())  file.write(bin);
    }
    file.close();
}
