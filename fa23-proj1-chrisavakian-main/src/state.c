#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t* state, unsigned int snum);
static char next_square(game_state_t* state, unsigned int snum);
static void update_tail(game_state_t* state, unsigned int snum);
static void update_head(game_state_t* state, unsigned int snum);
bool is_die(char next_spot);
static bool is_body(char c);

/* Task 1 */
game_state_t* create_default_state() {
  // create a gamestate pointer
  game_state_t* defaultGame = malloc(sizeof(game_state_t));

  //Assign the easy stuff
  defaultGame->num_rows = 18;
  defaultGame->num_snakes = 1;

  //Create an array of snake_t* which allocates the correct amount of storage
  struct snake_t* players = malloc(sizeof(struct snake_t));

  players[0].tail_row = 2;
  players[0].tail_col = 2;
  players[0].head_row = 2;
  players[0].head_col = 4;
  players[0].live = true;

  //Add snakes to the defaultGame attributes
  defaultGame->snakes = players;

  //making 2d array for board
    long unsigned int r = 18;
    long unsigned int c = 20;
  
    char **arr; 
  arr = (char**) malloc(r * sizeof(char*));
  
  for (int i = 0; i < r; i++) {
        arr[i] = (char *) calloc(c + 1, sizeof(char));

        for (int j = 0; j < c; j++) {
            arr[i][j] = ' ';
        }
        arr[i][c] = '\0';
    }
  //Filling the board with walls '#', fruits '*' and snake.
  //Filling the middle part with ' '

    for (int i = 0; i < sizeof(arr); i++) {
        for (int j = 0; j < sizeof(arr[i]); j++) {
            arr[i][j] = ' ';
        }
    }

  //Top/Bottom row filled with #
    for (int col = 0; col < c; col++) {
        arr[0][col] = '#';
        arr[r - 1][col] = '#';
    }
    
    //Left/Right columsn filled with #
    for (int row = 0; row < r; row++) {
        arr[row][0] = '#';
        arr[row][c - 1] = '#';
    }

        //Fruit
  arr[2][9] = '*';
        //Head
  arr[2][4] = 'D';
  //Body
  arr[2][3] = '>';
        //tail
  arr[2][2] = 'd';

  defaultGame->board = arr;

  return defaultGame;
}

/* Task 2 */
void free_state(game_state_t* state) {
    //We malloc the board and the snakes
    //So free the snakes first
    free(state->snakes);

    //Need to free the board now
    //Iterate over the 2D array
    for (int i = 0; i < state->num_rows; i++) {
        free(state->board[i]);
    }
    free(state->board);
    free(state);
  return;
}

/* Task 3 */
void print_board(game_state_t* state, FILE* fp) {
    for (int i = 0; i < state->num_rows; i++) {

        //fprintf(fp, state->board[i]);
        fputs(state->board[i], fp);
        
        //Every time  have finished a row, we need to new line
        fprintf(fp, "\n");
    }

  return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
//Why is there two save_boards?
/*
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  //(already implemented for you).
}
*/
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t* state, unsigned int row, unsigned int col) {
  return state->board[row][col];
}

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  if (c == 'w' || c == 'a' || c == 's' || c == 'd') {
      return true;
  }
  return false;       //plus 1 bc 0 indexed

}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  if (c == 'W' || c == 'A' || c == 'S'|| c == 'D' || c == 'x') {
      return true;
  }
  return false;
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  if (is_head(c) || is_tail(c) || c == '^' || c == '<' || c == 'v' || c == '>') {
      return true;
  }
  return false;
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
//what does this mean, does this mean ^ == w, < == a, etc? 
static char body_to_tail(char c) {  
 if (c == '^') {
      return 'w';
  } else if (c == '<') {
      return 'a';
  } else if (c == 'v') {
      return 's';
  } else if (c == '>') {
      return 'd';
  }
  return '?';
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  if (c == 'W') {
      return '^';
  } else if (c == 'A') {
      return '<';
  } else if (c == 'S') {
      return 'v';
  } else if (c == 'D') {
      return '>';
  }
  return '?';
}

/*cas = state->board[tailX][tailY];


  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  if (c == 'v' || c == 's' || c == 'S') {
      return cur_row + 1;
  }
  else if (c == '^' || c == 'w' || c == 'W') {
      return cur_row - 1;
  }
  return cur_row;
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  if (c == '>' || c == 'd' || c == 'D') {
      return cur_col + 1;
  }
  else if (c == '<' || c == 'a' || c == 'A') {
      return cur_col - 1;
  }
  return cur_col;
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t* state, unsigned int snum) {
    struct snake_t snakeOfInterest = state->snakes[snum];
    unsigned int snakeX = snakeOfInterest.head_row;
    unsigned int snakeY = snakeOfInterest.head_col;

    char snakeSymbol = state->board[snakeX][snakeY];

    unsigned int newX = get_next_row(snakeX, snakeSymbol);
    unsigned int newY = get_next_col(snakeY, snakeSymbol);

    return state->board[newX][newY];
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t* state, unsigned int snum) {
  //Creating a snake_t variable in order to make things simpler
  //Store the current head coordinates
  unsigned int currSnakeX = state->snakes[snum].head_row;
  unsigned int currSnakeY = state->snakes[snum].head_col;
  
  //Store the symbol of the snake
  char snakeSymbol = state->board[currSnakeX][currSnakeY];

  //Get the coordinates that the head of the snake will be going towards
  unsigned int newX = get_next_row(currSnakeX, snakeSymbol);
  unsigned int newY = get_next_col(currSnakeY, snakeSymbol);

  //Move the snake's head on the snake_t struct and the game board
  state->snakes[snum].head_row = newX;
  state->snakes[snum].head_col = newY;

  state->board[newX][newY] = snakeSymbol;

  //Do not know what to set this value as
  //This is the area where the head of the snake once occupied
  //TODO: Find out what to replace with teh old snake head position
  state->board[currSnakeX][currSnakeY] = head_to_body(snakeSymbol);

  return;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t* state, unsigned int snum) {
    unsigned int currSnakeX = state->snakes[snum].tail_row;
    unsigned int currSnakeY = state->snakes[snum].tail_col;

    char snakeSymbol = state->board[currSnakeX][currSnakeY];

    unsigned int newX = get_next_row(currSnakeX, snakeSymbol);
    unsigned int newY = get_next_col(currSnakeY, snakeSymbol);

    char whats_at_new_spot = state->board[newX][newY];

    state->snakes[snum].tail_row = newX;
    state->snakes[snum].tail_col = newY;
    
    state->board[newX][newY] = body_to_tail(whats_at_new_spot);
    state->board[currSnakeX][currSnakeY] = ' ';

    return;
} 

/* Task 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) { 
    //Iterate through every snake
    for (unsigned int i = 0; i < state->num_snakes; i++) {
       
        //If the snake is actually dead, then skip this cycle
        if (state->snakes[i].live == false) {
            continue;
        } 
        
        //Find the next space it's going into
        char next_spot = next_square(state, i);

        //Check if the snake is going to die
        if (is_die(next_spot) == true) {

            //replace the snake's head with x, and change it's state to dead
            unsigned int currSnakeX = state->snakes[i].head_row;
            unsigned int currSnakeY = state->snakes[i].head_col; 

            state->board[currSnakeX][currSnakeY] = 'x';

            state->snakes[i].live = false;

            continue;
        }

        //If check if the next spot has a fruit
        if (next_spot == '*') {
            //do not update the tail
            unsigned int snakeX = state->snakes[i].head_row;
            unsigned int snakeY = state->snakes[i].head_col;

           unsigned int fruitX = get_next_row(snakeX, state->board[snakeX][snakeY]);
           unsigned int fruitY = get_next_col(snakeY, state->board[snakeX][snakeY]);
           state->board[fruitX][fruitY] = ' ';

           update_head(state, i);

           add_food(state);
            
        } else {
            update_head(state, i); 
            update_tail(state, i);
        }
    }
  return;
}

bool is_die (char next_spot) {
    if (next_spot == '#' || is_snake(next_spot)) {
        return true;
    }
    return false;
}


/* Task 5 */
game_state_t* load_board(FILE* fp) {
   game_state_t *gameReturn = malloc(sizeof(struct game_state_t));
   char **board = malloc(sizeof(char*));
   board[0] = malloc(sizeof(char));
   
   //These represent which row and col we are currently at
   //This: arr[row][col]
   unsigned int row = 0;
   unsigned int col = 0;

   //Since we don't know how long anything is going to be, we will run this for as long as need be
   while (true) {
       //We want to get the next char, so fgetc will give us the next char
       char currChar = fgetc(fp); 
       
       //If we're at the end of the file, we get EOF
       if (currChar == EOF) {
           break;
       }

       //Some checks if we're at the end of the line
       if (currChar == '\n' || currChar == '\0') {
           board[row] = realloc(board[row], (col + 1) * sizeof(char));
           board[row][col] = '\0';

           row = row + 1;
           col = 0;
           
           gameReturn->num_rows = row;
           board = realloc(board, sizeof(char*) * row + 1);
           board[row] = NULL;

           continue;
       }
        
       //At this point, we're just at a normal part of the gameboard
       //So we need to store the char and most likely need to realloc
       board[row] = realloc(board[row], (col + 1) * sizeof(char));
       board[row][col] = currChar;
       col = col + 1;
   }

    gameReturn->board = board;
    gameReturn->num_snakes = 0;
    gameReturn->snakes = NULL;

  return gameReturn;
}

/*
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t* state, unsigned int snum) {
    unsigned int snakeTailX = state->snakes[snum].tail_row;
    unsigned int snakeTailY = state->snakes[snum].tail_col;
    
    int oldX;
    int oldY;

    while(is_head(state->board[snakeTailX][snakeTailY]) == false) {
        oldX = snakeTailX;
        oldY = snakeTailY;

        snakeTailX = get_next_row(oldX, state->board[oldX][oldY]);
        snakeTailY = get_next_col(oldY, state->board[oldX][oldY]);
    }
    state->snakes[snum].head_row = snakeTailX;
    state->snakes[snum].head_col = snakeTailY; 

  return;
}

static bool is_body(char c) {
  if (c == '^' || c == '<' || c == 'v' || c == '>') {
      return true;
  }
  return false;
}


/* Task 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
    int currSnake = 0;
    
    state->snakes = malloc(sizeof(struct snake_t)); 

    for (int x = 1; x < state->num_rows; x++) {
        for (int y = 1; state->board[x][y] != '#'; y++) {
            if (is_tail(state->board[x][y])) {
                state->snakes[currSnake].tail_row = x;
                state->snakes[currSnake].tail_col = y;
                find_head(state, currSnake);

                state->snakes[currSnake].live = true;

                currSnake = currSnake + 1;

                state->num_snakes++;
                state->snakes = realloc(state->snakes, (currSnake + 1) * sizeof(struct snake_t));
            }
        }
    }
    

  return state;
}
