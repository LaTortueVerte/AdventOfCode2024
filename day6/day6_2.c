#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define DEBUG 0

enum ALREADY_MAP_I {UP, RIGHT, DOWN, LEFT};

int main(){

    printf("day6_2.c\n");


    /*--------------------------------------------------------------------------------------------
                                            GET DATA
    --------------------------------------------------------------------------------------------*/

    FILE *fptr = fopen("input2.txt", "r");

    if(fptr == NULL) {
        printf("Not able to open the file.\n");
        return 1;
    }

    char line[256];

    char **map = NULL;

    int map_index = 0;
    int map_size = 0;

    int initial_guard_position[2];

    while(fgets(line, sizeof(line), fptr)) {

        // remove '\n' character at the end of the string
        
        size_t line_length = strlen(line);

        if (line[line_length - 1] == '\n') {
            line[line_length - 1] = '\0';
        }

        // create map if map is empty

        if (map == NULL){
            map = (char**)malloc(sizeof(char*) * line_length);
            assert(map != NULL);
        }

        // create new row in the row 
        map[map_index] = (char*)malloc(sizeof(char) * (line_length + 1));
        if (map_size == 0){ map_size = line_length;}
        assert(map[map_index] != NULL);
        strncpy(map[map_index], (char*)line, line_length); // '-1' for the '\n' character 

        map[map_index][line_length] = '\0';

        int char_index = 0;
        int err_limit = 200;

        while ((map[map_index][char_index] != '\0') && (map[map_index][char_index] != '^')){
            char_index++;
            assert(err_limit-- != 0);
        }

        if (map[map_index][char_index] == '^'){
            initial_guard_position[0] = map_index;
            initial_guard_position[1] = char_index;
        }

        map_index++; // next row
    }

    map_size = map_index;

    if (DEBUG){
        printf("map_size : %d\n", map_size);
        printf("guard position : %d %d\n", initial_guard_position[0], initial_guard_position[1]);
    }

    /*--------------------------------------------------------------------------------------------
                                            PROCESS DATA
    --------------------------------------------------------------------------------------------*/


    // init already map 

    int*** map_already = (int***)malloc(sizeof(int**) * 4);
    assert(map_already != NULL);

    for (int dir_map = 0 ; dir_map < 4 ; dir_map++){
        
        map_already[dir_map] = (int**)malloc(sizeof(int*) * map_size);
        assert(map_already[dir_map] != NULL);

        for (int line_map = 0 ; line_map < map_size ; line_map++){
            map_already[dir_map][line_map] = (int*)malloc(sizeof(int) * map_size);
            assert(map_already[dir_map][line_map] != NULL);
        }
    }

    // init var

    int Sum = 0;

    for (int i_map = 0 ; i_map < map_size ; i_map++ ){
        for (int j_map = 0 ; j_map < map_size ; j_map++ ){

            if ((map[i_map][j_map] != '#') && (map[i_map][j_map] != '^')){

                map[i_map][j_map] = '#'; // add new obstacle

                // clear already map -----------------------------------------------------------------------

                for (int dir_map = 0 ; dir_map < 4 ; dir_map++){
                    for (int line_map = 0 ; line_map < map_size ; line_map++){
                        for (int index_map = 0 ; index_map < map_size ; index_map++){
                            map_already[dir_map][line_map][index_map] = 0;
                        }
                    }
                }

                // Test new possibility -----------------------------------------------------------------------

                int is_stuck = 0;
                int guard_position[2] = {initial_guard_position[0], initial_guard_position[1]};

                int lap_nb = 0;

                // print map -----------------------------------------------------------------------

                    if (DEBUG){
                        printf("---------------------------------------------------------------\n");
                        printf("New obstacle in (%d,%d)\n",i_map, j_map);
                    }

                // Loop --------------------------------------------------------------------------------

                while (!is_stuck && (guard_position[0] >= 0) && (guard_position[0] < map_size) && (guard_position[1] >= 0) && (guard_position[1] < map_size)){

                    //if ((i_map == 3) && (j_map ==33) && (lap_nb % 100 == 0)){DEBUG = 1;} else {DEBUG = 0;}


                    if (map[guard_position[0]][guard_position[1]] == '^'){ // IF go up -----------------------------------------------------------------------

                        int is_front_empty = ((guard_position[0] - 1) >= 0);

                        if (is_front_empty && (map[guard_position[0] - 1][guard_position[1]] == '#')){ // detect wall in front

                            map[guard_position[0]][guard_position[1]] = '>';
                            map_already[RIGHT][guard_position[0]][guard_position[1]] = 1;

                        } else if (is_front_empty && (map_already[UP][guard_position[0] - 1][guard_position[1]] == 1)){ // detect loop

                            is_stuck = 1;

                        } else {

                            guard_position[0]--; // move guard
                            if (is_front_empty){
                                map[guard_position[0]][guard_position[1]] = '^';
                                map_already[UP][guard_position[0]][guard_position[1]] = 1;
                            }
                        }

                    } else if (map[guard_position[0]][guard_position[1]] == 'v'){ // IF go down -----------------------------------------------------------------------

                        int is_front_empty = ((guard_position[0] + 1) < map_size);

                        if (is_front_empty && (map[guard_position[0] + 1][guard_position[1]] == '#')){ // detect wall in front

                            map[guard_position[0]][guard_position[1]] = '<';
                            map_already[LEFT][guard_position[0]][guard_position[1]] = 1;

                        } else if (is_front_empty && (map_already[DOWN][guard_position[0] + 1][guard_position[1]] == 1)){ // detect loop

                            is_stuck = 1;

                        } else {

                            guard_position[0]++; // move guard
                            if (is_front_empty){
                                map[guard_position[0]][guard_position[1]] = 'v';
                                map_already[DOWN][guard_position[0]][guard_position[1]] = 1;
                            }
                        }

                    } else if (map[guard_position[0]][guard_position[1]] == '>'){ // IF go right -----------------------------------------------------------------------

                        int is_front_empty = ((guard_position[1] + 1) < map_size);

                        if (is_front_empty && (map[guard_position[0]][guard_position[1] + 1] == '#')){ // detect if case in front is in map

                            map[guard_position[0]][guard_position[1]] = 'v';
                            map_already[DOWN][guard_position[0]][guard_position[1]] = 1;

                        } else if (is_front_empty && (map_already[RIGHT][guard_position[0]][guard_position[1] + 1] == 1)){ // detect loop
                                
                            is_stuck = 1;
                            
                        } else {

                            guard_position[1]++; // move guard
                            if (is_front_empty){
                                map[guard_position[0]][guard_position[1]] = '>';
                                map_already[RIGHT][guard_position[0]][guard_position[1]] = 1;
                            }
                        }

                    } else if ((map[guard_position[0]][guard_position[1]] == '<')){ // IF go left -----------------------------------------------------------------------

                        int is_front_empty = ((guard_position[1] - 1) >= 0);

                        if (is_front_empty && (map[guard_position[0]][guard_position[1] - 1] == '#')){ // detect if case in front is in map

                            map[guard_position[0]][guard_position[1]] = '^';
                            map_already[UP][guard_position[0]][guard_position[1]] = 1;

                        } else if (is_front_empty && (map_already[LEFT][guard_position[0]][guard_position[1] - 1] == 1)){ // detect loop
                                
                            is_stuck = 1;
                            
                        } else {

                            guard_position[1]--; // move guard
                            if (is_front_empty){
                                map[guard_position[0]][guard_position[1]] = '<'; 
                                map_already[LEFT][guard_position[0]][guard_position[1]] = 1;
                            }
                        }
                    }

                    // print map --------------------------------------------------------------------------------

                    /*if (DEBUG){
                        printf("---------------------------------------------------------------\n");
                        for (int i = 0 ; i < map_size ; i++){
                            printf("%s\n",map[i]);
                        }
                        system("pause");



                    }*/
                    lap_nb++;
                }

                // count result -------------------------------------------------------------------------

                if (is_stuck){
                    Sum++;
                    if (DEBUG) {printf("Stuck\n");}
                } else {
                    if (DEBUG) {printf("Not stuck\n");}
                }

                // print map --------------------------------------------------------------------------------

                if (DEBUG){
                    for (int i = 0 ; i < map_size ; i++){
                        printf("%s\n",map[i]);
                    }
                    system("pause");
                }

                // reset map -----------------------------------------------------------------------

                map[i_map][j_map] = '.';

                for (int i2_map = 0 ; i2_map < map_size ; i2_map++ ){
                    for (int j2_map = 0 ; j2_map < map_size ; j2_map++ ){
                        if (map[i2_map][j2_map] != '#'){
                            map[i2_map][j2_map] = '.';
                        }
                    }
                }

                map[initial_guard_position[0]][initial_guard_position[1]] = '^';

                if (DEBUG){system("pause");}

            }
        }
    }

    // print results -----------------------------------------------------------------------

    printf("S = %d\n", Sum);

    // free map

    for (int i = 0 ; i < map_size ; i++){
        free(map[i]);
    }
    free(map);


    return 0;
}