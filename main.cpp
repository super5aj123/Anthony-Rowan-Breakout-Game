#include <GL/glut.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <ctime>

#include "circle.h"



float rowColorR[5] = { 1,0, 0,0.5,1 }; //Store the RGB values for the colors of the bricks
float rowColorG[5] = { 0, 1,0,0,1 };
float rowColorB[5] = { 0, 0,1,0.5,0 };

bool bricks[100]; //An array to store the status of every brick.



int windowLen = 1000;
int windowHeight = 500;

int lives = 4;

int musicTime = 0;
const int SPEED = 17; //Number of ms to wait between refreshing the screen. A value of 17 should be around 60 FPS, as 1000(# of ms)/60 = 16.666666...

bool liveBall = false; //Variable to track if there is currently a ball. If there is a ball, this will be true, otherwise it will be false. It starts as false.
int ballX = 400; //The X and Y location of the ball's center.
int ballY = 360;
char ballLR = 'l'; //Variable to track the ball's horizontal direction.
char ballUD = 'd'; //Variable to track the ball's vertical direction.
const int radius = 10;
int stallCount = 0; //Variable to track if it's time for the ball to move. This is done to let the user find the ball when it spawns.
int ballSpeed = 2; //Variable to track the speed of the ball.

int paddleX = 425; //This variable will hold the x position of the left side of the paddle. The other side will be calculated on the fly.

bool win = false; //Bool values to store which state the game should be in: tutorial screen, win screen, or lose screen.
bool loss = false;
bool gameStart = false;

const int brickLen = 50;
const int brickHeight = 25;

int endTime = 0; //Variable to track the current time. Will be used to calculate the game's speed, as well as the end time.

int gameStartTime = 0; //The time that the game began



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


void paddle() //This is the function to draw the paddle on the screen.
{
    glBegin(GL_POLYGON);
    glVertex2i(paddleX, 50);
    glVertex2i(paddleX + 200, 50);
    glVertex2i(paddleX + 200, 60);
    glVertex2i(paddleX, 60);
    glEnd();

}


void paddleCollision() //This is a function to check if the ball is in contact with the top of the paddle.
{
    if (ballY - 10 <= 60)
    {
        for (int i = ballX - 10; i <= ballX + 10; i++)
        {
            if (ballY - 10 <= 60 && ballY - 10 >= 50 && i >= paddleX && i <= paddleX + 200)
            {
                ballUD = 'u';
                if (ballSpeed < 5)
                {
                    ballSpeed++;
                }
                PlaySound(TEXT("Paddle.wav"), NULL, SND_ASYNC);
                return;
            }
        }
    }
}

void drawLives() //This function will draw both the icon and text to show the user the number of remaining lives.
{
    fillCircle(10, 15, 485);
    glRasterPos2f(30, 475);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'x');
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, lives + 48); //The ASCII codes for numbers start with 0 at code 48, so the correct ASCII code can be obtained by adding 48 to the lives count. This does cause issues with lives above 9, so if that is wanted, this function will need changed.
}

void wallCollision() //This is a function to track the ball's collision with a wall.
{
    if (liveBall == true)
    {
        if (ballX - radius <= 0)//If the left of the ball hits the wall
        {
            ballLR = 'r';
            PlaySound(TEXT("wallBounce.wav"), NULL, SND_ASYNC);
        }

        if (ballX + radius >= windowLen)//If the right side of the ball hits the wall
        {
            ballLR = 'l';
            PlaySound(TEXT("wallBounce.wav"), NULL, SND_ASYNC);
        }

        if (ballY - radius <= 0) //If the bottom of the ball hits the wall
        {
            liveBall = false; //If the ball hits the floor, it should die.
            PlaySound(TEXT("Death.wav"), NULL, SND_ASYNC);
        }

        if (ballY + radius >= windowHeight) //If the top of the ball hits the wall
        {
            ballUD = 'd';
            PlaySound(TEXT("wallBounce.wav"), NULL, SND_ASYNC);
        }
    }

}


void brickCollision() //This function is used to check for collision between the ball and bricks, as well as to bounce the ball off the bricks.
{
    if (ballY >= 325) //Check that the ball is in the proper y value to be hitting a brick
    {
        for (int i = 0; i < 100; i++)
        {
            if (bricks[i] == true) //Check if the brick is "alive"
            {
                int row = i / 20; //Calculate the row of the brick as in int value, in order to remove any decimal values.
                int col = i % 20; //Calculate the column of the brick as an int value in order to remove any decimal values.
                int brickX = col * brickLen; //Find the left x value of the brick by multiplying the number of columns by the brick length.
                int brickY = 450 - (brickHeight * (row + 1)); //Find the y value of the brick by multiplying the number of rows by the brick height.

                if (ballX + radius >= brickX && ballX - radius <= brickX + brickLen && ballY + radius >= brickY && ballY - radius <= brickY + brickHeight)
                    /*Check if the ball is colliding with the brick.
                    This should be done by comparing the ball's values to the brick's values, and ensuring that the ball's values are inside of the brick's values.
                    ballX + radius >= brickX checks that the right side of the ball is to the right of, or at the left side of the brick.
                    ballX - radius <= brickX + brickLen checks that the left side of the ball is to the left of or at the right side of the brick.
                    ballY + radius >= brickY checks that the top of the ball is above or at the bottom of the brick
                    ballY - radius <= brickY + brickHeight checks that the bottom of the ball is below or at the top of the brick.*/
                {
                    if (ballX + radius <= brickX + 5 || ballX - radius >= brickX + brickLen - 5) //If the ballX is <= than brickX or >= brickX+brickLen, then it hit the brick on the side, so the horizontal direction should be reversed.
                    {
                        if (ballX <= brickX && ballLR == 'r')
                        {
                            ballLR = 'l';
                        }
                        else if (ballX > brickX + brickLen && ballLR == 'l')
                        {
                            ballLR = 'r';
                        }
                    }
                    else //If the brick was not hit on its side, it was hit on the top or bottom, so reverse the vertical direction.
                    {
                        if (ballY <= brickY && ballUD == 'u')
                        {
                            ballUD = 'd';
                        }
                        else if (ballY > brickY && ballUD == 'd')
                        {
                            ballUD = 'u';
                        }
                    }

                    bricks[i] = false; //"Kill" the brick
                    PlaySound(TEXT("Brick.wav"), NULL, SND_ASYNC);
                    return;

                }
            }
        }
    }
}





void init()
{
    glClearColor(0.0, 0.0, 0.0, 1.0); //Set background color to black
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, windowLen, 0, windowHeight); //Create the viewbox, and set its size.
}

void checkWin() //A function to check if the player has won the game, by looping through all the bricks and checking if there are any left.
{
    for (int i = 0; i < 100; i++)
    {
        if (bricks[i] == true)
        {
            return;
        }
    }
    if (win == false)
    {
        win = true;
        endTime = time(NULL);
    }
    
}

void checkLoss() //Check if the user has any lives remaining, and if there is currently a ball in play. If there isn't, they have lost.
{
    if (lives == 0 && liveBall == false && loss == false)
    {
        loss = true;
        endTime = time(NULL);
    }
}

void tutorial() //A function to tell the user how to play the game.
{
    std::string intro = "Welcome to Breakout! Use the paddle to bounce the ball and break the bricks.";
    std::string intro2 = " The game is lost when all lives are lost. The game is won when all bricks are destroyed.";
    std::string controls = "Use the A and D, or arrow keys to move the paddle left and right. Use the space key to spawn a new ball.";
    std::string ballExplanation = "The ball will grow faster every time it hits the paddle, until it hits its maximum speed.";
    std::string start = "Press space to begin!";

    glRasterPos2f(5, windowHeight - 20);
    for (int i = 0; i < intro.length(); i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, intro[i]);
    }

    glRasterPos2f(0, windowHeight - 60);
    for (int i = 0; i < intro2.length(); i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, intro2[i]);
    }

    glRasterPos2f(5, windowHeight - 100);
    for (int i = 0; i < controls.length(); i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, controls[i]);
    }

    glRasterPos2f(5, windowHeight - 140);
    for (int i = 0; i < ballExplanation.length(); i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ballExplanation[i]);
    }

    glRasterPos2f(5, windowHeight - 180);
    for (int i = 0; i < start.length(); i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, start[i]);
    }
}

void winScreen() //A function to display a screen to show the user that they have won, and how long it took them.
{
    glColor3f(0, 1, 0);
    glRasterPos2f(450, 360);
    std::string message = "YOU WIN!";

    for (int i = 0; i < message.length(); i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, message[i]);
    }

    int secondsCount = endTime - gameStartTime;
    int minutes = secondsCount / 60;
    int seconds = secondsCount % 60;

    std::string timeMessage = "Time taken: " + std::to_string(minutes) + " minutes, " + std::to_string(seconds) + " seconds";

    glRasterPos2f(350, 300);
    for (int i = 0; i < timeMessage.length(); i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, timeMessage[i]);
    }
}

void lossScreen() //A function to display a screen telling the user that they have lost, and the number of bricks that were left.
{
    glColor3f(1, 0, 0);
    glRasterPos2f(450, 250);
    std::string message = "YOU LOSE";

    for (int i = 0; i < message.length(); i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, message[i]);
    }

    int bricksLeft = 0;
    for (int i = 0; i < 100; i++)
    {
        if (bricks[i] == true)
        {
            bricksLeft++;
        }
    }
    std::string bricksOutput = std::to_string(bricksLeft);
    bricksOutput = "Bricks Remaining: " + bricksOutput;

    glRasterPos2f(450, 200);
    for (int i = 0; i < bricksOutput.length(); i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, bricksOutput[i]);
    }

    int minutes = endTime / 60;
    int seconds = endTime % 60;
    std::string timeMessage = "You beat the game in " + std::to_string(minutes) + " minutes, and " + std::to_string(seconds) + " seconds";
}

void display() //Run all of the functions to draw the shapes, as well as change the drawing color.
{
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(1.0, 1.0, 1.0);//Set drawing color to white

    checkWin(); //Loop through all bricks to make sure a win has not occurred
    checkLoss(); //Check that a loss has not occurred by making sure that there is either a ball currently on screen, or there is at least one life remaining


    if (gameStart == false) //Display a tutorial screen
    {
        tutorial();
    }

    else if (win == true) //Display a win screen
    {
        winScreen();
    }

    else if (loss == true) //Display a lose screen
    {
        lossScreen();
    }

    else //Display the game
    {

        if (liveBall == true)
        {
            fillCircle(radius, ballX, ballY);

            if (ballLR == 'l')
            {
                ballX = ballX - ballSpeed;
            }

            else if (ballLR == 'r')
            {
                ballX = ballX + ballSpeed;
            }

            else
            {
                if (stallCount >= 5)
                {
                    if (rand() % 2 == 1)
                    {
                        ballLR = 'l';
                    }
                    else
                    {
                        ballLR = 'r';
                    }
                }


            }

            if (ballUD == 'u')
            {
                ballY = ballY + ballSpeed;
            }

            else if (ballUD == 'd')
            {
                ballY = ballY - ballSpeed;
            }

            else
            {
                if (stallCount >= 5)
                {
                    stallCount = 0;
                    ballUD = 'd';
                }

                else
                {
                    stallCount++;
                }
            }
        }


        paddle(); //Draw the paddle
        drawLives(); //Draw the lives counter
        placeBricks(); //Draw the bricks


        wallCollision(); //Check if the ball is currently colliding with the walls (or floor and ceiling)
        paddleCollision(); //Check if the ball is currently colliding with the paddle
        brickCollision(); //Check if the ball is currently colliding with the bricks
    }
    musicTime++;


    glutSwapBuffers();

    glFlush();


}

void keyboard_func(unsigned char c, int x, int y) //Function to handle key presses
{

    if (c == ' ' && liveBall == false && lives > 0 && gameStart == true) //If the user presses space after the game has been started and there is currently not a living ball, create a new ball.
    {
        liveBall = true;
        ballX = (rand() % 500) + 250;
        ballY = 300;
        ballUD = 's';
        ballLR = 's';
        ballSpeed = 2;
        lives--;
    }

    else if (gameStart == false && c == ' ') //If the user presses space and the game has not started, start the game.
    {
        gameStart = true;

        liveBall = true;
        ballX = (rand() % 500) + 250;
        ballY = 300;
        ballUD = 's';
        ballLR = 's';
        lives--;

        ballSpeed = 2;
        gameStartTime = time(NULL);
    }


    else if (c == 'd' && paddleX + 200 < windowLen) //Move the paddle right, as long as doing so would not move it off screen.
    {
        paddleX = paddleX + 15;
    }

    else if (c == 'a' && paddleX > 0) //Move the paddle left, as long as doing so would not move it off screen.
    {
        paddleX = paddleX - 15;
    }

    else if (c == 'r') //TEST, REMOVE BEFORE SUBMISSION
    {
        for (int i = 0; i < 100; i++)
        {
            bricks[i] = false;
        }
    }
}

void pattern_display(void)

{
    glutPostRedisplay(); //<=====
    Sleep(SPEED);
}

void arrowFunc(int key, int x, int y)
{
     if (key == GLUT_KEY_RIGHT && paddleX + 200 < windowLen) //Move the paddle right, as long as doing so would not move it off screen.
     {
         paddleX = paddleX + 15;
    }

     else if (GLUT_KEY_LEFT && paddleX > 0) //Move the paddle left, as long as doing so would not move it off screen.
     {
         paddleX = paddleX - 15;
    }
}

int main(int argc, char** argv)
{
    for (int i = 0; i < 100; i++) //Set the value of all the bricks to be "alive" when the game starts
    {
        bricks[i] = true;
    }
    srand(time(NULL));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowLen, windowHeight); //Set the size of the window to 600x600
    glutCreateWindow("Breakout: Anthony Rowan"); //Creates and names the window that will display the shapes.
    init();
    glutDisplayFunc(display); //Run the display function.
    glutIdleFunc(pattern_display);
    glutKeyboardFunc(keyboard_func);
    glutSpecialFunc(arrowFunc);
    glutMainLoop(); //Run the main glut functions
}