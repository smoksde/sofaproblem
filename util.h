#pragma once
#include <cmath>
#include <vector>

double degrees_to_radians(double degrees)
{
    return degrees * (M_PI / 180.0);
}

std::vector<double> linspace(double start, double end, int numPoints)
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

std::vector<glm::vec3> linspace_vec3(glm::vec3 start, glm::vec3 end, int numPoints)
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