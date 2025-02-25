#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#define FILENAME "input.txt"
#define MAX_LINE_SIZE 256   // size int
//#define DEBUG_SIDE
//#define DEBUG_AREA
//#define DEBUG

typedef enum DIRECTION {UP, RIGHT, DOWN, LEFT} DIRECTION;

// GLOBAL

typedef struct Processed_node {
    // processed state
    bool is_processed;

    // border state
    bool border_state[4];

} Processed_node;

typedef struct Garden{
    char** area;
    Processed_node** processed_area_cache;
    uint16_t width;
    uint16_t height;
} Garden;

typedef struct Position{
    int32_t line;
    int32_t column;
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
uint64_t get_number_of_sides(Garden* garden, Position* start_position);
uint64_t get_area_of_one_region(Garden* garden, Position* start_position);
void set_the_complete_side_as_processed(Garden* garden, Position* position_ptr, DIRECTION border_direction);

void delete_last_element(Position_list** position_list);

void free_garden(Garden* garden);

Garden* get_data_from_file();
uint64_t get_price_of_all_fences(Garden* garden);
uint64_t get_price_of_one_region(Garden* garden, Position* start_position);

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
        garden->processed_area_cache = (Processed_node**)malloc(sizeof(Processed_node*) * 1);
    } else {
        // verify if all lines have the same size
        assert(garden->width == text_size);
        garden->area = (char**)realloc(garden->area, sizeof(char*) * (garden->height + 1));
        garden->processed_area_cache = (Processed_node**)realloc(garden->processed_area_cache, sizeof(Processed_node*) * (garden->height + 1));
    }
    assert(garden->area != NULL);
    assert(garden->processed_area_cache != NULL);

    // allocate memory for the line 
    garden->area[garden->height] = (char*)malloc((text_size + 1) * sizeof(char));
    assert(garden->area[garden->height] != NULL);
    garden->processed_area_cache[garden->height] = (Processed_node*)malloc(text_size * sizeof(Processed_node));
    assert(garden->processed_area_cache[garden->height] != NULL);

    // copy text into the line
    strcpy(garden->area[garden->height], text_line);

    // set processed area cache to 0
    Processed_node* init_node;
    for (uint16_t column_index = 0 ; column_index < garden->width ; column_index++){
        init_node = &(garden->processed_area_cache[garden->height][column_index]);
        init_node->is_processed = 0;
        for (uint8_t border_direction_index = 0 ; border_direction_index < 4 ; border_direction_index++){
            init_node->border_state[border_direction_index] = 0;
        }
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

uint64_t get_area_and_number_of_sides_of_one_region(Garden* garden, Position* start_position){

    #ifdef DEBUG
        printf("\nCompute area size and number of sides");
    #endif

    // init variables ----------------------------------------------------------------------------
    uint16_t area_size = 0;     // area of the region
    uint16_t number_of_sides = 0;

    char plant_type = garden->area[start_position->line][start_position->column]; // plant type of the region

    Position_list *next_positions = create_position_list(); // list of plants to process

    // UP RIGHT DOWN LEFT 
    int16_t line_shift[4] = {-1,0,1,0}; // line neighbor shift 
    int16_t column_shift[4] = {0,1,0,-1}; // column neighbor shift
    char dir_string[4][50] = {"UP", "RIGHT", "DOWN", "LEFT"};

    Position* position_ptr = copy_position(start_position); // position of the current plant
    Position* neighbor_position_ptr = create_position(0, 0); // position of the neighbor plant currently being analysed

    // set current position as processed
    garden->processed_area_cache[position_ptr->line][position_ptr->column].is_processed = 1;

    //printf("\n\t'%c' (%d,%d) : ", plant_type, position_ptr->line, position_ptr->column);

    // browse all the region with the same plant type --------------------------------------------
    do {

        // increment area size by 1
        area_size++;

        #ifdef DEBUG
            printf("\n\t\t- Process (%d, %d) '%c'", position_ptr->line, position_ptr->column, plant_type);
            //printf("\n\t\t- next_positions : ");
            //print_position_list(next_positions);
        #endif

        // check all 4 neighbors ; add then to next_position list
        for (uint8_t neighbor_direction_index = 0 ; neighbor_direction_index < 4 ; neighbor_direction_index++){

            // compute neighbor position
            neighbor_position_ptr->line = position_ptr->line + line_shift[neighbor_direction_index];
            neighbor_position_ptr->column = position_ptr->column + column_shift[neighbor_direction_index];

            #ifdef DEBUG
                printf("\n\t\t\t- neighbor (%d, %d) '%c' : ", neighbor_position_ptr->line, neighbor_position_ptr->column, plant_type);
            #endif

            // check if neighbor is out of the garden (line 1 & 2) ; out of region (line 3)
            if ((neighbor_position_ptr->line >= garden->height) || (neighbor_position_ptr->column >= garden->width) || 
                (neighbor_position_ptr->line < 0) || (neighbor_position_ptr->column < 0) ||  
                (garden->area[neighbor_position_ptr->line][neighbor_position_ptr->column] != plant_type)){
                    
                #ifdef DEBUG
                    printf("out of region | ");
                #endif

                // check if border position was not already processed
                if (garden->processed_area_cache[position_ptr->line][position_ptr->column].border_state[neighbor_direction_index] == 0){

                    #ifdef DEBUG
                        printf("side %s not processed", dir_string[neighbor_direction_index]);
                    #endif

                    // search the complete side and set it as processed
                    set_the_complete_side_as_processed(garden, position_ptr, (DIRECTION)neighbor_direction_index);
                    number_of_sides++;
                } else {

                    #ifdef DEBUG
                        printf("side %s already processed", dir_string[neighbor_direction_index]);
                    #endif
                }
            
            // neighbor is in the region; check if neighbor was not processed (line 4)
            } else if (!garden->processed_area_cache[neighbor_position_ptr->line][neighbor_position_ptr->column].is_processed){
            
                // add neighbor to the list of node to process
                add_position_to_position_list(&next_positions, neighbor_position_ptr);

                // set neighbor position as processed
                garden->processed_area_cache[neighbor_position_ptr->line][neighbor_position_ptr->column].is_processed = 1;
                
                #ifdef DEBUG
                    printf("in region"); } else { printf("already processed or out of garden");
                #endif
            }
        }

    } while ((position_ptr = pop_last_element(&next_positions)) != NULL);

    // free memory
    free(next_positions);
    free(position_ptr);
    free(neighbor_position_ptr);

    #ifdef DEBUG 
        printf("\n Area = %d ; Sides = %d", area_size, number_of_sides);
    #endif

    //getchar();

    // return cost
    return area_size * number_of_sides;
}

void set_the_complete_side_as_processed(Garden* garden, Position* position_ptr, DIRECTION border_direction){

    // UP RIGHT DOWN LEFT 
    int16_t line_shift[4] = {-1,0,1,0}; // line neighbor shift
    int16_t column_shift[4] = {0,1,0,-1}; // column neighbor shift

    char plant_type = garden->area[position_ptr->line][position_ptr->column]; // plant type

    Position* border_position = create_position(0,0);

    // search on each side of the contour to find its limit
    for (int search_direction = -1 ; search_direction < 2 ; search_direction += 2){

        border_position->column = position_ptr->column;
        border_position->line = position_ptr->line;

        do{
            // debug
            #ifdef DEBUG
                printf("\nborder position (%d, %d) for %d direction was processed", border_position->line, border_position->column, border_direction);
            #endif

            // set border position as processed
            garden->processed_area_cache[border_position->line][border_position->column].border_state[border_direction] = 1;

            // if border UP / DOWN => move RIGHT and LEFT
            if ((border_direction == UP) || (border_direction == DOWN)){
                border_position->column += search_direction;
            } else { // move UP OR DOWN
                border_position->line += search_direction;
            }                    

        // continue while (border_position is in garden (line 1 & 2) and in the region (line 3) and while outer layer is out of garden (line 4 & 5) or out of the region (line 6)
        }while ((border_position->column >= 0) && (border_position->column < garden->width) && 
                (border_position->line >= 0) && (border_position->line < garden->height) &&
                (garden->area[border_position->line][border_position->column] == plant_type) && 

                ((border_position->column + column_shift[border_direction] < 0) || (border_position->column + column_shift[border_direction] >= garden->width) ||
                (border_position->line + line_shift[border_direction] < 0) || (border_position->line + line_shift[border_direction] >= garden->height) ||
                (garden->area[border_position->line + line_shift[border_direction]][border_position->column + column_shift[border_direction]] != plant_type)));        
    }

    // debug
    #ifdef DEBUG
        printf("\nend of side");
    #endif  

    // free
    free(border_position);
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
            if ((garden->processed_area_cache[position_ptr->line][position_ptr->column].is_processed == 0) && 
            (garden->processed_area_cache[position_ptr->line][position_ptr->column].border_state[0] == 0) &&
            (garden->processed_area_cache[position_ptr->line][position_ptr->column].border_state[1] == 0) &&
            (garden->processed_area_cache[position_ptr->line][position_ptr->column].border_state[2] == 0) && 
            (garden->processed_area_cache[position_ptr->line][position_ptr->column].border_state[3] == 0)){

                // compute the price of the area
                price += get_area_and_number_of_sides_of_one_region(garden, position_ptr);
            }
        }
    }

    // free memory
    free(position_ptr);

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