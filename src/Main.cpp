#include <iostream>
#include <filesystem>
#include <fstream>
#include <ctime>

#include "Console/ConsoleOutput.cpp"

// OpenCV 4
#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

std::string CASCADE_FILE_MAIN;
std::string IMAGE_DIR;
std::string LOGGING_DIR;

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
            Messages::Info("settings.conf is empty or corrupt.");
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
            else if (line.find("LOGGING_DIR") != std::string::npos)
            {
                LOGGING_DIR = line.substr(line.find_first_of('"') + 1, line.find_last_of('"') - line.find_first_of('"') - 1);
            }
        }
        settingsFile.close();
    }
    else
    {
        Messages::Info("Failed to open settings.conf file.");
        system("pause");
        exit(1);  // Use exit(1) to indicate an error
    }
}

std::string getCurrentTime() {
    time_t now = time(0);
    struct tm timeInfo;
    localtime_s(&timeInfo, &now);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "[%d/%m/%Y %H:%M]", &timeInfo);
    return buffer;
}

void log(const std::string& message) {
    std::ofstream logFile(LOGGING_DIR + "/system.log", std::ios::app);
    if (logFile.is_open()) {
        logFile << getCurrentTime() << " "; // Add current time before the log message
        logFile << message << std::endl;
        logFile.close();
        //std::cout << "Message logged successfully." << std::endl;
    }
    else {
        Messages::Error("Unable to open log file.");
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
            // Check if the file has a valid image extension
            if (!hasValidImageExtension(entry.path()))
            {
                continue; // Skip files with invalid extensions
            }

            cv::Mat compareImg = cv::imread(entry.path().string());
            if (compareImg.empty())
            {
                Messages::Error("Failed to read image: " + entry.path().filename().string());
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
                cv::matchTemplate(compareImg, gray(faceROI), result, cv::TM_CCOEFF_NORMED);
            }
            catch (const cv::Exception& e)
            {
                Messages::Error("Error occurred during template matching: " + std::string(e.what()));
                continue; // Skip to the next image
            }

            // Set a threshold for the match
            double threshold = 0.2;                             // 0.8 = default
            double minVal;                                      //  double maxVal;
            cv::Point minLoc;                                   //  cv::Point maxLoc;
            cv::minMaxLoc(result, &minVal, &maxVal, &minLoc);   //  cv::minMaxLoc(result, nullptr, &maxVal, nullptr, &maxLoc);

            if (maxVal > threshold)
            {
                // If a better match is found, update the best match information
                bestMatchName = entry.path().filename().string();
            }
            else if (maxVal > 0.1 && entry.path().filename().string() != bestMatchName)
            {
                // Print other matches to the console
                Messages::Info("Other match:  " + entry.path().filename().string());
                log("Hello World");
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

int main(int argc, char** argv)
{
    readSettings();

    // Load the face cascade
    cv::CascadeClassifier faceCascade(CASCADE_FILE_MAIN);

    // Check if the cascades exist and thus be loaded.
    if (!faceCascade.load(CASCADE_FILE_MAIN))
    {
        // Face cascade could not be found
        std::system("cls");
        Messages::Error("Could not find the face cascade.");
        std::system("pause");
        return -1;
    }

    // Create a VideoCapture object to capture video from webcam
    cv::VideoCapture capture(0);

    // If the webcam is not opened, then exit
    if (!capture.isOpened())
    {
        std::system("cls");
        Messages::Error("Error opening camera.");
        std::system("pause");
        return -1;
    }

    // Create the named window
    cv::namedWindow("Face Detection", cv::WINDOW_NORMAL);

    // Set initial size for the window
    cv::resizeWindow("Face Detection", 300, 300);

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
        detectAndDraw(frame, faceCascade, 1.1);

        // Display the frame
        cv::imshow("Face Detection", frame);

        // Wait for the user to press a key
        int key = cv::waitKey(1);

        // If the user presses Q, then break from the loop
        if (key == 27 ||  key == 'q' || key == 'Q')
        {
            break;
        }
    }

    // Close the VideoCapture object
    capture.release();

    // Destroy the window
    cv::destroyWindow("Face Detection");

    return 0;
}
