#pragma once

#include <cmath>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstring>
#include <ctime>

inline const char* readShaderFromFile(const std::string& filePath)
{
    std::ifstream shaderFile(filePath);
    if (!shaderFile.is_open())
    {
        std::cerr << "Failed to open shader file: " << filePath << std::endl;
        return "";
    }

    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    shaderFile.close();

    std::string shaderSourceStr = shaderStream.str();
    char* shaderSource = new char[shaderSourceStr.length() + 1];
    strcpy(shaderSource, shaderSourceStr.c_str());

    return shaderSource;
}

inline double degrees_to_radians(double degrees)
{
    return degrees * (M_PI / 180.0);
}

inline std::vector<double> linspace(double start, double end, int numPoints)
{
    std::vector<double> result;
    if (numPoints <= 1) {
        result.push_back(start);
        return result;
    }
    double step = (end - start) / (numPoints - 1);
    for (int i = 0; i < numPoints; i++) {
        result.push_back(start + i * step);
    }
    result.push_back(end);
    return result;
}

inline std::vector<glm::vec3> linspace_vec3(glm::vec3 start, glm::vec3 end, int numPoints)
{
    std::vector<glm::vec3> result;
    if (numPoints <= 1) {
        result.push_back(start);
        return result;
    }
    glm::vec3 step = (end - start) / (float)(numPoints - 1);
    for (int i = 0; i < numPoints; i++){
        result.push_back(start + (float)i * step);
    }
    result.push_back(end);
    return result;
}

inline double normalPDF(double x, double mean, double stddev) 
{
    return exp(-0.5 * pow((x - mean) / stddev, 2)) / (stddev * sqrt(2 * M_PI));
}

inline void writeToLogFile(const std::string& message) {
    // Open the log file in append mode
    std::ofstream logfile("logfile.txt", std::ios_base::app);

    // Get the current time
    std::time_t now = std::time(nullptr);
    char timeBuffer[80]; // Buffer to store formatted time
    strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

    // Write timestamp and message to the log file
    logfile << "[" << timeBuffer << "] " << message << std::endl;

    // Close the file
    logfile.close();
}

inline std::string buildStringFromYawSequence(const std::vector<double>& vec) {
    // Create a string stream to build the string
    std::ostringstream oss;

    // Iterate over the vector and append each element to the string stream
    for (size_t i = 0; i < vec.size(); ++i) {
        oss << vec[i];
        // Add a comma and space after each element, except for the last one
        if (i < vec.size() - 1) {
            oss << " ";
        }
    }

    // Convert the string stream to a string and return
    return oss.str();
}

inline std::string buildStringFromOffsetSequence(const std::vector<glm::vec3>& vec) {
    // Create a string stream to build the string
    std::ostringstream oss;

    // Iterate over the vector and append each element to the string stream
    for (size_t i = 0; i < vec.size(); ++i) {
        glm::vec3 temp = vec[i];

        oss << temp.x;
        oss << " ";
        oss << temp.y;

        // Add a comma and space after each element, except for the last one
        if (i < vec.size() - 1) {
            oss << " ";
        }
    }

    // Convert the string stream to a string and return
    return oss.str();
}

inline void saveYawVectorToFile(const std::vector<double>& vec, const std::string& filename) {
    std::ofstream outfile(filename, std::ios::binary); // Open file in binary mode

    if (outfile.is_open()) {
        // Write vector size
        size_t size = vec.size();
        outfile.write(reinterpret_cast<const char*>(&size), sizeof(size));

        // Write vector elements
        outfile.write(reinterpret_cast<const char*>(vec.data()), size * sizeof(double));

        outfile.close();
        std::cout << "Vector saved to file successfully." << std::endl;
    } else {
        std::cerr << "Unable to open file for writing." << std::endl;
    }
}

inline std::vector<double> loadYawVectorFromFile(const std::string& filename) {
    std::vector<double> vec;
    std::ifstream infile(filename, std::ios::binary); // Open file in binary mode

    if (infile.is_open()) {
        // Read vector size
        size_t size;
        infile.read(reinterpret_cast<char*>(&size), sizeof(size));

        // Resize vector and read elements
        vec.resize(size);
        infile.read(reinterpret_cast<char*>(vec.data()), size * sizeof(double));

        infile.close();
        std::cout << "Vector loaded from file successfully." << std::endl;
    } else {
        std::cerr << "Unable to open file for reading." << std::endl;
    }

    return vec;
}

inline void saveOffsetVectorToFile(const std::vector<glm::vec3>& vec, const std::string& filename) {
    std::ofstream outfile(filename, std::ios::binary); // Open file in binary mode

    if (outfile.is_open()) {
        // Write the size of the vector
        size_t size = vec.size();
        outfile.write(reinterpret_cast<const char*>(&size), sizeof(size));

        // Write each glm::vec3 element
        for (const auto& v : vec) {
            outfile.write(reinterpret_cast<const char*>(&v), sizeof(glm::vec3));
        }

        outfile.close();
        std::cout << "Vector of glm::vec3 saved to file successfully." << std::endl;
    } else {
        std::cerr << "Unable to open file for writing." << std::endl;
    }
}

inline std::vector<glm::vec3> loadOffsetVectorFromFile(const std::string& filename) {
    std::vector<glm::vec3> vec;
    std::ifstream infile(filename, std::ios::binary); // Open file in binary mode

    if (infile.is_open()) {
        // Read the size of the vector
        size_t size;
        infile.read(reinterpret_cast<char*>(&size), sizeof(size));

        // Resize the vector and read each glm::vec3 element
        vec.resize(size);
        for (size_t i = 0; i < size; ++i) {
            infile.read(reinterpret_cast<char*>(&vec[i]), sizeof(glm::vec3));
        }

        infile.close();
        std::cout << "Vector of glm::vec3 loaded from file successfully." << std::endl;
    } else {
        std::cerr << "Unable to open file for reading." << std::endl;
    }

    return vec;
}