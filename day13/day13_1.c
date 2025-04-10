#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <math.h>

// GLOBAL

#define FILENAME "input.txt"
#define MAX_BUTTON_VALUE 100
#define DEBUG

typedef struct Claw_machine{

    // a button 
    int a_button;
    int a_x;
    int a_y;

    // b button
    int b_button;
    int b_x;
    int b_y;

    // prize location
    int prize_x;
    int prize_y;

} Claw_machine;

// PROTOTYPES

Claw_machine* create_claw_machine(int a_x, int a_y, int b_x, int b_y, int prize_x, int prize_y);
Claw_machine* get_new_claw_machine_from_file(FILE* file_ptr);

int compute_button_command(Claw_machine* Claw_machine);

void print_claw_machine(Claw_machine *claw_machine);

void bezou_algo(int a, int b, int* u, int* v, int* pgcd);

void copy_list(int** list_src, int** list_dest, int size);

int get_integer_up(float nb);

int get_integer_below(float nb);

// FUNCTIONS

Claw_machine* create_claw_machine(int a_x, int a_y, int b_x, int b_y, int prize_x, int prize_y){

    Claw_machine* new_cmd = (Claw_machine*)malloc(sizeof(Claw_machine));
    assert(new_cmd != NULL);

    new_cmd->a_button = 0;
    new_cmd->a_x = a_x;
    new_cmd->a_y = a_y;
    new_cmd->b_button = 0;
    new_cmd->b_x = b_x;
    new_cmd->b_y = b_y;
    new_cmd->prize_x = prize_x;
    new_cmd->prize_y = prize_y;

    return new_cmd;
}

Claw_machine* get_new_claw_machine_from_file(FILE* file_ptr){

    int a_x, a_y, b_x, b_y, p_x, p_y;
    
    char line[256];

    // get A command
    if (!fgets(line, 100, file_ptr)){
        return NULL;
    }
    assert(strncmp(line, "Button A:", 9) == 0);
    sscanf(line, "Button A: X+%d, Y+%d", &a_x, &a_y);

    // get B command
    assert(fgets(line, 100, file_ptr));
    assert(strncmp(line, "Button B:", 9) == 0);
    sscanf(line, "Button B: X+%d, Y+%d", &b_x, &b_y);
    
    // get Prize
    assert(fgets(line, 100, file_ptr));
    assert(strncmp(line, "Prize:", 6) == 0);
    sscanf(line, "Prize: X=%d, Y=%d", &p_x, &p_y);

    // line jump
    fgets(line, 100, file_ptr);

    // create newq claw machine
    Claw_machine* new_claw_machine = create_claw_machine(a_x, a_y, b_x, b_y, p_x, p_y);

    return new_claw_machine;

}

void print_claw_machine(Claw_machine* claw_machine){

    printf("\n\nButton A -> X: %d, Y: %d", claw_machine->a_x, claw_machine->a_y);
    printf("\nButton B -> X: %d, Y: %d", claw_machine->b_x, claw_machine->b_y);
    printf("\nPrize -> X: %d, Y: %d", claw_machine->prize_x, claw_machine->prize_y);
}

void bezou_algo(int a, int b, int* u, int* v, int* pgcd){

    assert(a >= 0);
    assert(b >= 0);

    int* line_1 = (int*)malloc(sizeof(int) * 3);
    line_1[0] = a; 
    line_1[1] = 1; 
    line_1[2] = 0; 

    int* line_2 = (int*)malloc(sizeof(int) * 3);
    line_2[0] = b; 
    line_2[1] = 0; 
    line_2[2] = 1; 
    
    int* line_copy_tmp = (int*)malloc(sizeof(int) * 3);

    int quotient, i;

    printf("\n%d %d %d", line_1[0], line_1[1], line_1[2]);

    while (line_2[0] != 0){
        printf("\n%d %d %d", line_2[0], line_2[1], line_2[2]);

        quotient = line_1[0] / line_2[0];

        copy_list(&line_2, &line_copy_tmp, 3);

        for (i = 0 ; i < 3 ; i++){
            line_2[i] = line_1[i] - quotient * line_2[i];
        }

        copy_list(&line_copy_tmp, &line_1, 3);
    }

    *pgcd = line_1[0];
    *u = line_1[1];
    *v = line_1[2];

    printf("\n%d %d %d", line_1[0], line_1[1], line_1[2]);

    free(line_1);
    free(line_2);
    free(line_copy_tmp);
}

void copy_list(int** list_src, int** list_dest, int size){
    for (int i = 0 ; i < 3 ; i++){
        (*list_dest)[i] = (*list_src)[i];
    }
}

int get_integer_up(float nb){
    if (nb == (int)nb){
        return (int)nb;
    } else {
        return ((int)nb) + 1;
    }
}

int get_integer_below(float nb){
    return (int)nb;
}

int compute_button_command(Claw_machine* claw_machine){

    // --------------------------------------------------------------------------------
    // comput bezout coef u and v and pgcd


    int pgcd_x, u_x, v_x;
    bezou_algo( claw_machine->a_x,
                claw_machine->b_x, 
                &u_x, 
                &v_x,
                &pgcd_x);

    printf("\n>> u = %d ; v = %d ; pgcd = %d", u_x, v_x, pgcd_x);

    // is xf divisible by the PGCD(a_x, b_x) continue
    if (claw_machine->prize_x % pgcd_x != 0){
        #ifdef DEBUG
            printf("\nle pgcd de xa et xb ne divise pas xf");
        #endif
        return 0;
    }

    // --------------------------------------------------------------------------------
    // compute general form of button a and button b for xf
    // a_k = a_0 + k_x * x_b / pgcd(a_x, b_x)   =   A_a_k_x + B_a_k_x * k_x

    int A_a_k_x = u_x * claw_machine->prize_x / pgcd_x;
    int B_a_k_x = claw_machine->b_x / pgcd_x;

    #ifdef DEBUG
        printf("\na_k_x = %d + k * %d", A_a_k_x, B_a_k_x);
    #endif

    // b_k = b_0 + k_x * x_a / pgcd(a_x, b_x)   =   A_b_k_x + B_b_k_x * k_x

    int A_b_k_x = v_x * claw_machine->prize_x / pgcd_x;
    int B_b_k_x = claw_machine->a_x / pgcd_x;

    #ifdef DEBUG
        printf("\nb_k_x = %d - k * %d", A_b_k_x, B_b_k_x);
    #endif

    // --------------------------------------------------------------------------------
    // compute frame of k_x

    int min_a_k_x = get_integer_up((- u_x * claw_machine->prize_x) / claw_machine->b_x);
    int max_a_k_x = get_integer_below((100 * pgcd_x - u_x * claw_machine->prize_x) / claw_machine->b_x);

    int min_b_k_x = get_integer_up((v_x * claw_machine->prize_x - 100 * pgcd_x) / claw_machine->a_x);
    int max_b_k_x = get_integer_below((v_x * claw_machine->prize_x) / claw_machine->a_x);

    printf("\na_k_x = [%d ; %d], b_k_x = [%d ; %d]", min_a_k_x, max_a_k_x, min_b_k_x, max_b_k_x);

    // quit if frames don't intersect
    if ((min_a_k_x > max_b_k_x) || (min_b_k_x > max_a_k_x)){
        #ifdef DEBUG
            printf("\nframes a_k_x and b_k_x do not cross");
        #endif
        return 0;
    } 

    int min_k_x = (min_a_k_x > min_b_k_x) ? min_a_k_x : min_b_k_x; // get max min
    int max_k_x = (max_a_k_x > max_b_k_x) ? max_b_k_x : max_a_k_x; // get min max

    // --------------------------------------------------------------------------------
    // get the right couple (a_k, b_k)

    int solution_exist = 0;
    int min_cost_function, a_k, b_k;
    for (int k_x = min_k_x ; k_x <= max_k_x ; k_x++){

        int a_k_x = A_a_k_x + B_a_k_x * k_x;
        int b_k_x = A_b_k_x - B_b_k_x * k_x;

        #ifdef DEBUG
            printf("\nk = %d ; a_k_x = %d ; b_k_x = %d", k_x, a_k_x, b_k_x);
        #endif

        if ((a_k_x * claw_machine->a_y + b_k_x * claw_machine->b_y == claw_machine->prize_y) &&
            ((solution_exist == 0) || (min_cost_function > (a_k_x + b_k_x)))){

            #ifdef DEBUG
                printf(" save");
            #endif
                
            a_k = a_k_x;
            b_k = b_k_x;
            min_cost_function = a_k + b_k;
            solution_exist = 1;
        }
    }

    if (solution_exist){

        claw_machine->a_button = a_k;
        claw_machine->b_button = b_k;

        #ifdef DEBUG
            printf("\nsolution found : (%d, %d)", a_k, b_k);
        #endif

        return 1;

    } else {
        #ifdef DEBUG
            printf("\nno solutions found");
        #endif
        return 0;
    }

}

// MAIN

int main(){

    // open file containing the claw machine configurations
    FILE* file_ptr = fopen(FILENAME, "r");
    assert(file_ptr != NULL);

    // init number of tokens
    uint32_t nb_of_token = 0;    

    // Get all claw machines one by one
    Claw_machine* claw_machine = get_new_claw_machine_from_file(file_ptr);
    while (claw_machine != NULL){
        print_claw_machine(claw_machine);

        // -------------------------------------------------------
        // A & B COMPUTE PART
        // -------------------------------------------------------

        if (compute_button_command(claw_machine)){
            nb_of_token += claw_machine->a_button * 3 + claw_machine->b_button;
        }

        // -------------------------------------------------------
        // END OF : A & B COMPUTE PART
        // -------------------------------------------------------

        // get new claw machine
        free(claw_machine);
        claw_machine = get_new_claw_machine_from_file(file_ptr);

    }

    printf("\n Number of tokens : %d", nb_of_token);

    return 1;
}