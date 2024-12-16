#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#define MAX_NUMBERS_ON_A_LINE 50
#define NUMBER_OF_OPERATORS 2 
#define DEBUG 0

int main(){

    FILE* file_pt = fopen("input.txt","r");

    if (file_pt == NULL){
        printf("Not able to open the file.\n");
        return 1;
    }

    int nb_of_lines = 0;

    unsigned long long int potential_result;
    int numbers[MAX_NUMBERS_ON_A_LINE];
    int number_count;

    unsigned long long int Sum = 0;

    while (fscanf(file_pt, "%llu:", &potential_result) != EOF){

        /* --------------------------- GET LIST ---------------------------------- */

        printf("Potential result : %llu =? ", potential_result);

        number_count = 0;

        while (fscanf(file_pt, "%d", &numbers[number_count]) == 1){
            number_count++;
            if (fgetc(file_pt) == '\n') {
                break;
            }
        }

        assert(number_count != 0);

        for (int i = 0; i < number_count; i++) {
            printf("%d, ", numbers[i]);
        }

        printf("\n");

        /* ------------------------- PROCESS LIST ------------------------------- */

        if (number_count > 0){

            unsigned long long int computed_result = 0;
            
            int compute_code = 0;

            while ((compute_code < pow(NUMBER_OF_OPERATORS, number_count - 1)) && (computed_result != potential_result)){
                
                computed_result = numbers[0];
                if (DEBUG){printf("%d ", numbers[0]);}

                int code_in_process = compute_code;

                for (int number_index = 1 ; number_index < number_count ; number_index++){

                    if (code_in_process % NUMBER_OF_OPERATORS == 0){
                        computed_result *= numbers[number_index];
                        if (DEBUG){printf("* %d ", numbers[number_index]);}
                        
                    } else {
                        computed_result += numbers[number_index];
                        if (DEBUG){printf("+ %d ", numbers[number_index]);}
                    }

                    code_in_process /= NUMBER_OF_OPERATORS;

                    // Stop early if the result exceeds the target
                    if (computed_result > potential_result) {
                        break;
                    }
                }

                if (DEBUG){printf("= %llu : %llu\n", computed_result, potential_result);}

                compute_code++;
            } 

            if (computed_result == potential_result){
                if (DEBUG){printf("Computation found : %llu == %llu\n",computed_result, potential_result);}
                Sum += computed_result;
            } else if (DEBUG){
                printf("Computation not found %llu != %llu\n", computed_result, potential_result);
            }
        }
        printf("S = %llu\n", Sum);
        assert(Sum > 0);
        if(DEBUG){system("pause");}

        nb_of_lines++;
    }
    printf("Nb of lines = %d\n", nb_of_lines);
    printf("S = %llu\n", Sum);

    fclose(file_pt);

    return 0;
}