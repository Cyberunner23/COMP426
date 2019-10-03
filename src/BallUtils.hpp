#pragma once

#include <cmath>
#include <random>

#include "glm/glm.hpp"

#include "GLUtils.hpp"

// position, velocity and radius in pixels
struct BallState
{
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

bool collidesWithEdge(BallState b)
{
    bool collidesX = (unsigned int)b.Position.x - b.Radius < 0 || (unsigned int)b.Position.x + b.Radius > WinSize;
    bool collidesY = (unsigned int)b.Position.y - b.Radius < 0 || (unsigned int)b.Position.y + b.Radius > WinSize;
    return collidesX || collidesY;
}

int randRange(int min, int max)
{
    std::random_device rand;
    std::mt19937 gen(rand());
    std::uniform_int_distribution<> distrib(min, max);
    return distrib(gen);
}

BallState create_random_ball()
{
    unsigned int radius = randRange(1, 3) * 50;
    int velX = randRange(-20, 20);
    int velY = randRange(-20, 20);
    int posX = randRange(radius, WinSize - radius);
    int posY = randRange(radius, WinSize - radius);

    glm::vec3 color;
    switch(randRange(0, 3))
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

    return BallState { radius, glm::vec2 {posX, posY}, glm::vec2{velX, velY}, color };
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

        balls.emplace_back(ball);
        std::cout << "Created ball: Radius = " << ball.Radius
                  << "  Pos = (" << ball.Position.x << ", " << ball.Position.y
                  << ") Velocity = (" << ball.Velocity.x << ", " << ball.Velocity.y
                  << ") Color = (" << ball.Color.x << ", " << ball.Color.y << ", " << ball.Color.z << ")" << std::endl;
    }

    return balls;
}
