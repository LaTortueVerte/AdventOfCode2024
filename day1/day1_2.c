#include <stdio.h>
#include <stdlib.h>

/*

--- Part Two ---
Your analysis only confirmed what everyone feared: the two lists of location IDs are indeed very different.

Or are they?

The Historians can't agree on which group made the mistakes or how to read most of the Chief's handwriting, but in the commotion you notice an interesting detail: a lot of location IDs appear in both lists! Maybe the other numbers aren't location IDs at all but rather misinterpreted handwriting.

This time, you'll need to figure out exactly how often each number from the left list appears in the right list. Calculate a total similarity score by adding up each number in the left list after multiplying it by the number of times that number appears in the right list.

Here are the same example lists again:

3   4
4   3
2   5
1   3
3   9
3   3
For these example lists, here is the process of finding the similarity score:

The first number in the left list is 3. It appears in the right list three times, so the similarity score increases by 3 * 3 = 9.
The second number in the left list is 4. It appears in the right list once, so the similarity score increases by 4 * 1 = 4.
The third number in the left list is 2. It does not appear in the right list, so the similarity score does not increase (2 * 0 = 0).
The fourth number, 1, also does not appear in the right list.
The fifth number, 3, appears in the right list three times; the similarity score increases by 9.
The last number, 3, appears in the right list three times; the similarity score again increases by 9.
So, for these example lists, the similarity score at the end of this process is 31 (9 + 4 + 0 + 0 + 9 + 9).

Once again consider your left and right lists. What is their similarity score?

*/

int main(){

    int list_1[1001];
    int list_2[1001];

    int list_size = -1;

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

    int line_index = 0;

    while(fgets(line, 100, fptr)) {
        if (sscanf(line, "%d %d", &list_1[line_index], &list_2[line_index]) == EOF){
            printf("List are not similar.\n");
            return 1;
        }
        line_index++;
    }

    list_size = line_index;

    fclose(fptr);

    /* ----------------------------------------------------------------------------------------------------------------------
    PROCESS DATA 
    -----------------------------------------------------------------------------------------------------------------------*/

    // compute score

    int score = 0;

    for (int list_1_index = 0 ; list_1_index < list_size ; list_1_index++){

        int occ = 0;
        for (int list_2_index = 0 ; list_2_index < list_size ; list_2_index++){
            if(list_1[list_1_index] == list_2[list_2_index]){
                occ++;
            }
        }
        score += list_1[list_1_index] * occ;
    }

    printf("The score is : %d\n", score);

    system("pause");

    return 0;
}