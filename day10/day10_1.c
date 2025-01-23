#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINE_SIZE 512
#define RECURSIVE_MAX_DEPTH 10
#define DEBUG 1

enum topography_positon_type {NOT_PROCESSED, PROCESSED};

void print_map(uint8_t** topography_map, int map_size){ 
    for (int map_line_index = 0 ; map_line_index < map_size ; map_line_index++){
        printf("\n");
        for (int map_column_index = 0 ; map_column_index < map_size ; map_column_index++){
            printf("%d", topography_map[map_line_index][map_column_index]);
        }
    }
}

void reset_mask_map(uint8_t*** mask_map, int map_size){
    for (int map_line_index = 0 ; map_line_index < map_size ; map_line_index++){
        for (int map_column_index = 0 ; map_column_index < map_size ; map_column_index++){
            (*mask_map)[map_line_index][map_column_index] = NOT_PROCESSED;
        }
    }
}

int count_score_rec(uint8_t*** map_mask, uint8_t** topography_map, uint8_t map_size, int line_position, int column_position, int map_depth, int last_height){

    if  ((map_depth != 0) && // check depth limit
        (line_position >= 0) && (line_position < map_size) && (column_position >= 0) && (column_position < map_size) && // check map limits
        ((*map_mask)[line_position][column_position] == NOT_PROCESSED)){

        if (topography_map[line_position][column_position] == last_height + 1){ 

            (*map_mask)[line_position][column_position] = PROCESSED;

            if (topography_map[line_position][column_position] == 9){
                if (DEBUG) {printf("\n\t- trailend (%d, %d)",line_position, column_position);}
                return 1;
            }

            int score_sum = 0;

            score_sum += count_score_rec(map_mask, topography_map, map_size, (line_position - 1), column_position, map_depth - 1, last_height + 1);
            score_sum += count_score_rec(map_mask, topography_map, map_size, (line_position + 1), column_position, map_depth - 1, last_height + 1);
            score_sum += count_score_rec(map_mask, topography_map, map_size, line_position, (column_position - 1), map_depth - 1, last_height + 1);
            score_sum += count_score_rec(map_mask, topography_map, map_size, line_position, (column_position + 1), map_depth - 1, last_height + 1);

            return score_sum;
        }
    }

    return 0;
}

int main(){

    // open the text file
    FILE *file_pt = fopen("example.txt","r");
    assert(file_pt != NULL);

    // contains the map
    uint8_t **topography_map = NULL;
    int map_size;

    // contains one line of the textfile
    char text_line_buffer[MAX_LINE_SIZE];
    int map_line_index = 0;
    // construct the map line by line
    while (fgets(text_line_buffer, sizeof(text_line_buffer), file_pt)){

        bool is_last_char_line_jump = (text_line_buffer[strlen(text_line_buffer) - 1] == '\n');

        if (topography_map == NULL){

            // allocate more space
            map_size = (strlen(text_line_buffer) - is_last_char_line_jump);
            topography_map = (uint8_t**)malloc(sizeof(uint8_t*) * map_size);

        } 

        // create line
        assert(((strlen(text_line_buffer) - is_last_char_line_jump) == map_size));
        topography_map[map_line_index] = (uint8_t*)malloc(sizeof(uint8_t) * (strlen(text_line_buffer) - is_last_char_line_jump));
        assert(topography_map[map_line_index] != NULL);

        // copy data
        for (int map_column_index = 0 ; map_column_index < map_size ; map_column_index++){
            topography_map[map_line_index][map_column_index] = text_line_buffer[map_column_index] - '0';
        }

        map_line_index++;
    }

    print_map(topography_map, map_size);

    // create map mask
    uint8_t **mask_map = (uint8_t**)malloc(sizeof(uint8_t*) * map_size);
    for (map_line_index = 0 ; map_line_index < map_size ; map_line_index++){
        mask_map[map_line_index] = (uint8_t*)malloc(sizeof(uint8_t) * map_size);
        for (int map_column_index = 0 ; map_column_index < map_size ; map_column_index++){
            mask_map[map_line_index][map_column_index] = NOT_PROCESSED;
        }
    }

    // count trail score 
    int Score_sum = 0;
    for (map_line_index = 0 ; map_line_index < map_size ; map_line_index++){
        for (int map_column_index = 0 ; map_column_index < map_size ; map_column_index++){
            
            // find trailhead
            if (topography_map[map_line_index][map_column_index] == 0){

                // count map
                if (DEBUG) {printf("\n\t- trailhead (%d, %d)", map_line_index, map_column_index);}
                Score_sum += count_score_rec(&mask_map, topography_map, map_size, map_line_index, map_column_index, RECURSIVE_MAX_DEPTH, -1);

                if(DEBUG){
                    printf("\n\t(%d, %d) ; Score_sum = %d", map_line_index, map_column_index, Score_sum);
                    print_map(mask_map, map_size);
                    system("pause");
                }

                // reset map
                reset_mask_map(&mask_map, map_size);

            } 
        }
    }

    printf("\n\tScore_sum = %d", Score_sum);

    // free memory
    for (int map_line_index = 0 ; map_line_index < map_size ; map_line_index++){
        free(topography_map[map_line_index]);
    }
    free(topography_map);

    return 0;
}