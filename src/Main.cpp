#include "CameraGui.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RealTimeFacialRecognition w;
    w.show();
    return a.exec();
}
