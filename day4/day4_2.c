#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAB_SIZE 140
#define MAX_LINE_LENGTH 256 
#define DEBUG 0

char **textfile_to_tab(char* filename);
int count_X_MAS_in_tab(char** tab, int tab_size);

int main(){

    char **tab = textfile_to_tab("input2.txt");
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

    int occ_count = count_X_MAS_in_tab(tab, TAB_SIZE);

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

int count_X_MAS_in_tab(char** tab, int tab_size){

    int occ_count = 0;

    for (int line_index = 1 ; line_index < tab_size - 1 ; line_index++){
        for (int col_index = 1 ; col_index < tab_size - 1 ; col_index++){

            if(tab[line_index][col_index] == 'A'){

                if (   ((tab[line_index-1][col_index-1] == 'M') && (tab[line_index+1][col_index+1] == 'S') ||
                        (tab[line_index-1][col_index-1] == 'S') && (tab[line_index+1][col_index+1] == 'M') ) &&
                       ((tab[line_index-1][col_index+1] == 'M') && (tab[line_index+1][col_index-1] == 'S') ||
                        (tab[line_index-1][col_index+1] == 'S') && (tab[line_index+1][col_index-1] == 'M') )){

                    occ_count++;
                }
            }
        }
    }

    return occ_count;
}