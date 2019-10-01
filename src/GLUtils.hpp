#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#include "GL/glew.h"

#include "MathUtils.hpp"

const unsigned int WinSize = 800;

void DrawCircle(Vec2f position, Vec3f color, float radius)
{
    const unsigned int numSegments = 100;

    glColor3f(color.x, color.y, color.z);
    glTranslatef(position.x, position.y, 0.0f);

    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0, 0.0f, 0.0f);       // Center of circle
    GLfloat angle;
    for (int i = 0; i <= numSegments; i++) { // Last vertex same as first vertex
        angle = (float)i * 2.0f * M_PI / numSegments;  // 360 deg for all segments
        glVertex3f(cos(angle) * radius, sin(angle) * radius, 0.0f);
    }
    glEnd();
}

float LinearInterpolation(float destMin, float destMax, float srcMin, float srcMax, float srcVal)
{
    float ratio = (srcMax - srcMin) / (srcVal - srcMin);
    return ratio / (destMax - destMin) + destMin;
}

/*Vec2f PixelToGLPos(Vec2f PixelPos)
{
    return Vec2f
}*/
