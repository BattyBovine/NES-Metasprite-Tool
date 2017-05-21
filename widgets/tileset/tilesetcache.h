#ifndef TILESETCACHE_H
#define TILESETCACHE_H

#include <QPixmap>

#define TILESETCACHE_PIXMAP_KEY_FORMAT "b%1p%2"
//#define SPRITECACHE_PIXMAP_KEY_FORMAT "t%1m%2p%3"

class TilesetCache
{
public:
	static QPixmap find(int b,int p){return hPixmapList[QString(TILESETCACHE_PIXMAP_KEY_FORMAT).arg(b).arg(p)];}
	static void insert(int b,int p,QPixmap pix){hPixmapList.insert(QString(TILESETCACHE_PIXMAP_KEY_FORMAT).arg(b).arg(p),pix);}
private:
	TilesetCache(){}
	static QHash<QString,QPixmap> hPixmapList;
};

//class MetatileCache
//{
//public:
//	static QPixmap find(int t,int m,int p,int a){return hPixmapList[QString(METATILECACHE_PIXMAP_KEY_FORMAT).arg(t).arg(m).arg(p).arg(a)];}
//	static void insert(int t,int m,int p,int a,QPixmap pix){hPixmapList.insert(QString(METATILECACHE_PIXMAP_KEY_FORMAT).arg(t).arg(m).arg(p).arg(a),pix);}
//private:
//	MetatileCache(){}
//	static QHash<QString,QPixmap> hPixmapList;
//};

#endif // TILESETCACHE_H
