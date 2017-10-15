#ifndef TILESETCACHE_H
#define TILESETCACHE_H

#include <QPixmap>

#define TILESETCACHE_PIXMAP_KEY_FORMAT "b%1p%2"

class TilesetCache
{
public:
	static QPixmap find(int b,int p){return hPixmapList[QString(TILESETCACHE_PIXMAP_KEY_FORMAT).arg(b).arg(p)];}
	static void insert(int b,int p,QPixmap pix){hPixmapList.insert(QString(TILESETCACHE_PIXMAP_KEY_FORMAT).arg(b).arg(p),pix);}
private:
	TilesetCache(){}
	static QHash<QString,QPixmap> hPixmapList;
};

#endif // TILESETCACHE_H
