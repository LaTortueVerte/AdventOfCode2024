#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAB_SIZE 140
#define MAX_LINE_LENGTH 256 
#define DEBUG 0

char **textfile_to_tab(char* filename);
int count_occ_in_tab(char** tab, int tab_size, char* pattern);

int main(){

    char **tab = textfile_to_tab("input.txt");
    if (tab == NULL) {
        printf("Error: failed to read the file.\n");
        return 1;
    }

    // print tab 
    if (DEBUG)
    {
        for (int i = 0 ; i < TAB_SIZE ; i++){
            for (int j = 0 ; j < TAB_SIZE ; j++){
                printf("%c ",tab[i][j]);
            }
            printf("\n");
        }
    }

    // count 

    int occ_count = count_occ_in_tab(tab, TAB_SIZE, "XMAS");

    printf("results : %d", occ_count);

    // free memory

    for (int i = 0; i < TAB_SIZE; i++) {
        free(tab[i]);
    }
    free(tab);

    return 0;
}

char **textfile_to_tab(char* filename) {

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Not able to open the file.\n");
        return NULL;
    }

    char **new_tab = (char**)malloc(sizeof(char*) * TAB_SIZE);
    if (new_tab == NULL) {
        perror("Memory allocation failed.\n");
        fclose(file);
        return NULL;
    }

    int line_index = 0;
    char line[MAX_LINE_LENGTH];


    while ((line_index < TAB_SIZE) && (fgets(line, sizeof(line), file))) {

        line[strcspn(line, "\n")] = '\0'; // replace '\n' by '\0';
        new_tab[line_index] = (char*)malloc(sizeof(char) * (strlen(line) + 1));

        if (new_tab[line_index] == NULL) {
            perror("Memory allocation failed for line.\n");
            fclose(file);
            return NULL;
        }

        strcpy(new_tab[line_index++], line);
    }

    fclose(file);
    return new_tab;
}

int count_occ_in_tab(char** tab, int tab_size, char* pattern){

    if (strlen(pattern) == 0 ){
        return 0;
    }

    int occ_count = 0;

    for (int line_index = 0 ; line_index < tab_size ; line_index++){
        for (int col_index = 0 ; col_index < tab_size ; col_index++){

            if(tab[line_index][col_index] == pattern[0]){

                // test the 8 direction

                
                int tab_line_shift_index[8] = {1, 1, 0,-1,-1,-1, 0, 1};
                int tab_col_shift_index[8] =  {0, 1, 1, 1, 0,-1,-1,-1};
                char direction[8][50] =  {"bottom", "bottom right", "right", "top right", "top", "top left", "left", "bottom left"};

                for (int p_index = 0 ; p_index < 8 ; p_index++){

                    int pattern_index = 0;

                    int tab_line_index = line_index;
                    int tab_col_index = col_index;

                    do{
                        pattern_index++;

                        tab_line_index += tab_line_shift_index[p_index];
                        tab_col_index += tab_col_shift_index[p_index];

                    }while (    (tab_line_index >= 0) && (tab_line_index < tab_size) &&
                                (tab_col_index >= 0) && (tab_col_index < tab_size) &&
                                (pattern_index < strlen(pattern)) && 
                                (pattern[pattern_index] == tab[tab_line_index][tab_col_index]));

                    if (pattern_index == strlen(pattern)){
                        printf("start at (%d,%d) in %s direction\n", line_index, col_index, direction[p_index]);
                        occ_count++;
                    }
                }


            }
        }
    }

    return occ_count;
}