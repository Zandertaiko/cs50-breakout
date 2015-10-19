/*
* breakout.c
* Programmer: Zander Koren Møysal
* Date: 19.10.2015
* Computer Science 50
* Problem Set 3
* 
*
*/

// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Stanford Portable Library
#include <spl/gevents.h>
#include <spl/gobjects.h>
#include <spl/gwindow.h>

// height and width of game's window in pixels
#define HEIGHT 600
#define WIDTH 400

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// radius of ball in pixels
#define RADIUS 10

// lives
#define LIVES 3

// paddle width and height
#define PADDLE_WIDTH 50
#define PADDLE_HEIGHT 7

// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
void updateScoreboard(GWindow window, GLabel label, int points);
GObject detectCollision(GWindow window, GOval ball);

int main(void)
{
    // seed pseudorandom number generator
    srand48(time(NULL));

    // instantiate window
    GWindow window = newGWindow(WIDTH, HEIGHT);

    // instantiate bricks
    initBricks(window);

    // instantiate ball, centered in middle of window
    GOval ball = initBall(window);

    // instantiate paddle, centered at bottom of window
    GRect paddle = initPaddle(window);

    // instantiate scoreboard, centered in middle of window, just above ball
    GLabel label = initScoreboard(window);

    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;

    // number of points initially
    int points = 0;
    
    // speed of ball
    double speed = 2.0;
    double x_velocity = drand48() * speed;
    double y_velocity = speed; 
    
    // pause, 1 == true
    int paused = 0;
    

    // keep playing until game over
    while (lives > 0 && bricks > 0)
    {
        GEvent event = getNextEvent(MOUSE_EVENT);
        
        move(ball, x_velocity, y_velocity);
        
        GObject object = detectCollision(window, ball);
        
        if (event != NULL)
        {
            if (getEventType(event) == MOUSE_MOVED)
            {
                double x = getX(event) - PADDLE_WIDTH / 2;
                
                setLocation(paddle, x, (HEIGHT / 10) * 9);
            }
            else if (getEventType(event) == MOUSE_CLICKED && paused == 1)
            {
                setLocation(ball, WIDTH / 2 - 10, HEIGHT / 2);
                x_velocity = drand48() * speed;
                y_velocity = speed;
                lives -= 1;
                paused = 0;
            }
        }
        

        // bounce off right edge of window
        if (getX(ball) + getWidth(ball) >= WIDTH)
        {
            x_velocity = -x_velocity;
        }

        // bounce off left edge of window
        else if (getX(ball) <= 0)
        {
            x_velocity = -x_velocity;
        }
        
        // ball at bottom of window
        else if (getY(ball) + getHeight(ball) >= HEIGHT)
        {
            x_velocity = 0.0;
            y_velocity = 0.0;
            paused = 1;
        }
        
        // ball at top of window
        else if (getY(ball) <= 0)
        {
            y_velocity = -y_velocity;
        }

        // linger before moving again
        pause(10);
        
        
        /*
        * if the ball is colliding with the paddle, bounce.
        * else if the ball is colliding with a brick, remove it, add score
        * then bounce 
        */
        if (object)
        {
            if (object == paddle)
            {
                y_velocity = -y_velocity;
            }
            else if (strcmp(getType(object), "GRect") == 0)
            {
                y_velocity = -y_velocity;
                removeGWindow(window, object);
                points += 10;
                updateScoreboard(window, label, points);
                
            }
        }
        
    }

    // wait for click before exiting
    waitForClick();

    // game over
    closeGWindow(window);
    return 0;
}

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{
    int x = 2;
    int y = 50;
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            GRect brick = newGRect(x, y, 35, 10);
            setFilled(brick, true);
            
            if (i == 0)
            {
                setColor(brick, "RED");
            }
            else if(i == 1)
            {
                setColor(brick, "BLUE");
            }
            else if(i == 2)
            {
                setColor(brick, "MAGENTA");
            }
            else if(i == 3)
            {
                setColor(brick, "YELLOW");
            }
            else if(i == 4)
            {
                setColor(brick, "CYAN");
            }
            
            x += 40;
            add(window, brick);
            if(j == COLS -1)
            {
                x = 2;
                y += 20;
            }
        }
    }
}

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    GOval ball = newGOval(WIDTH / 2 - 10, HEIGHT / 2, 15, 15);
    setColor(ball, "BLACK");
    setFilled(ball, true);
    add(window, ball);
    return ball;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{
    GRect paddle = newGRect(WIDTH / 2 - (PADDLE_WIDTH / 2), (HEIGHT / 10) * 9,
                                             PADDLE_WIDTH, PADDLE_HEIGHT);
    setFilled(paddle, true);
    setColor(paddle, "BLACK");
    add(window, paddle);
    return paddle;
}

/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
    GLabel label = newGLabel("0");
    setFont(label, "SansSerif-40");
    setColor(label, "LIGHT_GRAY");
    setLocation(label, (WIDTH - getWidth(label)) / 2, 
                        (HEIGHT + getFontAscent(label)) / 2);
    add(window, label);
    return label;
}

/**
 * Updates scoreboard's label, keeping it centered in window.
 */
void updateScoreboard(GWindow window, GLabel label, int points)
{
    // update label
    char s[12];
    sprintf(s, "%i", points);
    setLabel(label, s);

    // center label in window
    double x = (WIDTH - getWidth(label)) / 2;
    double y = (HEIGHT + getFontAscent(label)) / 2;
    setLocation(label, x, y);
}//ckbox was ch

/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GOval ball)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // no collision
    return NULL;
}
