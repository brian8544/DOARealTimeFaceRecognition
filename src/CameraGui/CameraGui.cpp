#pragma once
#include "../PCH.h"
#include "RealTimeFacialRecognition.h"
#include "ui_RealTimeFacialRecognition.h"
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
    QGraphicsView* graphicsView = ui->CameraFeed;

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

        for (const auto& entry : std::filesystem::directory_iterator(imageDir))
        {
            // Check if the file has a valid image extension
            if (!Utilities::HasValidImageExtension(entry.path()))
            {
                continue; // Skip files with invalid extensions
            }

            cv::Mat compareImg = cv::imread(entry.path().string(), cv::IMREAD_GRAYSCALE);
            if (compareImg.empty())
            {
                qDebug() << "Failed to read image:" << entry.path().filename().string();
                continue; // Skip to the next image
            }

            // Resize the compare image to match the size of the detected face
            cv::Mat resizedCompareImg;
            cv::resize(compareImg, resizedCompareImg, faceROI.size());

            // Perform template matching to find the match
            cv::Mat result;
            cv::matchTemplate(resizedCompareImg, gray(faceROI), result, cv::TM_CCORR_NORMED);

            // Find the location of the best match
            cv::Point minLoc, maxLoc;
            cv::minMaxLoc(result, nullptr, &maxVal, nullptr, &maxLoc);

            if (maxVal > 0.2)  // Adjust the threshold as needed
            {
                // Update the best match information
                bestMatchName = entry.path().filename().string();
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

    if (frame.empty())
    {
        qDebug() << "Error: Could not capture a frame from the camera.";
        return;
    }

    // Convert the captured frame to grayscale
    cv::Mat grayFrame;
    cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);

    // Perform face detection using CascadeClassifier
    cv::CascadeClassifier faceCascade(Utilities::CASCADE_FILE_MAIN);
    std::vector<cv::Rect> faces;
    faceCascade.detectMultiScale(grayFrame, faces, 1.1, 4);

    // Draw rectangles around detected faces and perform recognition
    for (const cv::Rect& faceRect : faces)
    {
        // Draw a rectangle around the detected face
        cv::rectangle(frame, faceRect, cv::Scalar(255, 0, 0), 2);

        // Extract the detected face region
        cv::Mat detectedFace = grayFrame(faceRect).clone();

        // Perform recognition using the DetectAndDraw function
        RealTimeFacialRecognition::DetectAndDraw(detectedFace, faceCascade, 1.1);

        // Draw the processed frame with rectangles and recognition results
        cv::rectangle(frame, faceRect, cv::Scalar(0, 165, 255), 2);
        cv::putText(frame, "Recognized", cv::Point(faceRect.x, faceRect.y - 10), cv::FONT_HERSHEY_SIMPLEX, 0.9, cv::Scalar(0, 165, 255), 2);
    }

    // Display the frame with detected faces
    QImage qImage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
    QPixmap pixmap = QPixmap::fromImage(qImage.rgbSwapped());
    QGraphicsView* graphicsView = ui->CameraFeed;
    QGraphicsScene* cameraFeed = graphicsView->scene();
    cameraFeed->clear();
    cameraFeed->addPixmap(pixmap);
}

RealTimeFacialRecognition::~RealTimeFacialRecognition()
{
    delete ui;
}
