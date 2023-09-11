# Real-Time Facial Recognition -- failure, to be removed soon

This script detects faces in real-time using the OpenCV library. It captures frames from a camera/webcam and applies face detection algorithms to identify faces in the frames. It also compares the detected faces with a set of reference images to find potential matches.

## Prerequisites

-   OpenCV 4 library (4.6.0) 
-   C++ compiler (tested with: Visual Studio)

## Installation

1.  Install OpenCV 4 library on your system.
2.  Compile the script using a C++ compiler.

## Usage

1.  Place the script in the desired location on your system.
2.  Create a settings configuration file named `settings.conf` in the same directory as the script. Alternatively, for debug mode in Visual Studio, use `src/settings-dev.conf`.
3.  Run the compiled script from the command line.
4.  The script will open a window displaying the live video feed from the webcam.
5.  The script will detect faces in each frame and draw rectangles around them.
6.  If a matching face is found among the reference images, an orange box will be drawn around the face, and the name of the best match will be displayed as a label.
7.  Other potential matches will be printed to the console.

## Configuration

The script reads its configuration from the `settings.conf` file. Ensure that the configuration file is present and properly formatted. The following configuration parameters are used:

-   `CASCADE_FILE_MAIN`: Path to the XML file containing the trained face cascade classifier.
-   `IMAGE_DIR`: Path to the directory containing the reference images for comparison.

## Face Detection Algorithm

The script uses the following steps to perform face detection:

1.  Read the settings from the configuration file.
2.  Load the face cascade classifier from the specified XML file.
3.  Open the webcam using the VideoCapture object.
4.  Capture frames from the webcam.
5.  Convert each frame to grayscale.
6.  Detect faces in the grayscale frame using the face cascade classifier.
7.  For each detected face:
    -   Draw a rectangle around the face on the original frame.
    -   Compare the face with images in the specified directory.
    -   Find the best match among the reference images using template matching.
    -   If a match is found, draw an orange box around the face and display the name of the best match as a label.
    -   Print other potential matches to the console.
8.  Display the frame with the detected faces.
9.  Wait for the user to press a key.
10.  If the user presses ESC or 'q', exit the loop.
11.  Release the webcam and close the window.

## Customization

-   Add more valid image extensions to the `validExtensions` vector in the `hasValidImageExtension` function if needed.
-   Adjust the detection parameters in the `detectAndDraw` function to optimize face detection based on your requirements.
-   Modify the threshold value for template matching in the `detectAndDraw` function to control the matching sensitivity.

## Notes

-   Ensure that the required XML file for the face cascade classifier exists at the specified path.
-   Make sure the reference images are stored in the directory specified by `IMAGE_DIR`.
-   The script may throw errors if there are issues with file paths, file reading, or webcam availability.
