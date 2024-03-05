// This struct represents the cells in the maze.
// x and y are the coordinates of a cell
// walls contains 4 boolean vallues, which are correspondingly true if the top,
// right, bottom or left walls are present in the cell
// dist contains the distance from the end point of the maze
struct Cell {
    short x;
    short y;
    bool walls[4];
    short dist;
};

// you can change this number to change the dimensions of the maze. currently
// it's an 8*8 maze
#define NUM_CELLS 8

// this struct contains the 2d array containing cells.
// rows is the number of rows, and cols is the number of columns in the grid
struct Grid {
    short rows;
    short cols;
    Cell grid[NUM_CELLS][NUM_CELLS];
};

// just a function which returns absolute value of the number passed to it
int absolute(int num) {
    if (num > 0) {
        return num;
    } else {
        return -num;
    }
}

// Initialises a grid by doing the following:
// Sets the x and y for all it's cells
// sets the distance assuming there are no walls except the outer walls
// sets the outer walls for the required cells
// returns the grid struct
Grid make_grid() {
    Grid grid;
    grid.cols = NUM_CELLS;
    grid.rows = NUM_CELLS;
    for (int i = 0; i < grid.cols; i++) {
        for (int j = 0; j < grid.rows; j++) {
            grid.grid[i][j].x = i;
            grid.grid[i][j].y = j;
            grid.grid[i][j].walls[0] = false;
            grid.grid[i][j].walls[1] = false;
            grid.grid[i][j].walls[2] = false;
            grid.grid[i][j].walls[3] = false;
            if (i == 0) {
                grid.grid[i][j].walls[3] = true;
            } else if (i == grid.cols - 1) {
                grid.grid[i][j].walls[1] = true;
            }
            if (j == 0) {
                grid.grid[i][j].walls[0] = true;
                Serial.println("lol");
            } else if (j == grid.rows - 1) {
                grid.grid[i][j].walls[2] = true;
            }
            grid.grid[i][j].dist = absolute(2 * i - NUM_CELLS + 1) / 2 +
                                   absolute(2 * j - NUM_CELLS + 1) / 2;
        }
    }
    return grid;
}

// sets a wall to the grid. Apart from just setting one value to true, it also
// checks which side the wall is being inserted, and sets that side's cell's
// corresponding wall to true as well
short set_wall(Grid *grid, short x, short y, short wall_no) {
    // if wall is already set, just return
    if (grid->grid[x][y].walls[wall_no] == true) {
        return 0;
    }
    // set the wall to true
    grid->grid[x][y].walls[wall_no] = true;
    // chack the surrounding cells
    if (wall_no == 0) {
        grid->grid[x][y - 1].walls[2] = true;
    } else if (wall_no == 1) {
        grid->grid[x + 1][y].walls[3] = true;
    } else if (wall_no == 2) {
        grid->grid[x][y + 1].walls[0] = true;
    } else if (wall_no == 3) {
        grid->grid[x - 1][y].walls[1] = true;
    }
}

// runs the floodfill algorithm. takes a reference to the grid and an integer
// array that contains two values which are the x and y values of the current
// cell the mouse is in. It modifies both the grid and current cell.
// It returns 1 when the desired cell is reached.
short run_floodfill(Grid *grid, short *curr_cell) {
    // store the x and y coordinates in temorary local variables for conveniance
    short x = curr_cell[0];
    short y = curr_cell[1];

    // this variable (min_dist) will be used to hold the minimum value of
    // distance for the reachable cells around it. (by reachabele I mean the
    // cells around the current cell which are not blocked by a wall)
    short min_dist = NUM_CELLS * NUM_CELLS;

    // this variable holds the coordinats of the cell for which the distance is
    // min_dist
    short min_cell[2] = {0, 0};

    // if there is no top wall
    if (!grid->grid[x][y].walls[0]) {
        // if distance of the top cell is less than min_dist
        if (min_dist > grid->grid[x][y - 1].dist) {
            // set min_dist and min_cell accordingly
            min_dist = grid->grid[x][y - 1].dist;
            min_cell[0] = x;
            min_cell[1] = y - 1;
        }
    }
    // if there is no right wall
    if (!grid->grid[x][y].walls[1]) {
        // if distance of the right cell is less than min_dist
        if (min_dist > grid->grid[x + 1][y].dist) {
            // set min_dist and min_cell accordingly
            min_dist = grid->grid[x + 1][y].dist;
            min_cell[0] = x + 1;
            min_cell[1] = y;
        }
    }
    // if there is no bottom wall
    if (!grid->grid[x][y].walls[2]) {
        // if distance of the bottom cell is less than min_dist
        if (min_dist > grid->grid[x][y + 1].dist) {
            // set min_dist and min_cell accordingly
            min_dist = grid->grid[x][y + 1].dist;
            min_cell[0] = x;
            min_cell[1] = y + 1;
        }
    }
    // if there is no left wall
    if (!grid->grid[x][y].walls[3]) {
        // if distance of the left cell is less than min_dist
        if (min_dist > grid->grid[x - 1][y].dist) {
            // set min_dist and min_cell accordingly
            min_dist = grid->grid[x - 1][y].dist;
            min_cell[0] = x - 1;
            min_cell[1] = y;
        }
    }
    // if there is a cell around curr_cell with a lesser distance, go to that
    // cell
    if (min_dist < grid->grid[x][y].dist) {
        curr_cell[0] = min_cell[0];
        curr_cell[1] = min_cell[1];
    } else {
        // initialise a queue
        short queue[2][64];
        short start = 0;
        short end = 0;
        // add the current cell's coordinates to queue
        queue[0][start] = curr_cell[0];
        queue[1][start] = curr_cell[1];
        end++;
        // while queue is not empty
        while (end != start) {
            // if distance is 0, that means we have reached the destination, so
            // return 1
            if (grid->grid[x][y].dist == 0) {
                return 1;
            }
            // dequeue the x and y coordinates from the queue.
            short x = queue[0][start];
            short y = queue[1][start];
            start++;
            // if the start of the queue exceeds the length of the queue, roll
            // back to 0
            if (start >= 64) {
                start = 0;
            }

            // this min_dist and min_cell have the same purpose as before
            short min_dist = NUM_CELLS * NUM_CELLS;
            short min_cell[2] = {0, 0};

            // contains the coordinates of the cells around the current cell
            // which are not blocked by walls. to_add_len just contains the
            // length of this array
            short to_add[2][4];
            short to_add_len = 0;

            // these if statements are quite similar to the previous ones
            if (!grid->grid[x][y].walls[0]) {
                if (min_dist > grid->grid[x][y - 1].dist) {
                    min_dist = grid->grid[x][y - 1].dist;
                    min_cell[0] = x;
                    min_cell[1] = y - 1;
                }
                to_add[0][to_add_len] = x;
                to_add[1][to_add_len] = y - 1;
                to_add_len++;
            }
            if (!grid->grid[x][y].walls[1]) {
                if (min_dist > grid->grid[x + 1][y].dist) {
                    min_dist = grid->grid[x + 1][y].dist;
                    min_cell[0] = x + 1;
                    min_cell[1] = y;
                }
                to_add[0][to_add_len] = x + 1;
                to_add[1][to_add_len] = y;
                to_add_len++;
            }
            if (!grid->grid[x][y].walls[2]) {
                if (min_dist > grid->grid[x][y + 1].dist) {
                    min_dist = grid->grid[x][y + 1].dist;
                    min_cell[0] = x;
                    min_cell[1] = y + 1;
                }
                to_add[0][to_add_len] = x;
                to_add[1][to_add_len] = y + 1;
                to_add_len++;
            }
            if (!grid->grid[x][y].walls[3]) {
                if (min_dist > grid->grid[x - 1][y].dist) {
                    min_dist = grid->grid[x - 1][y].dist;
                    min_cell[0] = x - 1;
                    min_cell[1] = y;
                }
                to_add[0][to_add_len] = x - 1;
                to_add[1][to_add_len] = y;
                to_add_len++;
            }
            // if the current cell's value if less than or equal to the mminimum
            // cell around it, set the current cell's distance to min_dist + 1
            // and add the cells stored in to_add to the queue
            if (grid->grid[x][y].dist <= min_dist) {
                grid->grid[x][y].dist = min_dist + 1;
                for (short i = 0; i < to_add_len; i++) {
                    queue[0][end] = to_add[0][i];
                    queue[1][end] = to_add[1][i];
                    end++;
                    // roll back the end of the queue if it exceeds the length
                    // of the queue
                    if (end >= 64) {
                        end = 0;
                    }
                }
            }
        }
    }
    return 0;
}

// Prints the grid to serial. Made for easy debugging purposes
// current cell is represented by <>
short print_grid(Grid *grid, short *curr_cell) {
    for (short j = 0; j < grid->rows; j++) {
        for (short i = 0; i < grid->cols; i++) {
            if (grid->grid[i][j].walls[0]) {
                Serial.print(" __");
            } else {
                Serial.print("   ");
            }
        }
        Serial.println(" ");
        if (grid->grid[0][j].walls[3]) {
            Serial.print("|");
        } else {
            Serial.print(" ");
        }
        for (short i = 0; i < grid->cols; i++) {
            if (i == curr_cell[0] && j == curr_cell[1]) {
                Serial.print("<>");
            } else {
                Serial.print(grid->grid[i][j].dist);
                if (grid->grid[i][j].dist < 10) {
                    Serial.print(" ");
                }
            }
            if (grid->grid[i][j].walls[1]) {
                Serial.print("|");
            } else {
                Serial.print(" ");
            }
        }
        Serial.println(" ");
    }
    for (short i = 0; i < grid->cols; i++) {
        Serial.print(" __");
    }
    Serial.println(" ");

    return 0;
}

short curr_cell[2];
Grid grid;

void setup() {
    Serial.begin(115200);
    grid = make_grid();
    // set current cell to 0, 0 to start off with
    curr_cell[0] = 0;
    curr_cell[1] = 0;

    // set some walls. just for demostratin purposes, in actual operation, it
    // will be set by some other function based on the output of the infrared
    // sensors.
    set_wall(&grid, 4, 0, 2);
    set_wall(&grid, 3, 0, 2);
    set_wall(&grid, 2, 0, 2);
    set_wall(&grid, 1, 0, 2);
    set_wall(&grid, 0, 0, 2);
}

void loop() {
    Serial.println("");
    print_grid(&grid, curr_cell);
    while (true) {
        Serial.println("");
        if (run_floodfill(&grid, curr_cell)) {
            break;
        }
        delay(700);
        print_grid(&grid, curr_cell);
        Serial.print(curr_cell[0]);
        Serial.print(" ");
        Serial.println(curr_cell[1]);
    }
    while (true)
        ;
}
