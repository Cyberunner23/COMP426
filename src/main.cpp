#include <chrono>
#include <iostream>
#include <thread>

#include "BallUtils.hpp"
#include "GLUtils.hpp"
#include "CLUtils.hpp"

//*********************************************************
// Constants
//*********************************************************
const unsigned int FrameRate = 30;
const float FrameTime = 1.0f / FrameRate;


static void error_callback(int error, const char* description)
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
    double deltaT = currentTime - lastFrameStartTime;

    lastFrameStartTime = glfwGetTime();

    return deltaT;
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


void display_circles(BallState& state)
{
    for (auto i = 0; i < state.Count; ++i)
    {
        DrawCircle(glm::vec2 {state.Position[i].x, state.Position[i].y}, state.Color[i], state.Radius[i]);
    }
}


int main(int argc, char **argv)
{
    GLFWwindow* window;

    glfwSetErrorCallback(error_callback);
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

    // Host
    int count = 0;
    BallState state = initialize_balls(argc, argv, count);

    CLBallState clBallState{};
    CLState clState{};
    if (InitOpenCL(state, clBallState, clState))
    {
        std::cout << "INIT OPENCL FAILED!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        return -1;
    }
    else
    {
        std::cout << "INIT OPENCL SUCCESS" << std::endl;
    }

    cl_uint winSize = (cl_uint)WinSize;
    if (!SetBallUpdateKernelParamsInit(clBallState, clState, gravity, winSize))
    {
        std::cerr << "SetBallUpdateKernelParamsInit failed!" << std::endl;
        return -1;
    }

    if (!SetBallCollisionKernelParamsInit(clBallState, clState, winSize))
    {
        std::cerr << "SetBallCollisionKernelParamsInit failed!" << std::endl;
        return -1;
    }



    double lastFrameStartTime = glfwGetTime();
    while(!glfwWindowShouldClose(window))
    {
        float deltaT = (float)do_frame_rate_limiting(lastFrameStartTime);
        if (!BallUpdateBufferUpdate(clState, clBallState, deltaT))
        {
            std::cerr << "Failed to update delaT!!!" << std::endl;
            return -1;
        }

        glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if (!RunKernels(clState, clBallState))
        {
            std::cerr << "Failed to run kernels!!" << std::endl;
            return -1;
        }

        if(!ReadPositionBuffer(state, clBallState, clState))
        {
            std::cerr << "Failed to read buffer data!!!" << std::endl;
            return -1;
        }

        display_background();
        display_circles(state);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}