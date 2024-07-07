/*
lib toni
08/04/2024
*/
#ifndef tcp_util_h
#define tcp_util_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <list-util.h>
/*
This function create socket and establish the connection with the server
input: char array with the url or the ip address separate by dots, port number
return: the socket number or -1 if the error occurs when the socket is created
and -2 if the error occurs when the connection is established.
*/
int connectClient(char* url,int port);

/*
This function receive data from host and store it in a buffer
input: socket number s, buffer length len
output : buffer buf with the received data
return: number of received characters
*/
int receiveData (int s, char *buf, int len);

/*
This function receive data from host and store it in a buffer
input: socket number s, buffer length len
output : buffer unsigned buf with the received data
return: number of received characters
*/
int receiveDataU (int s,unsigned char *buf, int len);

/*
This function send buffer content to the host
input: socket number, buffer
return: number of sent characters
*/
int sendData (int s,char *buf);

/*
This function send buffer content to the host
input: socket number, Unsigned buffer , lenght buffer
return: number of sent characters
*/
int sendDataU (int s, unsigned char *buf,int len );

/*
This function creates the server socket
input: port number
return: socket number
*/
int socketServ (int port);

//methode pour les kill process
void sigchld_handler (int s);

/**
 * @brief Récupère l'adresse IP à partir d'une structure sockaddr.
 *
 * Cette fonction permet de récupérer l'adresse IP à partir d'une structure sockaddr,
 * en prenant en charge à la fois les adresses IPv4 et IPv6.
 *
 * @param sa Un pointeur vers une structure sockaddr contenant l'adresse IP.
 * @return Un pointeur void vers l'adresse IP extraite de la structure sockaddr.
 *         Si l'adresse est IPv4, le pointeur pointe vers une structure in_addr.
 *         Si l'adresse est IPv6, le pointeur pointe vers une structure in6_addr.
 *         NULL est retourné en cas d'erreur ou si la famille d'adresses n'est ni IPv4 ni IPv6.
 */
void *get_in_addr (struct sockaddr *sa);

/*
this function close a socket
input s socket number
return 0
*/
int disconnect(int);

/*
This function accept connection to the host
input: s socket number, 
output :client , server will receive the client ip address string
return: the client socket number
*/
int acceptServ (int sock, char *client);

/**
 * Fonction pour trouver le plus grand entier commun dans deux listes
 * @param list1: premiere liste a comparer
 * @param list2: deuxième liste a comparer
 * @param length1: longeur de liste1
 * @param length2: longeur de liste2
 */
int findMaxCommon(int *list1, int *list2, int length1, int length2);

/**
 * Fonction pour renvoyer le protocole commun client serveur
 * @param new_fd: socket de communication
 * @param listeProto: liste des protocoles du serveur
 * @param nbrProto: nombre de protocoles
 * @param MAXDATASIZE: longeur max du buffer
 * @return le numero du protocole ou 0
 */
int protocole_send(int new_fd ,int * listeProto,int nbrProto,int MAXDATASIZE);

/**
 * Fonction pour recevoir le protocole commun serveur
 * @param sockfd: socket de communication
 * @param protoList: liste des protocoles du client
 * @param nbrProto: nombre de protocoles
 * @param MAXDATASIZE: longeur max du buffer
 * @return le numero du protocole ou 0
 */
int protocole_receive(int sockfd,int * protoList,int nbrProto,int MAXDATASIZE);

/**
 * \brief Exécute un protocole spécifique en fonction du choix donné coté client.
 * 
 * Cette fonction prend en charge l'exécution de différents protocoles en fonction
 * du choix spécifié. Elle utilise un commutateur (switch) pour diriger le flux
 * d'exécution vers la fonction appropriée en fonction du choix.
 * 
 * \param choix Le choix du protocole à exécuter.
 * \param sockfd Le socket à utiliser pour la communication.
 * \param size Le nombre de noeuds à envoyer.
 * \param head Le pointeur vers le premier noeud de la liste de données.
 * \param buf Le tampon dans lequel stocker les données à envoyer.
 * \param bufSize La taille du tampon.
 */
void executeProtoClient(int choix,int sockfd,int size,struct data_node *head,unsigned char * buf,int bufSize);

/**
 * \brief Exécute un protocole spécifique en fonction du choix donné coté serveur.
 * 
 * Cette fonction prend en charge l'exécution de différents protocoles en fonction
 * du choix spécifié. Elle utilise un commutateur (switch) pour diriger le flux
 * d'exécution vers la fonction appropriée en fonction du choix.
 * 
 * \param choix Le choix du protocole à exécuter.
 * \param sockfd Le socket à utiliser pour la communication.
 * \param bufSize La taille du tampon.
 */
void executeProtoServer(int choix ,int sockfd ,int bufSize);



/**
 * \brief Implémente le protocole 33 du client.
 * 
 * Cette fonction implémente le protocole 33 du client. Elle envoie le nombre de noeuds
 * à envoyer au serveur, puis envoie les noeuds au serveur un par un en utilisant le format
 * spécifié.
 * 
 * \param sockfd Le socket à utiliser pour la communication.
 * \param size Le nombre de noeuds à envoyer.
 * \param head Le pointeur vers le premier noeud de la liste de données.
 * \param buf Le tampon dans lequel stocker les données à envoyer.
 * \param bufSize La taille du tampon.
 */
void proto33Client(int sockfd,int size,struct data_node * head,unsigned char * buf,int bufSize);

struct data_node * proto33Server(int new_fd, int bufSize,int * nbrDeNoeuds);

#endif
