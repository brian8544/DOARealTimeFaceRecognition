#pragma once
#include "PCH.h"
#include "CameraGui/CameraGui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RealTimeFacialRecognition w;
    w.show();
    return a.exec();
}
