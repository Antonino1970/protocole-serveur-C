/*
** Umina Antonino
** ProtoClient.c -- client qui envoi un nombre de noeuds passé en parametre a un serveur.
** chaque noeud contient un entier , une chaine de caractères et un floatant.
** le client envoi un hello et attent le hello du serveur puis il envoi sa liste de protocole
** lorsqu'il recoit en retour un numéro de protocole il envoi le nombre de noeuds puis les noeuds  
** si il n y a pas de protocole commun ,  il recois 0 et ferme la connection 
** Liège 06_05_2024

Enoncé
A l’aide de la librairie réalisée dans l’exercice précédent, réaliser une application client-serveur s’échangeant 
une liste d’éléments. Le client générera la liste contenant un nombre d’éléments passés en argument 
Chaque élément contiendra un entier, une chaîne de caractère et un flottant qui seront différents pour chaque 
élément de la liste (ces données pourront être générées aléatoirement ou suivre une règle que vous inventerez). 
Le client enverra la liste au serveur et celui-ci affichera la liste à l’écran. 
L’envoi des données se fera en suivant le point  7.5. Serialization—How to Pack Data de la documentation.


*/

#include <tcp-util.h>
#include <serial-util.h>
#include <list-util.h>
#define PORT 7777
#define MAXDATASIZE 128		// Nombre maximum du buffer

int main (int argc, char *argv[])
{  
    // Vérifie que l'utilisateur fournit un nom d'hôte ou une adresse IP en argument
  if (argc != 3)
    {
      fprintf (stderr, "usage: client hostname  + number of nodes\n");
      exit (1);
    }
  char *url = argv[1];  // adresse du serveur passé comme 1er parametre  
  int size = atoi(argv[2]);//conerti char * en int Le 2èm parametre, nombre de noeuds envoyés 
  int sockfd ;	//socket 
  int proto[]={1,2,33,15};// liste des protocoles connus par le client
  int nbrProto = sizeof(proto)/sizeof(int);//nbr d'entier ds la liste de proto
  unsigned char buf[128];
  int numProto = 0 ; // numero du proto
  //--------------structure data node -------------------------------
  struct data_node *head = NULL;  // first node of the data linked list
  struct data_node *node;         // node of the linked list used for iteration
  int i;  


  // ------------------generate the data list---------------------------------------- 
  char dataString[] = "protocol"; 
  head = node = create_node(0, dataString ,0.12345f);//on cree 1 noeud avec 2 pointeurs sur lui
  for (i = 1; i < size; i++) {
    char str[4]; // tableau qui va contenir les int convertis en chars  
    sprintf(str, "%d", i); // sortie formatée
    // Concaténation de dataString et str
    char temp_name[128];
    strcpy(temp_name, dataString);// copie de dataString dans temp_name
    strcat(temp_name, str);//concatenation de str et temp_name
  
    // Création du nœud avec le nom de données concaténé
    node->next = create_node(i, temp_name, ((0.0+i)/3));//son node.next devient l adresse du nouveau noeud cree  
    node = node->next;    
  }


  //----------------     connection      ---------------------
  sockfd = connectClient( url, PORT);  
  // ----------------------dialogue protocole -------------------
  numProto = protocole_receive(sockfd,proto,nbrProto,MAXDATASIZE);
  printf("protocol number is : %d\n",numProto);
  //----------envoi des noeuds via le protocole-------------------
  executeProtoClient(numProto,sockfd,size,head,buf,MAXDATASIZE);
  
  free_list(head); // libere la mémoire de la structure
  disconnect (sockfd);		// Ferme le socket
  return 0;			// Termine le programme avec un code de retour 0
}
