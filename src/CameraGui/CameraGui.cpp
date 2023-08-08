#pragma once
#include "../PCH.h"

#include "CameraGui.h"
#include "ui_CameraGui.h"

#include "../Utilities/Utilities.h"

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

    // Load the face cascade
    cv::CascadeClassifier faceCascade(Utilities::CASCADE_FILE_MAIN);

    // Check if the cascades exist and can be loaded
    if (!faceCascade.load(Utilities::CASCADE_FILE_MAIN))
    {
        qDebug() << "Could not find the face cascade.";
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

// Function for face detection
void RealTimeFacialRecognition::DetectAndDraw(cv::Mat& img, cv::CascadeClassifier& cascade, double scale)
{
    // Convert input image to grayscale
    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

    // Detect faces in the grayscale image
    std::vector<cv::Rect> faces;
    cascade.detectMultiScale(gray, faces, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));

    // Iterate over the detected faces
    for (const auto& faceROI : faces)
    {
        // Draw a rectangle around the face
        cv::rectangle(img, faceROI, cv::Scalar(255, 0, 0), 2);
        qDebug() << "Face recognized, but no match.";

        // Compare the face with images in the specified directory
        std::filesystem::path imageDir(Utilities::IMAGE_DIR);

        double maxVal = 0.0;  // Store the maximum match value
        std::string bestMatchName;  // Store the name of the best match

        cv::Mat compareImg;

        for (const auto& entry : std::filesystem::directory_iterator(imageDir))
        {
            // Check if the file has a valid image extension
            if (!Utilities::HasValidImageExtension(entry.path()))
            {
                continue; // Skip files with invalid extensions
            }

            compareImg = cv::imread(entry.path().string());
            if (compareImg.empty())
            {
                qDebug() << "Failed to read image:" << entry.path().filename().string();
                continue; // Skip to the next image
            }

            // Convert the compare image to grayscale
            cv::cvtColor(compareImg, compareImg, cv::COLOR_BGR2GRAY);

            // Resize the compare image to match the size of the detected face
            cv::resize(compareImg, compareImg, faceROI.size());

            // Perform template matching to find the match
            cv::Mat result;
            try
            {
                cv::matchTemplate(compareImg, gray(faceROI), result, cv::TM_CCORR_NORMED);
            }
            catch (const cv::Exception& e)
            {
                qDebug() << "Error occurred during template matching:" << e.what();
                continue; // Skip to the next image
            }

            // Set a threshold for the match
            double threshold = 0.2;                             // 0.8 = default
            double minVal;
            cv::Point minLoc;
            cv::minMaxLoc(result, &minVal, &maxVal, &minLoc);

            if (maxVal > threshold)
            {
                // If a better match is found, update the best match information
                bestMatchName = entry.path().filename().string();
            }
            else if (maxVal > 0.1)
            {
                // Print other matches to the console
                qDebug() << "Other match:" << entry.path().filename().string();
            }
        }

        if (!bestMatchName.empty())
        {
            // If a match is found, draw an orange box with the best match filename as a label
            cv::rectangle(img, faceROI, cv::Scalar(0, 165, 255), 2);
            cv::putText(img, bestMatchName, cv::Point(faceROI.x, faceROI.y - 10), cv::FONT_HERSHEY_SIMPLEX, 0.9, cv::Scalar(0, 165, 255), 2);
        }
    }
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

        // Perform face detection using CascadeClassifier
        cv::CascadeClassifier faceCascade(Utilities::CASCADE_FILE_MAIN);
        std::vector<cv::Rect> faces;
        cv::Mat grayFrame;
        cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
        faceCascade.detectMultiScale(grayFrame, faces, 1.1, 4);

        // Draw rectangles around detected faces and perform recognition
        for (const cv::Rect& faceRect : faces)
        {
            cv::rectangle(resizedFrame, faceRect, cv::Scalar(255, 0, 0), 2);

            // Convert the detected face region to grayscale
            cv::Mat detectedFace = grayFrame(faceRect);

            // Perform recognition using the existing function from OldScript
            RealTimeFacialRecognition::DetectAndDraw(detectedFace, faceCascade, 1.1);

            // Draw the processed frame with rectangles and recognition results
            qImage = QImage(resizedFrame.data, resizedFrame.cols, resizedFrame.rows, resizedFrame.step, QImage::Format_RGB888);
            pixmap = QPixmap::fromImage(qImage.rgbSwapped());
            cameraFeed->clear();
            cameraFeed->addPixmap(pixmap);
        }
    }
}

RealTimeFacialRecognition::~RealTimeFacialRecognition()
{
    delete ui;
}
