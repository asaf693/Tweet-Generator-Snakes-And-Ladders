#ifndef _MARKOV_CHAIN_H
#define _MARKOV_CHAIN_H

#include "linked_list.h"
#include <stdio.h>  // For printf(), sscanf()
#include <stdlib.h> // For exit(), malloc()
#include <stdbool.h> // for bool

#define ALLOCATION_ERROR_MASSAGE \
"Allocation failure: Failed to allocate new memory\n"


/***************************/
/*   insert typedefs here  */
/***************************/


/***************************/


/***************************/
/*        STRUCTS          */
/***************************/


typedef struct MarkovNodeFrequency {
    struct MarkovNode *MarkovNode;
    unsigned int frequency;
} MarkovNodeFrequency;

typedef struct MarkovNode {
    void *data;
    MarkovNodeFrequency * frequencies_list;
    unsigned int freq_list_len;
} MarkovNode;

/* DO NOT ADD or CHANGE variable names in this struct */
typedef struct MarkovChain {
    LinkedList *database;

    // pointer to a func that receives data from a generic type and prints it
    // returns void.
    void (*print_func)(void*);

    // pointer to a func that gets 2 pointers of generic data type(same one)
    // and compare between them */
    // returns: - a positive value if the first is bigger
    //          - a negative value if the second is bigger
    //          - 0 if equal
    int (*comp_func)(void*, void*);

    // a pointer to a function that gets a pointer of generic
    // data type and frees it.
    // returns void.
    void (*free_data)(void*);

    // a pointer to a function that gets a pointer of generic data type
    // and returns a newly allocated copy of it
    // returns a generic pointer.
    void* (*copy_func)(void*);

    //  a pointer to function that gets a pointer of generic data type
    //  and returns:
    //      - true if it's the last state.
    //      - false otherwise.
    bool (*is_last)(void*);
} MarkovChain;

/**
* If data_ptr in markov_chain, return it's node. Otherwise, create new
 * node, add to end of markov_chain's database and return it.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return markov_node wrapping given data_ptr in given chain's database,
 * returns NULL in case of memory allocation failure.
 */
Node* add_to_database(MarkovChain *markov_chain, void *data_ptr);

/**
* Check if data_ptr is in database. If so, return the markov_node wrapping it
 * the markov_chain, otherwise return NULL.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return Pointer to the Node wrapping given state, NULL if state not in
 * database.
 */
Node* get_node_from_database(MarkovChain *markov_chain, void *data_ptr);

/**
 * Add the second markov_node to the frequency list of the first markov_node.
 * If already in list, update it's occurrence frequency value.
 * @param first_node
 * @param second_node
 * @return success/failure: true if the process was successful, false if in
 * case of allocation error.
 */
bool add_node_to_frequencies_list(MarkovNode *first_node, MarkovNode
                  *second_node, MarkovChain *markov_chain);

/**
 * Free markov_chain and all of it's content from memory
 * @param markov_chain markov_chain to free
 */
void free_database(MarkovChain ** ptr_chain);

// generates a random number between 1 and max number.
int get_random_number (int max_number);

// mallocs and then adds to first node's freq list as it's first node
bool empty_list_add (MarkovNode *first_node, MarkovNodeFrequency current);

// reallocs and then adds to first node's freq list
bool full_list_add (MarkovNode *first_node, MarkovNodeFrequency current);

// checks if seconds node is in first node's freq list
bool in_freq_list_check (MarkovNode *first_node,MarkovNode *second_node,
                         MarkovChain *markov_chain);

// gets number of freq words inside markov node's freq list (including
// multiple frequencies)
int get_num_of_freq_nodes(MarkovNode *state_struct_ptr);

/**
 * Get one random state from the given markov_chain's database.
 * @param markov_chain
 * @return
 */
MarkovNode* get_first_random_node(MarkovChain *markov_chain);

/**
 * Choose randomly the next state, depend on it's occurrence frequency.
 * @param state_struct_ptr MarkovNode to choose from
 * @return MarkovNode of the chosen state
 */
MarkovNode* get_next_random_node(MarkovNode *state_struct_ptr);

/**
 * Receive markov_chain, generate and print random sentence out of it. The
 * sentence most have at least 2 words in it.
 * @param markov_chain
 * @param first_node markov_node to start with,
 *                   if NULL- choose a random markov_node
 * @param  max_length maximum length of chain to generate
 */
void generate_tweet(MarkovChain *markov_chain, MarkovNode *
first_node, int max_length);


#endif /* _MARKOV_CHAIN_H */