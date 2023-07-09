#include <string.h> // For strlen(), strcmp(), strcpy()
#include "markov_chain.h"

#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))
#define EMPTY (-1)
#define BOARD_SIZE 100
#define MAX_GENERATION_LENGTH 60
#define NUM_OF_ARGS 3
#define BASE 10
#define DICE_MAX 6
#define NUM_OF_TRANSITIONS 20

MarkovChain *markov_ch = NULL;
LinkedList *linked_list = NULL;

/**
 * represents the transitions by ladders and snakes in the game
 * each tuple (x,y) represents a ladder from x to if x<y or a snake otherwise
 */
const int transitions[][2] = {{13, 4},
                              {85, 17},
                              {95, 67},
                              {97, 58},
                              {66, 89},
                              {87, 31},
                              {57, 83},
                              {91, 25},
                              {28, 50},
                              {35, 11},
                              {8,  30},
                              {41, 62},
                              {81, 43},
                              {69, 32},
                              {20, 39},
                              {33, 70},
                              {79, 99},
                              {23, 76},
                              {15, 47},
                              {61, 14}};

/**
 * struct represents a Cell in the game board
 */
typedef struct Cell {
    int number; // Cell number 1-100
    int ladder_to;  // ladder_to represents the jump of the ladder
    int snake_to;  // snake_to represents the jump of the snake
    //both ladder_to and snake_to should be -1 if the Cell doesn't have them
} Cell;



///***************************/
///*   generic funcs  */
///***************************/

static int compare_int (void* data1, void* data2){
  Cell *cell1 = (Cell*) data1;
  Cell *cell2 = (Cell*) data2;
  if (cell1->number > cell2->number){
    return 1;
  }
  if (cell1->number < cell2->number){
    return -1;
  }
  return 0;
}



static bool checking_100 (void* data){
  Cell *cur_cell = (Cell *) data;
  if (cur_cell->number == BOARD_SIZE){
    return true;
  }
  return false;
}



static void print_single_step (void* data){
  MarkovNode *current = (MarkovNode *) data;
  Cell *cell = (Cell*) current->data;
  if ((int) cell->ladder_to != EMPTY){
    printf ("[%d]-ladder to %d -> ", (int) cell->number, (int)
    cell->ladder_to);
    return;
  }
  if ((int) cell->snake_to != EMPTY){
    printf ("[%d]-snake to %d -> ",
            (int) cell->number, (int) cell->snake_to);
    return;
  }
  if ((int) cell->number == BOARD_SIZE){
    printf ("[%d]", (int) cell->number);
    return;
  }
  printf ("[%d] -> ", (int) cell->number);
}



static void* cell_copy (void* data){
  Cell *new_cell = malloc (sizeof (Cell));
  if (new_cell == NULL){
    return NULL;
  }
  *new_cell = *(Cell*) data;
  return new_cell;
}



///***************************/
///*   non generic funcs  */
///***************************/


/** Error handler **/
static int handle_error(char *error_msg, MarkovChain **database)
{
  printf("%s", error_msg);
  if (database != NULL)
  {
    free_database(database);
  }
  return EXIT_FAILURE;
}



static int initialize(){
  linked_list = malloc (sizeof (LinkedList));
  if (linked_list == NULL){
    printf ("Allocation failure: linked_list malloc failed.\n");
    return EXIT_FAILURE;
  }
  linked_list->first = NULL;
  linked_list->last = NULL;
  linked_list->size = 0;
  markov_ch = malloc (sizeof (MarkovChain));
  if (markov_ch == NULL){
    printf ("Allocation failure: markov_chain malloc failed.\n");
    free (linked_list);
    return EXIT_FAILURE;
  }
  markov_ch->comp_func = (int (*) (void *, void *)) compare_int;
  markov_ch->free_data = (void (*) (void *)) free_database;
  markov_ch->print_func = (void (*) (void *)) print_single_step;
  markov_ch->is_last = (bool (*) (void *)) checking_100;
  markov_ch->copy_func = cell_copy;
  markov_ch->database = linked_list;
  return EXIT_SUCCESS;
}



static int args_check (int argc){
  if (argc != NUM_OF_ARGS){
    printf ("Usage: Number of args is wrong.\n");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}



static int create_board(Cell *cells[BOARD_SIZE])
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        cells[i] = malloc(sizeof(Cell));
        if (cells[i] == NULL)
        {
            for (int j = 0; j < i; j++) {
                free(cells[j]);
            }
            handle_error(ALLOCATION_ERROR_MASSAGE,NULL);
            return EXIT_FAILURE;
        }
        *(cells[i]) = (Cell) {i + 1, EMPTY, EMPTY};
    }

    for (int i = 0; i < NUM_OF_TRANSITIONS; i++)
    {
        int from = transitions[i][0];
        int to = transitions[i][1];
        if (from < to)
        {
            cells[from - 1]->ladder_to = to;
        }
        else
        {
            cells[from - 1]->snake_to = to;
        }
    }
    return EXIT_SUCCESS;
}



/**
 * fills database
 * @param markov_chain
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
static int fill_database(MarkovChain *markov_chain)
{
  Cell *cells[BOARD_SIZE];
  if (create_board (cells) == EXIT_FAILURE)
  {
    return EXIT_FAILURE;
  }
  MarkovNode *from_node = NULL, *to_node = NULL;
  size_t index_to;
  for (size_t i = 0; i < BOARD_SIZE; i++)
  {
    add_to_database (markov_chain, cells[i]);
  }

  for (size_t i = 0; i < BOARD_SIZE; i++)
  {
    from_node = get_node_from_database (markov_chain, cells[i])->data;

    if (cells[i]->snake_to != EMPTY || cells[i]->ladder_to != EMPTY)
    {
      index_to = MAX(cells[i]->snake_to, cells[i]->ladder_to) - 1;
      to_node = get_node_from_database (markov_chain, cells[index_to])
          ->data;
      add_node_to_frequencies_list (from_node, to_node, markov_chain);
    }
    else
    {
      for (int j = 1; j <= DICE_MAX; j++)
      {
        index_to = ((Cell *) (from_node->data))->number + j - 1;
        if (index_to >= BOARD_SIZE)
        {
          break;
        }
        to_node = get_node_from_database (markov_chain, cells[index_to])
            ->data;
        add_node_to_frequencies_list (from_node, to_node, markov_chain);
      }
    }
  }
  // free temp arr
  for (size_t i = 0; i < BOARD_SIZE; i++)
  {
    free (cells[i]);
  }
  return EXIT_SUCCESS;
}



static void path_printer (int num_of_paths){
  int current_num = 0;
  for (int i = 0; i < num_of_paths; ++i){
    current_num++;
    MarkovNode *first_node = markov_ch->database->first->data;
    printf ("Random Walk %d: ", current_num);
    generate_tweet (markov_ch, first_node,MAX_GENERATION_LENGTH);
  }
}



/**
 * @param argc num of arguments
 * @param argv 1) Seed
 *             2) Number of sentences to generate
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int main(int argc, char *argv[])
{
  if (args_check (argc) == EXIT_FAILURE){
    return EXIT_FAILURE;
  }
  unsigned int seed = (int) strtol (argv[1], NULL, BASE);
  int num_of_paths = (int) strtol (argv[2], NULL, BASE);
  srand(seed);
  if (initialize() == EXIT_FAILURE){
    return EXIT_FAILURE;
  }
  if (fill_database (markov_ch) == EXIT_FAILURE){
    markov_ch->free_data (&markov_ch);
    return EXIT_FAILURE;
  }
  path_printer(num_of_paths);
  markov_ch->free_data (&markov_ch);
  return EXIT_SUCCESS;
}
