#pragma once

#include "../PCH.h"

std::string CASCADE_FILE_MAIN;
std::string IMAGE_DIR;
std::string LOGGING_DIR;

void Initialize() {
    if (std::filesystem::is_directory(LOGGING_DIR)) {
        qDebug() << "Logging directory exists. Continuing.";
    }
    else {
        std::filesystem::create_directory(LOGGING_DIR);
        if (!std::filesystem::is_directory(LOGGING_DIR)) {
            qDebug() << "Failed to create logging directory.";
            system("pause");
            exit(1);
        }
        qDebug() << "Logging directory does not exist. It has been created. First launch?";
    }

    if (std::filesystem::is_directory(IMAGE_DIR)) {
        qDebug() << "Image directory exists. Continuing.";
    }
    else {
        std::filesystem::create_directory(IMAGE_DIR);
        if (!std::filesystem::is_directory(IMAGE_DIR)) {
            qDebug() << "Failed to create image directory.";
            system("pause");
            exit(1);
        }
        qDebug() << "Image directory does not exist. It has been created. First launch?";
    }

    if (std::filesystem::exists(LOGGING_DIR + "/system.log")) {
        qDebug() << "Log file exists. Appending.";
    }
    else {
        std::ofstream logFile(LOGGING_DIR + "/system.log");
        if (!logFile.is_open()) {
            qDebug() << "Failed to open log file.";
            system("pause");
            exit(1);
        }
        qDebug() << "Log file created successfully. First launch?";
    }
}

// Function to check if a file has a valid image extension
bool hasValidImageExtension(const std::filesystem::path& path) {
    std::string extension = path.extension().string();
    // List of valid image extensions (add more if needed)
    std::vector<std::string> validExtensions = { ".jpg", ".jpeg", ".png" };
    for (const auto& validExtension : validExtensions) {
        if (extension == validExtension) {
            return true;
        }
    }
    return false;
}

void readSettings() {
    // Since running debug mode in Visual Studio loads the executable at the root "/", we have to hardlink the conf file, otherwise it will throw a missing file error.
#ifdef QT_DEBUG
    std::ifstream settingsFile("src/settings-dev.conf");
#else
    std::ifstream settingsFile("settings.conf");
#endif
    if (settingsFile.is_open())
    {
        if (settingsFile.peek() == std::ifstream::traits_type::eof())
        {
            qDebug() << "settings.conf is empty or corrupt.";
            settingsFile.close();
            system("pause");
            exit(1);  // Use exit(1) to indicate an error
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
    else {
        Messages::Info("Failed to open settings.conf file.\n");
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

void logWrite(const std::string& message) {
    std::ofstream logFile(LOGGING_DIR + "/system.log", std::ios::app);
    if (logFile.is_open()) {
        logFile << getCurrentTime() << " "; // Add current time before the log message
        logFile << message << std::endl;
        logFile.close();
        //std::cout << "Message logged successfully." << std::endl;
    }
    else {
        qDebug() << "Unable to open log file.";
        system("pause");
        exit(1);  // Use exit(1) to indicate an error
    }
}
