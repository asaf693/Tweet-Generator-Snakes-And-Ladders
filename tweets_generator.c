#include <stdio.h>
#include "stdlib.h"
#include "string.h"
#include "markov_chain.h"


#define BASE 10
#define MAX_TWEET_LEN 20
#define MAX_SENTENCE_LEN 1001
#define MAX_WORD_LEN 101
#define MIN_NUM_OF_ARGS 4
#define MAX_NUM_OF_ARGS 5



int words_counter = 0;
char sentence[MAX_SENTENCE_LEN];
char word_before[MAX_WORD_LEN] = "empty"; // last word used
char current_word[MAX_WORD_LEN]; // current word used
int word_len;
int num_words_to_read = 0;
MarkovChain *markov_chain = NULL;
LinkedList *linked_l = NULL;



/***************************/
/*   generic funcs  */
/***************************/

static void print_char (void* data){
  MarkovNode *current = (MarkovNode*) data;
  printf (" %s", (char*) current->data);
}

static bool checking_point (void* data){
  char *cur_word = (char*) data;
  int len = (int) strlen (cur_word);
  if (cur_word[len-1] == '.'){
    return true;
  }
  return false;
}

static void* char_copy (void* data){
  char *new_word = malloc (sizeof (char)*((int) strlen (data)+1));
  if (new_word == NULL){
    return NULL;
  }
  strcpy (new_word, current_word); // copies current word to new
  return new_word;
}

/***************************/
/*   non generic funcs  */
/***************************/


static int initialize(){
  linked_l = malloc (sizeof (LinkedList));
  if (linked_l == NULL){
    printf ("Allocation failure: malloc failed.\n");
    return EXIT_FAILURE;
  }
  linked_l->first = NULL;
  linked_l->last = NULL;
  linked_l->size = 0;
  markov_chain = malloc (sizeof (MarkovChain));
  if (markov_chain == NULL){
    printf ("Allocation failure: malloc failed.\n");
    free (linked_l);
    return EXIT_FAILURE;
  }
  markov_chain->comp_func = (int (*) (void *, void *)) strcmp;
  markov_chain->free_data = (void (*) (void *)) free_database;
  markov_chain->print_func = (void (*) (void *)) print_char;
  markov_chain->is_last = (bool (*) (void *)) checking_point;
  markov_chain->copy_func = char_copy;
  markov_chain->database = linked_l;
  return EXIT_SUCCESS;
}

static int add_to_freq(MarkovChain *markov_ch){
  MarkovNode *current_markov = get_node_from_database
      (markov_ch, current_word)->data;
  MarkovNode *last_markov = get_node_from_database
      (markov_ch, word_before)->data;
  if (add_node_to_frequencies_list
  (last_markov, current_markov, markov_ch) == false)
  {
    markov_ch->free_data (&markov_ch);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}



static int fill_database(FILE *fp, int words_to_read, MarkovChain *markov_ch){
  while (fgets (sentence, MAX_SENTENCE_LEN, fp)){ // get sentence
    char *buffer = sentence; // fill buffer
    while (sscanf (buffer,"%s%n", current_word, &word_len) == 1){//stops at '_'
      char new_word[MAX_WORD_LEN];
      strcpy (new_word, current_word);
      buffer = (buffer + word_len);
      if (add_to_database (markov_ch, new_word) == NULL){
        markov_ch->free_data (&markov_ch);
        return EXIT_FAILURE;
      }
      if (markov_ch->comp_func (word_before, "empty") == 0) { // if word=empty
        strcpy (word_before, current_word);
        if (markov_ch->is_last (current_word) == true){//check . at end of word
          strcpy (word_before, "empty"); // resetting the word before
        }
        words_counter++;
        if (words_counter == words_to_read){
          return EXIT_SUCCESS;
        }
        continue;
      }
      if (add_to_freq (markov_ch) == EXIT_FAILURE){
        return EXIT_FAILURE;
      }
      strcpy (word_before, current_word);
      if (markov_ch->is_last (current_word) == true){ // check . at end of word
        strcpy (word_before, "empty"); // resetting the word before
      }
      words_counter++;
      if (words_counter == words_to_read){
        return EXIT_SUCCESS;
      }
    }
  }
  return EXIT_SUCCESS;
}


static int arg_check (int argc){
  if (argc != MIN_NUM_OF_ARGS && argc != MAX_NUM_OF_ARGS){
    printf ("Usage: Number of args is wrong.\n");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}


static void tweet_printer (int num_of_tweets){
  int current_num = 0;
  for (int i = 0; i < num_of_tweets; ++i){
    current_num++;
    MarkovNode *first_node = get_first_random_node (markov_chain);
    printf ("Tweet %d:", current_num);
    generate_tweet (markov_chain, first_node, MAX_TWEET_LEN);
  }
}

int main (int argc, char ** argv)
{
  if (arg_check (argc) == EXIT_FAILURE){
    return EXIT_FAILURE;
  }
  unsigned int seed = (int) strtol (argv[1], NULL, BASE);
  int num_of_tweets = (int) strtol (argv[2], NULL, BASE);
  srand(seed);
  FILE *in_file = fopen (argv[3], "r");
  if (in_file == NULL){
    printf ("Error: The given file is invalid.\n");
    return EXIT_FAILURE;
  }
  if (argc == MAX_NUM_OF_ARGS){
    num_words_to_read = (int) strtol (argv[4], NULL, BASE);
  }
  if (initialize() == EXIT_FAILURE){
    return EXIT_FAILURE;
  }
  if (fill_database(in_file, num_words_to_read,
                    markov_chain) == EXIT_FAILURE){
    printf ("Allocation failure: malloc/realloc failed.\n");
    return EXIT_FAILURE;
  }
  tweet_printer (num_of_tweets);
  markov_chain->free_data (&markov_chain);
  fclose (in_file);
  return EXIT_SUCCESS;
}

