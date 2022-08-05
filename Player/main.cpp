#include "playervideos.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    playerVideos w;

    w.show();
    return a.exec();
}
