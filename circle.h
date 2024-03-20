#pragma once
#include <GL/glut.h>

void circle(int radius, int centerX, int centerY) //This function will draw a circle centered around the centerX and centerY parameters, with the radius of the circle based on the radius parameter. It does this by using Bresenham's Circle Scan Algorithm.
{

    int x = 0;
    int y = radius;
    int decision = 3 - 2 * radius;

    glBegin(GL_POINTS);

    while (x <= y)
    {
        //Place the 8 points based on the one point that has been determined.
        glVertex2i(centerX + x, centerY + y);
        glVertex2i(centerX - x, centerY + y);
        glVertex2i(centerX + x, centerY - y);
        glVertex2i(centerX - x, centerY - y);
        glVertex2i(centerX + y, centerY + x);
        glVertex2i(centerX - y, centerY + x);
        glVertex2i(centerX + y, centerY - x);
        glVertex2i(centerX - y, centerY - x);

        if (decision < 0)
        {
            decision += 4 * x + 6;
        }
        else
        {
            decision += 4 * (x - y) + 10;
            y--;
        }
        x++;
    }

    glEnd();

}

void fillCircle(int radius,int centerx, int centery)
{
    for (int i = 0; i <= radius; i++)
    {
        circle(i, centerx, centery);
    }
}