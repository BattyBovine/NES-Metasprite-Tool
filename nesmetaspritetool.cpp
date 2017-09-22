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
	connect(ui->radio8x16,SIGNAL(toggled(bool)),this,SLOT(saveSpriteMode()));
	connect(ui->radioChrTable1,SIGNAL(toggled(bool)),this,SLOT(saveSpriteTable()));
	connect(ui->chkShowGrid,SIGNAL(toggled(bool)),this,SLOT(saveShowGrid()));
	connect(ui->chkSnapToGrid,SIGNAL(toggled(bool)),this,SLOT(saveSnapToGrid()));
	connect(ui->comboBankSize,SIGNAL(currentIndexChanged(int)),this,SLOT(saveBankSize()));
	connect(ui->comboPalettes,SIGNAL(currentIndexChanged(int)),this,SLOT(savePaletteSwatch()));
	connect(ui->radioPAL,SIGNAL(toggled(bool)),this,SLOT(saveRefreshRate()));

	QString lastchr = this->sSettings.value("LastOpenedChrFile","").toString();
	if(!lastchr.isEmpty())
		ui->gvGlobalTileset->loadCHRData(lastchr);
	else
		ui->gvGlobalTileset->loadCHRData(":/chr/blank.chr");
}

NESMetaspriteTool::~NESMetaspriteTool()
{
	delete ui;
}



void NESMetaspriteTool::keyPressEvent(QKeyEvent *e)
{
	if(ui->tabWidget->currentWidget()==ui->tabMetasprites) {
		if(e->modifiers()&Qt::ControlModifier) {
			switch(e->key()) {
			case Qt::Key_A:
				(e->modifiers()&Qt::ShiftModifier)?ui->gvMetasprite->deselectAllSprites():ui->gvMetasprite->selectAllSprites();
				break;
			case Qt::Key_X:
			case Qt::Key_C:
				ui->gvMetasprite->copySpritesToClipboard((e->key()==Qt::Key_X));
				break;
			case Qt::Key_V:
				ui->gvMetasprite->pasteSpritesFromClipboard();
				break;
			}
		} else {
			switch(e->key()) {
			case Qt::Key_Left:
			case Qt::Key_Right:
				ui->gvMetasprite->moveSelectedX((e->key()==Qt::Key_Right),(e->modifiers()&Qt::ShiftModifier));
				break;
			case Qt::Key_Up:
			case Qt::Key_Down:
				ui->gvMetasprite->moveSelectedX((e->key()==Qt::Key_Down),(e->modifiers()&Qt::ShiftModifier));
				break;
			case Qt::Key_PageUp:
				ui->gvMetasprite->moveSelectedUp();
				break;
			case Qt::Key_PageDown:
				ui->gvMetasprite->moveSelectedDown();
				break;
			case Qt::Key_H:
				ui->gvMetasprite->flipHorizontal();
				break;
			case Qt::Key_V:
				ui->gvMetasprite->flipVertical();
				break;
			case Qt::Key_Comma:
				ui->hsMetaspriteSlider->prevValue();
				break;
			case Qt::Key_Period:
				ui->hsMetaspriteSlider->nextValue();
				break;
			case Qt::Key_Delete:
				ui->gvMetasprite->deleteSelectedTiles();
				break;
			case Qt::Key_1:
			case Qt::Key_2:
			case Qt::Key_3:
			case Qt::Key_4:
				ui->gvMetasprite->setPaletteForSelected(e->key()-Qt::Key_1);
			}
		}
		return;
	} else if(ui->tabWidget->currentWidget()==ui->tabAnimation) {
		if(e->modifiers()&Qt::ControlModifier) {
			switch(e->key()) {
			default:
				break;
			}
		} else {
			switch(e->key()) {
			case Qt::Key_Comma:
				ui->hsFrameSlider->prevValue();
				break;
			case Qt::Key_Period:
				ui->hsFrameSlider->nextValue();
				break;
			case Qt::Key_BracketLeft:
				ui->hsAnimationSlider->prevValue();
				break;
			case Qt::Key_BracketRight:
				ui->hsAnimationSlider->nextValue();
				break;
//            case Qt::Key_Left:
//                ui->spinDelay->setValue(ui->spinDelay->value()-1);
//                break;
//            case Qt::Key_Right:
//                ui->spinDelay->setValue(ui->spinDelay->value()+1);
//                break;
//            case Qt::Key_PageUp:
//                ui->spinDelay->setValue(ui->spinDelay->value()+10);
//                break;
//            case Qt::Key_PageDown:
//                ui->spinDelay->setValue(ui->spinDelay->value()-10);
//                break;
			}
		}
	}

	QMainWindow::keyPressEvent(e);
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
	this->openMetaspriteBank();
	this->openAnimation();
}

void NESMetaspriteTool::saveProject()
{
	this->saveASMMetaspriteBank();
	this->saveASMAnimation();
}
void NESMetaspriteTool::saveProjectToFolder(QString f)
{
	QDir folder(f);
	if(!folder.exists()) {
		folder.mkdir(f);
	}
}

void NESMetaspriteTool::showWikiPage()
{
	QDesktopServices::openUrl(QUrl(NESMT_WIKI_URL));
}



void NESMetaspriteTool::saveSpriteMode()
{
	this->sSettings.setValue("SpriteMode", ui->radio8x16->isChecked());
}
void NESMetaspriteTool::saveSpriteTable()
{
	this->sSettings.setValue("SpriteTable1", ui->radioChrTable1->isChecked());
}
void NESMetaspriteTool::saveShowGrid()
{
	this->sSettings.setValue("ShowGrid", ui->chkShowGrid->isChecked());
}
void NESMetaspriteTool::saveSnapToGrid()
{
	this->sSettings.setValue("SnapToGrid", ui->chkSnapToGrid->isChecked());
}
void NESMetaspriteTool::saveBankSize()
{
	this->sSettings.setValue("BankSize", ui->comboBankSize->currentIndex());
}
void NESMetaspriteTool::savePaletteSwatch()
{
	this->sSettings.setValue("PaletteSwatch", ui->comboPalettes->currentIndex());
}
void NESMetaspriteTool::saveRefreshRate()
{
	this->sSettings.setValue("RefreshRate", ui->radioPAL->isChecked());
}
void NESMetaspriteTool::saveOpenedChrFile(QString file)
{
	this->sSettings.setValue("LastOpenedChrFile", file);
}

void NESMetaspriteTool::restoreSettings()
{
	ui->radio8x16->setChecked(this->sSettings.value("SpriteMode",false).toBool());
	ui->radioChrTable1->setChecked(this->sSettings.value("SpriteTable1",false).toBool());
	ui->chkShowGrid->setChecked(this->sSettings.value("ShowGrid",true).toBool());
	ui->chkSnapToGrid->setChecked(this->sSettings.value("SnapToGrid",false).toBool());
	ui->comboBankSize->setCurrentIndex(this->sSettings.value("BankSize",0).toInt());
	ui->comboPalettes->setCurrentIndex(this->sSettings.value("PaletteSwatch",0).toInt());
	ui->radioPAL->setChecked(this->sSettings.value("RefreshRate",false).toBool());
}



void NESMetaspriteTool::adjustSpriteSlot(int bankindex)
{
	int bankdiv = qPow(2,bankindex);
	int spriteslot = ui->spinSpriteSlot->value();
	ui->spinSpriteSlot->setMaximum(bankdiv-1);
	ui->spinSpriteSlot->setValue(spriteslot%bankdiv);
	ui->spinSpriteSlot->setEnabled(bankindex!=0);
}



void NESMetaspriteTool::openMetaspriteBank()
{
	QString filename = QFileDialog::getOpenFileName(this,ui->actionOpenMetaspriteBank->text(),
													this->sSettings.value("LastOpenedProjectDir","").toString(),
													tr("All files (*.*)"));
	if(filename.isEmpty())  return;

	QFileInfo info(filename);
	this->sSettings.setValue("LastOpenedProjectDir",info.absolutePath());
	QFile file(filename);
	if(!file.open(QIODevice::ReadOnly)) {
		QMessageBox::warning(this,tr(FILE_OPEN_ERROR_TITLE),tr(FILE_OPEN_ERROR_BODY),QMessageBox::NoButton);
		return;
	}
	file.close();

	ui->gvMetasprite->openMetaspriteFile(filename);
}

void NESMetaspriteTool::saveASMMetaspriteBank(QString path)
{
	QString filename;
	if(path.isEmpty())
		filename = QFileDialog::getSaveFileName(this,ui->actionSaveMetaspriteBankASM->text(),
												this->sSettings.value("LastOpenedProjectDir","").toString(),
												tr("All files (*.*)"));
	else
		filename = path;
	if(filename.isEmpty())  return;

	QFileInfo info(filename);
	this->sSettings.setValue("LastOpenedProjectDir",info.absolutePath());
	QFile file(filename);
	if(!file.open(QIODevice::WriteOnly)) {
		QMessageBox::warning(this,tr(FILE_SAVE_ERROR_TITLE),tr(FILE_SAVE_ERROR_BODY),QMessageBox::NoButton);
		return;
	}

	QString datatable_header = ui->lineASMLabel->text()+"_header:\n";
	datatable_header += QString("\t.word ")+ui->lineASMLabel->text()+QString("_lo\n");
	datatable_header += QString("\t.word ")+ui->lineASMLabel->text()+QString("_hi\n");
	datatable_header += QString("\t.word ")+ui->lineASMLabel->text()+QString("_banks\n");
	datatable_header += QString("\t.word ")+ui->lineASMLabel->text()+QString("_slots\n\n");

	file.write(datatable_header.toLocal8Bit());
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
	ui->gvPaletteManager->openPaletteFile(filename);
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
	QByteArray pal = ui->gvPaletteManager->createPaletteBinaryData();
	file.write(pal);
	file.close();
}



void NESMetaspriteTool::openAnimation()
{
	QString filename = QFileDialog::getOpenFileName(this,ui->actionOpenAnimation->text(),
													this->sSettings.value("LastOpenedProjectDir","").toString(),
													tr("All files (*.*)"));
	if(filename.isEmpty())  return;

	QFileInfo info(filename);
	this->sSettings.setValue("LastOpenedProjectDir",info.absolutePath());
	QFile file(filename);
	if(!file.open(QIODevice::ReadOnly)) {
		QMessageBox::warning(this,tr(FILE_OPEN_ERROR_TITLE),tr(FILE_OPEN_ERROR_BODY),QMessageBox::NoButton);
		return;
	}
	file.close();

	ui->gvAnimation->openAnimationFile(filename);
}

void NESMetaspriteTool::saveASMAnimation(QString path)
{
	QString filename;
	if(path.isEmpty())
		filename = QFileDialog::getSaveFileName(this,ui->actionSaveAnimationASM->text(),
												this->sSettings.value("LastOpenedProjectDir","").toString(),
												tr("All files (*.*)"));
	else
		filename = path;
	if(filename.isEmpty())  return;

	QFileInfo info(filename);
	this->sSettings.setValue("LastOpenedProjectDir",info.absolutePath());
	QFile file(filename);
	if(!file.open(QIODevice::WriteOnly)) {
		QMessageBox::warning(this,tr(FILE_SAVE_ERROR_TITLE),tr(FILE_SAVE_ERROR_BODY),QMessageBox::NoButton);
		return;
	}

	QString datatable_header = ui->lineASMLabel->text()+"_anim_header:\n";
	datatable_header += QString("\t.word ")+ui->lineASMLabel->text()+QString("_anim_ptr\n");
	datatable_header += QString("\t.word ")+ui->lineASMLabel->text()+QString("_anim_length\n\n");
	file.write(datatable_header.toLocal8Bit());
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

void NESMetaspriteTool::setAllSpriteSlots()
{
	ui->gvMetasprite->setAllSpriteSlots(ui->spinSpriteSlot->value());
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
