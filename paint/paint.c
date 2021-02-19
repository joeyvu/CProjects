// Assignment 1 19T3 COMP1511: CS Paint
// paint.c
// Welcome to Joe's Assingment. I hope you enjoy your time reading through it
// I have attempted stages 1 through to 3
// This program was written by Joe Nguyen (z5120839)
// 26/10/19
// Version 1.0.0 (2019-10-08): Assignment released.

#include <stdio.h>

// Features within math.h are used to help draw the ellipse
#include <math.h>

// The dimensions of the canvas (20 rows x 36 columns).
#define N_ROWS 20
#define N_COLS 36

// Shades (assuming your terminal has a black background).
#define BLACK 0
#define WHITE 4
#define DARK 1
#define GREY 2
#define LIGHT 3

#define DRAW_LINE_CMD 1


//FUNCTION PROTOTYPES BELOW:

// Displays the canvas.
void displayCanvas(int canvas[N_ROWS][N_COLS]);

// Clears the canvas by setting every pixel to be white.
void clearCanvas(int canvas[N_ROWS][N_COLS]);

// Changes the shade of parts of the canvas.
int change_shade(int change_in_shade, int current_shade);

// Copies a line and pastes it at the target location within the canvas.
void copy_paste_line(int canvas[N_ROWS][N_COLS], int start_row, int start_col, int length, int direction, int target_row,
                 int target_col);

// Copies a square and pastes it at the target location within the canvas.
void copy_paste_square(int canvas[N_ROWS][N_COLS], int start_row, int start_col, int length, int direction, int target_row,
                 int target_col);

// Draws an Ellipse on the canvas.
void draw_ellipse(int canvas[N_ROWS][N_COLS], int focus_1_row, int focus_1_col, int focus_2_row, 
                                                int focus_2_col, double length, int fill, int shade);

// Checks if the line you're trying to create stays inside the canvas.
int check_line_valid (int canvas[N_ROWS][N_COLS], int start_row, int start_col, int length, int direction);

// Checks if the square you're trying to create stays inside the canvas.
int check_square_valid (int canvas[N_ROWS][N_COLS], int start_row, int start_col, int length, int direction);


// Calculates the distance between two points. This is used in the Ellipse.
double distance(int row1, int col1, int row2, int col2);

// Draws a line on the canvas.
void draw_line(int canvas[N_ROWS][N_COLS], int start_row, int start_col, int length, int direction, int shade);

// Fills in a square on the canvas.
void fill_square(int canvas[N_ROWS][N_COLS], int start_row, int start_col, int length, int direction, int shade);

// FUNCTION PROTOTYPES ABOVE

// MAIN FUNCTION BELOW:

int main(void) {
    int canvas[N_ROWS][N_COLS];
    int instruction;
    int shade = BLACK;
    clearCanvas(canvas);
   
    // Scans in a set of instructions until the user ends their input.
    while (scanf("%d", &instruction) != EOF) {
        
        int start_row, start_col, length, direction;
       
       // Assigns different instruction inputs to their respective function.
        if (instruction == 1|| instruction == 2 || instruction == 4){
            
            scanf("%d %d %d %d", &start_row, &start_col, &length, &direction);
        }
        if (instruction == 1){
            draw_line(canvas, start_row, start_col, length, direction, shade);
        }
        if (instruction == 2){            
            fill_square(canvas, start_row, start_col, length, direction, shade);
        }

        if (instruction == 3){
            int change_in_shade;
            scanf("%d", &change_in_shade);
            shade = change_shade(change_in_shade, shade);
        }

        if (instruction == 4){
            int target_row, target_col;
            scanf("%d %d", &target_row, &target_col);
            if (direction % 90 == 0) {
                copy_paste_line(canvas, start_row, start_col, length, direction, target_row, target_col);
            }
            else {
                copy_paste_square(canvas, start_row, start_col, length, direction, target_row, target_col);
            }

        }
        if (instruction == 0){
            int focus_1_row, focus_1_col, focus_2_row, focus_2_col, fill;
            double ellipse_length;
            scanf("%d %d %d %d %lf %d", &focus_1_row, &focus_1_col, &focus_2_row, &focus_2_col, &ellipse_length, &fill);
            draw_ellipse(canvas, focus_1_row, focus_1_col, focus_2_row, focus_2_col, ellipse_length, fill, shade);
        }


    }
    
    displayCanvas(canvas);
       
    

    return 0;
}

// MAIN FUNCTION ABOVE

// FUNCTION CODE BELOW:

// Draw Line Function
void draw_line(int canvas[N_ROWS][N_COLS], int start_row, int start_col, int length, int direction, int shade) {

    // Draw Line Step 1: Adjust the direction input using modulus.   
    int d = direction % 360;
    int drawn = 0;
    int valid = 0;
 
    // Draw Line Step 2: If the function is a negative, adjust it to the opposite direction.
    if (length < 0 && d == 45) {
        length = length * -1;
        d = 225;
    }
    if (length < 0 && d == 135) {
        length = length * -1;
        d = 315;
    }
    if (length < 0 && d == 315) {
        length = length * -1;
        d = 135;
    }
    if (length < 0 && d == 225) {
        length = length * -1;
        d = 45;
    }
    
    
    // Draw Line Step 3: Proceed only if the line stays within the canvas.
    valid = check_line_valid(canvas, start_row, start_col, length, direction);

    if (valid == 1) {
        
        // Draw Line Step 4: Draw line according to different directions using 'drawn' as an increment.
        while (drawn < length) {
            //drawing up
            if (d == 0) {
                canvas[start_row - drawn][start_col] = shade;
            }
            //drawing right
            else if (d == 90) {
                canvas[start_row][start_col + drawn] = shade;
            }
            //drawing down
            else if (d == 180) {
                canvas[start_row + drawn][start_col] = shade;
            }
            //drawing left
            else if (d == 270) {
                canvas[start_row][start_col - drawn] = shade;
            }
            //drawing south east
            else if (d == 135) {
                canvas[start_row + drawn][start_col + drawn] = shade;
            }
            //drawing noth east
            else if (d == 45) {
                canvas[start_row - drawn][start_col + drawn] = shade;
            }
            //drawing south west
            else if (d == 225) {
                canvas[start_row + drawn][start_col - drawn] = shade;
            }
            //drawing north west
            else if (d == 315) {
                canvas[start_row - drawn][start_col -drawn] = shade;
            }
            drawn++;
        
        }
    }
}

// Fill square function
void fill_square(int canvas[N_ROWS][N_COLS], int start_row, int start_col, int length, int direction, int shade) {
    
    // Fill Square Step 1: adjust the direction using modulus.
    int d = direction % 360;
    int valid = 0;
    
    // Fill Square Step 2: create a line instead if the direction isn't diagonal.
    if (direction % 90 == 0) {
        draw_line(canvas, start_row, start_col, length, direction, shade);
    } 
    // Fill Square Step 3: Only proceed if the square stays inside the canvas.
    valid = check_square_valid(canvas, start_row, start_col, length, direction);
    
    // Fill Square Step 4: Fill the square by using loops in two directions.
    if (valid == 1) {
        
        int row_counter = start_row;
        int col_counter = start_col;
               
        // Filling the square north east
        if (d == 45) {             
            while (row_counter > start_row - length) {               
                while (col_counter < start_col + length) {                  
                    canvas[row_counter][col_counter] = shade;
                    col_counter++;
                }
                col_counter = start_col;
                row_counter--;
            }
        }

        // Filling square north west
        if (d == 315) {
            while (row_counter > start_row - length){
                while (col_counter > start_col - length){
                    canvas[row_counter][col_counter] = shade;
                    col_counter--;
                }
                col_counter = start_col;
                row_counter--;
            }

        }

        // filling sqaure south east
        if (d == 135) {
            while (row_counter < start_row + length){
                while (col_counter < start_col + length){
                    canvas[row_counter][col_counter] = shade;
                    col_counter++;
                }
                col_counter = start_col;
                row_counter++;
            }
        }

        // filling square south west
        if (d == 225) {
            while (row_counter < start_row + length){
                while (col_counter > start_col - length){
                    canvas[row_counter][col_counter] = shade;
                    col_counter--;
                }
                col_counter = start_col;
                row_counter++;
            }
            
        }
    }


}

// Change Shade function
int change_shade(int change_in_shade, int current_shade){
    // Change Shade Step 1: replace the current shade with the input for new shade
    int new_shade = current_shade;
    if (change_in_shade + current_shade >= 0 && change_in_shade + current_shade <= 4){
        new_shade = change_in_shade + current_shade;  
    }
    // Change Shade Step 2: Output the new shade instead
    return new_shade;
}

// Copy Paste Line function (or CPL)
void copy_paste_line(int canvas[N_ROWS][N_COLS], int start_row, int start_col, int length, 
                int direction, int target_row, int target_col) {
    if (check_line_valid(canvas, target_row, target_col, length, direction) == 0) {
        return;
    }
    // CPL Step 1: adjust the input direction using modulus
    int d = direction % 360;
    int drawn = 0;
    int copy_length = 0;
 
    // CPL Step 2: account for negative distances, and replace with the opposite direction
    if (length < 0 && d == 45) {
        copy_length = length * -1;
        d = 225;
    }
    if (length < 0 && d == 135) {
        copy_length = length * -1;
        d = 315;
    }
    if (length < 0 && d == 315) {
        copy_length = length * -1;
        d = 135;
    }
    if (length < 0 && d == 225) {
        copy_length = length * -1;
        d = 45;
    }

    // CPL Step 3: Draw the line using a 'drawn' as an increment
    // Also: Replace the output with the target position
    while (drawn < copy_length) {    
        //drawing up
        if (d == 0) {
            canvas[target_row - drawn][target_col] = canvas[start_row - drawn][start_col];
        }
        //drawing right
        else if (d == 90) {
            canvas[target_row][target_col + drawn]= canvas[start_row][start_col + drawn];
        }
        //drawing down
        else if (d == 180) {               
            canvas[target_row + drawn][target_col] = canvas[start_row + drawn][start_col];
        }
        //drawing left
        else if (d == 270) {
            canvas[target_row][target_col - drawn] = canvas[start_row][start_col - drawn];
        }
        //drawing south east
        else if (d == 135) {
            canvas[target_row + drawn][target_col + drawn] = canvas[start_row + drawn][start_col + drawn];
        }
        //drawing north east
        else if (d == 45) {
            canvas[target_row - drawn][target_col + drawn] = canvas[start_row - drawn][start_col + drawn];
        }
        //drawing south west
        else if (d == 225) {
            canvas[target_row + drawn][target_col - drawn] = canvas[start_row + drawn][start_col - drawn];
        }
        //drawing north east
        else if (d == 315) {
            canvas[target_row - drawn][target_col -drawn] = canvas[start_row - drawn][start_col -drawn];
        }

        drawn++;
        
    }
}

// Copy Paste Square Function (or CPS)
void copy_paste_square(int canvas[N_ROWS][N_COLS], int start_row, int start_col, int length, int direction, int target_row,
                 i nt target_col){
    // CPS Step 1: check if the input will be inside the canvas
    if (check_square_valid(canvas, target_row, target_col, length, direction) == 0) {
        return;
    }
    // CPS Step 2: adjust the direction to its modulus
    int d = direction % 360;
    
    // CPS Step 3: declare counters for the parameters of the square
    int row_counter = start_row;
    int col_counter = start_col;
    int target_row_counter = target_row;
    int target_col_counter = target_col;

               
    // CPS Step 4: Fill the square in different directions
    // Also: replace the output with the target location
    
    // filling the square north east
    if (d == 45) {             
        while (row_counter > start_row - length && target_row_counter > target_row - length) {               
            while (col_counter < start_col + length && target_col_counter < target_col + length) {                  
                canvas[target_row_counter][target_col_counter] = canvas[row_counter][col_counter];
                col_counter++;
                target_col_counter++;
            }
            col_counter = start_col;
            target_col_counter = target_col;
            row_counter--;
            target_row_counter--;
        }
    }

    // filling square north west
    if (d == 315) {
        while (row_counter > start_row - length && target_row_counter > target_row - length){
            while (col_counter > start_col - length && target_col_counter > target_col - length){
                canvas[target_row_counter][target_col_counter] = canvas[row_counter][col_counter];
                col_counter--;
                target_col_counter--;
            }
            col_counter = start_col;
            target_col_counter = target_col;
            row_counter--;
            target_row_counter--;
        }

    }

    // filling sqaure south east
    if (d == 135) {
        while (row_counter < start_row + length && target_row_counter < target_row + length ){
            while (col_counter < start_col + length && target_col_counter < target_col + length){
                canvas[target_row_counter][target_col_counter] = canvas[row_counter][col_counter];
                col_counter++;
                target_col_counter++;
            }
            col_counter = start_col;
            target_col_counter = target_col;
            row_counter++;
            target_row_counter++;
        }
    }

    // filling square south west
    if (d == 225) {
        while (row_counter < start_row + length && target_row_counter < target_row + length){
            while (col_counter > start_col - length && target_col_counter > target_col - length){
                canvas[target_row_counter][target_col_counter] = canvas[row_counter][col_counter];
                col_counter--;
                target_col_counter--;
            }
            col_counter = start_col;
            target_col_counter = target_col;
            row_counter++;
            target_row_counter++;
        }    
    }
}

// Check Line Valid function (or CLV)
int check_line_valid (int canvas[N_ROWS][N_COLS], int start_row, int start_col, int length, int direction){

    //CLV Step 1: adjust the direction to it modulus
    int d = direction % 360;
    int valid = 0;
        
    //CLV Step 2: Check if the start position inputs are inside in the array
    if ( 0 <= start_row && start_row < N_ROWS &&
         0 <= start_col && start_col < N_COLS) {
        
        //CLV Step 3: check if the input line will still inside boundaries of the array
        // Check for all directions

        // Bottom boundary
        if (d == 180 && start_row + length <= N_ROWS) {
            valid = 1;
        }
        // Right boundary
        if (d == 90 && start_col + length <= N_COLS){
            valid = 1;
        }
        // Left boundary
        if (d == 270 && start_col + 1 >= length){
            valid = 1;
        }
        // Top boundary
        if (d == 0 && start_row + 1 >= length){
            valid = 1;
        }
        // Top right boundary
        if (d == 45 && (start_row  +1 >= length && start_col + length  <= N_COLS)) {
            valid = 1;
        }
        // Bottom right boundary
        if (d == 135 && (start_col + length  <= N_COLS && start_row + length <= N_ROWS)) {
            valid = 1;
        }
        // Bottom left boundary
        if (d == 225 && (start_col + 1 >= length && start_row + length  <= N_ROWS)) {
            valid = 1;
        }
        // Top left boundary
        if (d == 315 && (start_col + 1 >= length && start_row + 1 >= length)) {
            valid = 1;
        }
    }
    return valid;
}

// Check Square Valid Function (Or CSV)
int check_square_valid (int canvas[N_ROWS][N_COLS], int start_row, int start_col, int length, int direction){

    // CSV Step 1: adjust the direction to its modulus
    int d = direction % 360;
    int valid = 0;    
     
    // CSV Step 2: Make sure the input is inside the canvas
    if ( 0 <= start_row && start_row < N_ROWS &&
         0 <= start_col && start_col < N_COLS) {

        // CSV Step 3: Make sure the square stays inside the canvas
        
        // Top right boundary
        if (d == 45 && (start_row  +1 >= length && start_col + length  <= N_COLS)) {
            valid = 1;
        }
        // Bottom right boundary
        if (d == 135 && (start_col + length  <= N_COLS && start_row + length <= N_ROWS)) {
            valid = 1;
        }
        // Bottom left boundary
        if (d == 225 && (start_col + 1 >= length && start_row + length  <= N_ROWS)) {
            valid = 1;
        }
        // Top right boundary
        if (d == 315 && (start_col + 1 >= length && start_row + 1 >= length)) {
            valid = 1;
        }               
    }
    return valid;

} 



//Draw Ellipse function
void draw_ellipse(int canvas[N_ROWS][N_COLS], int focus_1_row, int focus_1_col, int focus_2_row, 
                                                int focus_2_col, double length, int fill, int shade) {
    int row = 0;
   
   // Draw Ellipse Step 1: Scan all of the points of the canvas array
    while (row < N_ROWS) {
        int col = 0;
        while (col < N_COLS ) {
            double d1, d2;
            // Draw Ellipse Step 2: Colour the point if it satisfies the distance for the focal points 
            d1 = distance(row, col, focus_1_row, focus_1_col);
            d2 = distance(row, col, focus_2_row, focus_2_col);
            if ((d1 + d2) <= 2*length){
                canvas[row][col] = shade;
            }
            col++;
        }
        row++;
    }
    
}


// Displays the canvas, by printing the integer value stored in
// each element of the 2-dimensional canvas array.
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
double distance(int row1, int col1, int row2, int col2) {
    int row_dist = row2 - row1;
    int col_dist = col2 - col1;
    return sqrt((row_dist * row_dist) + (col_dist * col_dist));
}

// FUNCTION CODE ABOVE
