#pragma once

#include <algorithm>
#include <cmath>
#include <random>
#include <string>

#include "glm/glm.hpp"

#include "GLUtils.hpp"

#include "CL/cl.h"

float gravity = 0.3f * 5000; // 9.8m/s^2 * 5000px/m

// position, velocity and radius in pixels
struct BallState
{
    cl_float* Mass;
    cl_uint* Radius;
    cl_float2* Position;
    cl_float2* Velocity;
    glm::vec3* Color;
    int Count;
};

bool collides(BallState& balls, int index1, int index2)
{
    float dx = balls.Position[index1].x - balls.Position[index2].x;
    float dy = balls.Position[index1].y - balls.Position[index2].y;
    unsigned int rSum = balls.Radius[index1] + balls.Radius[index2];
    return (unsigned int)fabs(dx*dx + dy*dy) <= rSum*rSum;
}

int rand_range(int min, int max)
{
    std::random_device rand;
    std::mt19937 gen(rand());
    std::uniform_int_distribution<> distrib(min, max);
    return distrib(gen);
}

void create_random_ball(BallState& state, unsigned int index)
{
    cl_uint radius = rand_range(1, 3) * 50;
    float mass = 0.5;
    cl_float posX = rand_range(radius, WinSize - radius);
    cl_float posY = rand_range(radius, WinSize - radius);

    cl_float velY = rand_range(5, 15);
    cl_float velX = rand_range(100, 200);
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

    state.Mass[index] = mass;
    state.Radius[index] = radius;
    state.Position[index] = cl_float2 {posX, posY};
    state.Velocity[index] = cl_float2 {velX, velY};
    state.Color[index] = color;
}

BallState initialize_balls(int argc, char** argv, int& count)
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

    count = val;

    BallState balls{};
    balls.Mass = new cl_float[val];
    balls.Radius = new cl_uint[val];
    balls.Position = new cl_float2[val];
    balls.Velocity = new cl_float2[val];
    balls.Color = new glm::vec3[val];
    balls.Count = val;


    for (int i = 0; i < val; ++i)
    {
        create_random_ball(balls, i);

        bool collisionFound = false;
        for (int j = 0; j < i - 1; ++j)
        {
            if (collides(balls, i, j))
            {
                collisionFound = true;
            }
        }

        if (collisionFound)
        {
            --i;
            continue;
        }

        std::cout << "Created ball: Radius = " << balls.Radius[i]
                  << "  Pos = (" << balls.Position[i].x << ", " << balls.Position[i].y
                  << ") Velocity = (" << balls.Velocity[i].x << ", " << balls.Velocity[i].y
                  << ") Color = (" << balls.Color[i].x << ", " << balls.Color[i].y << ", " << balls.Color[i].z << ")" << std::endl;
    }

    return balls;
}

/*void update_ball(BallState& b, double deltaT)
{
    // Update velocity
    b.Velocity.y += gravity * deltaT;

    // Update Position
    b.Position.x += b.Velocity.x * deltaT;
    b.Position.y += b.Velocity.y * deltaT;

    // Ensure its still on screen
    b.Position.x = b.Position.x < (float)WinSize - b.Radius ? b.Position.x : (float)WinSize - b.Radius;
    b.Position.x = b.Position.x > (float)b.Radius ? b.Position.x : (float)b.Radius;

    b.Position.y = b.Position.y < (float)WinSize - b.Radius ? b.Position.y : (float)WinSize - b.Radius;
    b.Position.y = b.Position.y > (float)b.Radius ? b.Position.y : (float)b.Radius;
}*/
