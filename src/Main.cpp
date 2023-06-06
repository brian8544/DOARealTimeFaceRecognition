#include <iostream>

// OpenCV 4
#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "Main.h"

using namespace std;
using namespace cv;

// Function for face detection
void detectAndDraw(Mat& img, CascadeClassifier& cascade, CascadeClassifier& nestedCascade, double scale)
{
    // Convert input image to grayscale
    Mat gray;
    cvtColor(img, gray, COLOR_BGR2GRAY);

    // Detect faces in the grayscale image
    vector<Rect> faces;
    cascade.detectMultiScale(gray, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

    // Iterate over the detected faces
    for (size_t i = 0; i < faces.size(); i++)
    {
        // Draw a rectangle around the face
        rectangle(img, faces[i], Scalar(255, 0, 0), 2);

        /*
        // Right now it's very buggy, something takes lots of frames once initiated. Sometimes draws random dots in corners. Probably has to do with sunlight particles.
        // If a nested cascade has been loaded, then detect eyes within the detected faces
        if (nestedCascade.empty() == false)
        {
            vector<Rect> eyes;
            nestedCascade.detectMultiScale(gray(faces[i]), eyes, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(10, 10));
            // Iterate over the detected eyes
            for (size_t j = 0; j < eyes.size(); j++)
            {
                // Draw a circle around each eye
                circle(img, eyes[j].tl(), 5, Scalar(0, 255, 0), -1);
            }
        }
        */
    }
}

int main(int argc, const char** argv)
{
    // Load the face cascade
    CascadeClassifier faceCascade(CASCADE_FILE_LOCATION);

    // Load the eye cascade (optional)
    CascadeClassifier eyeCascade(EYE_CASCADE_FILE_LOCATION);

    // Check if the cascades exist and thus be loaded.
    if (!faceCascade.load(CASCADE_FILE_LOCATION)) 
    {
        // Face cascade could not be found
        system("cls");
        std::cout << "Could not find the face cascade." << std::endl;
        system("pause");
        return -1;
    }
    else if (!eyeCascade.load(EYE_CASCADE_FILE_LOCATION)) 
    {
        // Eye cascade could not be found
        system("cls");
        std::cout << "Could not find the eye cascade." << std::endl;
        system("pause");
        return -1;
    }

    // Create a VideoCapture object to capture video from webcam
    VideoCapture capture(0);

    // If the webcam is not opened, then exit
    if (!capture.isOpened())
    {
        system("cls");
        cout << "Error opening webcam." << endl;
        system("pause");
        return -1;
    }

    // Loop until the user presses ESC
    for (;;)
    {
        // Capture a frame from the webcam
        Mat frame;
        capture >> frame;

        // If the frame is empty, then break from the loop
        if (frame.empty())
        {
            break;
        }

        // Detect faces in the frame
        detectAndDraw(frame, faceCascade, eyeCascade, 1.1);

        // Display the frame
        imshow("Face Detection", frame);

        // Wait for the user to press a key
        int key = waitKey(1);

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
    destroyAllWindows();

    return 0;
}