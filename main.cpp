#include "nesmetaspritetool.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setOrganizationName("Batty Bovine Productions, LLC");
    a.setOrganizationDomain("battybovine.com");
    a.setApplicationName("NES Metasprite Tool");
    a.setApplicationVersion("0.1");

    NESMetaspriteTool w;
    w.show();

    return a.exec();
}
