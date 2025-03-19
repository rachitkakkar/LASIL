#include "FileHandler.hpp"
#include "Logger.hpp"

#include <fstream>
#include <sstream>

namespace DecafIO {

// Function to read the entire content of a file into a string with error handling
std::string readFileToString(const std::string& filename) {
  // Attempt to open the file
  std::ifstream file(filename, std::ios::in | std::ios::binary);
  
  // Check if the file was successfully opened
  if (!file) {
    DecafLogger::Logger::logMessage(DecafLogger::LogType::ERROR, "Could not open file: " + filename);
  }
  
  // Check if the file is empty
  if (file.peek() == std::ifstream::traits_type::eof()) {
    DecafLogger::Logger::logMessage(DecafLogger::LogType::ERROR, "File is empty: " + filename);
  }
  
  // Read the entire file content into a stringstream
  std::stringstream buffer;
  buffer << file.rdbuf();
  
  // Check if reading the file was successful
  if (file.bad()) {
    throw std::runtime_error("Error: Failed to read the file: " + filename);
  }
  
  // Close the file
  file.close();
  
  // Return the content as a string
  return buffer.str();
}

}