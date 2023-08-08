#include "CameraGui.h"
#include "./ui_CameraGui.h"

RealTimeFacialRecognition::RealTimeFacialRecognition(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RealTimeFacialRecognition)
{
    ui->setupUi(this);
}

RealTimeFacialRecognition::~RealTimeFacialRecognition()
{
    delete ui;
}

