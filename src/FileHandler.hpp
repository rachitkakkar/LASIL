#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <string>
#include <stdexcept>

namespace DecafIO {

// Function to read the entire content of a file into a string with error handling
std::string readFileToString(const std::string& filename);

}

#endif // FILE_HANDLER_H