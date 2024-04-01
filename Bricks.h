#pragma once

void placeBricks() //This function will place the bricks and lines used to help the user differentiate them.
{
    for (int row = 1; row <= 5; row++) //Iterate through the rows.
    {
        for (int i = 0; i < windowLen; i += brickLen) //Place the 20 bricks in each row.
        {
            if (bricks[i / brickLen + ((row - 1) * 20)] == true)
            {
                glColor3f(rowColorR[row - 1], rowColorG[row - 1], rowColorB[row - 1]);
                glBegin(GL_POLYGON);
                glVertex2i(i, 450 - (brickHeight * row));
                glVertex2i(i + brickLen, 450 - (brickHeight * row));
                glVertex2i(i + brickLen, 450 - (brickHeight * (row - 1)));
                glVertex2i(i, 450 - (brickHeight * (row - 1)));
                glEnd();

                glColor3f(0, 0, 0); //Place the vertical lines.
                glBegin(GL_LINES);
                glVertex2i(i + brickLen, 450 - (brickHeight * row));
                glVertex2i(i + brickLen, 450 - (brickHeight * (row - 1)));
                glEnd();
            }
        }
    }
}
