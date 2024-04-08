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
#include "renderer.h"
#include "optimizer.h"

const int frame_width = 1400;
const int frame_height = 1400;
const int time_resolution = 1000;

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("sofaproblem", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, frame_width, frame_height, SDL_WINDOW_OPENGL);
    if (!window)
    {
        std::cerr << "Failed to create SDL window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }
    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    GLenum err = glewInit();
    if (err != GLEW_OK) 
    {
        std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(err) << std::endl;
        return -1;
    }

    const char* vertexShaderSource = readShaderFromFile("shaders/vertex_shader.glsl");
    const char* fragmentShaderSource = readShaderFromFile("shaders/fragment_shader.glsl");

    // Compile and link the shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    float vertices[] = 
    {
        -10.0f, -10.0f, 0.0f,
        -10.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 0.0f,
        0.0f, -10.0f, 0.0f,
        -10.0f, -10.0f, 0.0f,

        -10.0f, 0.5f, 0.0f,
        -10.0f, 10.0f, 0.0f,
        0.5f, 10.0f, 0.0f,

        0.5f, 10.0f, 0.0f,
        0.5f, 0.5f, 0.0f,
        -10.0f, 0.5f, 0.0f,

        0.5f, 10.0f, 0.0f,
        10.0f, 10.0f, 0.0f,
        10.0f, -10.0f, 0.0f,

        10.0f, -10.0f, 0.0f,
        0.5f, -10.0f, 0.0f,
        0.5f, 10.0f, 0.0f
    };

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Create an off-screen framebuffer
    GLuint framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Create and attach a texture to the framebuffer
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frame_width, frame_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // Check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Framebuffer is not complete!" << std::endl;
        return -1;
    }

    // Bind the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // Reset viewport to window dimensions
    int width, height;
    SDL_GL_GetDrawableSize(window, &width, &height);
    glViewport(0, 0, width, height);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow(window);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    /*Uint32 start_time = SDL_GetTicks();
    Uint32 current_time = SDL_GetTicks();

    int frame_id = 0;
    int fps = 60;*/

    glm::vec3 anchor = glm::vec3(0.0f, 0.0f, 0.0f);

    glm::vec3 offset_start = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 offset_end = glm::vec3(0.0f, 0.0f, 0.0f);

    // From here

    // std::vector<double> yaw_sequence = linspace(degrees_to_radians(0.0), degrees_to_radians(90.0), time_resolution);
    // std::vector<glm::vec3> offset_sequence = linspace_vec3(offset_start, offset_end, time_resolution);
    std::vector<double> yaw_sequence = loadYawVectorFromFile("yaw_sequence.txt");
    std::vector<glm::vec3> offset_sequence = loadOffsetVectorFromFile("offset_sequence.txt");

    int population_amount = 10;
    int surviver_amount = 1;

    Renderer renderer(frame_width, frame_height, window, shaderProgram);

    Optimizer optimizer(time_resolution, yaw_sequence, offset_sequence, population_amount, surviver_amount);

    std::vector<std::vector<double>> yaw_sequences;
    std::vector<std::vector<glm::vec3>> offset_sequences;
    std::vector<int> population_scores(population_amount);

    int generations = 1000;

    std::vector<int> generation_scores(generations);

    bool quit = false;

    for (int i = 0; i < generations; i++)
    {
        int max_index = 0;
        int max_value = 0;

        optimizer.loadPopulation(yaw_sequences, offset_sequences);
        for (int j = 0; j < population_amount; j++)
        {
            SDL_Event event;
            while (SDL_PollEvent(&event)) 
            {
                if (event.type == SDL_QUIT) 
                {
                    quit = true;
                }
            }

            if (quit)
            {
                glDeleteVertexArrays(1, &VAO);
                glDeleteBuffers(1, &VBO);
                glDeleteProgram(shaderProgram);
                glDeleteShader(vertexShader);
                glDeleteShader(fragmentShader);

                SDL_GL_DeleteContext(glContext);
                SDL_DestroyWindow(window);
                SDL_Quit();
                return 0;
            }

            int remaining_pixel = renderer.Render(time_resolution, anchor, yaw_sequences[j], offset_sequences[j]);
            std::cout << "Generation " << i + 1 << " Individual " << j << " Pixel " << remaining_pixel << std::endl;
            population_scores[j] = remaining_pixel;

            if (remaining_pixel > max_value)
            {
                max_index = j;
                max_value = remaining_pixel;
            }
        }

        generation_scores[i] = max_value;

        // Log the score of the best and its parameter set
        // std::string message = buildStringFromYawSequence(yaw_sequences[max_index]) + buildStringFromOffsetSequence(offset_sequences[max_index]);
        // writeToLogFile(message);

        if (i + 1 == generations)
        {
            saveYawVectorToFile(yaw_sequences[max_index], "yaw_sequence.txt");
            saveOffsetVectorToFile(offset_sequences[max_index], "offset_sequence.txt");
        }

        optimizer.setSurvivedIndividual(yaw_sequences[max_index], offset_sequences[max_index]);
        optimizer.inflatePopulation();
    }

    for (int i = 0; i < generations; i++)
    {
        std::cout << "Score of generation " << i + 1 << " is " << generation_scores[i] << std::endl;
    }

    
    /*
    // Main loop
    bool quit = false;
    while (!quit && frame_id < time_resolution) 
    {
        SDL_Event event;
        while (SDL_PollEvent(&event)) 
        {
            if (event.type == SDL_QUIT) 
            {
                quit = true;
            }
        }

        current_time = SDL_GetTicks();
        Uint32 elapsed_time = current_time - start_time;

        if (elapsed_time >= 1000.0 / fps)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, offset_sequence[frame_id]);
            model = glm::translate(model, anchor);
            model = glm::rotate(model, float(yaw_sequence[frame_id]), glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::translate(model, -anchor);

            unsigned int modelUniformLocation = glGetUniformLocation(shaderProgram, "model");
            glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, glm::value_ptr(model));
            
            glDrawArrays(GL_TRIANGLES, 0, 18);

            SDL_GL_SwapWindow(window);

            start_time = SDL_GetTicks();
            frame_id++;
        }

        if (frame_id == time_resolution)
        {
            // Count percentage remaining in clear color
            int totalPixels = width * height;
            int clearColorPixels = 0;
            std::vector<GLubyte> pixelData(totalPixels * 4); // RGBA format
            glReadBuffer(GL_FRONT_LEFT);
            glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixelData.data());

            for (int i = 0; i < totalPixels * 4; i += 4)
            {
                if (pixelData[i] == 255 && pixelData[i + 1] == 255 && pixelData[i + 2] == 255 && pixelData[i + 3] == 255) 
                {
                    clearColorPixels++;
                }
            }

            float fractalRemaining = (static_cast<float>(clearColorPixels) / totalPixels);
            float percentageRemaining = (static_cast<float>(clearColorPixels) / totalPixels) * 100.0f;

            std::cout << "Fractal remaining in clear color: " << fractalRemaining << std::endl;
            std::cout << "Percentage remaining in clear color: " << percentageRemaining << "%" << std::endl;
            std::cout << "Remaining pixel: " << static_cast<float>(clearColorPixels) << " Total pixel: " << totalPixels << std::endl;
        }
    }*/

    

    // Until here it can be encapsulated

    // Clean up
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}