#ifndef CAMERAGUI_H
#define CAMERAGUI_H

#include "../PCH.h"

namespace Ui {
class RealTimeFacialRecognition;
}

class RealTimeFacialRecognition : public QMainWindow
{
    Q_OBJECT

public:
    RealTimeFacialRecognition(QWidget *parent = nullptr);
    ~RealTimeFacialRecognition();

private slots:
    void updateCameraFeed(); // Declare the updateCameraFeed slot

private:
    Ui::RealTimeFacialRecognition *ui; // The UI object
    cv::VideoCapture camera; // OpenCV camera capture object
};

#endif // CAMERAGUI_H
