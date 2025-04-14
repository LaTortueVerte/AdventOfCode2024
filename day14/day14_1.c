#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define FILENAME "input.txt"
#define BATHROOM_HEIGHT 103
#define BATHROOM_WIDTH 101
#define NB_OF_SECONDS 100

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
    FILE* file_ptr = fopen(FILENAME, "r");
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

int compute_safety_factor(Bathroom* bathroom){

    assert(bathroom != NULL);

    int quadrant_counter[4] = {0,0,0,0};
    int robot_position_x, robot_position_y;

    // for each robot in the bathroom
    for (int i_robot = 0 ; i_robot < bathroom->nb_of_robots ; i_robot++){

        robot_position_x = bathroom->robot_list[i_robot].position_x;
        robot_position_y = bathroom->robot_list[i_robot].position_y;

        if (robot_position_x < BATHROOM_WIDTH / 2){
            if (robot_position_y < BATHROOM_HEIGHT / 2){
                quadrant_counter[0]++;
            } else if (robot_position_y > BATHROOM_HEIGHT / 2){
                quadrant_counter[1]++;
            }
        } else if (robot_position_x > BATHROOM_WIDTH / 2){
            if (robot_position_y < BATHROOM_HEIGHT / 2){
                quadrant_counter[2]++;
            } else if (robot_position_y > BATHROOM_HEIGHT / 2) {
                quadrant_counter[3]++;
            }
        }
    }

    int safety_factor = quadrant_counter[0] * quadrant_counter[1] * quadrant_counter[2] * quadrant_counter[3];

    return safety_factor;
}

int main(){

    // get robots
    Bathroom* bathroom = convert_file_to_bathroom();
    #ifdef DEBUG
        printf("\nCreate Bathroom : %d robots", bathroom->nb_of_robots);
    #endif

    compute_position(bathroom, NB_OF_SECONDS);

    int safety_factor = compute_safety_factor(bathroom);

    // for each robot in the bathroom
    for (int i_robot = 0 ; i_robot < bathroom->nb_of_robots ; i_robot++){
        printf("\nNew robot : position = (%d,%d) ; velocity = (%d,%d)", bathroom->robot_list[i_robot].position_x,
                                                                        bathroom->robot_list[i_robot].position_y,
                                                                        bathroom->robot_list[i_robot].velocity_x,
                                                                        bathroom->robot_list[i_robot].velocity_y);  
    }


    printf("\nSafety factor = %d", safety_factor);

    return 1;
}