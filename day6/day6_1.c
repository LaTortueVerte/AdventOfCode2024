#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define DEBUG 0


int main(){

    /*--------------------------------------------------------------------------------------------
                                            GET DATA
    --------------------------------------------------------------------------------------------*/

    FILE *fptr = fopen("input.txt", "r");

    if(fptr == NULL) {
        printf("Not able to open the file.\n");
        return 1;
    }

    char line[256];

    char **map = NULL;

    int map_index = 0;
    int map_size = 0;

    int guard_position[2];

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
            guard_position[0] = map_index;
            guard_position[1] = char_index;
        }

        // next row
        map_index++;
    }

    map_size = map_index;

    printf("map_size : %d\n", map_size);
    printf("guard position : %d %d\n", guard_position[0], guard_position[1]);

    /*--------------------------------------------------------------------------------------------
                                            PROCESS DATA
    --------------------------------------------------------------------------------------------*/

    while ((guard_position[0] >= 0) && (guard_position[0] < map_size) && (guard_position[1] >= 0) && (guard_position[1] < map_size)){

        int last_guard_position[2] = {guard_position[0], guard_position[1]};

        if (map[guard_position[0]][guard_position[1]] == '^'){ // IF go up

            if (((guard_position[0] - 1) >= 0) && (map[guard_position[0] - 1][guard_position[1]] == '#')){ // detect wall in front
                map[guard_position[0]][guard_position[1]] = '>';
            } else {
                guard_position[0]--; // move guard

                map[last_guard_position[0]][last_guard_position[1]] = 'X';
                map[guard_position[0]][guard_position[1]] = '^';
            }

        } else if (map[guard_position[0]][guard_position[1]] == 'v'){ // IF go down

            if (((guard_position[0] + 1) < map_size) && (map[guard_position[0] + 1][guard_position[1]] == '#')){ // detect wall in front
                map[guard_position[0]][guard_position[1]] = '<';
            } else {
                guard_position[0]++; // move guard

                map[last_guard_position[0]][last_guard_position[1]] = 'X'; // move guard
                map[guard_position[0]][guard_position[1]] = 'v';
            }

        } else if (map[guard_position[0]][guard_position[1]] == '>'){ // IF go right

            if (((guard_position[1] + 1) < map_size) && (map[guard_position[0]][guard_position[1] + 1] == '#')){ // detect wall in front
                map[guard_position[0]][guard_position[1]] = 'v';
            } else {
                guard_position[1]++; // move guard

                map[last_guard_position[0]][last_guard_position[1]] = 'X'; 
                map[guard_position[0]][guard_position[1]] = '>';
            }

        } else if ((map[guard_position[0]][guard_position[1]] == '<')){ // IF go left 

            if (((guard_position[1] - 1) >= 0) && (map[guard_position[0]][guard_position[1] - 1] == '#')){ // detect wall in front
                map[guard_position[0]][guard_position[1]] = '^';
            } else {
                guard_position[1]--; // move guard

                map[last_guard_position[0]][last_guard_position[1]] = 'X'; 
                map[guard_position[0]][guard_position[1]] = '<'; 
            }

        }

        // print map

        if (DEBUG){

            printf("---------------------------------------------------------------\n");
            printf("guard position : %d %d : <%c>\n", guard_position[0], guard_position[1], map[guard_position[0]][guard_position[1]]);

            for (int i = 0 ; i < map_size ; i++){
                printf("%s\n",map[i]);
            }

            system("pause");
        }
    }

    // count path

    int Sum = 0;

    for (int i_map = 0 ; i_map < map_size ; i_map++ ){
        for (int j_map = 0 ; j_map < map_size ; j_map++ ){
                if (map[i_map][j_map] == 'X'){
                    Sum++;
                }
        }
    }

    printf("S = %d\n", Sum);

    // free map

    for (int i = 0 ; i < map_size ; i++){
        free(map[i]);
    }
    free(map);


    return 0;
}