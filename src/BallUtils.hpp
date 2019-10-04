#pragma once

#include <algorithm>
#include <cmath>
#include <random>
#include <string>

#include "glm/glm.hpp"

#include "GLUtils.hpp"

const float gravity = 0.3f * 5000; // 9.8m/s^2 * 5000px/m

// position, velocity and radius in pixels
struct BallState
{
    float Mass;
    unsigned int Radius;
    glm::vec2 Position;
    glm::vec2 Velocity;
    glm::vec3 Color;
};

bool collides(BallState b1, BallState b2)
{
    float dx = b1.Position.x - b2.Position.x;
    float dy = b1.Position.y - b2.Position.y;
    unsigned int rSum = b1.Radius + b2.Radius;
    return (unsigned int)fabs(dx*dx + dy*dy) <= rSum*rSum;
}

bool collides_with_edge_x(BallState& b)
{
     return (unsigned int)b.Position.x - b.Radius <= 0 || (unsigned int)b.Position.x + b.Radius >= WinSize;
}

bool collides_with_edge_y(BallState& b)
{
    return (unsigned int)b.Position.y - b.Radius <= 0 || (unsigned int)b.Position.y + b.Radius >= WinSize;
}

int rand_range(int min, int max)
{
    std::random_device rand;
    std::mt19937 gen(rand());
    std::uniform_int_distribution<> distrib(min, max);
    return distrib(gen);
}

BallState create_random_ball()
{
    unsigned int radius = rand_range(1, 3) * 50;
    float mass = 0.5;
    int posX = rand_range(radius, WinSize - radius);
    int posY = rand_range(radius, WinSize - radius);

    int velY = rand_range(5, 15);
    int velX = rand_range(5, 15);
    velX = (int) rand_range(0, 1) ? velX : -velX;
    velY = (int) rand_range(0, 1) ? velY : -velY;

    glm::vec3 color;
    switch(rand_range(0, 2))
    {
        case 0:
            color.x = 1.0f;
            color.y = 0.0f;
            color.z = 0.0f;
            break;
        case 1:
            color.x = 0.0f;
            color.y = 1.0f;
            color.z = 0.0f;
            break;
        case 2:
            color.x = 0.0f;
            color.y = 0.0f;
            color.z = 1.0f;
            break;
    }

    return BallState { mass, radius, glm::vec2 {posX, posY}, glm::vec2{velX, velY}, color };
}

std::vector<BallState> initialize_balls(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout << "Invalid number of arguments!" << std::endl;
        std::exit(-1);
    }

    int val = 0;
    try
    {
        val = std::stoi(argv[1]);
    }
    catch (std::invalid_argument& e)
    {
        std::cout << "The argument must be a number!" << std::endl;
        std::exit(-1);
    }

    if (val < 3 || val > 10)
    {
        std::cout << "The argument must be in between 3 and 10!" << std::endl;
        std::exit(-1);
    }

    std::cout << "Creating " << val << " balls." << std::endl;

    std::vector<BallState> balls;

    for (int i = 0; i < val; ++i)
    {
        BallState ball = create_random_ball();

        bool collisionFound = false;
        for (BallState ballState : balls)
        {
            if (collides(ballState, ball))
            {
                collisionFound = true;
            }
        }

        if (collisionFound)
        {
            --i;
            continue;
        }

        balls.push_back(ball);
        std::cout << "Created ball: Radius = " << ball.Radius
                  << "  Pos = (" << ball.Position.x << ", " << ball.Position.y
                  << ") Velocity = (" << ball.Velocity.x << ", " << ball.Velocity.y
                  << ") Color = (" << ball.Color.x << ", " << ball.Color.y << ", " << ball.Color.z << ")" << std::endl;
    }

    return balls;
}

void update_ball(BallState& b, double deltaT)
{
    // Update velocity
    b.Velocity.y += gravity * deltaT;

    // Update Position
    b.Position.x += b.Velocity.x * deltaT;
    b.Position.y += b.Velocity.y * deltaT;

    // Ensure its still on screen
    b.Position.x = std::min(b.Position.x, (float)WinSize - b.Radius);
    b.Position.x = std::max(b.Position.x, (float)b.Radius);

    b.Position.y = std::min(b.Position.y, (float)WinSize - b.Radius);
    b.Position.y = std::max(b.Position.y, (float)b.Radius);
}
