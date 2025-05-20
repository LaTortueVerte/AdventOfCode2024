#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>

/****************************************************/

#define MAP_FILENAME "input.txt"
#define LINE_BUFFER_SIZE 52
#define REC_MAX_DEPTH 100
//#define DEBUG 1
                       //  ^  <  v  >
int8_t line_shift[4]   = {-1, 0, 1, 0};
int8_t column_shift[4] = {0, -1, 0, 1}; 

/****************************************************/

typedef struct Map{
    char** list;
    uint16_t height;
    uint16_t width;
} Map;

typedef struct Robot_instructions{
    char* list;
    uint16_t size;
} Robot_instructions;

/****************************************************/

Map* create_Map();
Robot_instructions* create_Robot_instructions();
void add_line_to_map(char* map_line, Map* pt_map);
void add_instruction_to_list(Robot_instructions* pt_instructions_list, char curr_c);
void free_map(Map* pt_map);
void free_instruction(Robot_instructions* pt_instr);
void print_map(Map* pt_map);
void print_instructions(Robot_instructions* pt_instr);
void convert_file_to_map_struct(char* filename, Map** pt_map, Robot_instructions** pt_instructions_list);
void convert_element_to_bigger_element(char curr_char, char* converted_char_1, char* converted_char_2);
Map* get_bigger_warehouse_map(Map* pt_map);
uint8_t check_if_robot_can_move_rec(Map* map, uint8_t instruction_code, uint8_t* curr_position, uint8_t depth);
void robot_move_rec(Map* map, uint8_t instruction_code, uint8_t* curr_position, char replace_char, uint8_t depth);
uint8_t process_robot_rec(Map* map, uint8_t instruction_code, uint8_t* curr_position, char replace_char, uint8_t depth);
uint8_t get_instruction_nb(char c);
uint64_t get_sum_gps_coordinates(Map* map);

/****************************************************/

Map* create_Map(){
    Map* pt_map = malloc(sizeof(Map));
    pt_map->height = 0;
    pt_map->width = 0;
    pt_map->list = NULL;

    return pt_map;
}

Robot_instructions* create_Robot_instructions(){

    Robot_instructions* new_robot_instructions = malloc(sizeof(Robot_instructions));
    new_robot_instructions->list = NULL;
    new_robot_instructions->size = 0;

    return new_robot_instructions;
}

void add_line_to_map(char* map_line, Map* pt_map){

    assert(pt_map != NULL);
    assert(map_line != NULL);

    // delete the '\n' character at last position (if it exists)
    size_t map_line_size = strlen(map_line);
    if ((map_line_size != 0) && (map_line[map_line_size - 1] == '\n')){
        map_line[--map_line_size] = '\0';
    }

    // allocate new line
    if (pt_map->list == NULL){
        pt_map->width = map_line_size;
        pt_map->list = malloc(sizeof(char*));
    } else {
        assert(pt_map->width == map_line_size);
        pt_map->list = realloc(pt_map->list, sizeof(char*) * (pt_map->height + 1));
    }
    assert(pt_map->list != NULL);

    // create new line
    pt_map->list[pt_map->height] = malloc(sizeof(char) * (map_line_size + 1));
    assert(pt_map->list[pt_map->height] != NULL);

    // copy line 
    strcpy(pt_map->list[pt_map->height], map_line);

    // increment size
    pt_map->height++;
}

void add_instruction_to_list(Robot_instructions* pt_instructions_list, char curr_c){

    assert(pt_instructions_list != NULL);

    // allocate memory
    if (pt_instructions_list->list == NULL){
        pt_instructions_list->list = malloc(sizeof(char));
    } else {
        pt_instructions_list->list = realloc(pt_instructions_list->list, sizeof(char) * (pt_instructions_list->size + 1));
    }
    assert(pt_instructions_list->list);

    // add data
    pt_instructions_list->list[pt_instructions_list->size] = curr_c;
    
    // increment size
    pt_instructions_list->size++;
}

void free_map(Map* pt_map){
    assert(pt_map != NULL);
    for (int line_index = 0 ; line_index < pt_map->height ; line_index++){
        free(pt_map->list[line_index]); 
    }
    free(pt_map->list);
    free(pt_map);
}

void free_instruction(Robot_instructions* pt_instr){
    assert(pt_instr != NULL);
    free(pt_instr->list);
    free(pt_instr);
}

void print_map(Map* pt_map){

    assert(pt_map->list != NULL);
    for (int line_index = 0 ; line_index < pt_map->height ; line_index++){
        printf("\n");
        for (int column_index = 0 ; column_index < pt_map->width ; column_index++){
            printf("%c", pt_map->list[line_index][column_index]);
        }
    }
}

void print_instructions(Robot_instructions* pt_instr){

    assert(pt_instr != NULL);
    printf("\ninstructions : ");
    for (int instruct_index = 0 ; instruct_index < pt_instr->size ; instruct_index++){
        printf("%c ", pt_instr->list[instruct_index]);
    }
}

void convert_file_to_map_struct(char* filename, Map** pt_map, Robot_instructions** pt_instructions_list){
    
    FILE* file_ptr = fopen(filename, "r");
    assert(file_ptr != NULL);

    // get map
    *pt_map = create_Map();

    char map_line[LINE_BUFFER_SIZE];

    while (fgets(map_line, LINE_BUFFER_SIZE, file_ptr) && strcmp(map_line, "\n")) {

        // add new map line
        add_line_to_map(map_line, *pt_map);
    }

    // get robot instructions
    *pt_instructions_list = create_Robot_instructions();

    char curr_c;

    while((curr_c = fgetc(file_ptr)) != EOF){
        if (curr_c != '\n'){
            add_instruction_to_list(*pt_instructions_list, curr_c);
        }
    }
}

Map* get_bigger_warehouse_map(Map* pt_map){
    // create bigger map from smaller map : pt_map
    assert(pt_map != NULL);

    // init new map
    Map* wide_map = create_Map();
    wide_map->list = malloc(sizeof(char*) * pt_map->height);
    assert(wide_map->list != NULL);
    wide_map->height = pt_map->height;
    wide_map->width = pt_map->width * 2;

    #ifdef DEBUG
        printf("\n");
    #endif

    // copy data
    for (int line_index = 0 ; line_index < wide_map->height ; line_index++){

        // create new line
        wide_map->list[line_index] = malloc(sizeof(char) * wide_map->width);
        assert(wide_map->list[line_index] != NULL);

        // copy line
        for (int column_index = 0 ; column_index < pt_map->width ; column_index++){

            // convert small element into wider element
            convert_element_to_bigger_element(  pt_map->list[line_index][column_index], 
                                                &(wide_map->list[line_index][column_index * 2]), 
                                                &(wide_map->list[line_index][column_index * 2 + 1]));

            #ifdef DEBUG
                printf("%c%c", wide_map->list[line_index][column_index * 2], wide_map->list[line_index][column_index * 2 + 1]);
            #endif
            
        }

        #ifdef DEBUG
            printf("\n");
        #endif
    }

    return wide_map;
}

void convert_element_to_bigger_element(char curr_char, char* converted_char_1, char* converted_char_2){

            if (curr_char == 'O'){

                *converted_char_1 = '[';
                *converted_char_2 = ']';

            } else if ((curr_char == '.') || (curr_char == '#')){

                *converted_char_1 = curr_char;
                *converted_char_2 = curr_char;
        
            } else { // curr_char == '@'

                *converted_char_1 = '@';
                *converted_char_2 = '.';
            }
}

uint8_t check_if_robot_can_move_rec(Map* map, uint8_t instruction_code, uint8_t* curr_position, uint8_t depth){

    /********************************************************************************/

    // depth limitation
    assert(depth != 0);

    // map limitation
    assert(curr_position[0] < map->height);
    assert(curr_position[0] >= 0);
    assert(curr_position[1] < map->width);
    assert(curr_position[1] >= 0);

    /********************************************************************************/

    // get current character
    char curr_char = map->list[curr_position[0]][curr_position[1]];

    #ifdef DEBUG
        printf("\ncheck char : %c(%d,%d)", curr_char, curr_position[0], curr_position[1]);
    #endif

    // current location is a wall
    if (curr_char == '#'){

        // return : impossible
        #ifdef DEBUG
            printf("\nblocked");
        #endif

        return 0;
    }

    // current location is empty
    if (curr_char == '.'){

        // return : possible
        #ifdef DEBUG
            printf("\nempty");
        #endif

        return 1;
    } 

    // current position is the robot or is a box : need to verify
    // compute next position
    uint8_t next_position[2] = {curr_position[0] + line_shift[instruction_code], 
                                curr_position[1] + column_shift[instruction_code]};                         

    //  check next character
    uint8_t answer = check_if_robot_can_move_rec(map, instruction_code, next_position, depth - 1);
    if (!answer){
        return 0; // impossible
    }

    // compute next char
    char next_char = map->list[next_position[0]][next_position[1]];

    // else check if next char is a box part
    if ((instruction_code % 2 == 0) && ((next_char == '[') || (next_char == ']'))){ // vertical movement and next char is a box part

        #ifdef DEBUG
            printf("\nVertical AND next char is box part");
        #endif        

        // compute position of the other box part
        if (next_char == '['){
            next_position[1] += 1; // ']'
        } else {
            next_position[1] -= 1; // '['
        }
                                        
        uint8_t answer_2 = check_if_robot_can_move_rec(map, instruction_code, next_position, depth - 1);
        return answer_2;
    }

    return 1;
}

void robot_move_rec(Map* map, uint8_t instruction_code, uint8_t* curr_position, char replace_char, uint8_t depth){
   
    /********************************************************************************/

    // depth limitation
    assert(depth != 0);

    // map limitation
    assert(curr_position[0] < map->height);
    assert(curr_position[0] >= 0);
    assert(curr_position[1] < map->width);
    assert(curr_position[1] >= 0);

    /********************************************************************************/

    // get current character
    char curr_char = map->list[curr_position[0]][curr_position[1]];

    #ifdef DEBUG
        printf("\nchar %c(%d,%d) replace by %c", curr_char, curr_position[0], curr_position[1], replace_char);
    #endif

    // current location is a wall
    assert(curr_char != '#');

    // check if is empty
    if (curr_char == '.'){
        // replace character
        map->list[curr_position[0]][curr_position[1]] = replace_char;
        return;
    }

    // else : compute next position
    uint8_t next_position[2] = {curr_position[0] + line_shift[instruction_code], 
                                curr_position[1] + column_shift[instruction_code]};

    // process next char
    robot_move_rec(map, instruction_code, next_position, curr_char, depth - 1);

    // compute next char
    char next_char = map->list[next_position[0]][next_position[1]];

    // else check if next char is a box part
    if ((instruction_code % 2 == 0) && ((next_char == '[') || (next_char == ']'))){ // vertical movement and next char is a box part

        char second_char;
        uint8_t second_position[2] = {curr_position[0], curr_position[1]};

        // compute position of the other box part
        if (next_char == '['){
            second_char = map->list[second_position[0]][++second_position[1]];
            next_position[1] += 1; // ']'
        } else {
            second_char = map->list[second_position[0]][--second_position[1]];
            next_position[1] -= 1; // '['
        }

        #ifdef DEBUG
            printf("\nchar %c(%d,%d) replace by %c", second_char, second_position[0], second_position[1], '.');
        #endif
        
        // replace character
        map->list[second_position[0]][second_position[1]] = '.';
                      
        // process second box part
        robot_move_rec(map, instruction_code, next_position, second_char, depth - 1);


    }

    // replace character
    map->list[curr_position[0]][curr_position[1]] = replace_char;
}


uint8_t process_robot_rec(Map* map, uint8_t instruction_code, uint8_t* curr_position, char replace_char, uint8_t depth){

    /********************************************************************************/

    // depth limitation
    assert(depth != 0);

    // map limitation
    assert(curr_position[0] < map->height);
    assert(curr_position[0] >= 0);
    assert(curr_position[1] < map->width);
    assert(curr_position[1] >= 0);

    /********************************************************************************/

    // get current character
    char curr_char = map->list[curr_position[0]][curr_position[1]];

    #ifdef DEBUG
        printf("\ncheck char : %c(%d,%d)", curr_char, curr_position[0], curr_position[1]);
    #endif

    // current location is a wall
    if (curr_char == '#'){
        #ifdef DEBUG
            printf("\nblocked");
        #endif
        return 0;
    }

    // current location is empty
    if (curr_char == '.'){
        #ifdef DEBUG
            printf("\nempty");
        #endif
        map->list[curr_position[0]][curr_position[1]] = replace_char;
        return 1;
    } 

    // current location can move
    // compute next position
    uint8_t next_position[2] = {curr_position[0] + line_shift[instruction_code], 
                                curr_position[1] + column_shift[instruction_code]};

    uint8_t answer = process_robot_rec(map, instruction_code, next_position, curr_char, depth - 1);

    if (answer){
        #ifdef DEBUG
            printf("\nup");
        #endif
        map->list[curr_position[0]][curr_position[1]] = replace_char;
    }

    return answer;
}

uint8_t get_instruction_nb(char c){

    if (c == '^'){
        return 0;
    } else if (c == '<'){
        return 1;
    } else if (c == 'v'){
        return 2;
    } else { // c == '>'
        return 3;
    }
}

uint64_t get_sum_gps_coordinates(Map* map){

    assert(map->list != NULL);

    uint64_t Sum = 0;

    // for each location of the warehouse
    for (int line_index = 0 ; line_index < map->height ; line_index++){
        for (int column_index = 0 ; column_index < map->width ; column_index++){
            // check if there is a box on the location
            if (map->list[line_index][column_index] == '['){
                //printf("\nAdd %d", (100 * column_index + line_index));
                Sum += 100 * line_index + column_index;
            }
        }
    }

    return Sum;
}

/****************************************************/

int main(){

    // init
    Map* warehouse_map;
    Robot_instructions* instructions_list;
    
    // get data from file
    convert_file_to_map_struct(MAP_FILENAME, &warehouse_map, &instructions_list);
    
    // get robot position
    uint8_t robot_position[2] = {0,0};
    uint8_t stop = 0;
    while ((robot_position[0] < warehouse_map->height) && !stop){
        robot_position[1] = 0;
        while ((robot_position[1] < warehouse_map->width) && !stop){
            if ((warehouse_map->list[robot_position[0]][robot_position[1]] == '@')){
                stop = 1;
            } else {
                (robot_position[1])++;
            }
        }
        if (!stop){
            (robot_position[0])++;
        }
    }
    assert(stop != 0);

    // convert map to bigger warehouse
    Map* bigger_warehouse = get_bigger_warehouse_map(warehouse_map);
    robot_position[1] *= 2;

    #ifdef DEBUG
        printf("\nWarehouse created");
    #endif

    // print data
    print_map(bigger_warehouse);

    // print data
    print_instructions(instructions_list);

    // loop
    for (uint16_t instruction_index = 0 ; instruction_index < instructions_list->size ; instruction_index++){

        // get instruction code
        uint8_t instruction_nb = get_instruction_nb(instructions_list->list[instruction_index]);
        
        #ifdef DEBUG
            printf("\ncurrent instruction : %c", instructions_list->list[instruction_index]);
        #endif

        //getchar();

        // move the robot by one instruction
        if (check_if_robot_can_move_rec(bigger_warehouse, instruction_nb, robot_position, REC_MAX_DEPTH)){

            printf("\nCAN MOVE");

            // process movement
            robot_move_rec(bigger_warehouse, instruction_nb, robot_position, '.', REC_MAX_DEPTH);

            // change position
            robot_position[0] = robot_position[0] + line_shift[instruction_nb];
            robot_position[1] = robot_position[1] + column_shift[instruction_nb];
        } else {

            printf("\nCANNOT MOVE");
        }

        #ifdef DEBUG
            printf("\nrobot position : (%d,%d)", robot_position[0], robot_position[1]);
            print_map(bigger_warehouse);
            //getchar();
        #endif
    }

    // print data
    print_map(bigger_warehouse);

    // compute result
    uint64_t Sum = get_sum_gps_coordinates(bigger_warehouse); 
    printf("\nSum = %lu", Sum);

    // free
    free_map(warehouse_map);
    free_map(bigger_warehouse);
    free_instruction(instructions_list);

    return 1;
}