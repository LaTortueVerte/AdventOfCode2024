#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#define MAX_LINE_SIZE 512
#define PRINT_SHORT 1
#define DEBUG 0

typedef enum DISK_TYPE {FREE_SPACE, FILES} DISK_TYPE;

typedef struct disk_part{
    unsigned int size;
    unsigned int id;
    struct disk_part *next; 
    struct disk_part *prev; 
}disk_part;


void add_char_to_disk_char(disk_part** disk_map, int id, int size){

    if (size != 0) {
        
        // alocate new space 
        disk_part *new_disk_part = (disk_part*)malloc(sizeof(disk_part));
        assert(new_disk_part != NULL);

        // find end of the list
        if (*disk_map != NULL){
            disk_part *disk_part_pt = *disk_map;
            while (disk_part_pt->next != NULL){
                disk_part_pt = disk_part_pt->next;
            }
            disk_part_pt->next = new_disk_part;
            new_disk_part->prev = disk_part_pt;
        } else {
            *disk_map = new_disk_part;
            new_disk_part->prev = NULL;
        }

        // add new data
        new_disk_part->id = id;
        new_disk_part->size = size;
        new_disk_part->next = NULL;
    }
}

void print_disk_map(disk_part* disk_map){

    // print disk map
    disk_part *disk_part_pt = disk_map;
    while (disk_part_pt != NULL){

        if (disk_part_pt->id == -1){
            if (!PRINT_SHORT){
                for (int print_i = 0 ; print_i < disk_part_pt->size ; print_i++){
                    printf(".");
                }
            } else {
                printf("'.'(%u) - ", disk_part_pt->size);
            }
        } else {
            if (!PRINT_SHORT){
                for (int print_i = 0 ; print_i < disk_part_pt->size ; print_i++){
                    printf("%u", disk_part_pt->id);
                }
            } else {
                printf("%u(%u) - ", disk_part_pt->id, disk_part_pt->size);
            }
        }

        disk_part_pt = disk_part_pt->next;
    }
}


int main(){
    
    // open text file
    FILE *file_pt = fopen("input.txt","r");
    if (file_pt == NULL){
        printf("Not able to open the file.\n");
        return 1;
    }

    // contains the new_char
    char new_char = fgetc(file_pt);

    // contains the disk map and its size
    disk_part* disk_map = NULL;

    int used_disk_size = 0;
    int total_disk_size = 0;

    // get disk map from text file
    DISK_TYPE disk_type = FILES;
    int file_index = 0;
    while ((new_char != EOF) && (new_char != '\n')){

        assert(isdigit(new_char));

        int disk_part_size = (new_char - '0');
        
        if (disk_type == FILES){
            
            add_char_to_disk_char(&disk_map, file_index, disk_part_size);
            used_disk_size += disk_part_size;
            disk_type = FREE_SPACE;
            file_index++;

        } else {

            add_char_to_disk_char(&disk_map, -1, disk_part_size);
            disk_type = FILES;
        }

        total_disk_size += disk_part_size;
        new_char = fgetc(file_pt);
    }
    assert(disk_map != NULL);

    print_disk_map(disk_map);
    system("pause");

    // reorganize files
    int stop_condition = 0;
    while (!stop_condition){

        // get 'last file' and 'first free space' from disk map
        disk_part *disk_part_pt = disk_map;
        disk_part *last_file = NULL;
        disk_part *first_free_space = NULL;
        while (disk_part_pt != NULL){

            // get first free space 
            if (first_free_space == NULL){
                if (disk_part_pt->id == -1){
                    first_free_space = disk_part_pt;
                }

            // get last file
            } else if (disk_part_pt->id != -1){
                last_file = disk_part_pt;
            }

            // browse disk map
            disk_part_pt = disk_part_pt->next;
        }

        // check if first free space emplacement is find
        if ((first_free_space != NULL) && (last_file != NULL)){
            // first free emplacement is found

            // fill first hole (3 possibilities)
            if (first_free_space->size > last_file->size){

                // place last file before first free space
                assert(last_file->prev != NULL);
                last_file->prev->next = last_file->next;
                if (last_file->next != NULL){
                    last_file->next->prev = last_file->prev;
                } 
                if (first_free_space->prev != NULL){
                    first_free_space->prev->next = last_file;
                    last_file->prev = first_free_space->prev;
                } else {
                    last_file->prev = NULL;
                }
                last_file->next = first_free_space;
                first_free_space->prev = last_file;

                // reduce size of free space size
                first_free_space->size -= last_file->size;

            } else {

                first_free_space->id = last_file->id;

                if (first_free_space->size < last_file->size){

                    // a part of the last file fill the entire first free space
                    last_file->size -= first_free_space->size;

                } else { // first_free_space->size == last_file->size

                    // the last file fill the first free space : destroy last part
                    assert(last_file->prev != NULL);
                    last_file->prev->next = last_file->next;
                    if (last_file->next != NULL){
                        last_file->next->prev = last_file->prev;
                    } 

                    free(last_file);
                }
            }

        } else {
            // there is no more free space
            stop_condition = 1;
        }

        if (DEBUG){
            print_disk_map(disk_map);
            system("pause");
        }    
    }

    printf("\nEnd result : ");
    print_disk_map(disk_map);

    // compute result
    unsigned long long int result = 0;
    int coef = 0;
    disk_part *disk_part_pt = disk_map;
    while ((disk_part_pt != NULL) && (disk_part_pt->id != -1)){
        unsigned long long int add = disk_part_pt->id * (disk_part_pt->size * coef + (disk_part_pt->size - 1) * disk_part_pt->size / 2); ;
        if (!DEBUG){printf("\nAdd {id:%u ; size:%u} => %lld to result", disk_part_pt->id, disk_part_pt->size, add);}
        result += add;
        coef += disk_part_pt->size;
        disk_part_pt = disk_part_pt->next;
    }

    printf("\nS : %llu",  result);
}