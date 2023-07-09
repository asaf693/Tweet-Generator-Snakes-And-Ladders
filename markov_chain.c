#include "markov_chain.h"
#include "string.h"
#include "stdbool.h"



int num_of_freq_nodes;
bool is_there_a_point = true;


Node* add_to_database(MarkovChain *markov_chain, void *data_ptr)
{
  void *new_data = markov_chain->copy_func(data_ptr);
  if (new_data == NULL){
    return NULL;
  }
  Node *current = markov_chain->database->first; // placing holder
  for (int i = 0; i < markov_chain->database->size; i++){
    if (markov_chain->comp_func(current->data->data, new_data) == 0) {
      free (new_data);
      return current;
    }
    current = current->next; // moving to the next node
  }
  // getting memory for a markov node
  MarkovNode *new_markov = malloc (sizeof (MarkovNode));
  if (new_markov == NULL){ // malloc check
    free (new_data);
    return NULL;
  }
  // getting memory for freq_list
  MarkovNodeFrequency *new_freq = malloc (sizeof (MarkovNodeFrequency));
  if (new_freq == NULL){ // malloc check
    free (new_data);
    free (new_markov);
    return NULL;
  }
  // assigning the entire markov node
  *new_markov = (MarkovNode) {new_data,
                              (MarkovNodeFrequency *) new_freq,
                              0};
  if (add (markov_chain->database, new_markov) != 0) { // add node
    free (new_data);
    free (new_markov);
    free (new_freq);
    return NULL;
  }
  return markov_chain->database->last;
}



Node* get_node_from_database(MarkovChain *markov_chain, void *data_ptr)
{
  Node *current_node = markov_chain->database->first; // placing holder
  for (int i = 0; i < markov_chain->database->size; i++){
    if (markov_chain->comp_func(current_node->data->data, data_ptr) == 0) {
      return current_node;
    }
    current_node = current_node->next; // moving to the next node
  }
  return NULL;
}


bool empty_list_add (MarkovNode *first_node, MarkovNodeFrequency current){
  first_node->frequencies_list = malloc (sizeof (MarkovNodeFrequency));
  if (first_node->frequencies_list == NULL){ // malloc check
    return false;
  }
  first_node->freq_list_len = 1; // updating freq_list len to 1
  first_node->frequencies_list[0] = current; // placing current node
  return true;
}


bool in_freq_list_check (MarkovNode *first_node,MarkovNode *second_node,
                         MarkovChain *markov_chain){
  for (int i = 0; i < (int) first_node->freq_list_len; i++) {
    if (markov_chain->comp_func
                (first_node->frequencies_list[i].MarkovNode->data,
                second_node->data) == 0) {
      first_node->frequencies_list[i].frequency++;
      return true;
    }
  }
  return false;
}

bool full_list_add (MarkovNode *first_node, MarkovNodeFrequency current){
  first_node->frequencies_list = realloc(first_node->frequencies_list,
                                         ((first_node->freq_list_len + 1))
                                         * sizeof (MarkovNodeFrequency));
  if (first_node->frequencies_list == NULL){ // realloc check
    return false;
  }
  first_node->freq_list_len ++; // increasing list len by 1
  first_node->frequencies_list[first_node->freq_list_len-1] = current;
  // placing current node
  return true;
}


bool add_node_to_frequencies_list(MarkovNode *first_node, MarkovNode
                        *second_node, MarkovChain *markov_chain)
{
  MarkovNodeFrequency current = {second_node, 1};
  if (first_node->frequencies_list == NULL){ // if list is empty
    return empty_list_add (first_node, current);
  }
  else { // if not empty - checking if node already in
    if (in_freq_list_check (first_node, second_node, markov_chain) == true){
      return true;
    }
    return full_list_add (first_node, current);
  }
}



void free_database(MarkovChain ** ptr_chain)
{
  if ((*ptr_chain)->database != NULL){
    Node *current_node = (*ptr_chain)->database->first; // placing holder
    if (current_node == NULL){
      free ((*ptr_chain)->database);
      free (*ptr_chain);
      return;
    }
    Node *next = current_node->next; // keeping the next node before freeing
    for (int i = 0; i < (*ptr_chain)->database->size; i++){
      if (current_node->data->frequencies_list != NULL){
        free (current_node->data->frequencies_list);
      }
      if (current_node->data->data != NULL){
        free (current_node->data->data);
      }
      if (current_node->data != NULL){
        free (current_node->data); // free markov node
      }
      free (current_node); // free node
      current_node = NULL;
      current_node = next; // moving to the next node
      if (i != (*ptr_chain)->database->size - 1){
        next = next->next;
      }
    }
    free ((*ptr_chain)->database);
    free (*ptr_chain);
  }
  else {
    free (*ptr_chain);
  }
}


int get_random_number (int max_number){
  return rand() % max_number;
}


MarkovNode* get_first_random_node(MarkovChain *markov_chain){
  is_there_a_point = true;
  while (is_there_a_point){
    Node *current_node = markov_chain->database->first;
    int num = get_random_number (markov_chain->database->size);
    for (int i = 0; i < num; ++i){
      current_node = current_node->next;
    }
    void *data_chosen = current_node->data->data;
    if (markov_chain->is_last (data_chosen) != true){
      is_there_a_point = false;
      return current_node->data;
    }
  }
  return NULL;
}


int get_num_of_freq_nodes(MarkovNode *state_struct_ptr){
  num_of_freq_nodes = 0;
  MarkovNodeFrequency *current_freq_node = state_struct_ptr->frequencies_list;
  int num = state_struct_ptr->freq_list_len;
  for (int i = 0; i < num; ++i){
    for (int j = 0; j < (int) current_freq_node->frequency; ++j){
      num_of_freq_nodes++;
    }
    current_freq_node = current_freq_node+1;
  }
  return num_of_freq_nodes;
}


MarkovNode* get_next_random_node(MarkovNode *state_struct_ptr){
  MarkovNodeFrequency *current_freq_node = state_struct_ptr->frequencies_list;
  int markov_num_of_freq_nodes = get_num_of_freq_nodes (state_struct_ptr);
  int num = get_random_number (markov_num_of_freq_nodes);
  int i = 0;
  while (i < num){
    for (int j = 0; j < (int) current_freq_node->frequency; ++j){
      i++;
      if (i == (num+1)){
        return current_freq_node->MarkovNode;
      }
    }
    current_freq_node = current_freq_node+1;
  }
  return current_freq_node->MarkovNode;
}


void generate_tweet(MarkovChain *markov_chain, MarkovNode *first_node,
                    int max_length){
  MarkovNode *current_node = first_node;
  if (first_node == NULL){
    current_node = get_first_random_node (markov_chain);
  }
  for (int i = 0; i < max_length; ++i){
    if (markov_chain->is_last (current_node->data) == true || i ==
                                                              max_length - 1){
      markov_chain->print_func(current_node);
      printf ("\n");
      return;
    }
    markov_chain->print_func(current_node);
    current_node = get_next_random_node (current_node);
  }
}

