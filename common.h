#ifndef COMMON_H
#define COMMON_H

#include <QVector>
#include <QList>
#include <QRgb>
#include <QGraphicsItem>

#define FILE_OPEN_ERROR_TITLE       "Error opening file"
#define FILE_OPEN_ERROR_BODY        "Could not open file. Please make sure you have the necessary permissions to access files in this location."
#define FILE_SAVE_ERROR_TITLE       "Error saving file"
#define FILE_SAVE_ERROR_BODY        "Could not save file. Please make sure you have the necessary permissions to save files to this location."

#define PM_FILE_OPEN_ERROR_TITLE    "Error opening palette file"
#define PM_FILE_OPEN_ERROR_BODY     "Could not open palette file. Please make sure you have the necessary permissions to access files in this location."

#define MSM_FILE_OPEN_ERROR_TITLE   "Error opening metasprite file"
#define MSM_FILE_OPEN_ERROR_BODY    "Could not open metasprite file. Please make sure you have the necessary permissions to access files in this location."
#define MSM_EOF_ERROR_TITLE         "Invalid data"
#define MSM_EOF_ERROR_BODY          "Error reading metasprite data: Unexpected end of file."
#define MSM_COUNT_ERROR_TITLE       "Invalid data"
#define MSM_COUNT_ERROR_BODY        "Error reading metasprite data: Sprite counts do not match length of data."

typedef QVector<QRgb> PaletteVector;
typedef QList<QGraphicsItem*> GraphicsItemList;

#endif // COMMON_H

