/****************************************************************************************
*
* Data linked list manipulation functions
*
****************************************************************************************/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <list-util.h>


/* HEADER PROTOTYPES IMPLEMENTATION */

/**
 * \brief Affiche les éléments d'une liste chaînée de données.
 * 
 * Cette fonction affiche les éléments de la liste chaînée spécifiée. Elle parcourt la liste
 * à partir du nœud donné et affiche chaque nœud avec son numéro d'index, le nombre entier,
 * la chaîne de caractères et la valeur en virgule flottante.
 * 
 * \param node Le pointeur vers le premier nœud de la liste de données.
 * \param size Le nombre total d'éléments dans la liste.
 */
void print_list(struct data_node * node, int size) {//pointeur struct de node et le nombre d'éléments 
    int i = 0;

    while (node) {
        printf("Node %d/%d{\n\tint:\t%d\n\tstring:\t%s\n\tfloat:\t%f\n}\n",
                i + 1, size,
                node->nbr, node->name,node->dbl);
        node = node->next;
        i++;
    }
}

/**
 * \brief Affiche les éléments d'une liste de protocoles.
 * 
 * Cette fonction affiche les éléments d'une liste de protocoles spécifiée. Elle parcourt
 * la liste et affiche chaque élément entier séparé par des virgules, encadré par des crochets.
 * 
 * \param listeProto Le tableau des protocoles.
 * \param size La taille du tableau des protocoles.
 */
void print_listproto(int * listeProto , int size) { //pointeur struct de node et le nombre d'éléments 
    printf("[");
    for (int i = 0 ; i< size; i++){
        printf("%d",listeProto[i]);
        if (i< size-1){
            printf(", ");
        }
    }
    printf("]\n");
} 



/**
 * \brief Crée un nouveau nœud de données.
 * 
 * Cette fonction alloue de la mémoire pour un nouveau nœud de données et initialise
 * ses champs avec les valeurs spécifiées.
 * 
 * \param nbr Le nombre entier à stocker dans le nœud.
 * \param name La chaîne de caractères à stocker dans le nœud.
 * \param doubles La valeur en virgule flottante à stocker dans le nœud.
 * \return Le pointeur vers le nouveau nœud de données.
 */
struct data_node * create_node(int nbr, char* name ,double doubles) {
    struct data_node * node = (struct data_node *) malloc(sizeof(struct data_node));

    node->nbr = nbr;
    node->name = malloc(sizeof(char) * strlen(name) + 1);
    node->dbl= doubles;
    strncpy(node->name, name, strlen(name));
    node->next = NULL;

    return node;
}

/**
 * \brief Libère la mémoire allouée pour une liste chaînée de données.
 * 
 * Cette fonction libère la mémoire allouée pour chaque nœud d'une liste chaînée
 * de données, y compris les chaînes de caractères stockées dans chaque nœud.
 * 
 * \param node Le pointeur vers le premier nœud de la liste de données.
 */
void free_list(struct data_node *node) {
    struct data_node *next_node;

    while (node) {
        // save next node pointer
        next_node = node->next;

        // free the string & the node
        free(node->name);
        free(node);

        // iterate on next node
        node = next_node;
    }
}