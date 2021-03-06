// Welcome to my assingment. I have attempted stages 1 through to 4
// Assignment 2 19T3 COMP1511: Pokedex
// pokedex.c
// This program was written by Joe Vu Nguyen z5120839
// on 24/11/2019


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "pokedex.h"


struct pokedex {
    struct pokenode *head;
    struct pokenode *start;
    int size;
};

struct pokenode {
    Pokemon pokemon;
    int found; 
    int firstFound ;
    struct pokenode *evolve;
    struct pokenode *next;
    struct pokenode *prev;
};

/* 
Finds the length of the given string. 
Input: String
Output: Length of string
*/
int string_length(char* inp){
    int i = 0;
    while (inp[i] != '\0'){
        i++;
    }
    return i;
}

/* 
Printing Stars for all the non-found Pokemon according to the length of the input string. 
Input: String
Output: prints stars as per the length of the string. 
*/
void print_stars(char* name){

    int len = string_length(name);
    int i = 0;
    
    while (i< len){
        printf("*");
        i ++;
    }
    printf("\n");
}

void print_ques(char* name){

    int len = string_length(name);
    int i = 0;
    
    while (i< len){
        printf("?");
        i ++;
    }

}

/* 
Print the poketypes depending on the pokemon_type number. 
Input: Types array, len.
Output: Prints the pokemons as per the input array. 
*/
void printPokeTypes(int types[], int len){
    int i,j,k;
    const char* pokeType[] = {"NONE_TYPE",
    "Normal",
    "Fire",
    "Fighting",
    "Water",
    "Flying",
    "Grass",
    "Poison",
    "Electric",
    "Ground",
    "Psychic",
    "Rock",
    "Ice",
    "Bug",
    "Dragon",
    "Ghost",
    "Dark",
    "Steel",
    "Fairy",
    };
    
    for (i = 0;i< len; i++){
        for (j = i+ 1;j < len; ){
            if (types[j] == types[i]){
                for (k = j; k<len;k++){
                    types[k] =types[k+1];
                } 
                len--;
            }
            else{
                j++;
            }
        }
    }
    
    for(i = 0; i <len; i++){
        if (types[i] != 0){
           printf("%s\n", pokeType[types[i]]); 
        }
        
    }
}

//Formats numbers with a hastag and with 3 numbers
//e.g 1 -> #001, 20 -> #020, 300 -> #300
int number_format(int number){
    if (number <10){
        printf("#00");
        }    
    
    else if (number < 100){
        printf("#0");
    }
    
    else{
        printf("#");
    }
    return number;
}

//Same formatting above just wihout the #
int number_format_details(int number){
    if (number <10){
        printf("00");
        }    
    
    else if (number < 100){
        printf("0");
    }
    
    return number;
}

// Creates a new pokenode struct and returns a pointer to it.
static struct pokenode *new_pokenode(Pokemon pokemon);

// Creates a new Pokedex, and returns a pointer to it.
// Note: you will need to modify this function.
Pokedex new_pokedex(void) {

    // Malloc memory for a new Pokedex, and check that the memory
    // allocation succeeded.   
    Pokedex pokedex = malloc(sizeof(struct pokedex));
    assert(pokedex != NULL);

    // Set the head of the linked list to be NULL.
    // (i.e. set the Pokedex to be empty)
    pokedex->head = NULL; // This is the currently selected pokemon. 
    pokedex -> start = NULL; //Mainains the pointer to the start of the pokedex.
    pokedex->size = 0; // The size of the pokedex.

    return pokedex;
}

// Create a new pokenode struct and returns a pointer to it.
// This function allocates memory for a pokenode, sets its pokemon
// pointer to point to the specified pokemon, and returns a pointer to
// the allocated memory.
static struct pokenode *new_pokenode(Pokemon pokemon) {
   struct pokenode *p;   
   p =  malloc(sizeof(struct pokenode));
   p -> pokemon = pokemon;
   p -> found = 0; //If found then 1 otherwise 0-.
   p -> firstFound = 0;  //This is used for exploring. 
   p -> next = NULL; // Next pokemon.
   p -> prev = NULL;  //Prev pokemon.
   p -> evolve = NULL; //To maintain a pointer to the next evo.
   return p;

    
}

//////////////////////////////////////////////////////////////////////
//                         Stage 1 Functions                        //
//////////////////////////////////////////////////////////////////////

// Add a new Pokemon to the Pokedex.
void add_pokemon(Pokedex pokedex, Pokemon pokemon) {
       
    struct pokenode *node;
    //If the size is 0 then the added pokemon should be the start pokemon and also the selected pokemon (head in my struct). 
    if (pokedex->size == 0){
        node  = new_pokenode(pokemon);
        pokedex-> size ++;
        pokedex -> head = node;
        pokedex -> start = node;
        node -> prev = NULL;
    }
    else{
    //If pokemons have been already added to the pokedex then loop till the last pokemon and add the new pokemon to its next pointer.
        node = new_pokenode(pokemon);
        struct pokenode *h;
        h = pokedex -> start;
        int i = 1;
        //Looping through to the last pokemon. 
        while (i < pokedex->size){
            h = h -> next; 
            i ++;
        }
        //Adding the pokemon.
        h -> next = node;
        node -> prev = h;
        pokedex -> size++;
    }

}

// Print out the details of the currently selected Pokemon.
void detail_pokemon(Pokedex pokedex) {

    // The poketypes is an array and takes in the poketupe in int 
    // and then prints the pokemon type in string format. 
    char* pokeType[] = {"NONE_TYPE",
    "Normal",
    "Fire",
    "Fighting",
    "Water",
    "Flying",
    "Grass",
    "Poison",
    "Electric",
    "Ground",
    "Psychic",
    "Rock",
    "Ice",
    "Bug",
    "Dragon",
    "Ghost",
    "Dark",
    "Steel",
    "Fairy",
    };
    struct pokenode *node;
    node = pokedex -> head;
    
    // If the pokemon in not yet been found then then dust print dashes.
    if (node -> found == 0){
        printf("ID: ") ;
        printf("%d\n", number_format_details(pokemon_id(node -> pokemon)));
        printf("Name: "); 
        print_stars(pokemon_name(node -> pokemon));
        printf("Height: --\n");
        printf("Weight: --\n");
        printf("Type: --\n");
    }
    
    // If the pokemon is found then print the names 
    // and use the pokeType to get the pokemon type in string. 
    else{
        printf("ID: ") ;
        printf("%d\n", number_format_details(pokemon_id(node -> pokemon)));
        printf("Name: %s\n", pokemon_name(node -> pokemon));
        printf("Height: %.1fm\n", pokemon_height(node -> pokemon));
        printf("Weight: %.1fkg\n", pokemon_weight(node -> pokemon));
        printf("Type: %s ", pokeType[pokemon_first_type(node -> pokemon)]) ;
        if (pokemon_second_type(node -> pokemon) != 0){
           printf("%s\n", pokeType[pokemon_second_type(node -> pokemon)]); 
        }
        else{
            printf("\n");
        }
        
    }

}

// Return the currently selected Pokemon.
Pokemon get_current_pokemon(Pokedex pokedex) {
    if (pokedex -> size == 0){
        printf("NO pokemon!!");
        exit(1);
    }
    
    //Returns the head of the pokemon if the pokedex is not empty.
    return pokedex -> head-> pokemon;
}

// Sets the currently selected Pokemon to be 'found'.
void find_current_pokemon(Pokedex pokedex) {
    pokedex -> head -> found = 1;
}

// Print out all of the Pokemon in the Pokedex.
void print_pokemon(Pokedex pokedex) {
    
    struct pokenode *node;
    node = pokedex -> start;
    struct pokenode *curr;
    curr = pokedex -> head;
    int len = pokedex -> size;
    int i = 0;
    

    // Looping through all the nodes in the pokedex. 
    while (i < len){

        // If the pokemon is the head we need to print '--> '
        if (pokemon_id(node -> pokemon) == pokemon_id(curr -> pokemon)){

            // Check if the pokemon is found or not and and depending on 
            // print out * or the real pokemon name. 
            if(node -> found == 0){
                printf("--> ");
                printf("%d: ", number_format(pokemon_id(node -> pokemon)));
                print_stars(pokemon_name(node -> pokemon));
            }
            else{
                printf("--> ");
                printf("%d: %s\n", number_format(pokemon_id(node -> pokemon)), pokemon_name(node -> pokemon));
            }
        }
        
        // If not the selected pokemon then just print as it is. 
        else{
            if(node -> found == 0){
                printf("    ");
                printf("%d: ", number_format(pokemon_id(node -> pokemon)));
                print_stars(pokemon_name(node -> pokemon));
            }
            else{
                printf("    ");
                printf("%d: %s\n", number_format(pokemon_id(node -> pokemon)), pokemon_name(node -> pokemon));
            }
        }
        node = node -> next;
        i ++;
    }
    
    
   
}

//////////////////////////////////////////////////////////////////////
//                         Stage 2 Functions                        //
//////////////////////////////////////////////////////////////////////

// Change the currently selected Pokemon to be the next Pokemon in the Pokedex.
void next_pokemon(Pokedex pokedex) {

    // Go to the next pokemon in the pokedex, If next is Null then dont so anything. 
    if (pokedex -> size != 0){
        struct pokenode *node;
        node = pokedex -> head;

        if (node -> next != NULL){
            pokedex -> head = node -> next;
     
        }
    }
    
    
    
}

// Change the currently selected Pokemon to be the previous Pokemon in the Pokedex.
void prev_pokemon(Pokedex pokedex) {
    if (pokedex -> size != 0){
    
        struct pokenode *curr;
        struct pokenode *start;
        curr = pokedex -> head;
        start = pokedex-> start;
        
        if (pokemon_id(start->pokemon) != pokemon_id(curr -> pokemon)){
            pokedex -> head = curr -> prev;
        }
    }
    
}

// Change the currently selected Pokemon to be the Pokemon with the ID `id`.
void change_current_pokemon(Pokedex pokedex, int id) {
    struct pokenode *node;
    node = pokedex-> start;
    int len = pokedex -> size;
    int i = 0;
    
    // Loop through the linked list and if there is any pokemon 
    // with the specified id just return that otherwise dont return anything.
    while ( i < len){
        if (pokemon_id(node -> pokemon) == id){
            pokedex -> head = node;
        }
        node = node -> next;
        i ++;
    }
}

// Remove the currently selected Pokemon from the Pokedex.
void remove_pokemon(Pokedex pokedex) {

    struct pokenode *curr;
    struct pokenode *prv;

    curr = pokedex -> head;
    int len = pokedex -> size;
    
    if (len == 1){
        destroy_pokemon(pokedex-> head->pokemon);
        free(pokedex->head);
        pokedex-> size--;
    }
    
    
    // If there are no pokemons after the current one. 
    else if (curr -> next == NULL){
        curr = curr -> prev;
        destroy_pokemon(curr -> next -> pokemon);
        free(curr -> next);
        pokedex -> head = curr;
        pokedex -> size --;
        
    }
    
    else{
        // Checking if the selected is the start of the pokedex
        if (pokemon_id(pokedex -> head->pokemon) == pokemon_id(pokedex -> start->pokemon)){
            pokedex -> head = pokedex -> head -> next;
            pokedex -> start = pokedex -> start -> next;

            destroy_pokemon(pokedex -> head -> prev->pokemon);
            free(pokedex -> head -> prev);
            pokedex -> size-- ;
        }
        
        else{
            curr = pokedex -> head -> next;
            prv = pokedex -> head -> prev;
            curr -> prev = prv;
            prv -> next = curr;
            destroy_pokemon(pokedex -> head->pokemon);
            free(pokedex -> head);
            pokedex->head = curr;
            pokedex -> size--;
            
        }
        
    }
}

// Destroy the given Pokedex and free all associated memory.
void destroy_pokedex(Pokedex pokedex) {
    
    // Looping through all the nodes and then free them one by one. 
    int len = pokedex -> size;

    // If no pokemon in the pokedex, 
    if (len == 0){
        free(pokedex);
    }

    // If only one pokemon in the pokedex. 
    else if (len == 1){   
        destroy_pokemon(pokedex ->  head->pokemon);
        free(pokedex ->  head);        
        free(pokedex);
    }
    
    // If there are several pokemon in the pokedex. 
    else{
        struct pokenode *curr;
        curr = pokedex -> start;
        while(curr->next != NULL){
            curr = curr -> next;
            destroy_pokemon(curr -> prev -> pokemon);
            free(curr -> prev);

        }
        if (curr != NULL){
            destroy_pokemon(curr -> pokemon);
        }
        free(curr);
        free(pokedex);
    }

}

//////////////////////////////////////////////////////////////////////
//                         Stage 3 Functions                        //
//////////////////////////////////////////////////////////////////////

// Print out all of the different types of Pokemon in the Pokedex.
void show_types(Pokedex pokedex) {
    struct pokenode *node;
    node = pokedex -> start;
    int len = pokedex -> size;
    int i = 0;
    int seen_types[1000];
    int j = 0;
    while (i < len){
        //Adding all the seen type in the seen types list.
        seen_types[j] = pokemon_first_type(node -> pokemon); 
        j++;
        seen_types[j] = pokemon_second_type(node-> pokemon);
        j++;
        i += 1;
        node = node -> next;    
    }
    // Function to print the pokemon type depending on the number. 
    printPokeTypes(seen_types, j);
}

// Set the first not-yet-found Pokemon of each type to be found.
void go_exploring(Pokedex pokedex) {
    struct pokenode *node;
    node = pokedex -> start;
    int len = pokedex -> size;

    int iter = 0;

    
    
    int type_1, type_2;
    int seen[19] = {0}; //maintaining a seen array which works like a counter, if the type has been seen or not. 
    
    while (iter < len){

        // Check if the pokemon has been found yet.
        type_1 = pokemon_first_type(node -> pokemon);
        type_2 = pokemon_second_type(node -> pokemon);
        
        // Checks if it has been found for the first time.
        if (seen[type_1] < 100){
            seen[type_1] += 100;
            node -> found = 1;
        }
        
        if (seen[type_2] < 100){
            seen[type_2] += 100;
            node->found = 1;
        }    
        iter ++;
        node = node -> next;
    }


}

// Return the total number of Pokemon in the Pokedex.
int count_total_pokemon(Pokedex pokedex) {
    return(pokedex->size);
}

// Return the number of Pokemon in the Pokedex that have been found.
int count_found_pokemon(Pokedex pokedex) {
    struct pokenode *node;
    node = pokedex -> start;
    int len = pokedex -> size;

    int iter = 0;
    int count = 0;
    
    // Counting the number of found pokemon.
    while (iter < len){
        // If found then adding 1 to the npde.
        if (node -> found == 1){
            count ++;
        }
        iter ++;
        node = node -> next;
    }
    return (count);
    
}

//////////////////////////////////////////////////////////////////////
//                         Stage 4 Functions                        //
//////////////////////////////////////////////////////////////////////

// Add the information that the Pokemon with the ID `from_id` can
// evolve into the Pokemon with the ID `to_id`.
void add_pokemon_evolution(Pokedex pokedex, int from_id, int to_id) {
    struct pokenode *node;
    node = pokedex -> start;
    int len = pokedex -> size;
    int iter = 0;
    int flag_1 = 0;
    int flag_2 = 0;
    
    // Checking if both are same.   
    if (from_id == to_id){
        printf("Both ids are same!\n");
        exit(1);
    }
    //Checking if from id exists
    while (iter < len){
         if (from_id == pokemon_id(node -> pokemon)){
            flag_1 = 1;
         }
         iter++;
         node = node -> next;
    }    
    if (flag_1 == 0){
        printf("The pokemon you're trying to evolve does not exist!\n");
        exit(1);
    }
    //resetting iterators
    node = pokedex -> start;
    iter = 0;
    
    //Checking if to id exists
    while (iter < len){
         if (to_id == pokemon_id(node -> pokemon)){
            flag_2 = 1;
         }
         iter++;
         node = node -> next;
    }    
    if (flag_2 == 0){
        printf("The pokemon you're trying to evolve to does not exist!\n");
        exit(1);
    }
    
    //Getting the evolution nodes for the pokemons. 
    struct pokenode *from_node;
    struct pokenode *to_node;
    from_node = NULL;
    to_node = NULL;
    node = pokedex -> start;
    iter = 0;
    while (iter < len){
         if (to_id == pokemon_id(node -> pokemon)){
            to_node = node;
         }
         if (from_id == pokemon_id(node -> pokemon)){
            from_node = node;
         }
         iter++;
         node = node -> next;
    }  
    if (from_node != NULL && to_node != NULL){
        from_node -> evolve = to_node;

    }

}

// Show the evolutions of the currently selected Pokemon.
void show_evolutions(Pokedex pokedex) {
    char* pokeType[] = {"NONE_TYPE",
    "Normal",
    "Fire",
    "Fighting",
    "Water",
    "Flying",
    "Grass",
    "Poison",
    "Electric",
    "Ground",
    "Psychic",
    "Rock",
    "Ice",
    "Bug",
    "Dragon",
    "Ghost",
    "Dark",
    "Steel",
    "Fairy",
    };
    
    struct pokenode *node;
    node = pokedex -> head;
    //If no evolutions
    if (node -> evolve == NULL){
        if (node -> found == 1){
            printf("#");
            printf("%d", number_format_details(pokemon_id(node -> pokemon)));
            printf(" %s [%s] --> ", pokemon_name(node -> pokemon), pokeType[pokemon_first_type(node -> pokemon)]);
        }
        //If not yet found then print Question marks. 
        else{
            printf("#");
            printf("%d", number_format_details(pokemon_id(node -> pokemon)));
            printf(" ???? [????]\n");
        }
        return;
    }


    //If Evolutions exist
    while(node->evolve != NULL){

        if (node -> found == 1){
            printf("#");
            printf("%d", number_format_details(pokemon_id(node -> pokemon)));
            printf(" %s [%s] --> ", pokemon_name(node -> pokemon), pokeType[pokemon_first_type(node -> pokemon)]);
        }
        //If the pokemon is not yet found. 
        else{
            printf("#");
            printf("%d", number_format_details(pokemon_id(node -> pokemon)));
            printf(" ???? [????] --> ");
            
        }
        node = node-> evolve;
    }
        
        if (node -> found == 1){
            printf("#");
            printf("%d", number_format_details(pokemon_id(node -> pokemon)));
            printf(" %s [%s]\n", pokemon_name(node -> pokemon), pokeType[pokemon_first_type(node -> pokemon)]);
        }
        else{
            printf("#");
            printf("%d", number_format_details(pokemon_id(node -> pokemon)));
            printf(" ???? [????]");
        }
    printf("\n");
    
}

// Return the pokemon_id of the Pokemon that the currently selected.
// Pokemon evolves into.
int get_next_evolution(Pokedex pokedex) {
    if (pokedex->head == NULL){
        printf("The Pokedex is empty.");
        exit(1);
    }
    //If the pokemon has no evolutions =. 
    if (pokedex->head->evolve == NULL){
        return (DOES_NOT_EVOLVE);
    }
    //Return the evolution of the pokemon. 
    else{
        return(pokemon_id(pokedex->head->evolve->pokemon));
    }
    return 0;
}

//////////////////////////////////////////////////////////////////////
//                         Stage 5 Functions                        //
//////////////////////////////////////////////////////////////////////

// Create a new Pokedex which contains only the Pokemon of a specified
// type from the original Pokedex.
Pokedex get_pokemon_of_type(Pokedex pokedex, pokemon_type type) {
    fprintf(stderr, "exiting because you have not implemented the get_pokemon_of_type function\n");
    exit(1);
}

// Create a new Pokedex which contains only the Pokemon that have
// previously been 'found' from the original Pokedex.
Pokedex get_found_pokemon(Pokedex pokedex) {
    fprintf(stderr, "exiting because you have not implemented the get_found_pokemon function\n");
    exit(1);
}

// Create a new Pokedex containing only the Pokemon from the original
// Pokedex which have the given string appearing in its name.
Pokedex search_pokemon(Pokedex pokedex, char *text) {
    fprintf(stderr, "exiting because you have not implemented the search_pokemon function\n");
    exit(1);
}

// Add definitions for your own functions here.
// Make them static to limit their scope to this file.
