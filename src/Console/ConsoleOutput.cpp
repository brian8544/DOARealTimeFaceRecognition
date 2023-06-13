#pragma once
#include <iostream>
#include "Colors.h"

namespace Messages {

    // Function to print the current time inline with std::cout
    inline static void printCurrentTime()
    {
        time_t now = time(0);
        struct tm timeInfo;
        localtime_s(&timeInfo, &now);
        char buffer[80];
        strftime(buffer, sizeof(buffer), "[%d/%m/%Y %H:%M]", &timeInfo);
        std::cout << buffer << " ";
    }

    /*
    static void Info(const std::string& text) {
        printCurrentTime();
        std::cout << GREEN << "[INFO]: " << text << RESET_COLOR << "";
    }
    */

    static void Info(const std::string& text) {
        std::cout << GREEN << "" << text << RESET_COLOR << "";
    }

    static void Notice(const std::string& text) {
        std::cout << YELLOW << "" << text << RESET_COLOR << "";
    }

    static void Error(const std::string& text) {
        std::cout << RED << "" << text << RESET_COLOR << "";
    }

}