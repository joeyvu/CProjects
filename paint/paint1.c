// Assignment 1 19T3 COMP1511: CS Paint
// paint.c
//
// This program was written by YOUR-NAME-HERE (z5555555)
// on INSERT-DATE-HERE
//
// Version 1.0.0 (2019-10-08): Assignment released.

#include <stdio.h>

// Note: you may find the square root function (sqrt) from the math
// library useful for drawing ellipses in Stage 3 and Stage 4.
#include <math.h>

// The dimensions of the canvas (20 rows x 36 columns).
#define N_ROWS 20
#define N_COLS 36

// Shades (assuming your terminal has a black background).
#define BLACK 0
#define WHITE 4

// IF YOU NEED MORE #defines ADD THEM HERE


// Provided helper functions:
// Display the canvas.
void displayCanvas(int canvas[N_ROWS][N_COLS]);

// Clear the canvas by setting every pixel to be white.
void clearCanvas(int canvas[N_ROWS][N_COLS]);

// Calculate the distance between two points.
// Note: you will only need this function for the Draw Ellipse command
// in Stages 3 and 4.
double distance(int row1, int col1, int row2, int col2);


// ADD PROTOTYPES FOR YOUR FUNCTIONS HERE

void draw_line(int canvas[N_ROWS][N_COLS], int start_row, int start_col, int length, int direction);


int main(void) {
    
    int canvas[N_ROWS][N_COLS];
    int instruction, start_row, start_col, length, direction;
    
        // recieve input
        scanf("%d %d %d %d %d", &instruction, &start_row, &start_col, &length, &direction);

        clearCanvas(canvas);  
                    
            if (instruction == 1) {
                draw_line(canvas, start_row, start_col, length, direction);
        }
    
    

    // TODO: Add your code here!

    // Hint: start by scanning in the command.
    //
    // If the command is the "Draw Line" command, scan in the rest of
    // the command (start row, start col, length, direction) and use
    // that information to draw a line on the canvas.
    //
    // Once your program can draw a line, add a loop to keep scanning
    // commands until you reach the end of input, and process each
    // command as you scan it.

    displayCanvas(canvas);

    return 0;
}



// ADD CODE FOR YOUR FUNCTIONS HERE


// function to draw a line
void draw_line(int canvas[N_ROWS][N_COLS], int start_row, int start_col, int length, int direction) {
    
    int d = direction % 360;
    int drawn = 0;

    while (drawn < length) {
            //drawing up
        if (d == 0) {
            canvas[start_row - drawn][start_col] = 0;
        }
            //drawing right
        else if (d == 90) {
            canvas[start_row][start_col + drawn] = 0;
        }
            //drawing down
        else if (d == 180) {
            canvas[start_row + drawn][start_col] = 0;
        }
            //drawing left
        else if (d == 270) {
            canvas[start_row][start_col - drawn] = 0;
        }
        drawn++;
    
        }
}
    

    
    
    
    


// Displays the canvas, by printing the integer value stored in
// each element of the 2-dimensional canvas array.
//
// You should not need to change the displayCanvas function.
void displayCanvas(int canvas[N_ROWS][N_COLS]) {
    int row = 0;
    while (row < N_ROWS) {
        int col = 0;
        while (col < N_COLS) {
            printf("%d ", canvas[row][col]);
            col++;
        }
        row++;
        printf("\n");
    }
}


// Sets the entire canvas to be blank, by setting each element in the
// 2-dimensional canvas array to be WHITE (which is #defined at the top
// of the file).
//
// You should not need to change the clearCanvas function.
void clearCanvas(int canvas[N_ROWS][N_COLS]) {
    int row = 0;
    while (row < N_ROWS) {
        int col = 0;
        while (col < N_COLS) {
            canvas[row][col] = WHITE;
            col++;
        }
        row++;
    }
}

// Calculate the distance between two points (row1, col1) and (row2, col2).
// Note: you will only need this function for the Draw Ellipse command
// in Stages 3 and 4.
double distance(int row1, int col1, int row2, int col2) {
    int row_dist = row2 - row1;
    int col_dist = col2 - col1;
    return sqrt((row_dist * row_dist) + (col_dist * col_dist));
}
