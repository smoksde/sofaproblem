#include "renderer.h"

Renderer::Renderer(int frame_width, int frame_height, SDL_Window* window, GLuint shaderProgram)
: FRAME_WIDTH(frame_width), FRAME_HEIGHT(frame_height), window(window), shaderProgram(shaderProgram)
{
   
}

int Renderer::Render(int time_resolution, glm::vec3 anchor, std::vector<double> yaw_sequence, std::vector<glm::vec3> offset_sequence)
{
    // Clean window
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Render and stack frames
    for (int i = 0; i < time_resolution; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, offset_sequence[i]);
        model = glm::translate(model, anchor);
        model = glm::rotate(model, float(yaw_sequence[i]), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::translate(model, -anchor);

        unsigned int modelUniformLocation = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, glm::value_ptr(model));
        
        glDrawArrays(GL_TRIANGLES, 0, 18);
    }
    SDL_GL_SwapWindow(window);

    SDL_Delay(1);

    // Count percentage remaining in clear color
    int totalPixels = FRAME_WIDTH * FRAME_HEIGHT;
    int clearColorPixels = 0;
    std::vector<GLubyte> pixelData(totalPixels * 4); // RGBA format
    glReadBuffer(GL_FRONT_LEFT);
    glReadPixels(0, 0, FRAME_WIDTH, FRAME_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, pixelData.data());

    for (int i = 0; i < totalPixels * 4; i += 4)
    {
        if (pixelData[i] == 255 && pixelData[i + 1] == 255 && pixelData[i + 2] == 255 && pixelData[i + 3] == 255) 
        {
            clearColorPixels++;
        }
    }

    return clearColorPixels;
}