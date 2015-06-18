#ifndef CHRTHREAD_H
#define CHRTHREAD_H

#include <QThread>
#include <QImage>
#include <QFile>
#include <QMessageBox>


#define CHR_OPEN_ERROR_TITLE    "Error opening file"
#define CHR_OPEN_ERROR_BODY     "The file could not be opened."
#define CHR_SIZE_ERROR_TITLE    "Could not open CHR file"
#define CHR_SIZE_ERROR_BODY     "CHR file must be a multiple of 4KB."


class CHRThread : public QThread
{
	Q_OBJECT
public:
	explicit CHRThread(QThread *parent = 0);
	~CHRThread();

	void loadFile(QString);

signals:
	void sendCHRImageData(QImage);

private:
	void run();

	QString sFile;
};

#endif // CHRTHREAD_H
