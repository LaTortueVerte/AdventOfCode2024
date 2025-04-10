#include <diophantine_equation_solver.h>

equation_2_variables* diophantine_equation_solver(equation_2_variables* equation_to_solve){
    /* find a solution couple (x,y) E */

    // --------------------------------------------------------------------------------
    // compute bezout coef u and v and pgcd(a,b)

    int pgcd, u, v;
    bezou_algo( equation_to_solve->a,
                equation_to_solve->b, 
                &u, 
                &v,
                &pgcd);
    
    #ifdef DEBUG
        printf("\n>> u = %d ; v = %d ; pgcd = %d", u_x, v_x, pgcd_x);
    #endif

    // is c divisible by the PGCD(a, b) continue
    if (equation_to_solve->c % pgcd != 0){
        #ifdef DEBUG
            printf("\nle pgcd de xa et xb ne divise pas xf");
        #endif
        return 0;
    }

    // --------------------------------------------------------------------------------
    // compute general form of button a and button b for c
    // a_k = u * c / pgcd(a, b) + k * b / pgcd(a, b) = a_0 + m_a * k

    equation_2_variables* a_equation = (equation_2_variables*)malloc(sizeof(equation_2_variables));
    
    int64_t a_0 = u * equation_to_solve->c / pgcd;
    int64_t m_a = equation_to_solve->b / pgcd;

    #ifdef DEBUG
        printf("\na_k_x = %lld + k * %lld", A_a_k_x, B_a_k_x);
    #endif

    // b_k = v * c / pgcd(a, b) + k * a / pgcd(a, b) = b_0 + m_b * k

    int64_t b_0 = v * equation_to_solve->c / pgcd;
    int64_t m_b = equation_to_solve->a / pgcd;

    #ifdef DEBUG
        printf("\nb_k_x = %lld - k * %lld", A_b_k_x, B_b_k_x);
    #endif


}

void bezou_algo(int a, int b, int* u, int* v, int* pgcd){
    /* compute the pgcd(a,b) and the couple (u,v) which is solution of the equation a*u + b*v = pgcd(a,b)*/

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
