#ifndef UTILITIES_H
#define UTILITIES_H

#include "../PCH.h"

namespace Utilities {
    extern std::string CASCADE_FILE_MAIN;
    extern std::string IMAGE_DIR;
    extern std::string LOGGING_DIR;

    bool HasValidImageExtension(const std::filesystem::path& path);
}

#endif // UTILITIES_H
