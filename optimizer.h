#pragma once

#include <iostream>
#include <vector>
#include <random>

#include "libs/glm/glm.hpp"

#include "util.h"

class Optimizer
{
    private:
    int time_resolution;
    int population_amount;
    int surviver_amount;

    std::vector<double> root_yaw_sequence;
    std::vector<glm::vec3> root_offset_sequence;

    std::vector<double> survived_yaw_sequence;
    std::vector<glm::vec3> survived_offset_sequence;

    std::vector<std::vector<double>> yaw_sequences;
    std::vector<std::vector<glm::vec3>> offset_sequences;

    protected:

    public:
    Optimizer(int time_resolution, std::vector<double> root_yaw_sequence, std::vector<glm::vec3> root_offset_sequence, int population_amount, int surviver_amount);
    void inflatePopulation();
    std::vector<double> addNoiseToYaw(std::vector<double> yaws);
    std::vector<glm::vec3> addNoiseToOffset(std::vector<glm::vec3> offsets);
    void loadPopulation(std::vector<std::vector<double>>& yaws, std::vector<std::vector<glm::vec3>>& offsets);
    void setSurvivedIndividual(std::vector<double> yaw, std::vector<glm::vec3> offset);
};