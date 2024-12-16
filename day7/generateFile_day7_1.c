#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_NUMBER 1000  // 10^13
#define NUM_LINES 20  // Nombre de lignes à générer
#define MAX_NB_OF_NUMBER 10
#define MIN_NB_OF_NUMBER 3

// Fonction pour générer un nombre aléatoire entre 1 et MAX_NUMBER
long long rand_num() {
    return (rand() % (MAX_NUMBER - 1)) + 1;
}

// Fonction pour générer une opération aléatoire entre l'addition et la multiplication
char rand_op() {
    return (rand() % 2 == 0) ? '+' : '*';
}

// Fonction pour calculer le résultat d'une opération de gauche à droite
long long calculate(long long *numbers, int size, char *ops) {
    long long result = numbers[0];
    for (int i = 0; i < size - 1; ++i) {
        if (ops[i] == '+') {
            result += numbers[i + 1];
        } else if (ops[i] == '*') {
            result *= numbers[i + 1];
        }
    }
    return result;
}

int main() {
    srand(time(NULL));  // Initialisation de la graine pour la génération aléatoire

    FILE *file = fopen("output.txt", "w");
    if (!file) {
        printf("Erreur d'ouverture du fichier.\n");
        return 1;
    }

    // Générer plusieurs lignes
    for (int line = 0; line < NUM_LINES; ++line) {
        int num_count = rand() % (MAX_NB_OF_NUMBER - MIN_NB_OF_NUMBER + 1) + MIN_NB_OF_NUMBER;  // Nombre de termes entre 3 et 7
        long long numbers[num_count];
        char ops[num_count - 1];

        // Générer les nombres aléatoires et les opérations
        for (int i = 0; i < num_count; ++i) {
            numbers[i] = rand_num();
            if (i < num_count - 1) {
                ops[i] = rand_op();
            }
        }

        // Calculer le résultat de l'opération
        long long result = calculate(numbers, num_count, ops);

        // Écrire dans le fichier, avec les nombres, le résultat et les opérations
        fprintf(file, "%lld: ", result);
        
        // Afficher les nombres et les opérations dans le fichier
        for (int i = 0; i < num_count; ++i) {
            fprintf(file, "%lld", numbers[i]);
            if (i < num_count - 1) {
                fprintf(file, " ");
            }
        }
        fprintf(file, "\n");

        // Afficher l'opération dans le format désiré
        printf("%lld : ", result);
        for (int i = 0; i < num_count; ++i) {
            printf("%lld", numbers[i]);
            if (i < num_count - 1) {
                printf(" %c ", ops[i]);  // Afficher les opérateurs entre les nombres
            }
        }
        printf("\n");
    }

    fclose(file);

    printf("Fichier 'output.txt' cree avec succes.\n");

    return 0;
}
