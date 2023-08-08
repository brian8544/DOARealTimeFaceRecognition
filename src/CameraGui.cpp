#include "PCH.h"
#include "CameraGui.h"
#include "ui_CameraGui.h" // Include the ui_CameraGui.h header

RealTimeFacialRecognition::RealTimeFacialRecognition(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RealTimeFacialRecognition)
    , camera(0)
{
    ui->setupUi(this);

    if (!camera.isOpened())
    {
        qDebug() << "Error: Could not open camera.";
        return;
    }

    // Get a reference to the QGraphicsView widget named "CameraFeed" from the UI
    QGraphicsView* graphicsView = ui->CameraFeed; // Assuming CameraFeed is the QGraphicsView

    // Set up the QGraphicsScene if not already set up
    if (!graphicsView->scene())
    {
        QGraphicsScene* cameraFeed = new QGraphicsScene(this);
        graphicsView->setScene(cameraFeed);
    }

    // Create a timer for continuous updating of the camera feed
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &RealTimeFacialRecognition::updateCameraFeed);
    timer->start(30); // Update every 30 milliseconds
}

void RealTimeFacialRecognition::updateCameraFeed()
{
    cv::Mat frame;

    // Capture a frame from the camera
    camera >> frame;

    if (!frame.empty())
    {
        // Calculate the target size based on QGraphicsView dimensions
        QSize targetSize = ui->CameraFeed->viewport()->size();

        // Resize the OpenCV frame to match the target size
        cv::Mat resizedFrame;
        cv::resize(frame, resizedFrame, cv::Size(targetSize.width(), targetSize.height()));

        // Convert the resized OpenCV Mat to Qt QImage
        QImage qImage(resizedFrame.data, resizedFrame.cols, resizedFrame.rows, resizedFrame.step, QImage::Format_RGB888);

        // Convert QImage to QPixmap
        QPixmap pixmap = QPixmap::fromImage(qImage.rgbSwapped());

        // Get the QGraphicsView widget named "CameraFeed" from the UI
        QGraphicsView* graphicsView = ui->CameraFeed;

        // Set the new pixmap in the QGraphicsScene
        QGraphicsScene* cameraFeed = graphicsView->scene();
        cameraFeed->clear();
        cameraFeed->addPixmap(pixmap);
    }
}

RealTimeFacialRecognition::~RealTimeFacialRecognition()
{
    delete ui;
}
