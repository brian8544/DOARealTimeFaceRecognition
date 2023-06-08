#include <iostream>
#include <filesystem>
#include <fstream>

// OpenCV 4
#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

std::string CASCADE_FILE_MAIN;
std::string IMAGE_DIR;

// Function to check if a file has a valid image extension
bool hasValidImageExtension(const std::filesystem::path& path) {
    std::string extension = path.extension().string();
    // List of valid image extensions (add more if needed)
    std::vector<std::string> validExtensions = { ".jpg", ".jpeg", ".png", ".bmp" };
    for (const auto& validExtension : validExtensions) {
        if (extension == validExtension) {
            return true;
        }
    }
    return false;
}

void readSettings()
{
    // Since running debug mode in Visual Studio loads the executable at the root "/", we have to hardlink the conf file, otherwise it will throw a missing file error.
#ifdef _DEBUG
    std::ifstream settingsFile("src/settings-dev.conf");
#else
    std::ifstream settingsFile("settings.conf");
#endif
    if (settingsFile.is_open())
    {
        if (settingsFile.peek() == std::ifstream::traits_type::eof())
        {
            std::cout << "settings.conf is empty or corrupt." << std::endl;
            settingsFile.close();
        }
        std::string line;
        while (std::getline(settingsFile, line))
        {
            if (line.find("CASCADE_FILE_MAIN") != std::string::npos)
            {
                CASCADE_FILE_MAIN = line.substr(line.find_first_of('"') + 1, line.find_last_of('"') - line.find_first_of('"') - 1);
            }
            else if (line.find("IMAGE_DIR") != std::string::npos)
            {
                IMAGE_DIR = line.substr(line.find_first_of('"') + 1, line.find_last_of('"') - line.find_first_of('"') - 1);
            }
        }
        settingsFile.close();
    }
    else
    {
        std::cout << "Failed to open settings.conf file." << std::endl;
        system("pause");
        exit(1);  // Use exit(1) to indicate an error
    }
}

// Function for face detection
void detectAndDraw(cv::Mat& img, cv::CascadeClassifier& cascade, double scale)
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

        double maxVal = 0.0;  // Store the maximum match value
        std::string bestMatchName;  // Store the name of the best match

        for (const auto& entry : std::filesystem::directory_iterator(imageDir))
        {
            // Check if the entry is a valid image file
            if (!hasValidImageExtension(entry.path()))
            {
                continue;
            }

            // Load the image file
            cv::Mat img2 = cv::imread(entry.path().string(), cv::IMREAD_GRAYSCALE);

            // Resize the image to match the size of the face
            cv::Mat resizedImg;
            cv::resize(img2, resizedImg, faceROI.size());

            // Normalize the resized image
            cv::normalize(resizedImg, resizedImg, 0, 255, cv::NORM_MINMAX, CV_8UC1);

            // Calculate the match value using normalized correlation
            double matchVal = cv::matchShapes(gray(faceROI), resizedImg, cv::CONTOURS_MATCH_I3, 0);

            // Update the best match if the current match value is higher
            if (matchVal > maxVal)
            {
                maxVal = matchVal;
                bestMatchName = entry.path().filename().string();
            }
        }

        // Draw the best match name on the image
        cv::putText(img, bestMatchName, cv::Point(faceROI.x, faceROI.y - 10), cv::FONT_HERSHEY_SIMPLEX, 0.9, cv::Scalar(255, 0, 0), 2);
    }
}

int main()
{
    // Read the settings from the configuration file
    readSettings();

    // Load the main cascade classifier for face detection
    cv::CascadeClassifier faceCascade;
    if (!faceCascade.load(CASCADE_FILE_MAIN))
    {
        std::cout << "Error loading cascade file." << std::endl;
        system("pause");
        return -1;  // Use return -1 to indicate an error
    }

    // Load the mp4 file
    cv::VideoCapture capture("contrib/testing/group.mp4");

    // Check if the video file is opened successfully
    if (!capture.isOpened())
    {
        std::cout << "Error opening video file." << std::endl;
        system("pause");
        return -1;
    }

    // Loop until the end of the video file
    while (capture.isOpened())
    {
        // Capture a frame from the video file
        cv::Mat frame;
        capture >> frame;

        // If the frame is empty, then the video has ended
        if (frame.empty())
        {
            break;
        }

        // Detect faces in the frame
        detectAndDraw(frame, faceCascade, 1.1);

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

    // Release the capture and destroy windows
    capture.release();
    cv::destroyAllWindows();

    return 0;
}
