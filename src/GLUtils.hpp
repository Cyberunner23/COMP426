#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#include "GLFW/glfw3.h"
#include "glm/glm.hpp"


const unsigned int WinSize = 800;
const double pi = 3.1415926535897;

float LinearInterpolation(float destMin, float destMax, float srcMin, float srcMax, float srcVal);
glm::vec2 PixelToGLPos(glm::vec2 pixelPos);

void DrawGradientSquare(glm::vec2 position)
{
    glPushMatrix();

    glm::vec2 glPos = PixelToGLPos(position);

    glTranslatef(glPos.x, glPos.y, -0.001f);

    glBegin(GL_QUADS);					// Start Drawing Quads
    glColor3f(1.0f,0.0f,0.0f);
    glVertex3f(-0.25f, 0.25f, 0.0f);
    glColor3f(0.0f,1.0f,0.0f);
    glVertex3f( 0.25f, 0.25f, 0.0f);
    glColor3f(0.0f,0.0f,1.0f);
    glVertex3f( 0.25f,-0.25f, 0.0f);
    glColor3f(1.0f,1.0f,1.0f);
    glVertex3f(-0.25f,-0.25f, 0.0f);
    glEnd();

    glPopMatrix();
}

void DrawCircle(glm::vec2 position, glm::vec3 color, unsigned int radius)
{
    const unsigned int numSegments = 100;

    glPushMatrix();

    glm::vec2 glPos = PixelToGLPos(position);
    glTranslatef(glPos.x, glPos.y, 0.0f);

    float glRadius = LinearInterpolation(0, 1, 0, (float)WinSize / 2, (float)radius);

    glBegin(GL_TRIANGLE_FAN);
    glColor4f(color.x, color.y, color.z, 0.5f);

    glVertex3f(0.0, 0.0f, 0.0f);       // Center of circle
    float angle;
    for (int i = 0; i <= numSegments; i++) { // Last vertex same as first vertex
        angle = (float)i * 2.0f * (float)pi / numSegments;  // 360 deg for all segments
        glVertex3f(cos(angle) * glRadius, sin(angle) * glRadius, 0.0f);
    }
    glEnd();

    glPopMatrix();
}

float LinearInterpolation(float destMin, float destMax, float srcMin, float srcMax, float srcVal)
{
    return ((destMax - destMin)/(srcMax - srcMin)) * (srcVal - srcMin) + destMin;
}

glm::vec2 PixelToGLPos(glm::vec2 pixelPos)
{
    float glX = LinearInterpolation(-1.0f, 1.0f, 0, WinSize, pixelPos.x);
    float glY = LinearInterpolation(1.0f, -1.0f, 0, WinSize, pixelPos.y);
    return glm::vec2{glX, glY};
}
