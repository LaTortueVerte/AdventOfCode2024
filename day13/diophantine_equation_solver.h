#ifndef DIOPHANTINE_EQUATION_SOLVER_H_
#define DIOPHANTINE_EQUATION_SOLVER_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct equation_2_variables{
    // a * x + b * y = c
    int64_t a;
    int64_t b;
    int64_t c;
} equation_2_variables;

int diophantine_equation_solver(equation_2_variables* equation_to_solve);

bezou_algo(int a, int b, int* u, int* v, int* pgcd);

#endif