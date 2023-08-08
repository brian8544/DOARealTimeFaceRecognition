#ifndef REALTIMEFACIALRECOGNITION_H
#define REALTIMEFACIALRECOGNITION_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class RealTimeFacialRecognition; }
QT_END_NAMESPACE

class RealTimeFacialRecognition : public QMainWindow
{
    Q_OBJECT

public:
    RealTimeFacialRecognition(QWidget *parent = nullptr);
    ~RealTimeFacialRecognition();

private:
    Ui::RealTimeFacialRecognition *ui;
};
#endif // REALTIMEFACIALRECOGNITION_H
