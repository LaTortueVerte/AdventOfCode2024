#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main() {

    FILE *file = fopen("input2.txt", "r");

    if (file == NULL) {
        perror("Not able to open the file.\n");
        return 1;
    }

    char do_pattern[50] = "do()";
    char* do_pattern_ptr = do_pattern;
    char dont_pattern[50] = "don't()";
    char* dont_pattern_ptr = dont_pattern;
    

    char mul_pattern[50] = "mul(";
    char* mul_pattern_ptr = mul_pattern;

    char new_char;

    int sum = 0;

    int do_condition = 1; // enabled by default

    while ((new_char = fgetc(file)) != EOF) {

        // DO PATTERN DETECTION

        if (new_char == *do_pattern_ptr){ // char from do pattern detected
            do_pattern_ptr++;

            if (*do_pattern_ptr == 0){ // pattern detected
                do_condition = 1;
                printf("do()\n");
                do_pattern_ptr = do_pattern;
            }

        } else { // reset pattern detection
            do_pattern_ptr = do_pattern;
        }

        // DONT PATTERN DETECTION

        if (new_char == *dont_pattern_ptr){ // char from do pattern detected
            dont_pattern_ptr++;

            if (*dont_pattern_ptr == 0){ // pattern detected
                do_condition = 0;
                printf("don't()\n");
                dont_pattern_ptr = dont_pattern;
            }

        } else { // reset pattern detection
            dont_pattern_ptr = dont_pattern;
        }

        // MUL PATTERN DETECTION 

        if (do_condition){
            if (new_char == *mul_pattern_ptr){ // char from mul pattern detected
                mul_pattern_ptr++;

                if (*mul_pattern_ptr == 0){ // mul pattern detected

                    int num_1 = 0;
                
                    while (((new_char = fgetc(file)) != EOF) && (isdigit(new_char))) { // assemble first number
                        num_1 *= 10;
                        num_1 += new_char - '0';
                    }

                    if (new_char == ','){ // ',' separator detected 

                        int num_2 = 0;

                        while (((new_char = fgetc(file)) != EOF) && (isdigit(new_char))) { // assemble second number
                            num_2 *= 10;
                            num_2 += new_char - '0';
                        }
                        
                        if (new_char == ')'){ // final ')' detected 
                            sum += num_1 * num_2;

                            printf("mul(%d,%d)\n",num_1, num_2);
                        }
                    }
                    mul_pattern_ptr = mul_pattern;
                }

            } else { // reset pattern detection
                mul_pattern_ptr = mul_pattern;
            }
        }
    }

    printf("Sum = %d", sum);

    fclose(file);
    return 0;
}
