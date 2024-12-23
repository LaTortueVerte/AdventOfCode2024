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
    printf("\n");
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
    FILE *file_pt = fopen("input2.txt","r");
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

    // create end of disk map :
    disk_part *disk_part_pt = disk_map;
    disk_part* end_of_disk_map = NULL;
    while (disk_part_pt != NULL){

        if (disk_part_pt->id != -1){
            end_of_disk_map = disk_part_pt;
        }
        // browse disk map
        disk_part_pt = disk_part_pt->next;
    }

    // print disk map
    print_disk_map(disk_map);
    system("pause");

    // reorganize files
    while (end_of_disk_map != NULL){

        if(DEBUG){printf("\nEnd disk map {id:%u ; size:%u}", end_of_disk_map->id, end_of_disk_map->size);}

        // Search next end_of_disk_map (for next loop lap)
        disk_part *next_end_of_disk_map = end_of_disk_map->prev;
        while ((next_end_of_disk_map != NULL) && (next_end_of_disk_map->id == -1)){
            // browse disk map
            next_end_of_disk_map = next_end_of_disk_map->prev;
        }    

        // get 'first free space' which can contains 'end_of_disk_map'
        disk_part_pt = disk_map;
        while ((disk_part_pt != NULL) && ((disk_part_pt != end_of_disk_map)) && ((disk_part_pt->id != -1) || (disk_part_pt->size < end_of_disk_map->size))){
            // browse disk map
            disk_part_pt = disk_part_pt->next;
        }    

        // check searching results    
        if ((disk_part_pt == NULL) || (disk_part_pt == end_of_disk_map)){
            // no free space found
            if(DEBUG){printf("\nNo free space found");}

        } else {
            // a free space was found
            if(DEBUG){printf("\nFree space found {id:%u ; size:%u}", disk_part_pt->id, disk_part_pt->size);} 
            
            //2 possibilities : free space size > file size or free space size = file size
            if (disk_part_pt->size > end_of_disk_map->size){

                // 1st possibility : modify last file end_of_disk_map and split free space

                // create new space file
                disk_part* new_disk_part = (disk_part*)malloc(sizeof(disk_part));
                new_disk_part->id = end_of_disk_map->id;
                new_disk_part->size = end_of_disk_map->size;

                // insert 'new_disk_part' between 'disk_part_pt' and 'disk_part_pt->prev'
                new_disk_part->next = disk_part_pt;
                new_disk_part->prev = disk_part_pt->prev;
                disk_part_pt->prev = new_disk_part;
                if(new_disk_part->prev != NULL){
                    new_disk_part->prev->next = new_disk_part;
                }
                
                // reduce size of free space size
                disk_part_pt->size -= end_of_disk_map->size;
                // end_of_disk_map become free space
                end_of_disk_map->id = -1;

            } else {

                // 2nd possibility : the last file fill the first free space : destroy last part
                disk_part_pt->id = end_of_disk_map->id;
                end_of_disk_map->id = -1;

            }
        }

        // modify new end of the disk map
        end_of_disk_map = next_end_of_disk_map;

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
    disk_part_pt = disk_map;
    while ((disk_part_pt != NULL)){
        if (disk_part_pt->id != -1){
            unsigned long long int add = disk_part_pt->id * (disk_part_pt->size * coef + (disk_part_pt->size - 1) * disk_part_pt->size / 2); ;
            if (DEBUG){printf("\nAdd {id:%u ; size:%u} => %lld to result", disk_part_pt->id, disk_part_pt->size, add);}
            result += add;
            
        }

        coef += disk_part_pt->size;
        disk_part_pt = disk_part_pt->next;
    }

    printf("\nS : %llu",  result);
}