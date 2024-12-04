#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAB_SIZE 10
#define MAX_LINE_LENGTH 100  // Longueur maximale pour une ligne

char **textfile_to_tab(char* filename);

int main() {

    char **tab = textfile_to_tab("input2.txt");

    if (tab == NULL) {
        printf("Error: failed to read the file.\n");
        return 1;
    }

    // Affichage des caractères
    for (int i = 0; i < TAB_SIZE; i++) {
        for (int j = 0; j < TAB_SIZE; j++) {
            printf("%c ", tab[i][j]);
        }
        printf("\n");
    }

    // Libération de la mémoire
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

    // Allocation mémoire pour TAB_SIZE lignes
    char **new_tab = (char**)malloc(sizeof(char*) * TAB_SIZE);
    if (new_tab == NULL) {
        perror("Memory allocation failed.\n");
        fclose(file);
        return NULL;
    }

    int line_index = 0;
    char line[MAX_LINE_LENGTH];

    // Lire chaque ligne du fichier
    while (line_index < TAB_SIZE && fgets(line, sizeof(line), file)) {
        // Enlever le caractère de fin de ligne '\n' si présent
        line[strcspn(line, "\n")] = '\0';  // Remplacer le '\n' par '\0'

        // Allouer de la mémoire pour chaque ligne lue, incluant l'espace pour le caractère '\0'
        new_tab[line_index] = (char*)malloc(sizeof(char) * (strlen(line) + 1));

        if (new_tab[line_index] == NULL) {
            perror("Memory allocation failed for line.\n");
            fclose(file);
            return NULL;
        }

        // Copier la ligne dans le tableau
        strcpy(new_tab[line_index], line);
        line_index++;
    }

    fclose(file);
    return new_tab;
}
