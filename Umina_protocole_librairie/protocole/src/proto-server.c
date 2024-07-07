/*
** Umina Antonino
** ProtoServer.c -- serveur qui recois un nombre de noeuds passé en parametre par un client.
** chaque noeud contient un entier , une chaine de caractères et un floatant.
** si le server recois un hello il renvois hello au client 
** lorsqu'il recoit la liste le serveur la cpmare a saen retour un numéro de protocole il envoi le nombre de noeuds puis les noeuds  
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
#include <stdio.h>
#include <stdlib.h>
#include <tcp-util.h>
#include <serial-util.h>
#include <list-util.h>

#define PORT 7777
#define MAXDATASIZE 128		// how many pending connections queue will hold

int main (void)
{
  int sockfd, new_fd;		// listen on sock_fd, new connection on new_fd
  char s[INET6_ADDRSTRLEN];
  int protoList[]={1,22,24,33,45,18,45};
  int nbrProtoS = sizeof(protoList)/sizeof(int);//nbr d'entier ds la liste de proto
  int protoNum;  
  

  // ----------------creation socket return socketnumber---------------------
  sockfd = socketServ (PORT);
  if (sockfd<0){
    perror ("server: socket"); 
    exit (1);
  }  
  printf ("server: waiting for connections...\n");

  //------------------------------------------------------------------------
  //boucle infinie pour garder le serveur a l'ecoute  
  // ----------------------------------------------------------------------
  while (1){// main accept() loop      
    new_fd = acceptServ (sockfd, s);
    if (new_fd < 0)
	    {
	    printf ("socket error");
	    }
    printf("server: got connection from %s\n", s);
    if (!fork ()){			// this is the child process
	    disconnect (sockfd);	// child doesn't need the listener
      // ------------- dialogue protocole --------------------------------------------------------
      if ((protoNum = protocole_send(new_fd,protoList,nbrProtoS,MAXDATASIZE))<0){
        perror ("server:no common protocols"); 
        if (protoNum == 0){
          send(new_fd,0,sizeof(int),0);    
        }
        printf("num proto : %d\n",protoNum);
        exit (1);
      }
      printf("protocole is : %d\n",protoNum);

      // ------------- execution du protocole --------------------------------------------------------
      executeProtoServer(protoNum,new_fd,MAXDATASIZE);   


      // fermeture du socket enfant -------------------------------------
	    disconnect (new_fd);
	    exit (0);
	  }
    disconnect (new_fd);
  }
  return 0;
}
