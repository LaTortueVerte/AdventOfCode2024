#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define FILENAME_INPUT "input.txt"
#define BATHROOM_HEIGHT 103
#define BATHROOM_WIDTH 101

#define MAX_NB_OF_SECONDS 10000
#define FILENAME_OUTPUT "output.txt"

#define LINE_BUFFER_SIZE 50

#define DEBUG 1

typedef struct Robot{
    int position_x;
    int position_y;
    int velocity_x;
    int velocity_y;
}Robot;

typedef struct Bathroom{
    Robot* robot_list;
    int nb_of_robots; 

    int width;
    int height;
}Bathroom;

// prototypes

Bathroom* convert_file_to_bathroom();
void compute_position(Bathroom* bathroom, int nb_of_seconds);
int compute_safety_factor(Bathroom* bathroom);
int positive_modulo(int i, int n);
int** create_bathroom_list();
void reset_bathroom(int** bathroom_list);
void convert_bathroom_struct_to_bathroom_list(Bathroom* bathroom, int** bathroom_list);
void add_bathroom_list_to_file(int** bathroom_list, int nb_of_seconds, FILE* file_ptr);
void free_bathroom_list(int*** bathroom_list);
void free_bathroom(Bathroom** bathroom);

// functions

int positive_modulo(int i, int n) {
    return (i % n + n) % n;
}

Bathroom* convert_file_to_bathroom(){

    // create bathroom
    Bathroom* new_bathroom = malloc(sizeof(Bathroom));
    assert(new_bathroom != NULL);

    #ifdef DEBUG
        printf("\nCreate bathroom");
    #endif

    // init bathroom
    new_bathroom->height = BATHROOM_HEIGHT;
    new_bathroom->width = BATHROOM_WIDTH;
    new_bathroom->nb_of_robots = 0;

    // open file
    FILE* file_ptr = fopen(FILENAME_INPUT, "r");
    assert(file_ptr != 0);

    // init file reading
    char line[LINE_BUFFER_SIZE];
    int position_x, position_y, velocity_x, velocity_y;

    // read file
    while(fgets(line, LINE_BUFFER_SIZE, file_ptr)) {

        // get new line
        sscanf(line, "p=%d,%d v=%d,%d", &position_x, &position_y, &velocity_x, &velocity_y);

        // add robot to bathroom
        if (new_bathroom->nb_of_robots == 0){
            new_bathroom->robot_list = malloc(sizeof(Robot));
        }
        else 
        {
            new_bathroom->robot_list = realloc(new_bathroom->robot_list, sizeof(Robot) * (new_bathroom->nb_of_robots + 1));
        }

        assert(new_bathroom->robot_list != NULL);
        new_bathroom->robot_list[new_bathroom->nb_of_robots].position_x = position_x;
        new_bathroom->robot_list[new_bathroom->nb_of_robots].position_y = position_y;
        new_bathroom->robot_list[new_bathroom->nb_of_robots].velocity_x = velocity_x;
        new_bathroom->robot_list[new_bathroom->nb_of_robots].velocity_y = velocity_y;
        new_bathroom->nb_of_robots++;

        #ifdef DEBUG
            printf("\nNew robot : position = (%d,%d) ; velocity = (%d,%d)", new_bathroom->robot_list[new_bathroom->nb_of_robots - 1].position_x,
                                                                            new_bathroom->robot_list[new_bathroom->nb_of_robots - 1].position_y,
                                                                            new_bathroom->robot_list[new_bathroom->nb_of_robots - 1].velocity_x,
                                                                            new_bathroom->robot_list[new_bathroom->nb_of_robots - 1].velocity_y);
        #endif            
    }

    return new_bathroom;
}

void compute_position(Bathroom* bathroom, int nb_of_seconds){

    assert(bathroom != NULL);

    // for each robot in the bathroom
    for (int i_robot = 0 ; i_robot < bathroom->nb_of_robots ; i_robot++){

        // compute robot position in <nb_of_seconds> second(s)
        bathroom->robot_list[i_robot].position_x = positive_modulo((bathroom->robot_list[i_robot].position_x + bathroom->robot_list[i_robot].velocity_x * nb_of_seconds), BATHROOM_WIDTH);
        bathroom->robot_list[i_robot].position_y = positive_modulo((bathroom->robot_list[i_robot].position_y + bathroom->robot_list[i_robot].velocity_y * nb_of_seconds), BATHROOM_HEIGHT);
    }
}

int** create_bathroom_list(){
    int** bathroom_list = malloc(sizeof(int*) * BATHROOM_HEIGHT);
    assert(bathroom_list != NULL);

    for (int line_index = 0 ; line_index < BATHROOM_HEIGHT ; line_index++){
        bathroom_list[line_index] = malloc(sizeof(int) * BATHROOM_WIDTH);
        assert(bathroom_list[line_index] != NULL);
    }

    return bathroom_list;
}

void reset_bathroom(int** bathroom_list){
    for (int i = 0 ; i < BATHROOM_HEIGHT ; i++){
        for (int j = 0 ; j < BATHROOM_WIDTH ; j++){
            bathroom_list[i][j] = 0;
        }
    }
}

void convert_bathroom_struct_to_bathroom_list(Bathroom* bathroom, int** bathroom_list){
    // for each robot in the bathroom
    for (int i_robot = 0 ; i_robot < bathroom->nb_of_robots ; i_robot++){
        bathroom_list[bathroom->robot_list[i_robot].position_x][bathroom->robot_list[i_robot].position_y] = 1;
    }
}

void add_bathroom_list_to_file(int** bathroom_list, int nb_of_seconds, FILE* file_ptr){
    fprintf(file_ptr, "line : %d\n", nb_of_seconds);

    for (int line_index = 0 ; line_index < BATHROOM_HEIGHT ; line_index++){
        for (int column_index = 0 ; column_index < BATHROOM_WIDTH ; column_index++){
            if (bathroom_list[line_index][column_index]){
                fprintf(file_ptr, "# ");
            } else {
                fprintf(file_ptr, "  ");
            }
            
        }
        fprintf(file_ptr, "\n");
    }
}

void free_bathroom_list(int*** bathroom_list) {
    for (int i = 0; i < BATHROOM_HEIGHT; i++) {
        for (int j = 0; j < BATHROOM_WIDTH; j++) {
            free((*bathroom_list)[i][j]); // Free each integer
        }
        free((*bathroom_list)[i]); // Free each row
    }
    free(bathroom_list); // Free the array of pointers
}

void free_bathroom(Bathroom** bathroom){
    free((*bathroom)->robot_list);
    free(bathroom);
}

int main(){

    // get robots
    Bathroom* bathroom = convert_file_to_bathroom();
    #ifdef DEBUG
        printf("\nCreate Bathroom : %d robots", bathroom->nb_of_robots);
    #endif

    // convert file to bathroom struct
    convert_file_to_bathroom();

    // init bathroom list
    int** bathroom_list = create_bathroom_list();
    
    // init file bathroom
    FILE* file_ptr = fopen(FILENAME_OUTPUT, "w");
    assert(file_ptr != NULL);

    // for each lap (second), 
    for (int loop_index = 0 ; loop_index < MAX_NB_OF_SECONDS ; loop_index++){

        printf("\nline : %d", loop_index);

        // compute postion of all the robots 1 second later
        compute_position(bathroom, 1);

        // reset int** bathroom list 
        reset_bathroom(bathroom_list);

        // convert bathroom struct to bathroom list
        convert_bathroom_struct_to_bathroom_list(bathroom, bathroom_list);

        // add bathroom list to file output
        add_bathroom_list_to_file(bathroom_list, loop_index + 1, file_ptr);
    }

    // free
    free_bathroom_list(&bathroom_list);
    free_bathroom(&bathroom);

    fclose(file_ptr);
    
    return 1;
}