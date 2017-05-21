#include "chrthread.h"

CHRThread::CHRThread(QThread *parent) : QThread(parent)
{
	this->iBankSize = 0x100;
}

CHRThread::~CHRThread()
{
}



void CHRThread::loadFile(QString f)
{
	this->sFile = f;
	this->start();
}



void CHRThread::run()
{
	if(this->sFile.isEmpty()) return;

	QFile file(this->sFile);
	if(!file.open(QIODevice::ReadOnly)) {
		emit(this->error(tr(CHR_OPEN_ERROR_TITLE),tr(CHR_OPEN_ERROR_BODY)));
		return;
	}

	int filesize = file.size();
	if((filesize%0x0200)!=0) {
		emit(this->error(tr(CHR_SIZE_ERROR_TITLE),tr(CHR_SIZE_ERROR_BODY)));
		return;
	}

	QByteArray chrdata;
	while(file.bytesAvailable()>=16) {
		char bitplanes[2][8];
		file.read(bitplanes[0],8);
		file.read(bitplanes[1],8);
		for(int i=0; i<8; i++) {
			quint16 bpbyte1 = bitplanes[0][i];
			quint16 bpbyte2 = bitplanes[1][i];
			quint16 mask = 0x80;
			for(int j=0; j<8; j++) {
				quint8 index = 0;
				if((bpbyte1&mask)&&(bpbyte2&mask))  index = 3;
				if(!(bpbyte1&mask)&&(bpbyte2&mask)) index = 2;
				if((bpbyte1&mask)&&!(bpbyte2&mask)) index = 1;
				chrdata.append(index);
				mask >>= 1;
			}
		}
	}
	file.close();

	QImage imgtileset(128, (filesize>>5), QImage::Format_Indexed8);
	imgtileset.fill(0);
	imgtileset.setColor(0,qRgb(0x00,0x00,0x00));
	imgtileset.setColor(1,qRgb(0x55,0x55,0x55));
	imgtileset.setColor(2,qRgb(0xAA,0xAA,0xAA));
	imgtileset.setColor(3,qRgb(0xFF,0xFF,0xFF));

	for(int imgy=0; imgy<((filesize/0x0200)*2); imgy++) {
		for(int imgx=0; imgx<16; imgx++) {
			for(int tiley=0; tiley<8; tiley++) {
				for(int tilex=0; tilex<8; tilex++) {
					imgtileset.setPixel(tilex+(8*imgx),
										tiley+(8*imgy),
										chrdata.at(((imgx+(imgy*16))*64)+(tilex+(tiley*8))));
				}
			}
		}
	}

	emit(this->sendCHRImageData(imgtileset));
}
