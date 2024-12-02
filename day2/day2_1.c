#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*

--- Day 2: Red-Nosed Reports ---
Fortunately, the first location The Historians want to search isn't a long walk from the Chief Historian's office.

While the Red-Nosed Reindeer nuclear fusion/fission plant appears to contain no sign of the Chief Historian, the engineers there run up to you as soon as they see you. Apparently, they still talk about the time Rudolph was saved through molecular synthesis from a single electron.

They're quick to add that - since you're already here - they'd really appreciate your help analyzing some unusual data from the Red-Nosed reactor. You turn to check if The Historians are waiting for you, but they seem to have already divided into groups that are currently searching every corner of the facility. You offer to help with the unusual data.

The unusual data (your puzzle input) consists of many reports, one report per line. Each report is a list of numbers called levels that are separated by spaces. For example:

7 6 4 2 1
1 2 7 8 9
9 7 6 2 1
1 3 2 4 5
8 6 4 4 1
1 3 6 7 9
This example data contains six reports each containing five levels.

The engineers are trying to figure out which reports are safe. The Red-Nosed reactor safety systems can only tolerate levels that are either gradually increasing or gradually decreasing. So, a report only counts as safe if both of the following are true:

The levels are either all increasing or all decreasing.
Any two adjacent levels differ by at least one and at most three.
In the example above, the reports can be found safe or unsafe by checking those rules:

7 6 4 2 1: Safe because the levels are all decreasing by 1 or 2.
1 2 7 8 9: Unsafe because 2 7 is an increase of 5.
9 7 6 2 1: Unsafe because 6 2 is a decrease of 4.
9 7 6 2 1: Unsafe because 6 2 is a decrease of 4.
1 3 2 4 5: Unsafe because 1 3 is increasing but 3 2 is decreasing.
8 6 4 4 1: Unsafe because 4 4 is neither an increase or a decrease.
1 3 6 7 9: Safe because the levels are all increasing by 1, 2, or 3.
So, in this example, 2 reports are safe.

Analyze the unusual data from the engineers. How many reports are safe?

*/

enum Order {NONE, INCREASING, DECREASING};

int main(){

    int text_size = 0;

    /* ----------------------------------------------------------------------------------------------------------------------
    GET INPUT 
    -----------------------------------------------------------------------------------------------------------------------*/

    // store the filename of the input of the exercice
    char filename[50] = "input.txt";

    // setup pointer and stuff of the file
    FILE *fptr;
    fptr = fopen(filename, "r");
    if(fptr == NULL) {
        printf("Not able to open the file.\n");
        return 1;
    }

    // variable to store the content of a line
    char line[100];
    // variable to store a new number from a line
    char *new_extract_number;
    char *last_extract_number;

    // variable to store safe count
    int safe_count = 0;

    // variable which stop the safe control
    int is_safe;

    // variable that define the order (increasing or decreasing) of the line
    enum Order line_order; 

    // loop on lines of input.txt
    while(fgets(line, 100, fptr)) {

        // reset order;
        line_order = NONE;
        is_safe = 1;

        printf("new line : %s",line);
        
        // get first number
        new_extract_number = atoi(strtok(line, " "));
        printf("%d ", new_extract_number);
        last_extract_number = new_extract_number;

        while (is_safe && (new_extract_number = atoi(strtok(NULL, " ")))) {

            printf("%d ", new_extract_number);

            if (line_order == NONE){
                line_order = last_extract_number < new_extract_number ? INCREASING : DECREASING;
            }else if ((line_order == INCREASING) && (last_extract_number > new_extract_number) || (line_order == DECREASING) && (last_extract_number < new_extract_number)){
                is_safe = 0;
            }
            int diff = abs(last_extract_number - new_extract_number);
            if ((diff > 3) || (diff == 0)){
                is_safe = 0;
            }

            last_extract_number = new_extract_number;
        }

        if (is_safe){
            safe_count++;
            printf(": safe\n");
        }else{
            printf(": unsafe\n");
        }

        text_size++;

        system("pause");
    }

    fclose(fptr);

    /* ----------------------------------------------------------------------------------------------------------------------
    PROCESS DATA 
    -----------------------------------------------------------------------------------------------------------------------*/


    printf("The safe count is : %d\n", safe_count);

    system("pause");

    return 0;
}