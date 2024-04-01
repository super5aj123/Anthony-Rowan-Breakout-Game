#include "circle.h"
#include <GL/glut.h>
#include <time.h>
#include <windows.h>
#include <iostream>
#include <string>

/*TODO:
Create start screen
Create game over screen
Create win screen
Randomize ball spawn location
Create music
Have music tracks play dependant on the number of lives remaining
Have music tracks loop
Add sound effects for the ball bouncing off of different surfaces
*/


float rowColorR[5] = { 1,0, 0,0.5,1}; //Store the RGB values for the colors of the bricks
float rowColorG[5] = { 0, 1,0,0,1};
float rowColorB[5] = { 0, 0,1,0.5,0};

bool bricks[100]; //An array to store the status of every brick.



int windowLen = 1000;
int windowHeight = 500;

int lives = 99; //TEMP VALUE CHANGE TO 4 BEFORE SUBMISSION

int musicTime = 0;
const int SPEED = 17; //Number of ms to wait between refreshing the screen. A value of 17 should be around 60 FPS, as 1000(# of ms)/60 = 16.666666...

bool liveBall = false; //Variable to track if there is currently a ball. If there is a ball, this will be true, otherwise it will be false. It starts as false.
int ballX = 400; //The X and Y location of the ball's center. This will be randomly generated when the game is complete, but is set to the center for now.
int ballY = 360;
char ballLR = 'l'; //Variable to track the ball's horizontal direction.
char ballUD = 'd'; //Variable to track the ball's vertical direction.
const int radius = 10;
int stallCount = 0;

int paddleX = 425; //This variable will hold the x position of the left side of the paddle. The other side will be calculated on the fly.

bool win = false; //Bool values to store which state the game should be in: tutorial screen, win screen, or lose screen.
bool loss = false;
bool gameStart = false;

const int brickLen = 50;
const int brickHeight = 25;

void playMusic() //This is the function to play the different music tracks.
{

    if (lives == 3) //Song for all lives remaining
    {
        PlaySound(TEXT("3Lives.wav"), NULL, SND_ASYNC);
    }
    
    else if (lives == 2) //Song for 2 spare lives remaining
    {
        PlaySound(TEXT("2Lives.wav"), NULL, SND_ASYNC);
    }

    else if (lives == 1) //Song for 1 spare life remaining
    {
        PlaySound(TEXT("1Lives.wav"), NULL, SND_ASYNC);
    }

    else if (lives == 0) //Song for final life
    {
        PlaySound(TEXT("0Lives.wav"), NULL, SND_ASYNC);
    }
}

void placeBricks() //This function will place the bricks and lines used to help the user differentiate them.
{
    for (int row = 1; row <= 5; row++) //Iterate through the rows.
    {
        for (int i = 0; i < windowLen; i += brickLen) //Place the 20 bricks in each row.
        {
            if (bricks[i/brickLen + ((row - 1) * 20)] == true)
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
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, lives+48);
}

void wallCollision() //This is a function to track the ball's collision with a wall.
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
    }

    if (ballY + radius >= windowHeight) //If the top of the ball hits the wall
    {
        ballUD = 'd';
    }
}


void brickCollision()
{
    if (ballY >= 325) //Check that the ball is in the proper y value to be hitting a brick
    {
        for (int i = 0; i < 100; i++)
        {
            if (bricks[i] == true) //Check if the brick is "alive"
            {
                int row = i / 20; //Calculate the row of the brick as in int value, in order to remove any decimal values.
                int col = i % 20; //Calculate the column of the brick as an int value in order to remove any decimal values.
                int brickX = col * brickLen; //Find the x value of the brick.
                int brickY = 450 - (brickHeight * (row + 1)); //Find the y value of the brick.

                if (ballX + radius >= brickX && ballX - radius <= brickX + brickLen && ballY + radius >= brickY && ballY - radius <= brickY + brickHeight) //Check if the ball is colliding with the brick
                {
                    if (ballX <= brickX || ballX >= brickX + brickLen) //The ball has collided with the brick, so check if it was on the left or right of the brick, and if it was, reverse the horizontal direction. This is done by seeing if the ball's center x position is to the left or right of the brick, and if it is, the ball hit the brick on its side.
                    {
                        if (ballLR == 'l')
                        {
                            ballLR = 'r';
                        }

                        else if (ballLR == 'r')
                        {
                            ballLR = 'l';
                        }
                    }
                    else //If the brick was not hit on its side, it was hit on the top or bottom, so reverse the vertical direction.
                    {
                        if (ballUD == 'u')
                        {
                            ballUD = 'd';
                        }

                        else if (ballUD == 'd')
                        {
                            ballUD = 'u';
                        }
                    }

                    bricks[i] = false; //"Kill" the brick
                    return; //The brick has been destroyed, so stop looping on this frame.
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

void checkWin() //A function to check if the player has won the game.
{
    for (int i = 0; i < 100; i++)
    {
        if (bricks[i] == true)
        {
            return;
        }
    }
    win = true;
}

void checkLoss()
{
    if (lives == 0 && liveBall == false)
    {
        loss = true;
    }
}

void tutorial()
{
    std::string intro = "Welcome to Breakout! Use the paddle to bounce the ball and break the bricks.";
    std::string intro2 = " The game is lost when all lives are lost. The game is won when all bricks are destroyed.";
    std::string controls = "Use the A and D keys to move the paddle left and right. Use the space key to spawn a new ball.";
    std::string start = "Press space to begin!";

    glRasterPos2f(5, windowHeight-20);
    for (int i = 0; i < intro.length(); i++)
    {
        glutBitmapCharacter (GLUT_BITMAP_HELVETICA_18, intro[i]);
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
    for (int i = 0; i < start.length(); i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, start[i]);
    }
}

void winScreen()
{
    glColor3f(0, 1, 0);
    glRasterPos2f(0, 400);
    std::string message = "YOU WIN!";

    for (int i = 0; i < message.length(); i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, message[i]);
    }
}

void lossScreen()
{
    glColor3f(1, 0, 0);
    glRasterPos2f(0, 400);
    std::string message = "YOU LOSE";

    for (int i = 0; i < message.length(); i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, message[i]);
    }
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
                ballX = ballX - 5;
            }

            else if (ballLR == 'r')
            {
                ballX = ballX + 5;
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
                ballY = ballY + 5;
            }

            else if (ballUD == 'd')
            {
                ballY = ballY - 5;
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

    if (c == ' ' && liveBall == false && lives>0 && gameStart == true) //If the user presses space after the game has been started and there is currently not a living ball, create a new ball.
    {
        liveBall = true;
        ballX = (rand()%500)+250;
        ballY = 300;
        ballUD = 's';
        ballLR = 's';
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
    }


    if (c == 'd' && paddleX+200<windowLen) //Move the paddle right, as long as doing so would not move it off screen.
    {
        paddleX = paddleX + 15;
    }
    
    if (c == 'a' && paddleX>0) //Move the paddle left, as long as doing so would not move it off screen.
    {
        paddleX = paddleX - 15;
    }
}

void pattern_display(void)

{
    glutPostRedisplay(); //<=====
    Sleep(SPEED);
}

int main(int argc, char** argv)
{
    for (int i = 0; i < 100; i++)
    {
        bricks[i] = true;
    }
    playMusic();
    srand(time(NULL));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowLen, windowHeight); //Set the size of the window to 600x600
    glutCreateWindow("Breakout: Anthony Rowan"); //Creates and names the window that will display the shapes.
    init();
    glutDisplayFunc(display); //Run the display function.
    glutIdleFunc(pattern_display);
    glutKeyboardFunc(keyboard_func);
    glutMainLoop(); //Run the main glut functions
}