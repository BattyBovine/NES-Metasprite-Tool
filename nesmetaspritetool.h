#ifndef NESMETASPRITETOOL_H
#define NESMETASPRITETOOL_H

#include <QMainWindow>
#include <QGraphicsRectItem>
#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopServices>
#include <QDirIterator>
#include <QTextStream>
#include <QRegularExpression>
#include <QSettings>
#include <QEvent>

#define NESMT_WIKI_URL				"https://github.com/BattyBovine/NES-Metasprite-Tool/wiki"

#define FILE_OPEN_ERROR_TITLE       "Error opening file"
#define FILE_OPEN_ERROR_BODY        "Could not open file. Please make sure you have the necessary permissions to access files in this location."
#define FILE_SAVE_ERROR_TITLE       "Error saving file"
#define FILE_SAVE_ERROR_BODY        "Could not save file. Please make sure you have the necessary permissions to save files to this location."


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
	void newProject();
	void openProject();
	void saveProject();
	void saveProjectToFolder(QString);
	void showWikiPage();

	void saveSpriteMode();
	void saveSpriteTable();
	void saveShowGrid();
	void saveSnapToGrid();
	void saveBankSize();
	void savePaletteSwatch();
	void saveRefreshRate();
	void saveOpenedChrFile(QString);
	void restoreSettings();

	void adjustSpriteSlot(int);
	void setFrameIntentionallyBlank(bool);

	void openMetaspriteBank();
	void saveASMMetaspriteBank(QString path="");
	void saveBinaryMetaspriteBank(QString path="");

	void openCHR();
	void openPalette();
	void savePalette(QString path="");

	void openAnimation();
	void saveASMAnimation(QString path="");

	void setNewPaletteFile(QString);
	void setAnimationLabelPrefix(QString);
//    void setPlayButtonText();
//    void setPauseButtonText();

protected:
	void keyPressEvent(QKeyEvent*);

private:

	Ui::NESMetaspriteTool *ui;

	QSettings sSettings;
};

#endif // NESMETASPRITETOOL_H
