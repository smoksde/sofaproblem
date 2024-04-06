#include <SDL2/SDL.h>
//#include <SDL2/SDL_opengl.h>
#include <GL/glew.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <cmath>

#include "libs/glm/glm.hpp"
#include "libs/glm/ext/matrix_transform.hpp"
#include "libs/glm/gtc/matrix_transform.hpp"
#include "libs/glm/gtc/type_ptr.hpp"

#include "util.h"

const int largerWidth = 1400;
const int largerHeight = 1400;

const int width = 1400;
const int height = 1400;

const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    uniform mat4 model; // Model matrix
    void main()
    {
        gl_Position = model * vec4(aPos, 1.0);
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    void main()
    {
        FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    }
)";



int main()
{
    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Off-screen Rendering", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
    if (!window)
    {
        std::cerr << "Failed to create SDL window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }
    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    // Initialize GLEW
    GLenum err = glewInit();
    if (err != GLEW_OK) 
    {
        std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(err) << std::endl;
        return -1;
    }

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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, largerWidth, largerHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
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

    // Initialize timing variables
    Uint32 start_time = SDL_GetTicks();
    Uint32 current_time = SDL_GetTicks();

    int frame_id = 0;
    int fps = 60;
    int time_resolution = 1000;

    glm::vec3 anchor = glm::vec3(0.0f, 0.0f, 0.0f);

    glm::vec3 offset_start = glm::vec3(0.5f, 0.0f, 0.0f);
    glm::vec3 offset_end = glm::vec3(-0.5f, 0.0f, 0.0f);

    std::vector<double> yaw_sequence = linspace(degrees_to_radians(0.0), degrees_to_radians(90.0), time_resolution);
    std::vector<glm::vec3> offset_sequence = linspace_vec3(offset_start, offset_end, time_resolution);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

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

        // Render to the off-screen framebuffer
        /*glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glViewport(0, 0, largerWidth, largerHeight);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        */

        current_time = SDL_GetTicks();
        Uint32 elapsed_time = current_time - start_time;

        if (elapsed_time >= 1000 / fps)
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
    }

    // SDL_Delay(2000);
    // SDL_GL_SwapWindow(window);

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
