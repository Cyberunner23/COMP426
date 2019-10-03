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

void handle_wall_collisions(std::vector<BallState>& balls)
{
    for (BallState& ball : balls)
    {
        handle_wall_collision(ball);
    }
}

glm::vec2 compute_elastic_collision(glm::vec2 x1, glm::vec2 x2, glm::vec2 v1, glm::vec2 v2, float m1, float m2)
{
    std::cout << "m1: " << m1 << " m2: " << m2 << std::endl;
    return v1 - /*((2 * m2) / (m1 + m2)) **/ glm::dot((v1 - v2), (x1 - x2)) * (x1 - x2);
}

void handle_ball_ball_collision(BallState& b1, BallState& b2)
{
    b1.Velocity = compute_elastic_collision(b1.Position, b2.Position, b1.Velocity, b2.Velocity, b1.Mass, b2.Mass);
    b2.Velocity = compute_elastic_collision(b2.Position, b1.Position, b2.Velocity, b1.Velocity, b2.Mass, b1.Mass);
}

void handle_ball_ball_collisions(std::vector<BallState>& balls)
{
    for (int i = 0; i < balls.size(); ++i)
    {
        for (int j = i + 1; j < balls.size(); ++j)
        {
            if (collides(balls.at(i), balls.at(j)))
            {
                handle_ball_ball_collision(balls.at(i), balls.at(j));
            }
        }
    }
}

void handle_collisions(std::vector<BallState>& balls)
{
    handle_wall_collisions(balls);
    handle_ball_ball_collisions(balls);
}

void update_balls(std::vector<BallState>& balls, double deltaT)
{
    // Update positions
    std::vector<std::thread> threads;
    for (BallState& state : balls)
    {
        std::thread t ([&state, deltaT](){update_ball(state, deltaT);});
        threads.push_back(std::move(t));
    }

    for (std::thread& t : threads)
    {
        t.join();
    }

    // Handle collisions
    handle_collisions(balls);
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

    std::vector<BallState> balls = initialize_balls(argc, argv);

    double lastFrameStartTime = glfwGetTime();
    while(!glfwWindowShouldClose(window))
    {
        double deltaT = do_frame_rate_limiting(lastFrameStartTime);

        glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        std::cout << 1 / deltaT << std::endl;
        update_balls(balls, deltaT);

        display_background();
        display_circles(balls);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    return 0;
}