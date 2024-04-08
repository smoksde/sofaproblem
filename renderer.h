#pragma once

#include <iostream>
#include <vector>
#include <ctime>
#include <cmath>

#include <SDL2/SDL.h>
#include <GL/glew.h>

#include "libs/glm/glm.hpp"
#include "libs/glm/ext/matrix_transform.hpp"
#include "libs/glm/gtc/matrix_transform.hpp"
#include "libs/glm/gtc/type_ptr.hpp"
#include "util.h"

class Renderer
{
    private:
    GLuint shaderProgram;
    SDL_Window* window;

    const int FRAME_WIDTH;
    const int FRAME_HEIGHT;

    protected:
    public:
    Renderer(int frame_width, int frame_height, SDL_Window* window, GLuint shaderProgram);
    int Render(int time_resolution, glm::vec3 anchor, std::vector<double> yaw_sequence, std::vector<glm::vec3> offset_sequence);
};