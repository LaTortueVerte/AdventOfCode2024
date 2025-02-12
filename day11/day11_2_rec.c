#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#define NB_OF_REPETITIONS 75
#define FILENAME "input.txt"
//#define DEBUG 1

// list to construct file

typedef struct Number {
    uint64_t value;
    int nb_of_repetitions;
} Number;

typedef struct Number_list {
    Number* list;
    uint64_t size;
} Number_list;

// list to construct cache

typedef struct Node_cache_number {
    __uint128_t value;
    int nb_of_repetitions;
    uint64_t nb_of_descendants;

    struct Node_cache_number* next;
} Node_cache_number;

typedef struct Node_cache_list {
    Node_cache_number** list;
    int size;
} Node_cache_list;

// -------------------------------------------------- tool functions -------------------------------------------------------

void print_number_list(Number_list *number_list){

    if (number_list != NULL){

        printf("\n");
        for (int number_index = 0 ; number_index < number_list->size ; number_index++){
            printf("%llu(%d) ", number_list->list[number_index].value, number_list->list[number_index].nb_of_repetitions);
        }
    }
}

void print_cache_list(Node_cache_list* node_cache_number_list){
    printf("\nNode cache number list :");
    for (int i = 0 ; i < node_cache_number_list->size ; i++){
        Node_cache_number* tmp_node = node_cache_number_list->list[i];
        printf("\n\t- (%d)", (i + 1));
        while (tmp_node != NULL){
            printf("%llu[%llu] ", tmp_node->value, tmp_node->nb_of_descendants);
            tmp_node = tmp_node->next;
        }
    }
}

int get_number_of_digits(__uint128_t number){
    assert(number >= 0);

    int nb_of_digits = 1;
    while (number > 9){
        number /= 10;
        nb_of_digits++;
    }

    return nb_of_digits;
}

// ------------------------------------------------ create section --------------------------------------------------------

Number_list* create_number_list(){
    Number_list* new_number_list = (Number_list*)malloc(sizeof(Number_list));
    assert(new_number_list != NULL);
    new_number_list->size = 0;
    new_number_list->list = NULL;

    return new_number_list;
}

Node_cache_list* create_number_cache_list(){
    Node_cache_list* new_number_list = (Node_cache_list*)malloc(sizeof(Node_cache_list));
    new_number_list->list = (Node_cache_number**)malloc(sizeof(Node_cache_number*) * NB_OF_REPETITIONS);
    new_number_list->size = NB_OF_REPETITIONS;

    for (int node_cache_index = 0 ; node_cache_index < NB_OF_REPETITIONS ; node_cache_index++){
        new_number_list->list[node_cache_index] = NULL;
    }

    return new_number_list;  
}

// ------------------------------------------------- adder section --------------------------------------------------------

void add_number_to_list(Number_list* number_list, __uint128_t value, int nb_of_repetitions){
    assert(number_list != NULL);
    
    if (number_list->size == 0){
        number_list->list = (Number*)malloc(sizeof(Number));
    } else {
        number_list->list = (Number*)realloc(number_list->list, (number_list->size + 1) * sizeof(Number));
    }
    assert(number_list->list != NULL);

    (number_list->list)[number_list->size].value = value;
    (number_list->list)[number_list->size].nb_of_repetitions = nb_of_repetitions; 

    number_list->size++;
}

void add_node_cache_number(Node_cache_list* node_cache_number_list, __uint128_t value, int nb_of_repetitions, uint64_t nb_of_descendants){

    // create new node
    Node_cache_number* new_node = (Node_cache_number*)malloc(sizeof(Node_cache_number));
    assert(new_node != NULL);
    new_node->value = value;
    new_node->nb_of_repetitions = nb_of_repetitions;
    new_node->nb_of_descendants = nb_of_descendants;
    new_node->next = NULL;

    // add new node 
    if (node_cache_number_list->list[nb_of_repetitions - 1] == NULL){
        // list is empty
        node_cache_number_list->list[nb_of_repetitions - 1] = new_node;

    } else if ((node_cache_number_list->list[nb_of_repetitions - 1])->value > value){
        // element is placed at the first place
        new_node->next = (node_cache_number_list->list[nb_of_repetitions - 1]);
        node_cache_number_list->list[nb_of_repetitions - 1] = new_node;
        
    } else {
        // element is placed in the list
        Node_cache_number* node_pt = node_cache_number_list->list[nb_of_repetitions - 1];
        while ( (node_pt->next != NULL) &&
                (node_pt->next->value < value)){

            node_pt = node_pt->next;
        }
        
        new_node->next = node_pt->next;
        node_pt->next = new_node;
    }
}

// ------------------------------------------------- getter section --------------------------------------------------------

uint64_t get_number_of_descendants_node(Node_cache_list* node_cache_number_list, __uint128_t value, int nb_of_repetitions){

    #ifdef DEBUG
        print_cache_list(node_cache_number_list);
    #endif

    Node_cache_number* tmp_node = node_cache_number_list->list[nb_of_repetitions - 1];

    while  ((tmp_node != NULL) && 
            (tmp_node->value < value) && 
            ((tmp_node->nb_of_repetitions != nb_of_repetitions) || (tmp_node->value != value))){

        tmp_node = tmp_node->next;
    }

    if ((tmp_node != NULL) && 
        (tmp_node->nb_of_repetitions == nb_of_repetitions) && 
        (tmp_node->value == value)){

        return tmp_node->nb_of_descendants;

    } else {

        return 0;
    }
}

void get_data_from_text_file(Number_list* number_list){

    // build the number list 
    FILE* file_ptr = fopen(FILENAME,"r");
    assert(file_ptr != NULL); 

    int current_char;
    int in_coming_int = 0;

    while((current_char = fgetc(file_ptr)) != EOF){

        if (isdigit(current_char)){

            // build new number
            in_coming_int *= 10;
            in_coming_int += current_char - '0';

        } else { 
            
            // add new number
            add_number_to_list(number_list, in_coming_int, NB_OF_REPETITIONS);
            in_coming_int = 0;
        }
    }

    fclose(file_ptr);
}

// --------------------------------------------------- main -----------------------------------------------------------------

uint64_t rec_get_number_of_descendants(Node_cache_list* cache_number_list, __uint128_t value, int nb_of_repetitions){

    #ifdef DEBUG 
        printf("\n%llu (%d)", value, nb_of_repetitions);
    #endif

    if (nb_of_repetitions == 0){
        #ifdef DEBUG 
            printf(" return 1");
        #endif
        return 1;
    }

    uint64_t nb_of_descendants = get_number_of_descendants_node(cache_number_list, value, nb_of_repetitions);

    if (nb_of_descendants != 0){ // check if number is already known 

        #ifdef DEBUG 
            printf(" return %li", nb_of_descendants);
        #endif
        return nb_of_descendants;

    } else { // number in unknown 
        
        // first possibility value = 0
        if (value == 0){
            nb_of_descendants = rec_get_number_of_descendants(cache_number_list, 1, (nb_of_repetitions - 1));

        } else {
            //int nb_of_digits = log10(value) + 1;
            int nb_of_digits = get_number_of_digits(value);
            // second possibility nb of digits of value is even
            if (nb_of_digits % 2 == 0){

                    // compute first number
                    __uint128_t first_nb = value;
                    __uint128_t ten_pow = 1; //pow(10, nb_of_digits/2);
                    for (int i = 0 ; i < nb_of_digits/2; i++){
                        ten_pow *= 10;
                    }
                    first_nb /= ten_pow; 

                    // compute second number
                    __uint128_t second_nb = value - first_nb * ten_pow;

                    // compute nb of descendants on the two branches
                    nb_of_descendants = rec_get_number_of_descendants(cache_number_list, first_nb, (nb_of_repetitions - 1)) + rec_get_number_of_descendants(cache_number_list, second_nb, (nb_of_repetitions - 1));
            
            } else { // third possibility nb of digits of value is odd
                //assert(9114003988986932 < value);
                __uint128_t new_value = value * 2024;
                nb_of_descendants = rec_get_number_of_descendants(cache_number_list, new_value, (nb_of_repetitions - 1));
            }
        }
    }

    assert(nb_of_descendants > 0);

    #ifdef DEBUG 
        printf("\n\t%llu (%d) : bring up %li", value, nb_of_repetitions, nb_of_descendants);
    #endif

    // save data
    add_node_cache_number(cache_number_list, value, nb_of_repetitions, nb_of_descendants);

    // return data
    return nb_of_descendants;
}

int main(){

    printf("\nday11_2_rec");

    Number_list* p_number_list = create_number_list();

    get_data_from_text_file(p_number_list);

    print_number_list(p_number_list);

    // data processing : blink

    uint64_t total_nb_of_descendants = 0;
    Node_cache_list* cache_number_list = create_number_cache_list();

    for (int number_index = 0 ; number_index < p_number_list->size ; number_index++){

        printf("\n%.2f%%",(number_index * 100.0 / p_number_list->size));
        uint64_t num = rec_get_number_of_descendants(cache_number_list,
                                                p_number_list->list[number_index].value, 
                                                p_number_list->list[number_index].nb_of_repetitions);

        assert(num > 0);

        total_nb_of_descendants += num;

        assert(total_nb_of_descendants > 0);
    }
    
    //print result
    printf("\nlist size : %llu", total_nb_of_descendants);

    // free data
    free(p_number_list->list);
    free(p_number_list);
    for (int i = 0 ; i < cache_number_list->size ; i++){
        Node_cache_number *pt = cache_number_list->list[i];
        Node_cache_number *k;
        while (pt != NULL){
            k = pt;
            pt = pt->next;
            free(k);
        }
    }
    free(cache_number_list);
    
    return 0;
}