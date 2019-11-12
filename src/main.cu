#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "BallUtils.hpp"
#include "GLUtils.hpp"

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

void display_circles(std::vector<BallState> balls)
{
    for (BallState ball : balls)
    {
        DrawCircle(ball.Position, ball.Color, ball.Radius);
    }
}

void handle_wall_collision(BallState& ball)
{
    if (collides_with_edge_x(ball))
    {
        float vX = ball.Velocity.x;
        ball.Velocity.x = -vX;
    }

    if (collides_with_edge_y(ball))
    {
        float vY = ball.Velocity.y;
        ball.Velocity.y = -vY;
    }
}

void handle_ball_ball_collision(BallState& b1, BallState& b2)
{

    glm::vec2 delta = b1.Position - b2.Position;
    float r = (float)b1.Radius + (float)b2.Radius;
    float dist2 = glm::dot(delta, delta);

    if (dist2 >= r * r)
    {
        return;
    }

    float d = glm::length(delta);

    glm::vec2 mtd;
    if (d != 0.0f)
    {
        mtd = delta * ((((float)b1.Radius + (float)b2.Radius) - d)/d);
    }
    else
    {
        d = (float)b1.Radius +  (float)b2.Radius - 1.0f;
        delta = glm::vec2 {b1.Radius + b2.Radius, 0.0f};
        mtd = delta * ((((float)b1.Radius + (float)b2.Radius) - d)/d);
    }

    float im1 = 1 / b1.Mass; // inverse mass quantities
    float im2 = 1 / b2.Mass;

    b1.Position = b1.Position + (mtd * (im1 / (im1 + im2)));
    b2.Position = b2.Position - (mtd * (im1 / (im1 + im2)));

    glm::vec2 v = (b1.Velocity - b2.Velocity);
    float vn = glm::dot(v, glm::normalize(mtd));

    if (vn > 0.0f) return;

    float i = (-(1.0f + 0.85f) * vn) / (im1 + im2);
    glm::vec2 impulse = mtd * i * 0.001f;

    b1.Velocity = b1.Velocity + (impulse * im1);
    b2.Velocity = b2.Velocity - (impulse * im2);
}

void handle_collisions(std::vector<BallState>& balls)
{
    for (int i = 0; i < balls.size(); ++i)
    {
        handle_wall_collision(balls[i]);

        for(int j = i + 1; j < balls.size(); j++)
        {
            handle_ball_ball_collision(balls[i], balls[j]);
        }
    }
}

__global__ void update_ball_position(BallState* balls, double deltaT)
{
    auto id = threadIdx.x;
    update_ball(balls[id], deltaT);
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
    std::vector<BallState> balls = initialize_balls(argc, argv);
    auto count = balls.size();
    auto memorySize = count * sizeof(BallState);
    // Device
    std::cout << memorySize << std::endl;
    BallState* deviceBalls;
    cudaMalloc(&deviceBalls, memorySize);

    double lastFrameStartTime = glfwGetTime();
    while(!glfwWindowShouldClose(window))
    {
        double deltaT = do_frame_rate_limiting(lastFrameStartTime);

        glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // CopyHost to GPU
        cudaMemcpy(deviceBalls, balls.data(), memorySize, cudaMemcpyHostToDevice);

        int blockSize = count;
        int gridSize = 1;
        update_ball_position<<<gridSize, blockSize>>>(deviceBalls, deltaT);
        cudaMemcpy(balls.data(), deviceBalls, memorySize, cudaMemcpyDeviceToHost);

        handle_collisions(balls);

        display_background();
        display_circles(balls);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    cudaFree(deviceBalls);

    return 0;
}