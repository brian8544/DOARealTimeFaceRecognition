#pragma once
#include "PCH.h"
#include "CameraGui/CameraGui.h"
#include "Utilities/Utilities.cpp"

int main(int argc, char *argv[])
{
    // Before starting, set/check environment.
    Utilities::ReadSettings();
    Utilities::Initialize();

    QApplication a(argc, argv);
    RealTimeFacialRecognition w;
    w.show();
    return a.exec();
}
