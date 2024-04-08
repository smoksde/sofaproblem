#include "optimizer.h"

Optimizer::Optimizer(int time_resolution, std::vector<double> root_yaw_sequence, std::vector<glm::vec3> root_offset_sequence, int population_amount, int surviver_amount)
: time_resolution(time_resolution), root_yaw_sequence(root_yaw_sequence), root_offset_sequence(root_offset_sequence), population_amount(population_amount), surviver_amount(surviver_amount)
{
    survived_yaw_sequence = root_yaw_sequence;
    survived_offset_sequence = root_offset_sequence;

    std::vector<std::vector<double>> init_yaw_sequences(population_amount, std::vector<double>(time_resolution));
    std::vector<std::vector<glm::vec3>> init_offset_sequences(population_amount, std::vector<glm::vec3>(time_resolution));

    yaw_sequences = init_yaw_sequences;
    offset_sequences = init_offset_sequences;

    std::cout << "Constructor of Optimizer" << std::endl;

    inflatePopulation();

    std::cout << "Constructor of Optimizer" << std::endl;
}

void Optimizer::inflatePopulation()
{
    for (int i = 0; i < population_amount; i++)
    {
        if (i == 0)
        {
            yaw_sequences[i] = survived_yaw_sequence;
            offset_sequences[i] = survived_offset_sequence;
        }
        else
        {
            yaw_sequences[i] = addNoiseToYaw(survived_yaw_sequence);
            offset_sequences[i] = addNoiseToOffset(survived_offset_sequence);
        }
    }
}

std::vector<double> Optimizer::addNoiseToYaw(std::vector<double> yaws) {
    std::random_device rd;
    std::mt19937 gen(rd());

    double lower_bound = -0.1;
    double upper_bound = 0.1;
    std::uniform_real_distribution<double> distribution(lower_bound, upper_bound);

    lower_bound = 0;
    upper_bound = time_resolution - 1;
    std::uniform_real_distribution<double> mean_distribution(lower_bound, upper_bound);

    lower_bound = 1;
    upper_bound = time_resolution - 1 / 10.0;
    std::uniform_real_distribution<double> stddev_distribution(lower_bound, upper_bound);

    double mean = mean_distribution(gen);
    double stddev = stddev_distribution(gen);

    double multiplier = distribution(gen);

    for (size_t i = 0; i < time_resolution; i++) {
        double add = multiplier * normalPDF((double)i, mean, stddev);
        yaws[i] = yaws[i];
    }

    return yaws;
}

std::vector<glm::vec3> Optimizer::addNoiseToOffset(std::vector<glm::vec3> offsets)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    double lower_bound = -0.01;
    double upper_bound = 0.01;
    std::uniform_real_distribution<double> distribution(lower_bound, upper_bound);

    lower_bound = 0;
    upper_bound = time_resolution - 1;
    std::uniform_real_distribution<double> mean_distribution(lower_bound, upper_bound);

    lower_bound = 1;
    upper_bound = time_resolution - 1 / 10.0;
    std::uniform_real_distribution<double> stddev_distribution(lower_bound, upper_bound);

    double mean_x = mean_distribution(gen);
    double stddev_x = stddev_distribution(gen);
    double multiplier_x = distribution(gen);

    double mean_y = mean_distribution(gen);
    double stddev_y = stddev_distribution(gen);
    double multiplier_y = distribution(gen);

    for (int i = 0; i < time_resolution; i++)
    {
        double add_x = multiplier_x * normalPDF((double)i, mean_x, stddev_x);
        double add_y = multiplier_y * normalPDF((double)i, mean_y, stddev_y);
        offsets[i] = offsets[i] + glm::vec3((float)add_x, (float)add_y, 0.0f);
    }

    return offsets;
}

void Optimizer::loadPopulation(std::vector<std::vector<double>>& yaws, std::vector<std::vector<glm::vec3>>& offsets)
{
    yaws = yaw_sequences;
    offsets = offset_sequences;
}

void Optimizer::setSurvivedIndividual(std::vector<double> yaws, std::vector<glm::vec3> offsets)
{
    survived_yaw_sequence = yaws;
    survived_offset_sequence = offsets;
}