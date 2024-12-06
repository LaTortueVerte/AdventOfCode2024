#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

#define RECURSIVE_DETPH 30

#define DEBUG 0

/*-------------------------------------------------------------------------------------------------*/

typedef struct Node{

    int id;                 // id of the node

    int nb_of_neighbours;   // number of neighbours of the node
    int* next_ids;          // list of ids of neighbours
    struct Node** next_pts; // list of pointers of neighbours

}Node;

typedef struct Graph{

    int nb_of_nodes;                            // number of nodes in the graph
    int* processed_node_id_list;                // list of all ids of the graph
    struct Node** processed_node_pointer_list;   // list of all node pointers of the graph 

}Graph;

/*-------------------------------------------------------------------------------------------------*/

Graph *init_graph();
void destroy_graph();

Node *create_node(int id);
void destroy_node(int id);

int init_file_opening();

int get_new_list_from_file(int** list_pt, int* list_size);
int get_new_node_link_from_file(int* num1_pt, int* num2_pt);

int get_node_index(int node_id);
Node *get_node_pointer(int node_id);

void add_new_node_to_processed_list_of_graph(Node* new_node);
void add_new_node_to_neighbour_list_of_node(Node* parent_node, Node* neighbor_node);

int is_update_correct_and_modify_list(int starting_node_id, int objective_node_id, int** list, int list_size);

/*
int is_list_a_path_of_graph(int* list, int list_size);
int is_there_a_graph_path_between_recursive(Node *starting_node, Node *objective_node, int detph_limit);
int is_there_a_graph_path_between(Node *starting_node, Node *objective_node, int detph_limit);

int is_in_list(Node** node_list, int list_size, Node* node);
*/

void print_graph();
void print_list(int* list, int list_size);

/*-------------------------------------------------------------------------------------------------*/

FILE *fptr;

Graph *graph; 

char filename[50] = "input2.txt";

/*-------------------------------------------------------------------------------------------------*/

int main(){

    init_file_opening();

    graph = init_graph();

    int node_1_id, node_2_id;

    while(get_new_node_link_from_file(&node_1_id, &node_2_id)){ // while node links exists in textfile
        // got a new node link

        //printf("get new nodes : %d and %d\n", node_1_id, node_2_id);

        Node *node_1;
        Node *node_2;

        if((node_1 = get_node_pointer(node_1_id)) == NULL){ // try to get node_1 pt from graph list
            // node 1 does not exist
            //printf("node %d does not exist.\n", node_1_id);
            node_1 = create_node(node_1_id);
            add_new_node_to_processed_list_of_graph(node_1);
        } else {
            //printf("node %d exists.\n", node_1->id);
        }

        if((node_2 = get_node_pointer(node_2_id)) == NULL){ // try to get node_2 pt from graph list
            // node 2 does not exist
            //printf("node %d does not exist.\n", node_2_id);
            node_2 = create_node(node_2_id);
            add_new_node_to_processed_list_of_graph(node_2);
        } else {
            //printf("node %d exists.\n", node_2->id);
        }

        add_new_node_to_neighbour_list_of_node(node_1, node_2);

    }

    print_graph();



    int* list;
    int list_size;

    int Sum = 0;

    while(get_new_list_from_file(&list, &list_size)){ // while list exists in textfile
        // got a new list

        printf("------------------------------------------------\n");

        print_list(list, list_size);

        if (DEBUG){printf("Number of nodes to check : %d\n", graph->nb_of_nodes);}

        int was_incorrect = 0;

        int is_correct;
        int error_timer = 100;

        do { 

            is_correct = 1;

            int node_index = 0;
            int error_timer = 100;

            while ((node_index < list_size) && is_correct){ 

                Node* node = get_node_pointer(list[node_index]);

                int neighbour_node_index = 0;

                while ( (neighbour_node_index < node->nb_of_neighbours) && 
                        (is_correct = is_update_correct_and_modify_list(    node->id, 
                                                                            node->next_pts[neighbour_node_index]->id, 
                                                                            &list, 
                                                                            list_size))){

                    neighbour_node_index++;
                }
                node_index++;
            } 

            if(!is_correct){
                printf("List is incorrect, new_list : \n");
                print_list(list, list_size);

                was_incorrect = 1;
            }

            assert(error_timer-- != 0);

        } while(!is_correct);

        assert(is_correct);

        if (was_incorrect){
            printf("List is correct, add %d to Sum.\n", list[list_size/2]);
            Sum += list[list_size/2];
        }

        free(list);
    }

    printf("Sum = %d", Sum);

    fclose(fptr);

    return 0;
}

/*-------------------------------------------------------------------------------------------------*/

Graph *init_graph(){

    Graph *new_graph = (Graph*)malloc(sizeof(Graph));
    new_graph->nb_of_nodes = 0;
    new_graph->processed_node_id_list = NULL;
    new_graph->processed_node_pointer_list = NULL;

    return new_graph;
}

Node *create_node(int id){

    // create node 

    Node * new_node = (Node*)malloc(sizeof(Node));

    new_node->id = id;
    new_node->nb_of_neighbours = 0;
    new_node->next_ids = NULL;
    new_node->next_pts = NULL;

    return new_node;

}

int init_file_opening(){

    fptr = fopen(filename, "r");

    if(fptr == NULL) {
        printf("Not able to open the file.\n");
        return 1;
    }
}

int get_new_node_link_from_file(int* num1_pt, int* num2_pt){
    /* get the two numbers from a line in the textfile , return 1 if no error, else return 0 */

    char line[256];

    if(fgets(line, 100, fptr)) {

        return (sscanf(line, "%d|%d", num1_pt, num2_pt) != EOF);

    }

    return 0;
}

int get_new_list_from_file(int** list_pt, int* list_size){
    /* get the two numbers from a line in the textfile , return 1 if no error, else return 0 */

    char line[256];

    if(fgets(line, 100, fptr)) {

        // Check if line is empty

        if (line == NULL || line[0] == '\0') {
            *list_size = 0;
            return 0;
        }

        // Count nb of numbers

        *list_size = 1;
        for (int i = 0; line[i] != '\0'; i++) {
            if (line[i] == ',') {
                (*list_size)++;
            }
        }

        // Create list 

        *list_pt = (int*)malloc(*list_size * sizeof(int));
        assert(*list_pt != NULL);

        for (int i = 0 ; i < *list_size ; i++){
            (*list_pt)[i] = 0;
        }

        // Fill list

        int num_index = 0;

        for (int char_index = 0 ; char_index < strlen(line) ; char_index++ ){

            if (isdigit(line[char_index])){

                (*list_pt)[num_index] *= 10;
                (*list_pt)[num_index] += line[char_index] - '0';

            } else if (line[char_index] == ','){

                num_index++;

            } else {
                //printf("unrecognized character : >%c<\n", line[char_index]);
            }
        }

        return 1;
    }

    return 0;
}

int get_node_index(int node_id){
    /* get the index of the node with the id node_id if it is already in the graph, else return -1 */

    int node_index = 0;

    while ((node_index < graph->nb_of_nodes) && (node_id != graph->processed_node_id_list[node_index])){
        node_index++;
    }

    return node_index == graph->nb_of_nodes ? -1 : node_index;

}

Node *get_node_pointer(int node_id){
    /* get the pointer address of the node with the id node_id if it is already in the graph, else return NULL */

    if(graph->processed_node_pointer_list == NULL){
        return NULL;
    }

    int node_list_index = 0;

    while ((node_list_index < graph->nb_of_nodes) && (graph->processed_node_pointer_list[node_list_index]->id != node_id)){
        node_list_index++;
    }

    return (node_list_index == graph->nb_of_nodes) ? NULL : graph->processed_node_pointer_list[node_list_index];
}

void add_new_node_to_processed_list_of_graph(Node* new_node){

    if (graph->nb_of_nodes == 0){

        graph->processed_node_id_list = (int*)malloc(sizeof(int));
        assert( graph->processed_node_id_list != NULL );

        graph->processed_node_id_list[0] = new_node->id;

        graph->processed_node_pointer_list = (Node**)malloc(sizeof(Node*));
        assert( graph->processed_node_pointer_list != NULL );

        graph->processed_node_pointer_list[0] = new_node;

    } else {

        graph->processed_node_id_list = (int*)realloc(graph->processed_node_id_list, sizeof(int) * (graph->nb_of_nodes + 1));
        assert( graph->processed_node_id_list != NULL );

        graph->processed_node_id_list[graph->nb_of_nodes] = new_node->id;

        graph->processed_node_pointer_list = (Node**)realloc(graph->processed_node_pointer_list, sizeof(Node*) * (graph->nb_of_nodes + 1));
        assert( graph->processed_node_pointer_list != NULL );

        graph->processed_node_pointer_list[graph->nb_of_nodes] = new_node;

    }

    graph->nb_of_nodes++;

}

void add_new_node_to_neighbour_list_of_node(Node* parent_node, Node* neighbor_node){

    if (parent_node->nb_of_neighbours == 0){

        parent_node->next_ids = (int*)malloc(sizeof(int));
        assert( parent_node->next_ids != NULL );

        parent_node->next_ids[0] = neighbor_node->id;

        parent_node->next_pts = (Node**)malloc(sizeof(Node*));
        assert( parent_node->next_pts != NULL );

        parent_node->next_pts[0] = neighbor_node;

    } else {

        parent_node->next_ids = (int*)realloc(parent_node->next_ids, sizeof(int) * (parent_node->nb_of_neighbours + 1));
        assert( parent_node->next_ids != NULL );

        parent_node->next_ids[parent_node->nb_of_neighbours] = neighbor_node->id;

        parent_node->next_pts = (Node**)realloc(parent_node->next_pts, sizeof(Node*) * (parent_node->nb_of_neighbours + 1));
        assert( parent_node->next_pts != NULL );

        parent_node->next_pts[parent_node->nb_of_neighbours] = neighbor_node;

    }

    parent_node->nb_of_neighbours++;

}

int is_update_correct_and_modify_list(int starting_node_id, int objective_node_id, int** list, int list_size){

    int node_index = 0;

    int starting_node_index = -1;
    int objective_node_index = -1;

    while ((node_index < list_size) && ((starting_node_index == -1) || (objective_node_index == -1))){
        if (starting_node_id == (*list)[node_index]){
            starting_node_index = node_index;
        }
        if (objective_node_id == (*list)[node_index]){
            objective_node_index = node_index;
        }
        node_index++;
    }

    if ((starting_node_index != -1) && (objective_node_index != -1) && (starting_node_index > objective_node_index)){ //detect incorrect list and modify it

        int tmp = (*list)[starting_node_index];
        (*list)[starting_node_index] = (*list)[objective_node_index];
        (*list)[objective_node_index] = tmp;
    }

    if (DEBUG){
        if ((starting_node_index != -1) && (objective_node_index != -1) && (starting_node_index < objective_node_index)){
            printf("\t%d(%d) < %d(%d) : ",starting_node_id, starting_node_index, objective_node_id, objective_node_index);
        } else if ((starting_node_index == -1) || (objective_node_index == -1)) {
            printf("\t%d not linked in list with %d : ",starting_node_id, objective_node_id);
        } else {
            printf("\tError : %d(%d) ?!=? %d(%d) : ",starting_node_id, starting_node_index, objective_node_id, objective_node_index);
        }
        
        printf("is_update_correct : %d\n", ((starting_node_index == -1) || (objective_node_index == -1) || (starting_node_index < objective_node_index)));
        
    }

    return (starting_node_index == -1) || (objective_node_index == -1) || (starting_node_index < objective_node_index);
}

/*
int is_list_a_path_of_graph(int* list, int list_size){

    int list_index = 0;

    while ((list_index < list_size - 1) && (is_there_a_graph_path_between(              get_node_pointer(list[list_index]), 
                                                                                        get_node_pointer(list[list_index + 1]), 
                                                                                        RECURSIVE_DETPH))){        
        printf("%d -> ", list[list_index]);
        list_index++;
    }

    if (list_index < list_size - 1){
        printf("X -> %d\n", list[list_index + 1]);
    } else {
        printf("%d\n", list[list_index]);
    }

    return list_index == (list_size - 1);
}

int is_there_a_graph_path_between_recursive(Node *starting_node, Node *objective_node, int detph_limit){

    printf("\t%d: %d->?->%d\n", detph_limit, starting_node->id,objective_node->id);

    assert(detph_limit != 0); // check recursive limit

    if (starting_node == objective_node){ // check master condition
        printf("\t%d = %d\n", starting_node->id,objective_node->id);
        return 1; 
    }

    printf("\tCheck for neighbours :\n");

    int neighbour_index = 0;

    while ( (neighbour_index < starting_node->nb_of_neighbours) && 
            (!is_there_a_graph_path_between_recursive(starting_node->next_pts[neighbour_index], objective_node, detph_limit - 1))){  // try neighbours

        neighbour_index++;
    }

    if (neighbour_index != starting_node->nb_of_neighbours){
        return 1;
    }

    return 0;
}

int is_there_a_graph_path_between(Node *starting_node, Node *objective_node, int detph_limit){

    //if(starting_node->id == 62 && objective_node->id == 99){
    //printf("%d->?->%d\n",starting_node->id,objective_node->id);}

    Node* processed_nodes[256];
    assert(processed_nodes != NULL);
    int processed_nodes_size = 0;

    Node* to_process_nodes[256];
    assert(to_process_nodes != NULL);
    int to_process_nodes_size = 0;

    to_process_nodes[to_process_nodes_size++] = starting_node;

    while ((to_process_nodes_size != 0) && (to_process_nodes[0] != objective_node)){

        //printf("\tCheck node : %d\n", to_process_nodes[0]->id);

        // add neighbours nodes to to_process_nodes list
        for (int n_index = 0 ; n_index < to_process_nodes[0]->nb_of_neighbours ; n_index++){ // loop on neighbours nodes

            if (!is_in_list(processed_nodes, processed_nodes_size, to_process_nodes[0]->next_pts[n_index]) &&
                !is_in_list(to_process_nodes, to_process_nodes_size, to_process_nodes[0]->next_pts[n_index])){

                to_process_nodes[to_process_nodes_size++] = to_process_nodes[0]->next_pts[n_index];
                //printf("\t\t- %d added to to_process_nodes list.\n", to_process_nodes[0]->next_pts[n_index]->id);

            }
        }

        // add the processed node to the processed_nodes list 
        processed_nodes[processed_nodes_size++] = to_process_nodes[0];

        // delete the processed node from the to_process_nodes list
        for (int n_index = 0 ; n_index < (to_process_nodes_size - 1) ; n_index++){
            to_process_nodes[n_index] = to_process_nodes[n_index + 1];
        }
        to_process_nodes_size--;

        //printf("\tto_process_nodes_size = %d | processed_nodes_size = %d\n",to_process_nodes_size, processed_nodes_size);

    }

    return (to_process_nodes_size != 0);

}

int is_in_list(Node** node_list, int list_size, Node* node){

    int node_index = 0;

    //printf("\t\t\tSearch for %d(%d) in size : %d :\n", node->id, node, list_size);

    while ((node_index < list_size) && (node != node_list[node_index])){
        //printf("\t\t\t\t%d(%d) != %d(%d)\n", node->id, node, node_list[node_index]->id, node_list[node_index]);
        node_index++;
    }

    //if (node_index < list_size){
    //   printf("\t\t\t\t%d(%d) == %d(%d)\n", node->id, node, node_list[node_index]->id, node_list[node_index]);
    //}

    return (node_index != list_size);
}*/

void print_graph(){

    printf("Graph : \n");

    for (int node_index = 0 ; node_index < graph->nb_of_nodes ; node_index++){

        assert(graph->processed_node_pointer_list[node_index] != NULL);

        Node *node = graph->processed_node_pointer_list[node_index];

        printf("\tNode %d :\n", node->id);
        
        for(int neighbor_node_index = 0 ; neighbor_node_index < node->nb_of_neighbours ; neighbor_node_index++){

            assert(node->next_pts[neighbor_node_index] != NULL);

            printf("\t\t-> %d\n", node->next_pts[neighbor_node_index]->id);

        }
    }
}

void print_list(int* list, int list_size){

    printf("list [%d] = ", list_size);

    for (int i = 0 ; i < list_size ; i++){
        printf("%d ", list[i]);
    }
    printf("\n");
}
