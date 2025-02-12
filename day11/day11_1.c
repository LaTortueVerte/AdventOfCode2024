#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>

#define NB_OF_REPETITIONS 25

typedef struct Int_List{
    unsigned long long int* list;
    int size;
}Int_List;

Int_List *create_Int_List(){
    Int_List* new_list = (Int_List*)malloc(sizeof(Int_List));
    new_list->list = NULL;
    new_list->size = 0;

    return new_list;
}

void add_to_list(Int_List **number_list, unsigned long long int new_element){
    if((*number_list)->size == 0){
        (*number_list)->list = (unsigned long long int*)malloc(sizeof(unsigned long long int));
    } else {
        (*number_list)->list = (unsigned long long int*)realloc((*number_list)->list, ((*number_list)->size + 1) * sizeof(unsigned long long int));
    }

    ((*number_list)->list)[(*number_list)->size] = new_element;
    ((*number_list)->size)++;
}

void print_Int_List(Int_List *number_list){
    printf("\n");
    for (int i = 0 ; i < number_list->size ; i++){
        printf("%llu ", number_list->list[i]);
    }
}

int get_number_of_digits(unsigned long long int number){
    assert(number >= 0);

    int nb_of_digits = 1;
    while (number > 9){
        number /= 10;
        nb_of_digits++;
    }

    return nb_of_digits;
}

int main(){

    // build the number list 
    FILE* file_ptr = fopen("input.txt","r");
    assert(file_ptr != NULL); 

    int curr_c;
    int in_coming_int = 0;
    Int_List* number_list = create_Int_List();

    while((curr_c = fgetc(file_ptr)) != EOF){
        if (isdigit(curr_c)){
            in_coming_int *= 10;
            in_coming_int += curr_c - '0';
        } else { 
            add_to_list(&number_list, in_coming_int);
            in_coming_int = 0;
        }
    }

    print_Int_List(number_list);
    fclose(file_ptr);

    // blink
    for (int lap_index = 0 ; lap_index < NB_OF_REPETITIONS ; lap_index++){
        printf("\nLap n°%d",lap_index);

        int nb_index = 0;
        while (nb_index < number_list->size){

            //printf("\n\t%llu : ", number_list->list[nb_index]);
            
            if (number_list->list[nb_index] == 0){
                //printf("is 0");

                number_list->list[nb_index] = 1;

            } 
            else {
                
                int nb_of_digits = get_number_of_digits(number_list->list[nb_index]);
                //printf("(nb of digits : %d) ",nb_of_digits);

                if (nb_of_digits % 2 == 0){
                    //printf("cut in 2 :");
                    
                    // compute first number
                    unsigned long long int first_nb = number_list->list[nb_index];
                    unsigned long long int ten_pow = 1;
                    for (int n = 0 ; n < nb_of_digits/2 ; n++){
                        ten_pow *= 10;
                    }
                    first_nb /= ten_pow; 

                    // compute second number
                    unsigned long long int second_nb = number_list->list[nb_index] - first_nb * ten_pow;

                    // insert the numbers into the number_list
                    add_to_list(&number_list, 0);

                    for (int n = number_list->size - 2 ; n > nb_index ; n--){
                        number_list->list[n+1] = number_list->list[n];
                    }

                    number_list->list[nb_index] = first_nb;
                    number_list->list[nb_index + 1] = second_nb;
                    //printf("\n\t\tadd %llu & %llu", first_nb, second_nb);

                    nb_index++;

                } else {
                    //printf(" mult by 2024");
                    number_list->list[nb_index] *= 2024;
                }
            }
            nb_index++;
        }
        //print_Int_List(number_list);
    }

    printf("\nlist size : %d", number_list->size);

    free(number_list->list);
    free(number_list);
    
    return 0;
}