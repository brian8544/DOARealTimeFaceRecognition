#include <iostream>
#include <filesystem>

// OpenCV 4
#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

//#include "Main.h"
#define CASCADE_FILE_MAIN "D:\\Repositories\\RealtimeFacialRecognition\\contrib\\cascades\\haarcascade_frontalface_alt2.xml"
#define CASCADE_FILE_EYES "D:\\Repositories\\RealtimeFacialRecognition\\contrib\\cascades\\haarcascade_eye_tree_eyeglasses.xml"
#define IMAGE_DIR "D:/Repositories/RealtimeFacialRecognition/contrib/images"

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
        cv::Rect faceROI = faces[i];
        cv::rectangle(img, faceROI, cv::Scalar(255, 0, 0), 2);

        // Compare the face with images in the specified directory
        std::filesystem::path imageDir(IMAGE_DIR);
        for (const auto& entry : std::filesystem::directory_iterator(imageDir))
        {
            cv::Mat compareImg = cv::imread(entry.path().string());
            if (compareImg.empty())
            {
                std::cout << "Failed to read image: " << entry.path().filename().string() << std::endl;
                continue; // Skip to the next image
            }

            // Convert the compare image to grayscale
            cv::cvtColor(compareImg, compareImg, cv::COLOR_BGR2GRAY);

            // Resize the compare image to match the size of the detected face
            cv::resize(compareImg, compareImg, faceROI.size());

            // Perform template matching to find the best match
            cv::Mat result;
            try
            {
                cv::matchTemplate(compareImg, gray(faceROI), result, cv::TM_CCOEFF_NORMED);
            }
            catch (const cv::Exception& e)
            {
                std::cout << "Error occurred during template matching: " << e.what() << std::endl;
                continue; // Skip to the next image
            }

            // Set a threshold for the match
            double threshold = 0.1; // 0.8 = default
            double minVal, maxVal;
            cv::Point minLoc, maxLoc;
            cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

            if (maxVal > threshold)
            {
                // If a match is found, draw an orange box with the filename as a label
                cv::rectangle(img, faceROI, cv::Scalar(0, 165, 255), 2);
                cv::putText(img, entry.path().filename().string(), cv::Point(faceROI.x, faceROI.y - 10), cv::FONT_HERSHEY_SIMPLEX, 0.9, cv::Scalar(0, 165, 255), 2);
            }
        }
    }
}

int main(int argc, char** argv)
{
    // Load the face cascade
    cv::CascadeClassifier faceCascade(CASCADE_FILE_MAIN);

    // Load the eye cascade (optional)
    cv::CascadeClassifier eyeCascade(CASCADE_FILE_EYES);

    // Check if the cascades exist and thus be loaded.
    if (!faceCascade.load(CASCADE_FILE_MAIN))
    {
        // Face cascade could not be found
        std::system("cls");
        std::cout << "Could not find the face cascade." << std::endl;
        std::system("pause");
        return -1;
    }
    else if (!eyeCascade.load(CASCADE_FILE_EYES))
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
