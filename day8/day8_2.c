#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_LINE_SIZE 512

typedef struct frequency_antenna_list{
    char frequency;     // the frequency of the antennas : lowercase, uppercase or digit
    int nb_of_antennas; // number of antennas 
    int* line_index;    // list of line-positions
    int* column_index;  // list of column-positions
} frequency_antenna_list;

void add_city_map_line(char*** city_map, int* city_map_size, char* new_line){

    // allocate new space for a new line on the city_map
    if (city_map_size == 0){
        (*city_map) = (char**)malloc(sizeof(char*) * ((*city_map_size) + 1));
    } else {
        (*city_map) = (char**)realloc((*city_map), sizeof(char*) * ((*city_map_size) + 1));
    }
    assert((*city_map) != NULL);

    // check if last character is a '\n' and removed it
    if (new_line[strlen(new_line) - 1] == '\n'){
        new_line[strlen(new_line) - 1] = '\0';
    }

    // create a new line on the city_map
    (*city_map)[(*city_map_size)] = (char*)malloc(sizeof(char) * (strlen(new_line) + 1));
    assert((*city_map)[(*city_map_size)] != NULL);

    // fill the new created line 
    strcpy((*city_map)[(*city_map_size)], new_line);

    // increase city_map size by 1
    (*city_map_size)++;
}

frequency_antenna_list *find_freq_in_freq(frequency_antenna_list* freq_list, int freq_list_size, char freq){
    int freq_index = 0;

    // check if freq already exists in freq_list 
    while ((freq_index < freq_list_size) && (freq_list[freq_index].frequency != freq)){
        freq_index++;
    }

    // if freq exists => send
    if (freq_index < freq_list_size){
        return &(freq_list[freq_index]);
    } else {
        return NULL;
    }
}

void add_freq_antenna(frequency_antenna_list** freq_list, int* freq_list_size, char freq, int line_index, int column_index){

    frequency_antenna_list* found_freq = find_freq_in_freq(*freq_list, *freq_list_size, freq);

    // if freq does not exist => create new freq struct
    if (found_freq == NULL){

        if (*freq_list_size == 0){
            (*freq_list) = (frequency_antenna_list*)malloc(sizeof(frequency_antenna_list));
        } else {
            (*freq_list) = (frequency_antenna_list*)realloc((*freq_list), sizeof(frequency_antenna_list) * (*freq_list_size + 1));
        }
        assert((*freq_list) != NULL);

        found_freq = &((*freq_list)[*freq_list_size]);
        (*freq_list_size)++;

        found_freq->column_index = NULL;
        found_freq->line_index = NULL;
        found_freq->frequency = freq;
        found_freq->nb_of_antennas = 0;
    }

    // add freq position to freq struct
    if (found_freq->nb_of_antennas == 0){
        found_freq->line_index = (int*)malloc(sizeof(int));
        found_freq->column_index = (int*)malloc(sizeof(int));
    } else {
        found_freq->line_index = (int*)realloc(found_freq->line_index, sizeof(int) * (found_freq->nb_of_antennas + 1));
        found_freq->column_index = (int*)realloc(found_freq->column_index, sizeof(int) * (found_freq->nb_of_antennas + 1));
    }
    assert(found_freq->line_index != NULL);
    assert(found_freq->column_index != NULL);

    found_freq->line_index[found_freq->nb_of_antennas] = line_index;
    found_freq->column_index[found_freq->nb_of_antennas] = column_index;

    (found_freq->nb_of_antennas)++;
}

void create_antinodes(frequency_antenna_list freq_antenna, int occ_index_1, int occ_index_2, char*** city_map, int city_map_size, int* nb_of_antinode_antennas){

    assert(occ_index_1 >= 0);
    assert(occ_index_1 < freq_antenna.nb_of_antennas);
    assert(occ_index_2 >= 0);
    assert(occ_index_2 < freq_antenna.nb_of_antennas);

    int delta_line = freq_antenna.line_index[occ_index_1] - freq_antenna.line_index[occ_index_2];
    int delta_column = freq_antenna.column_index[occ_index_1] - freq_antenna.column_index[occ_index_2];
    
    // compute antinode position from occurence from the occ_index 1 and 2 from the freq_antenna occurence list
    int antinode_line = freq_antenna.line_index[occ_index_1];
    int antinode_column = freq_antenna.column_index[occ_index_1];

    // check if the antinode position is in the city map
    while ((antinode_line >= 0) && (antinode_line < city_map_size) &&
        (antinode_column >= 0) && (antinode_column < strlen((*city_map)[antinode_line]))){ 

        // check if the antinode position is free or is overlaping an antenna
        if (((*city_map)[antinode_line][antinode_column] == '.') || (((*city_map)[antinode_line][antinode_column] != '.')) && ((*city_map)[antinode_line][antinode_column] != '#')){

            (*city_map)[antinode_line][antinode_column] = '#';
            (*nb_of_antinode_antennas)++;
        }

        antinode_line += delta_line;
        antinode_column += delta_column;
    }
}

int main(){

    // open text file
    FILE *file_pt = fopen("input2.txt","r");
    if (file_pt == NULL){
        printf("Not able to open the file.\n");
        return 1;
    }

    // contains one line of the textfile
    char text_line_buffer[MAX_LINE_SIZE];

    // the constructed map and its size
    char** city_map = NULL;
    int city_map_size = 0;

    // list of all antennas and their number
    frequency_antenna_list* freq_list;
    int freq_list_size = 0;

    // while city_map lines can be extract from text file 
    while (fgets(text_line_buffer, sizeof(text_line_buffer), file_pt)){

        // add line to city_map
        add_city_map_line(&city_map, &city_map_size, text_line_buffer);

        // add each frequency to the freq_list
        for (int char_index = 0 ; char_index < strlen(city_map[city_map_size - 1]) ; char_index++){
            if ((city_map[city_map_size - 1][char_index] != '.') && (city_map[city_map_size - 1][char_index] != '#')){
                add_freq_antenna(   &freq_list, 
                                    &freq_list_size, 
                                    city_map[city_map_size - 1][char_index], 
                                    (city_map_size - 1),
                                    char_index);
            }
        }
    }

    // print initial city_map 
    printf("intial city map : %d lines\n", city_map_size);
    for (int line_index = 0 ; line_index < city_map_size ; line_index++){
        printf("%s\n", city_map[line_index]);
    }

    // print freq list
    printf("\nfreq_list : %d freq\n", freq_list_size);
    for (int freq_index = 0 ; freq_index < freq_list_size ; freq_index++){
        printf("%c : ", freq_list[freq_index].frequency);
        for (int freq_position_index = 0 ; freq_position_index < freq_list[freq_index].nb_of_antennas ; freq_position_index++){
            printf("(%d, %d) ", freq_list[freq_index].line_index[freq_position_index], freq_list[freq_index].column_index[freq_position_index]);
        }
        printf("\n");
    }

    system("pause");

    // init antinode antenna counter
    int nb_of_antinode_antennas = 0;

    // for each frequency detected on the city map (from freq_list)
    for (int freq_index = 0 ; freq_index < freq_list_size ; freq_index++){

        // for each couple of antenna of the same frequency
        for (int occ_index_1 = 0 ; occ_index_1 < freq_list[freq_index].nb_of_antennas ; occ_index_1++){
            for (int occ_index_2 = occ_index_1 + 1; occ_index_2 < freq_list[freq_index].nb_of_antennas ; occ_index_2++){
                if (occ_index_1 != occ_index_2){

                    // add antinodes
                    create_antinodes(freq_list[freq_index], occ_index_1, occ_index_2, &city_map, city_map_size, &nb_of_antinode_antennas);
                    create_antinodes(freq_list[freq_index], occ_index_2, occ_index_1, &city_map, city_map_size, &nb_of_antinode_antennas);

                }
            }
        }
    }

    // print final city_map 
    printf("final city map : %d antinode antennas\n", nb_of_antinode_antennas);
    for (int line_index = 0 ; line_index < city_map_size ; line_index++){
        printf("%s\n", city_map[line_index]);
    }

    // free space 

    for (int line_index = 0 ; line_index < city_map_size ; line_index++){
        free(city_map[line_index]);
    }
    free(city_map);

    for (int freq_index = 0; freq_index < freq_list_size ; freq_index++){
        free(freq_list[freq_index].line_index);
        free(freq_list[freq_index].column_index);
    }
    free(freq_list);

    return 0;
}