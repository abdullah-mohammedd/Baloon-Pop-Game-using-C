/**
 * Author: Mohammed Abdullah
 * Description: Code Implementation of the BPGame functions.
 *
 **/
// file: bpgame.c
#include "bpgame.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

typedef struct bpstate {
    char * board; // Use a linear array to store the 2D matrix
    struct bpstate * next;
    int score;
    int nrows;
    int ncols;
} BPState;

struct bpgame {
    BPState * current;
};


/*** Helper functions for bpstate ***/
BPState * bps_create(int nrows, int ncols)
{
    // Check if nrows and ncols are within range
    if (nrows < 0 || nrows > MAX_ROWS || ncols < 0 || ncols > MAX_COLS)
        return NULL;

    BPState * bp_state = (BPState*)calloc(1, sizeof(BPState));

    bp_state->ncols = ncols;
    bp_state->nrows = nrows;
    bp_state->board = (char*)calloc(nrows * ncols, 1);

    // Failed to allocate memory for the board
    if (!bp_state->board)
    {
        free(bp_state);
        return NULL;
    }

    // Initialize the board with all air
    memset(bp_state->board, None, nrows * ncols);

    return bp_state;
}

void bps_destroy(BPState * bp_state)
{
    // Check for null state
    if (!bp_state)
        return;

    free(bp_state->board);
    free(bp_state);
}

BPState * bps_copy(const BPState * bp_state_to_copy)
{
    // Check for null state to copy
    if (!bp_state_to_copy)
        return NULL;

    // Create a bp state instance
    BPState * bp_state = bps_create(bp_state_to_copy->nrows, bp_state_to_copy->ncols);

    // Check if success
    if (!bp_state)
        return NULL;

    // Copy board data
    memcpy(bp_state->board, bp_state_to_copy->board, bp_state->nrows * bp_state->ncols);
    bp_state->score = bp_state_to_copy->score;

    return bp_state;
}

char bps_get(BPState * bp_state, int r, int c)
{
    // Check for null state
    if (!bp_state)
        return -1;
    
    // Check if indices are out of range
    if (r < 0 || r >= bp_state->nrows || c < 0 || c >= bp_state->ncols)
        return -1;
    
    return bp_state->board[r * bp_state->ncols + c];
}

void bps_set(BPState * bp_state, int r, int c, char val)
{
    // Check for null state
    if (!bp_state)
        return;
    
    // Check if indices are out of range
    if (r < 0 || r >= bp_state->nrows || c < 0 || c >= bp_state->ncols)
        return;

    bp_state->board[r * bp_state->ncols + c] = val;
}

int bps_pop(BPState * bp_state, int r, int c, char balloon)
{
    // Base case
    // Check if r and c are within bounds
    if (r < 0 || r >= bp_state->nrows || c < 0 || c >= bp_state->ncols)
        return 0;

    // Check if the cell is the same as the given balloon
    if (bps_get(bp_state, r, c) != balloon)
        return 0;
    
    // Pop the balloon
    bps_set(bp_state, r, c, None);

    // Pop the neighbors and return the total number of balloons pop
    return 1 + bps_pop(bp_state, r - 1, c, balloon)
             + bps_pop(bp_state, r, c - 1, balloon)
             + bps_pop(bp_state, r, c + 1, balloon)
             + bps_pop(bp_state, r + 1, c, balloon);
}

/***

/*** IMPLEMENTATION OF bp_XXXX FUNCTIONS HERE ****/

/**
* Function: bp_create
* Parameters: number of rows and columns
* Description: returns pointer to newly created
* board with given number of rows
* and columns.
*
* Populates board with random balloons.
*
* If nrows or ncols is out of range,
* an error message is printed to stderr
* and NULL is returned.
*
*/
BPGame * bp_create(int nrows, int ncols)
{
    // Check the rows and cols if within the value of state
    if (nrows > MAX_ROWS || ncols > MAX_COLS)
    {
        fprintf(stderr, "Rows and columns are out of range.\n");
        return NULL;
    }

    BPGame * game = (BPGame*)malloc(sizeof(BPGame));

    // Create the top state
    game->current = bps_create(nrows, ncols);

    // Fill the board with random ballons
    static char items[] = { Red, Blue, Green, Yellow };

    // Seed the random generator
    srand(time(NULL));

    // Fill the board per column to make sure that the board is
    // initially compact
    for (int j = 0; j < ncols; j++)
    {
        for (int i = 0; i < nrows; i++)
        {
            bps_set(game->current, i, j, items[rand() % 4]);
        }
    }

    return game;
}

/**
* Function: bp_create_from_mtx
* Parameters: number of rows and columns and 2D array of char
* Description: returns pointer to newly created
* board with given number of rows
* and columns.
*
* Populates board contents of given 2D matrix (assumes
* given dimensions).
*
* Error conditions:
* If nrows or ncols is out of range, an error
message
* is printed to stderr and NULL is returned.
*
* If any element in the matrix is not one of the
understood
* color symbols (from the #define-s at top of
this file),
* an error message is pritnted to stderr and
NULL is returned.
*
*/
BPGame * bp_create_from_mtx(char mtx[][MAX_COLS], int nrows, int ncols)
{
    BPGame * game = bp_create(nrows, ncols);

    if (!game)
        return NULL;

    // Copy the matrix
    for (int i = 0; i < nrows; i++)
    {
        for (int j = 0; j < ncols; j++)
        {
            bps_set(game->current, i, j, mtx[i][j]);
        }
    }

    return game;
}
/**
* Function: bp_destroy
* Description: deallocates all dynamically allocated
* memory associated with the board.
*
* Comment: this might start out as
* a one-liner.
*
*/
void bp_destroy(BPGame * b)
{
    // Loop through each stored state and
    for (BPState *state = b->current, *nextState = NULL; state; state = nextState)
    {
        nextState = state->next;
        bps_destroy(state);
    }

    free(b);
}

/**
* Function: bp_display
* Description: pretty-prints the current board.
*
* uses '|', '+' and '-' to "draw" the boundaries
* of the board.
*
* For full credit rows and columns are labeled with
* their indices. Column indices displayed "vertically"
*
* Examples:
*
* This board is full of balloons (like at the beginning of a game).
*
* +-----------------------+
* 0 | + + + = + ^ ^ o + = = |
* 1 | ^ = = o + o + + + ^ = |
* 2 | ^ + ^ o + o + = = = = |
* 3 | = ^ o o = o + + + = = |
* +-----------------------+
* 0 0 0 0 0 0 0 0 0 0 1
* 0 1 2 3 4 5 6 7 8 9 0
**
* This board has some empty slots represented by '.'
* +-----------------------+
* 0 | + + + = + ^ ^ o + = = |
* 1 | ^ = o o + o + + + ^ = |
* 2 | ^ + . o + . + = . . = |
* 3 | . . . o . . + + . . = |
* +-----------------------+
* 0 0 0 0 0 0 0 0 0 0 1
* 0 1 2 3 4 5 6 7 8 9 0
**
* Here is the same board using a space ' ' to
* represent an empty slot.
*
* +-----------------------+
* 0 | + + + = + ^ ^ o + = = |
* 1 | ^ = o o + o + + + ^ = |
* 2 | ^ + o + + = = |
* 3 | o + + = |
* +-----------------------+
* 0 0 0 0 0 0 0 0 0 0 1
* 0 1 2 3 4 5 6 7 8 9 0
*
*/
void bp_display(BPGame * b)
{
    // Draw the header
    printf("   +-");
    for (int j = 0; j < b->current->ncols; j++)
    {
        printf("--");
    }
    printf("+\n");

    for (int i = 0; i < b->current->nrows; i++)
    {
        printf("%2d ", i);
        printf("| ");

        for (int j = 0; j < b->current->ncols; j++)
        {
            printf("%c ", bps_get(b->current, i, j));
        }

        printf("| ");

        printf("\n");
    }

    // Draw the footer
    printf("   +-");
    for (int j = 0; j < b->current->ncols; j++)
    {
        printf("--");
    }
    printf("+\n");

    // Draw indexes below (tens digit)
    printf("     ");
    for (int j = 0; j < b->current->ncols; j++)
    {
        printf("%d ", j / 10);
    }
    printf("\n");

    // Draw indexes below (ones digit)
    printf("     ");
    for (int j = 0; j < b->current->ncols; j++)
    {
        printf("%d ", j % 10);
    }
    printf("\n");
}

void bp_display_STD(BPGame * b)
{
    for (int i = 0; i < b->current->nrows; i++)
    {
        for (int j = 0; j < b->current->ncols; j++)
        {
            printf("%c", bps_get(b->current, i, j));
        }

        printf("\n");
    }
}

/**
* Function: bp_pop
* Description: (attempts to) "pop" the balloon at (r,c) (if any)
* and its associated "color cluster"
*
* Cluster must be of AT LEASE SIZE 2 (a balloon that
* is its own cluster will not be popped).
*
* Returns: number of balloons popped (which may be zero).
*
* Side effects:
* locations of popped balloons become None (as in the
"color"
* None in the #define at the top of this file).
*
* adds n*(n-1) to score if n balloons were popped.
*
* Notes: does not require that the board be "compact". But
presumably,
* most client applicationw would only call on a compact board.
*
*/
int bp_pop(BPGame * b, int r, int c)
{
    // Check if point is a balloon
    // If it's not a balloon, nothing to pop.
    if (bps_get(b->current, r, c) == None)
    {
        return 0;
    }

    int n = 0;
    // Make a copy of the current state
    BPState * copy = bps_copy(b->current);

    // No pop if the copy state can't be created
    if (!copy)
        return 0;

    // Pop the balloons
    n = bps_pop(copy, r, c, bps_get(copy, r, c));

    // Check if there are enough balloons in the cluster
    if (n < 2)
    {
        bps_destroy(copy);
        return 0;
    }

    // Append the new state
    copy->next = b->current;
    b->current = copy;

    // Update the score
    copy->score += n * (n - 1);

    return n;
}

/**
* Function: bp_is_compact
* Description: determines if all balloons are as "high" as possible.
* Returns 1 if this is the case
* Returns 0 otherwise.
*
* Note: a balloon is as high as possible if and only
* if all slots in its column ABOVE it are also balloons
* (non-empty)
*
*/
int bp_is_compact(BPGame * b)
{
    for (int i = 1; i < b->current->nrows; i++)
    {
        for (int j = 0; j < b->current->ncols; j++)
        {
            // Check if balloon
            if (bps_get(b->current, i, j) != None)
            {
                // Check if the above
                if (bps_get(b->current, i - 1, j) == None)
                {
                    return 0;
                }
            }
        }
    }

    return 1;
}

/**
* Function: bp_float_one_step
* Description: moves all balloons that that are NOT AS HIGH AS
POSSIBLE
* up one spot.
*
* Example:
*
* before
*
* +-----------------------+
* 0 | + + + = + ^ + = = |
* 1 | ^ + o + + ^ = |
* 2 | ^ + o + + = = |
* 3 | o + + = = |
* +-----------------------+
* 0 0 0 0 0 0 0 0 0 0 1
* 0 1 2 3 4 5 6 7 8 9 0
*
* after
*
* +-----------------------+
* 0 | + + + = + ^ + + + = = |
* 1 | ^ + o + o + = ^ = |
* 2 | ^ o + + + = = |
* 3 | = |
* +-----------------------+
* 0 0 0 0 0 0 0 0 0 0 1
* 0 1 2 3 4 5 6 7 8 9 0
**
* Note: think animation by repeating this until it
* is compact (perhaps displaying each intermediate
* configuration).
*/
void bp_float_one_step(BPGame * b)
{
    for (int i = 1; i < b->current->nrows; i++)
    {
        for (int j = 0; j < b->current->ncols; j++)
        {
            // Check if balloon
            if (bps_get(b->current, i, j) != None)
            {
                // Check if the above
                if (bps_get(b->current, i - 1, j) == None)
                {
                    // Swap
                    bps_set(b->current, i - 1, j, bps_get(b->current, i, j));
                    bps_set(b->current, i, j, None);
                }
            }
        }
    }
}

/**
* Function: bp_score
* Description: returns the current score
*
*/
int bp_score(BPGame * b)
{
    return b->current->score;
}

/**
* Function: bp_get_balloon
* Description: returns color of balloon at given location as a char.
* if (r,c) is out of range, -1 is returned.
*
*/
int bp_get_balloon(BPGame * b, int r, int c)
{
    return bps_get(b->current, r, c);
}

/**
* Function: bp_can_pop
* Description: returns 1 if the player can pop some balloons;
* 0 otherwise.
*
* If the board is empty, of course nothing can
* be popped.
* If there are no adjacent balloons of the same
* color, there also are no moves.
*
* Notes: probable usage is to determine when game is over.
*/
int bp_can_pop(BPGame * b)
{
    for (int i = 0; i < b->current->nrows - 1; i++)
    {
        for (int j = 0; j < b->current->ncols - 1; j++)
        {
            // Check if this is a balloon
            if (bps_get(b->current, i, j) != None)
            {
                char balloon = bps_get(b->current, i, j);

                // Check if the sides are also balloon
                if (bps_get(b->current, i, j + 1) == balloon)
                    return 1;

                if (bps_get(b->current, i + 1, j) == balloon)
                    return 1;
            }
        }
    }

    // Board is unpopable.
    return 0;
}
/**
* Function: bp_undo
* Description: "undoes" most recent successful pop (if any),
* restoring the board to the exact state it was
* in immediately prior to that pop.
*
* Also restores score to corresponding previous value.
*
* Returns 1 if successful; 0 otherwise (i.e., when in
the initial state --
* no moves have been made to be undone).
*
* NOTE: bp_undo can be called repeatedly to back up all
the way to the beginning
* of the game.
*
*/
int bp_undo(BPGame * b)
{
    // Check if the board is in initial state.
    if (!b->current->next)
        return 0;

    // Get the current state
    BPState * curState = b->current;

    // Rewind to the previous state
    b->current = curState->next;

    // Delete the current state
    bps_destroy(curState);

    return 1;
}
