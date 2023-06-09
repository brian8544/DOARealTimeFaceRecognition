#include <iostream>

// Color constants
const std::string RESET_COLOR = "\033[0m";
const std::string BLACK = "\033[30m";
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string BLUE = "\033[34m";
const std::string MAGENTA = "\033[35m";
const std::string CYAN = "\033[36m";
const std::string WHITE = "\033[37m";
const std::string BRIGHT_BLACK = "\033[90m";
const std::string BRIGHT_RED = "\033[91m";
const std::string BRIGHT_GREEN = "\033[92m";
const std::string BRIGHT_YELLOW = "\033[93m";
const std::string BRIGHT_BLUE = "\033[94m";
const std::string BRIGHT_MAGENTA = "\033[95m";
const std::string BRIGHT_CYAN = "\033[96m";
const std::string BRIGHT_WHITE = "\033[97m";

namespace Messages {

    // Function to print the current time inline with std::cout
    static void printCurrentTime()
    {
        time_t now = time(0);
        struct tm timeInfo;
        localtime_s(&timeInfo, &now);
        char buffer[80];
        strftime(buffer, sizeof(buffer), "[%d/%m/%Y %H:%M]", &timeInfo);
        std::cout << buffer << " ";
    }

	static void Info(const std::string& text) {
        printCurrentTime();
        std::cout << GREEN << "[INFO]: " << text << RESET_COLOR << std::endl;
	}

    static void Notice(const std::string& text) {
        printCurrentTime();
        std::cout << YELLOW << "[NOTICE]: " << text << RESET_COLOR << std::endl;
    }

    static void Error(const std::string& text) {
        printCurrentTime();
        std::cerr << RED << "[ERROR]: " << text << RESET_COLOR << std::endl;
    }

}