#include <iostream>

// OpenCV 4
#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "Main.h"

// Function for face detection
void detectAndDraw(cv::Mat& img, cv::CascadeClassifier& cascade, cv::CascadeClassifier& nestedCascade, double scale)
{
    // Convert input image to grayscale
    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

    // Detect faces in the grayscale image
    std::vector<cv::Rect> faces;
    cascade.detectMultiScale(gray, faces, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));

    // Iterate over the detected faces
    for (size_t i = 0; i < faces.size(); i++)
    {
        // Draw a rectangle around the face
        cv::rectangle(img, faces[i], cv::Scalar(255, 0, 0), 2);
    }
}

int main(int argc, char** argv)
{
    // Load the face cascade
    cv::CascadeClassifier faceCascade(CASCADE_FILE_LOCATION);

    // Load the eye cascade (optional)
    cv::CascadeClassifier eyeCascade(EYE_CASCADE_FILE_LOCATION);

    // Check if the cascades exist and thus be loaded.
    if (!faceCascade.load(CASCADE_FILE_LOCATION))
    {
        // Face cascade could not be found
        std::system("cls");
        std::cout << "Could not find the face cascade." << std::endl;
        std::system("pause");
        return -1;
    }
    else if (!eyeCascade.load(EYE_CASCADE_FILE_LOCATION))
    {
        // Eye cascade could not be found
        std::system("cls");
        std::cout << "Could not find the eye cascade." << std::endl;
        std::system("pause");
        return -1;
    }

    // Create a VideoCapture object to capture video from webcam
    cv::VideoCapture capture(0);

    // If the webcam is not opened, then exit
    if (!capture.isOpened())
    {
        std::system("cls");
        std::cout << "Error opening webcam." << std::endl;
        std::system("pause");
        return -1;
    }

    // Loop until the user presses ESC
    for (;;)
    {
        // Capture a frame from the webcam
        cv::Mat frame;
        capture >> frame;

        // If the frame is empty, then break from the loop
        if (frame.empty())
        {
            break;
        }

        // Detect faces in the frame
        detectAndDraw(frame, faceCascade, eyeCascade, 1.1);

        // Display the frame
        cv::imshow("Face Detection", frame);

        // Wait for the user to press a key
        int key = cv::waitKey(1);

        // If the user presses ESC, then break from the loop
        if (key == 27)
        {
            break;
        }
        else if (key == 'q')
        {
            break;
        }
    }

    // Close the VideoCapture object
    capture.release();

    // Destroy all windows
    cv::destroyAllWindows();

    return 0;
}