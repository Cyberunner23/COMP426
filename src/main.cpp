#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "BallUtils.hpp"
#include "GLUtils.hpp"

//*********************************************************
// Constants
//*********************************************************
const unsigned int FrameRate = 30;
const float FrameTime = 1.0f / FrameRate;


static void errorCallback(int error, const char* description)
{
    std::cout << "[ERROR][GLFW]: " << description << std::endl;
}



double do_frame_rate_limiting(double& lastFrameStartTime)
{
    double currentTime = glfwGetTime();

    double deltaTime = currentTime - lastFrameStartTime;
    if (deltaTime < FrameTime)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds((unsigned int)((FrameTime - deltaTime) * 1000)));
    }

    currentTime = glfwGetTime();
    double currentFPS = 1 / (currentTime - lastFrameStartTime);

    lastFrameStartTime = glfwGetTime();

    return currentFPS;
}

void display_background()
{
    for (int x = 0; x < 4; ++x)
    {
        for (int y = 0; y < 4; ++y)
        {
            float posX = ((float)(2 * x) + 1) * (float)WinSize / 8;
            float posY = ((float)(2 * y) + 1) * (float)WinSize / 8;
            DrawGradientSquare(glm::vec2{posX, posY});
        }
    }
}

void display_circles(std::vector<BallState> balls)
{
    for (BallState ball : balls)
    {
        DrawCircle(ball.Position, ball.Color, ball.Radius);
    }
}

int main(int argc, char **argv)
{
    GLFWwindow* window;

    glfwSetErrorCallback(errorCallback);
    if (!glfwInit())
    {
        std::cout << "[ERROR][GLFW]: Failed to init GLFW" << std::endl;
        return -1;
    }

    window = glfwCreateWindow(WinSize, WinSize, "COMP 426 A1", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        std::cout << "[ERROR][GLFW]: Failed to create GLFW window" << std::endl;
        return -1;
    }

    glfwMakeContextCurrent(window);

    std::vector<BallState> balls = initialize_balls(argc, argv);

    double lastFrameStartTime = glfwGetTime();
    while(!glfwWindowShouldClose(window))
    {
        do_frame_rate_limiting(lastFrameStartTime);

        glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        display_background();
        display_circles(balls);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    return 0;
}