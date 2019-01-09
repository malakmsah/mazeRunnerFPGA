//--------------------------------------------------------
// Application demonstrator: MAZE game
//--------------------------------------------------------


#include "EDK_CM0.h"
#include "core_cm0.h"
#include "edk_driver.h"
#include "edk_api.h"
#include <time.h>
#include <stdio.h>

#define MAX_SIZE 1000

static char dig1, dig2, dig3, dig4;
static char dig5, dig6, dig7, dig8;
int x, y, tarposx, tarposy, startposx, startposy, mazecolor, height, width, success, starttime, currenttime, newx, newy, deadline, i, d;
int col;
char direction;
int trials, successes;

int wallDotArrayIndex;
typedef struct {
    int x, y;
} wallDot;
wallDot wallDotsArray[MAX_SIZE];

//---------------------------------------------
// UART ISR
//---------------------------------------------
void UART_ISR(void) {
    char c;
    c = *(unsigned char *) AHB_UART_BASE;        //read a character from UART
    *(unsigned char *) AHB_UART_BASE = c;    //write the character to UART

}

//---------------------------------------------
// TIMER ISR
//---------------------------------------------
void Timer_ISR(void) {
    dig4++;

    if (dig4 == 10) {
        dig4 = 0;
        dig3++;
        if (dig3 == 10) {
            dig3 = 0;
            dig2++;
            if (dig2 == 10) {
                dig2 = 0;
                dig1++;
            }
        }
    }
    seven_seg_write(dig1, dig2, dig3, dig4);
    timer_irq_clear();
}

/**
 *
 * @param trials
 */
void displayTrials(int trials) {
    dig4 = trials;

    if (dig4 == 10) {
        dig4 = 0;
        dig3++;
        if (dig3 == 10) {
            dig3 = 0;
            dig2++;
            if (dig2 == 10) {
                dig2 = 0;
                dig1++;
            }
        }
    }
    seven_seg_write(dig1, dig2, dig3, dig4);
}

/**
 *
 * @param success
 */
void displaySuccess(int success) {
    dig8 = success;

    if (dig8 == 10) {
        dig8 = 0;
        dig7++;
        if (dig7 == 10) {
            dig7 = 0;
            dig6++;
            if (dig6 == 10) {
                dig6 = 0;
                dig5++;
            }
        }
    }
    seven_seg_write2(dig5, dig6, dig7, dig8);
}

//--- Draw Outline/Borders-------------------------------
void DrawOutline(void) {
    // left vertical  line:
    for (int yt = 0; yt < 110; yt++)
        VGA_plot_pixel(0, yt, BLUE);

    // right vertical line:
    for (int yt = 0; yt < 110; yt++)
        VGA_plot_pixel(90, yt, BLUE);

    // top horizontal border:
    for (int xt = 0; xt < 90; xt++)
        VGA_plot_pixel(xt, 0, BLUE);

    // bottom horizontal border:
    for (int xt = 0; xt <= 90; xt++)
        VGA_plot_pixel(xt, 110, BLUE);
}

//--- Write function-------------------------------------------
/**
 *
 * @param text
 */
void write(char *text) {
    int length = 0;
    while (text[length] != '\0') {
        VGAPutc(text[length]);
        length = length + 1;
    }
    VGAPutc('\n');
    VGAPutc('\0');
}

//////// DRAW MAZE ////////
/**
 *
 * @param dotX
 * @param dotY
 * @param color
 */
void DrawDot(int dotX, int dotY, int color) {

    VGA_plot_pixel(dotX, dotY, color);

    wallDot temp;
    temp.x = dotX;
    temp.y = dotY;
    wallDotsArray[wallDotArrayIndex] = temp;
    wallDotArrayIndex = wallDotArrayIndex + 1;

}

/**
 *
 * @param startLineX
 * @param startLineY
 * @param length
 * @param color
 */
void DrawLineH(int startLineX, int startLineY, int length, int color) {
    int i;
    int endofLineX = startLineX + length;
    for (i = startLineX; i < endofLineX; i++) {

        DrawDot(i, startLineY, color);
    }
}

/**
 *
 * @param startLineX
 * @param startLineY
 * @param length
 * @param color
 */
void DrawLineV(int startLineX, int startLineY, int length, int color) {
    int i;
    int endofLineV = startLineY + length;
    for (i = startLineY; i < endofLineV; i++) {

        DrawDot(startLineX, i, color);
    }
}

//--- Draw maze ----------------------------------------
void DrawMaze(void) {
    int mazecolor = 14;
    int startLocationColor = RED;
    int endLocationColor = GREEN;
    wallDotArrayIndex = 0;

    switch (read_switch()) {
        case 0:
            col = RED;
            break;
        case 1:
            col = 3;
            break;
        case 2:
            col = BLUE;
            break;
        case 3:
            col = WHITE;
            break;
        default:
            col = 7;
            break;
    }

    // Draw external borders
    DrawLineH(startposx, startposy, width, mazecolor); // top border
    DrawLineH(startposx, startposy + height - 1, width, mazecolor); // bottom border
    DrawLineV(startposx, startposy + 5, height - 5, mazecolor); // left border
    DrawLineV(startposx + width - 1, startposy, height - 5, mazecolor); // right border

    // Draw inner horizontal borders
    DrawLineH(startposx + 1, startposy + 15, 15, mazecolor); // start: 41,42
    DrawLineH(startposx + 1, startposy + 25, 8, mazecolor); // start: 41,44
    DrawLineH(startposx + 7, startposy + 30, 8, mazecolor); // start: 42,46
    DrawLineH(startposx + 20, startposy + 40, 20, mazecolor); // start: 44,49
    DrawLineH(startposx + 23, startposy + 8, 7, mazecolor); // start: 44,49

    // Draw inner vertical borders
    DrawLineV(startposx + 15, startposy + 15, 30, mazecolor); // start: 44,42
    DrawLineV(startposx + 8, startposy + 40, 10, mazecolor); // start: 42,48
    DrawLineV(startposx + 30, startposy + 1, 30, mazecolor); // start: 46,41
    DrawLineV(startposx + 23, startposy + 8, 15, mazecolor); // start: 46,41

    // draw start location
    DrawDot(startposx - 2, startposy, startLocationColor);

    // draw end location
    DrawDot(startposx + width + 2, startposy + height - 1, endLocationColor);

}

//---Game_init function------------------------------------------
void game_init(void) {
    displayTrials(trials);
    trials = trials + 1;

    timer_init(Timer_Load_Value_For_One_Sec, Timer_Prescaler, 1); // maybe they should be inside Game_init
    timer_enable();

    i = 0;

    startposx = 20;
    startposy = 30;
    height = 50;
    width = 40;
    tarposx = startposx + width;
    tarposy = startposy + height;

    starttime = 0;
    currenttime = starttime;
    deadline = 1000000;

    DrawOutline();
    DrawMaze();  // draw horizontal and vertical lines of the maze

    write(" ---  MAZE GAME  ---\n\n\nUse the following keys \nto move the maze runner: \n w: move UP\n s: move DOWN \n d: move RIGHT \n a: move left");

    x = startposx;  // maze runner start  location 20
    y = startposy + 2; // maze runner start  location 32
    VGA_plot_pixel(x, y, BLUE);
}

/**
 *
 * @param x
 * @param y
 * @return
 */
int hitWall(int x, int y) {
    int j;
    int hit = 0;
    for (j = 0; j <= wallDotArrayIndex; j++) {
        if ((wallDotsArray[j].x == x) && (wallDotsArray[j].y == y))
            hit++;
    }
    if (hit != 0)
        return 1;
    else
        return 0;
}

int inMaze(int x, int y) {
    if (x >= startposx && x <= tarposx && y >= startposy && y <= tarposy) {
        return 1;
    } else {
        return 0;
    }
}

// ---target is reached?-------------------
/**
 *
 * @return
 */
int targetisreached(void) {
    if (
            ((x == tarposx) && ((y == tarposy - 1) || (y == tarposy - 2) || (y == tarposy - 3) || (y == tarposy - 4))))
        return 1;
    else
        return 0;
}

//--- gameover--------------------------------
void gameover(void) {
    //trigger audio();
    clear_screen();
    write("GAME OVER");
    write_LED(15);
    currenttime = starttime;
    delay(1000);
    SoC_init();
    game_init();
}

void clear_console(void) {
    write("\n\n\n\n\n\n\n\n\n\n\n\n\n");
}

//---------------------------------------------
// Main Function
//---------------------------------------------
void outOfMazeAction(int x, int y) {
    write("Error: Out of Maze!");
    VGA_plot_pixel(x, y, BLACK);
    newx = 20;
    newy = 32;
    VGA_plot_pixel(newx, newy, BLUE);
    x = newx;
    y = newy;
}

/**
 * 
 * @return
 */
int main(void) {

    SoC_init();
    game_init();

    trials = successes = 0;

    while (1) {
        if (KBHIT()) // keyboard hits
        {
            direction = UartGetc();  //getchar(); //UartGetc();
        }

        switch (direction) {
            case 'a': //119: //(char)LEFT:
                if (inMaze(x, y)) {
                    if (!hitWall(x - 1, y)) {
                        VGA_plot_pixel(x, y, BLACK);
                        newx = x - 1;
                        VGA_plot_pixel(newx, y, WHITE);
                        x = newx;
                    } else {
                        write("Error: Left Wall Collision!");
                    }
                } else {
                    outOfMazeAction(x, y);
                }
                break;
            case 'd' ://100: //(char) RIGHT:
                if (inMaze(x, y)) {
                    if (!hitWall(x + 1, y)) {//60
                        newx = x + 1;
                        if (newx <= tarposx) {
                            VGA_plot_pixel(x, y, BLACK);
                            VGA_plot_pixel(newx, y, WHITE);
                            x = newx;
                        }
                    } else {
                        write("Error: Right Wall Collision!");
                    }
                } else {
                    outOfMazeAction(x, y);

                }
                break;
            case 's': //115: //(char) DOWN:
                if (inMaze(x, y)) {
                    if (!hitWall(x, y + 1)) {
                        VGA_plot_pixel(x, y, BLACK);
                        newy = y + 1;
                        VGA_plot_pixel(x, newy, WHITE);
                        y = newy;
                        //	VGAPutc(direction);
                    } else
                        write("Error: Bottom Wall Collision!");
                } else {
                    outOfMazeAction(x, y);

                }
                break;
            case 'w': //97: //(char) UP:
                if (inMaze(x, y)) {
                    if (!hitWall(x, y - 1)) {
                        VGA_plot_pixel(x, y, BLACK);
                        newy = y - 1;
                        VGA_plot_pixel(x, newy, WHITE);
                        y = newy;
                        //	VGAPutc(direction);
                    } else
                        write("Error: Upper Wall Collision!");
                } else {
                    outOfMazeAction(x, y);

                }
                break;

            case 27: //ESC;
                clear_screen();
                clear_console();
                game_init();
                break;
            default:
                break;
        }

        if (targetisreached()) {
            success = 1;
            write("\n\n\n");
            write("** SUCCESS! **");
            write_LED(8);
            currenttime = starttime;

            displaySuccess(successes);
            successes = successes + 1;

            newx = 20;
            newy = 32;
            VGA_plot_pixel(x, y, BLACK);
            VGA_plot_pixel(newx, newy, BLUE);
            x = newx;
            y = newy;
            write("To play again hit Esc!");


        } else if (currenttime >= deadline) {
            success = 0;
            gameover();

        } else {

            delay(500);
            currenttime += 1;
        }
        direction = ' ';
    }
}