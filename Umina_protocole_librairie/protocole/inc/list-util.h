#pragma once
/****************************************************************************************
*
* Data linked list structure & manipulation functions prototypes
*
****************************************************************************************/

#include <stdint.h>
#include <stddef.h>

/** Data linked list structure */
struct data_node {
    int nbr;
    char * name;
    double dbl;
    struct data_node * next;

};

/**
 * Prints a list of proto
 * 
 * @param listProto: pointer sur liste
 * @param size: number of nodes to print
 */
void print_listproto(int * listeProto , int size);

/**
 * Prints a list of nodes
 * 
 * @param node: head node pointer
 * @param size: number of nodes to print
 */
void print_list(struct data_node * node, int size);




/**
 * Creates a new node with random values
 * 
 * @param nbr: the int16_t data 
 * @param name: the char* name 
 * @param f: the float f 
 * @return the pointer of the node created
 */
struct data_node * create_node(int nbr, char* name, double dbl);


/**
 * Frees the data linked list
 * 
 * @param node: head node pointer
 */
void free_list(struct data_node *node);