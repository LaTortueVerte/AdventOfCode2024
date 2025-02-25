#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#define FILENAME "input.txt"
#define MAX_LINE_SIZE 256   // size int
//#define DEBUG

// GLOBAL

typedef struct Garden{
    char** area;
    bool** processed_area_cache;
    uint16_t width;
    uint16_t height;
} Garden;

typedef struct Position{
    uint16_t line;
    uint16_t column;
} Position;

typedef struct Position_list{
    Position* positions;
    uint16_t size; 
} Position_list;

// PROTO

void print_garden(Garden* garden);
void print_position_list(Position_list* position_list);

Garden* create_garden();
Position_list* create_position_list();
Position* create_position(uint16_t line_index, uint16_t column_index);
Position* copy_position(Position* position);

void add_line_to_garden(Garden* garden, char* text_line);
void add_position_to_position_list(Position_list** position_list, Position* position);

Position *pop_last_element(Position_list** position_list);
Position* get_last_element(Position_list* position_list);

void delete_last_element(Position_list** position_list);

void free_garden(Garden* garden);

Garden* get_data_from_file();
uint64_t get_price_of_all_fences(Garden* garden);
uint32_t get_price_of_one_region(Garden* garden, Position* start_position);

// FUNCT

// print

void print_garden(Garden* garden){

    assert(garden != NULL);

    // print garden size
    printf("\n\nGarden (%d,%d) :", garden->height, garden->width);

    // print garden
    for (uint16_t line_index = 0 ; line_index < garden->height ; line_index++){
        printf("\n");
        for (uint16_t column_index = 0 ; column_index < garden->width; column_index++){
            printf("%c", garden->area[line_index][column_index]);
        }
    }
}

void print_position_list(Position_list* position_list){
    assert(position_list != NULL);

    printf("[");

    for (uint16_t position_index = 0 ; position_index < position_list->size ; position_index++){
        printf("(%d, %d) ", position_list->positions[position_index].line, position_list->positions[position_index].column);
    }

    printf("]");
}

// create

Garden* create_garden(){
    Garden* new_garden = (Garden*)malloc(sizeof(Garden));
    assert(new_garden != NULL);
    new_garden->area = NULL;
    new_garden->processed_area_cache = NULL;
    new_garden->height = 0;
    new_garden->width = 0;
    
    return new_garden;
}

Position_list* create_position_list(){
    Position_list* new_position_list = (Position_list*)malloc(sizeof(Position_list));
    assert(new_position_list != NULL);
    new_position_list->positions = NULL;
    new_position_list->size = 0;
    
    return new_position_list;
}

Position* create_position(uint16_t line_index, uint16_t column_index){
    Position* new_position = (Position*)malloc(sizeof(Position));
    new_position->line = line_index;
    new_position->column = column_index;

    return new_position;
}

Position* copy_position(Position* position){
    assert(position != NULL);

    Position* new_position = (Position*)malloc(sizeof(Position));
    new_position->line = position->line;
    new_position->column = position->column;

    return new_position;
}

// add

void add_line_to_garden(Garden* garden, char* text_line){

    assert(garden != NULL);
    assert(text_line != NULL);

    // delete the '\n' character at last position (if it exists)
    size_t text_size = strlen(text_line);
    if ((text_size != 0) && (text_line[text_size - 1] == '\n')){
        text_line[--text_size] = '\0';
    }

    // allocate memory for a new line
    if (garden->area == NULL){
        // set line size
        garden->width = text_size;
        garden->area = (char**)malloc(sizeof(char*) * 1);
        garden->processed_area_cache = (bool**)malloc(sizeof(bool*) * 1);
    } else {
        // verify if all lines have the same size
        assert(garden->width == text_size);
        garden->area = (char**)realloc(garden->area, sizeof(char*) * (garden->height + 1));
        garden->processed_area_cache = (bool**)realloc(garden->processed_area_cache, sizeof(bool*) * (garden->height + 1));
    }
    assert(garden->area != NULL);
    assert(garden->processed_area_cache != NULL);

    // allocate memory for the line 
    garden->area[garden->height] = (char*)malloc((text_size + 1) * sizeof(char));
    assert(garden->area[garden->height] != NULL);
    garden->processed_area_cache[garden->height] = (bool*)malloc(text_size * sizeof(bool));
    assert(garden->processed_area_cache[garden->height] != NULL);

    // copy text into the line
    strcpy(garden->area[garden->height], text_line);

    // set processed area cache to 0
    for (uint16_t column_index = 0 ; column_index < garden->width ; column_index++){
        garden->processed_area_cache[garden->height][column_index] = 0;
    }

    // increment garden size
    garden->height++;
}

void add_position_to_position_list(Position_list** position_list, Position* position){

    assert((*position_list) != NULL);

    // allocate memory for a new line
    if ((*position_list)->positions == NULL){
        (*position_list)->positions = (Position*)malloc(sizeof(Position));
    } else {
        (*position_list)->positions = (Position*)realloc((*position_list)->positions, sizeof(Position) * ((*position_list)->size + 1));
    }
    assert((*position_list)->positions != NULL);

    // set position into list
    (*position_list)->positions[(*position_list)->size].line = position->line;
    (*position_list)->positions[(*position_list)->size].column = position->column;

    // increment size
    (*position_list)->size++;
}

// get 

Position *pop_last_element(Position_list** position_list){
    assert((*position_list) != NULL);
    
    // get the last element of the list
    Position* last_position = get_last_element(*position_list);

    // init copy of position
    Position* position_copy = NULL;

    // check if list is not empty
    if (last_position != NULL){
        // copy the last position
        position_copy = copy_position(get_last_element(*position_list));
        // delete last element of the list
        delete_last_element(position_list);
    } 

    return position_copy;
}

Position *get_last_element(Position_list* position_list){
    assert(position_list != NULL);

    // return last element of the list or NULL if the list is empty
    if (position_list->size == 0){
        return NULL;
    } else {
        return &(position_list->positions[position_list->size - 1]);
    }   
}

// delete

void delete_last_element(Position_list** position_list){
    assert(*position_list != NULL);

    // re allocate size
    (*position_list)->positions = (Position*)realloc((*position_list)->positions, sizeof(Position) * ((*position_list)->size - 1));

    // reduce size
    (*position_list)->size--;
}

// free

void free_garden(Garden* garden){
    assert(garden != NULL);
    
    // free garden area and processed_area_cache
    for (uint16_t line_index = 0 ; line_index < garden->height ; line_index++){
        free(garden->area[line_index]);
        free(garden->processed_area_cache[line_index]);
    }
    free(garden->area);
    free(garden->processed_area_cache);

    // free garden
    free(garden);
}

// main 

Garden* get_data_from_file(){
    /* transform data from textfile to square garden struct */

    // access to file
    FILE* file_ptr = fopen(FILENAME, "r");
    assert(file_ptr != NULL);

    // create new list square garden
    Garden* garden = create_garden();

    // browse file
    char text_line_buffer[MAX_LINE_SIZE];
    while (fgets(text_line_buffer, sizeof(text_line_buffer), file_ptr)){
        add_line_to_garden(garden, text_line_buffer);
    }

    fclose(file_ptr);

    // send data
    return garden;
}

uint64_t get_price_of_all_fences(Garden* garden){

    // init price
    uint64_t price = 0;

    #ifdef DEBUG
        printf("\n\nIt contains :");
    #endif

    // browse into the garden
    Position* position_ptr = create_position(0, 0);
    for (position_ptr->line = 0 ; position_ptr->line < garden->height ; position_ptr->line++){
        for (position_ptr->column = 0 ; position_ptr->column < garden->width ; position_ptr->column++){

            // check if position was not processed 
            if (garden->processed_area_cache[position_ptr->line][position_ptr->column] == 0){

                // compute the price of the area
                price += get_price_of_one_region(garden, position_ptr);
            }
        }
    }

    // free memory
    free(position_ptr);

    return price;
}

uint32_t get_price_of_one_region(Garden* garden, Position* start_position){

    // init variables
    uint16_t perimeter = 0; // perimeter of the region
    uint16_t area_size = 0; // area of the region

    char plant_type = garden->area[start_position->line][start_position->column]; // plant type of the region

    Position_list *next_positions = create_position_list(); // list of plants to process

    int16_t line_shift[4] = {-1,1,0,0}; // line neighbor shift 
    int16_t column_shift[4] = {0,0,-1,1}; // column neighbor shift 

    Position* position_ptr = copy_position(start_position); // position of the current plant
    Position* neighbor_position_ptr = create_position(0, 0); // position of the neighbor plant currently being analysed

    // set current position as processed
    garden->processed_area_cache[position_ptr->line][position_ptr->column] = 1;

    #ifdef DEBUG
        printf("\n\t- A region of %c plants with price ", plant_type);
    #endif

    // browse all the region with the same plant type
    do {

        // increment area size by 1
        area_size++;

        #ifdef DEBUG
            printf("\n\t\t- Process (%d, %d)(%c)", position_ptr->line, position_ptr->column, plant_type);
            printf("\n\t\t- next_positions : ");
            print_position_list(next_positions);
        #endif

        // check all 4 neighbors 
        for (uint8_t shift_index = 0 ; shift_index < 4 ; shift_index++){

            // compute neighbor position
            neighbor_position_ptr->line = position_ptr->line + line_shift[shift_index];
            neighbor_position_ptr->column = position_ptr->column + column_shift[shift_index];

            #ifdef DEBUG
                printf("\n\t\t\t- neighbor (%d, %d)(%c) : ", neighbor_position_ptr->line, neighbor_position_ptr->column, plant_type);
            #endif

            // check if neighbor is out of the garden or a different plant type
            if ((neighbor_position_ptr->line >= garden->height) || (neighbor_position_ptr->column >= garden->width) ||
                (garden->area[neighbor_position_ptr->line][neighbor_position_ptr->column] != plant_type)){
                
                // increment perimeter by 1
                perimeter++;

                #ifdef DEBUG
                    printf("out of region");
                #endif

            // neighbor is in the garden and is of the same plant type : check if neighbor was already processed
            } else if (!garden->processed_area_cache[neighbor_position_ptr->line][neighbor_position_ptr->column]){
            
                // add neighbor to the list of node to process
                add_position_to_position_list(&next_positions, neighbor_position_ptr);

                // set nieghbor position as processed
                garden->processed_area_cache[neighbor_position_ptr->line][neighbor_position_ptr->column] = 1;
                
                #ifdef DEBUG
                    printf("in region"); } else { printf("already processed");
                #endif
            }
        }

    } while ((position_ptr = pop_last_element(&next_positions)) != NULL);

    // free memory
    free(next_positions);
    free(position_ptr);
    free(neighbor_position_ptr);

    // compute price of the fences
    uint32_t price = perimeter * area_size;

    #ifdef DEBUG
        printf("\nPrice : %d * %d = %d ", perimeter, area_size, price);
    #endif

    // return cost
    return price;
}

int main(){

    // get data
    Garden* garden = get_data_from_file();

    // print tab
    print_garden(garden);

    uint64_t fences_price = get_price_of_all_fences(garden);
    printf("\nfences cost : %llu", fences_price);

    free_garden(garden);

    return 0;
}