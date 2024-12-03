#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main() {

    FILE *file = fopen("input.txt", "r");

    if (file == NULL) {
        perror("Not able to open the file.\n");
        return 1;
    }

    char char_pattern[50] = "mul(";
    char* pattern_ptr = char_pattern;

    char new_char;

    int sum = 0;

    while ((new_char = fgetc(file)) != EOF) {

        if (new_char == *pattern_ptr){ // char from pattern detected
            pattern_ptr++;

            if (*pattern_ptr == 0){ // pattern detected

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
                pattern_ptr = char_pattern;
            }

        } else { // reset pattern detection
            pattern_ptr = char_pattern;
        }
    }

    printf("Sum = %d", sum);

    fclose(file);
    return 0;
}
