#include "GL\glew.h"
#include "GL\freeglut.h"

#include "GLUtils.hpp"



void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    DrawCircle(Vec2f{0.25, 0}, Vec3f{1.0, 1.0, 0.0}, 0.5f);
    glutSwapBuffers();


}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(WinSize, WinSize);
    glutCreateWindow("COMP426 A1");
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}