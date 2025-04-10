#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <math.h>

// GLOBAL

#define FILENAME "input.txt"
#define MAX_BUTTON_VALUE 100
#define MAX_LOOP 10000000000000
#define DEBUG

typedef struct Claw_machine{

    // a button 
    int64_t a_button;
    int a_x;
    int a_y;

    // b button
    int64_t b_button;
    int b_x;
    int b_y;

    // prize location
    int64_t prize_x;
    int64_t prize_y;

} Claw_machine;
 

// PROTOTYPES

Claw_machine* create_claw_machine(int a_x, int a_y, int b_x, int b_y, int prize_x, int prize_y);
Claw_machine* get_new_claw_machine_from_file(FILE* file_ptr);

int compute_button_command(Claw_machine* Claw_machine);

void print_claw_machine(Claw_machine *claw_machine);

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
    printf("\nPrize -> X: %lld, Y: %lld", claw_machine->prize_x, claw_machine->prize_y);
}

int compute_button_command(Claw_machine* claw_machine){

    int64_t determinant = claw_machine->a_x * claw_machine->b_y - claw_machine->a_y * claw_machine->b_x;
    
    if (determinant == 0){
        printf("\ninfinite number of solution");
        system("pause");

        // need diophantine equation solver
    }

    // compute B

    int64_t e1 = claw_machine->a_x * claw_machine->prize_y - claw_machine->prize_x * claw_machine->a_y;

    if ((e1 % determinant) != 0){
        return 0;
    }

    claw_machine->b_button = e1 / determinant;

    // compute A 

    e1 = claw_machine->prize_x - claw_machine->b_button * claw_machine->b_x;
    if ((e1 % claw_machine->a_x) != 0){
        return 0;
    }

    claw_machine->a_button = e1 / claw_machine->a_x;

    // debug

    #ifdef DEBUG
        printf("\nsolution = [%lld, %lld]", claw_machine->a_button, claw_machine->b_button);
    #endif

    assert((claw_machine->a_button * claw_machine->a_x + claw_machine->b_button * claw_machine->b_x) == claw_machine->prize_x);
    assert(claw_machine->a_button >= 0);
    assert(claw_machine->b_button >= 0);

    return 1;
}

// MAIN

int main(){

    // open file containing the claw machine configurations
    FILE* file_ptr = fopen(FILENAME, "r");
    assert(file_ptr != NULL);

    // init number of tokens
    uint64_t nb_of_token = 0;    

    // Get all claw machines one by one
    Claw_machine* claw_machine = get_new_claw_machine_from_file(file_ptr);
    while (claw_machine != NULL){

        claw_machine->prize_x += 10000000000000;
        claw_machine->prize_y += 10000000000000;

        print_claw_machine(claw_machine);

        // -------------------------------------------------------
        // A & B COMPUTE PART
        // -------------------------------------------------------

        if (compute_button_command(claw_machine)){
            uint64_t add = claw_machine->a_button * 3 + claw_machine->b_button;
            printf("\n=> add %llu tokens to total : %llu", add, nb_of_token);
            nb_of_token += add;
        } else {

            #ifdef DEBUG
                printf("\n=> No solutions ");
            #endif
        }

        // -------------------------------------------------------
        // END OF : A & B COMPUTE PART
        // -------------------------------------------------------

        //getchar();
        
        // get new claw machine
        free(claw_machine);
        claw_machine = get_new_claw_machine_from_file(file_ptr);

    }

    printf("\n Number of tokens : %lld", nb_of_token);

    return 1;
}


//(20000000016800 --1818181819708 * 11)
//