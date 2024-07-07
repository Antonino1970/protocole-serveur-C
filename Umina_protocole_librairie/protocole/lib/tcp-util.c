/*
lib.c toni : utile pour tcp
08/04/2024
*/
#include <tcp-util.h>
#include <list-util.h>
#include <serial-util.h>
#include <list-util.h>

#define BACKLOG 10

/*
This function receive data from host and store it in a buffer
input: socket number s, buffer length len
output : buffer buf with the received data
return: number of received characters
*/
int receiveData (int s, char *buf, int len) {
  int numbytes;
  numbytes = recv (s, buf, len - 1, 0);
  buf[numbytes] = '\0';
  if (numbytes == -1)
    {
      perror ("recv");
      return (1);
    }
  return numbytes;
}

/*
This function receive data from host and store it in a buffer
input: socket number s, buffer length len
output : buffer unsigned buf with the received data
return: number of received characters
*/

int receiveDataU (int s, unsigned char *buf, int len)
{
  int numbytes;
  numbytes = recv (s, buf, len, 0); 
  if (numbytes == -1)
    {
      perror ("recv");
      return (1);
    }
  return numbytes;
}

/*
This function send buffer content to the host
input: socket number, buffer
return: number of sent characters
*/
int sendData (int s, char *buf)
{
  int sc = strlen (buf);
  if (send (s, buf, sc, 0) == -1){
    perror ("send");
    return (1);}
  return sc;
}

/*
This function send buffer content to the host
input: socket number, Unsigned buffer , lenght buffer
return: number of sent characters
*/
int sendDataU (int s, unsigned char *buf,int len )
{  
  if (send (s, buf, len, 0) == -1){
    perror ("send");
    return (1);}
  return len;
}

/*
This function creates the server socket
input: port number
return: socket number
*/
int socketServ (int port)
{
  int sockfd;			// Déscripteur de fichier de la socket
  struct addrinfo hints, *servinfo, *p;
  int yes = 1;
  int rv;
  char portS[20];
  struct sigaction sa;
  sprintf (portS, "%d", port);	//transforme int port en string portS

  memset (&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;	// IPv4 ou IPv6
  hints.ai_socktype = SOCK_STREAM;	// TCP
  hints.ai_flags = AI_PASSIVE;	// Utiliser mon IP

  if ((rv = getaddrinfo (NULL, portS, &hints, &servinfo)) != 0)
    {
      fprintf (stderr, "getaddrinfo: %s\n", gai_strerror (rv));
      return -1;
    }
  // Boucle pour trouver une adresse valide à laquelle se lier
  for (p = servinfo; p != NULL; p = p->ai_next)
    {
      if ((sockfd = socket (p->ai_family, p->ai_socktype,
			    p->ai_protocol)) == -1)
	{
	  perror ("server: socket");
	  continue;
	}
      if (setsockopt (sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
		      sizeof (int)) == -1)
	{
	  perror ("setsockopt");
	  return -2;
	}
      if (bind (sockfd, p->ai_addr, p->ai_addrlen) == -1)
	{
	  close (sockfd);
	  perror ("server: bind");
	  continue;
	}
      break;			// Sortir de la boucle si la liaison a réussi
  }
  freeaddrinfo (servinfo);	// Libération de la mémoire
  if (p == NULL)
    {
      fprintf (stderr, "server: failed to bind\n");
      return -3;
    }

  // mise a l'ecoute  
  if (listen (sockfd, BACKLOG) == -1)
    {
      perror ("listen");
      return -4;
    }
  sa.sa_handler = sigchld_handler;	// reap all dead processes
  sigemptyset (&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction (SIGCHLD, &sa, NULL) == -1)
    {
      perror ("sigaction");
      return -5;
    }
  return sockfd;		// Renvoie le descripteur de fichier de la socket
}

//methode pour les kill process
void sigchld_handler (int s)
{
  (void) s;			// quiet unused variable warning

  // waitpid() might overwrite errno, so we save and restore it:
  int saved_errno = errno;

  while (waitpid (-1, NULL, WNOHANG) > 0);

  errno = saved_errno;
}

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
void * get_in_addr (struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET)
    {
      return &(((struct sockaddr_in *) sa)->sin_addr);
    }

  return &(((struct sockaddr_in6 *) sa)->sin6_addr);
}

/*
this function close a socket
input s socket number
return 0
*/
int disconnect(int s){
    close(s);
    return 0;
}

/*
This function create socket and establish the connection with the server
input: char array with the url or the ip address separate by dots, port number
return: the socket number or -1 if the error occurs when the socket is created
and -2 if the error occurs when the connection is established.
*/
int connectClient(char* url,int port){
  char portString [10];
	sprintf(portString,"%d",port);// convertit le port en char
  int sockfd ;// listen on sock_fd
	struct addrinfo addressInfo, *servinfo, *p; //Structure addrinfo pour stocker les options de configuration.
	// servinfo: Pointeur vers une liste d'adresses réseau.
	// p: Itérateur pour la liste d'adresses réseau.
	int rv;// servira de valeur de retour
	char s[INET6_ADDRSTRLEN];// Chaîne pour stocker l'adresse IP en format texte.

  memset(&addressInfo, 0, sizeof addressInfo);
	addressInfo.ai_family = AF_UNSPEC; // permet à getaddrinfo de retourner des adresses IPv4 ou IPv6
	addressInfo.ai_socktype = SOCK_STREAM; // indique que nous voulons utiliser TCP

	// Convertit un nom d'hôte et un numéro de port en une liste d'adresses réseau utilisables.
	// &addressInfo =  Pointeur vers une structure addrinfo contenant des options de configuration.
	// &servinfo: Pointeur vers une variable qui recevra la liste des adresses réseau.
	if ((rv = getaddrinfo(url, portString, &addressInfo, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv)); 
		return -1;
	}
    	// Boucle à travers toutes les adresses disponibles et se connecte à la première qui fonctionne
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket"); // affichage de l'erreur s'il y a eu un problème avec la création du socket
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			perror("client: connect"); // affichage de l'erreur s'il y a eu un problème avec la connexion au serveur
			close(sockfd);
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n"); // affichage de l'échec de la connexion si aucune connexion n'a pu être établie
		return -2;
	}

	// Conversion de l'adresse IP de binaire en texte
	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("client: connecting to %s\n", s); // affichage de l'adresse à laquelle le client se connecte

	freeaddrinfo(servinfo); // libération de la mémoire utilisée par les informations d'adresse

    return  sockfd;
}


/*
This function find the max comon protocole between 2 list
input: list1, list2 leng of those list
return: numproto the max comon protocole of the 2 list or 0 
*/
int findMaxCommon(int *list1, int *list2, int length1, int length2) {
    int numProto = 0; // Initialiser le plus grand entier commun à 0
    for (int i = 0; i < length1; i++) {
        for (int j = 0; j < length2; j++) {
            if (list1[i] == list2[j]) { // Vérifier si les entiers sont identiques dans les deux listes
                if (list1[i] > numProto) { // Si l'entier est plus grand que le plus grand entier commun trouvé jusqu'à présent
                    numProto = list1[i]; // Mettre à jour le plus grand entier commun
                }
            }
        }
    }
    return numProto; // Retourner le plus grand entier commun (ou 0 si aucun entier commun n'est trouvé)
}

/*
This function accept connection to the host
input: s socket number, 
output :client , server will receive the client ip address string
return: the client socket number
*/
int
acceptServ (int sock, char *client)
{
  socklen_t sin_size;
  struct sockaddr_storage their_addr;	// connector's address information
  int new_fd;
  //int port = 0;
  sin_size = sizeof their_addr;
  new_fd = accept (sock, (struct sockaddr *) &their_addr, &sin_size);  
  if (new_fd == -1)
    {
      perror ("accept");
      return -2;
    }
  //conversion binaire 
  inet_ntop (their_addr.ss_family,
	     get_in_addr ((struct sockaddr *) &their_addr), client,
	     INET6_ADDRSTRLEN);
  //port = ntohs (((struct sockaddr_in *) &their_addr)->sin_port);  
  return new_fd;
}

/**
 * Fonction pour renvoyer le protocole commun client serveur
 * @param new_fd: socket de communication
 * @param listeProto: liste des protocoles du serveur
 * @param nbrProto: nombre de protocoles
 * @param MAXDATASIZE: longeur max du buffer
 * @return le numero du protocole ou 0
 */
int protocole_send(int new_fd ,int * listeProtoServer,int nbrProtoServer,int MAXDATASIZE){
  int bufChar = 0;
  unsigned char uBuf[MAXDATASIZE];
  char buf[MAXDATASIZE];
  // reception du buffer client
	bufChar = receiveData (new_fd, buf, MAXDATASIZE);
  //printf ("received chars from client : %d\n", bufChar);	    
  if (bufChar >=5 && strncmp(buf,"hello",5)==0){
    bufChar = sendData (new_fd, buf);
    printf ("Server : good pass received , send : %d  chars\n", bufChar);
  }
  else{
  printf("bad echo\n");
    strcpy(buf, "bad request");
    bufChar = sendData (new_fd, buf);
        printf ("Server send : %d  chars\n", bufChar);
    disconnect (new_fd);
    exit (0);
    }
    // reception du nbr de la liste proto
    int nbrdeProtoClient;    
    receiveDataU(new_fd, uBuf,MAXDATASIZE);
    unpack(uBuf,"h",&nbrdeProtoClient);
    //recv (new_fd, &nbrdeProtoClient,sizeof(nbrdeProtoClient),0);
    printf ("received nbr elements in protocole List from client : %d\n", nbrdeProtoClient);
    int listProtoClient[nbrdeProtoClient];
    // reception de la liste proto     
    recv (new_fd, listProtoClient,nbrdeProtoClient *sizeof(int),0);
    print_listproto(listProtoClient,nbrdeProtoClient);
    print_listproto(listeProtoServer,nbrProtoServer);
    int result = 0;
    result = findMaxCommon (listProtoClient,listeProtoServer,nbrdeProtoClient,nbrProtoServer);    
    //renvoi du numero de proto commun ou 0 pour fermer
    pack(uBuf,"h",result);
    sendDataU(new_fd,uBuf,sizeof(result));
    //send(new_fd,&result,sizeof(result),0);
    return result ;
}

/**
 * Fonction pour recevoir le protocole commun serveur
 * @param sockfd: socket de communication
 * @param protoList: liste des protocoles du client
 * @param nbrProto: nombre de protocoles
 * @param MAXDATASIZE: longeur max du buffer
 * @return le numero du protocole ou 0
 */
int protocole_receive(int sockfd,int * protoList,int nbrProto,int MAXDATASIZE){
    unsigned char uBuf[MAXDATASIZE];
    char buf[MAXDATASIZE];
    char protoPass[] = "hello";
      // Envoie du message de bienvenue
    sendData(sockfd,protoPass);
    //si le serveur accepte il renvoi hello
    int bufChar = receiveData (sockfd, buf, MAXDATASIZE);
    //si retour est hello on envoi la liste des proto
    if (bufChar >=5 && strncmp(buf,"hello",5)==0){
      pack(uBuf,"h",nbrProto);
      sendDataU(sockfd,uBuf,sizeof(nbrProto));
	    //send(sockfd,&nbrProto,sizeof(nbrProto),0);
        send(sockfd,protoList,nbrProto *sizeof(int),0);
	    printf (" send list proto\n");
    }
     else{
        printf("bad echo\n");
        strcpy(buf, "bad msg receive\n");    
        exit (0);
      }
    // Reçoit le num de proto
    int numProto;
    receiveDataU(sockfd, uBuf,MAXDATASIZE);
    unpack(uBuf,"h",&numProto);
    //recv (sockfd, &numProto,sizeof(numProto),0);    
    return numProto;
}

/**
 * \brief Exécute un protocole spécifique en fonction du choix donné.
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
void executeProtoClient(int choix , int sockfd , int size , struct data_node * head , unsigned char *buf,int bufSize) {
    switch (choix) {
        case 1:
            printf("a implémenter.\n");
            break;
        case 2:
            printf("a implémenter.\n");
            break;
        case 15:
            printf("a implémenter.\n");
            break;
        case 33:
            proto33Client(sockfd,size,head, buf,bufSize);
            break;
        default:
            printf("invalide protocol.\n");
            break;
    }
}

/**
 * \brief Exécute un protocole spécifique en fonction du choix donné.
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
void executeProtoServer(int choix ,int sockfd ,int bufSize) {
      struct data_node *head = NULL;
      int nbrDeNoeuds ;
    switch (choix) {

        case 1:
            printf("a implémenter.\n");
            break;
        case 22:
            printf("a implémenter.\n");
            break;
        case 24:
            printf("a implémenter.\n");
            break;
        case 33:         
            
            head =  proto33Server(sockfd,bufSize,&nbrDeNoeuds);
            print_list(head,nbrDeNoeuds);
            printf("end of receive .\n");
            free_list(head); 
            break;
        case 45:
            printf("a implémenter.\n");
            break;
        case 18:
            printf("a implémenter.\n");
            break;
        default:
            printf("invalide protocol.\n");
            break;
    }
}

/**
 * \brief Implémente le protocole 33 du client.
 * 
 * Cette fonction implémente le protocole 33 du client. Elle envoie le nombre de noeuds
 * à envoyer au serveur, puis envoie les noeuds au serveur un par un en utilisant le format
 * spécifié.
 * 
 * \param sockfd Le descripteur de socket à utiliser pour la communication.
 * \param size Le nombre de noeuds à envoyer.
 * \param head Le pointeur vers le premier noeud de la liste de données.
 * \param buf Le tampon dans lequel stocker les données à envoyer.
 * \param bufSize La taille du tampon.
 */
void proto33Client(int sockfd,int size,struct data_node * head, unsigned char * buf,int bufSize){
    // envois le nombre de noeuds a envoyer par le client
  send(sockfd,&size,sizeof(size),0);
  // envois les noeuds au serveur
    struct data_node * current = head;
    while (current != NULL) {
      pack(buf, "hsd",current->nbr, current->name,current->dbl);
      sendDataU(sockfd,buf,bufSize);
      //send(sockfd,buf,128,0);   
      current = current->next;
    } 
}

/**
 * \brief Implémente le protocole 33 du serveur.
 * 
 * Cette fonction implémente le protocole 33 du serveur. Elle recois le nombre de noeuds
 * à recevoir du client, puis recois les noeuds un par un en utilisant le format
 * spécifié.
 * 
 * \param new_fd Le descripteur de socket à utiliser pour la communication.
 * \param bufSize La taille du tampon.
 */
struct data_node * proto33Server(int new_fd,int bufSize,int * nbrDeNoeuds){
    //---------------------structure---------------------------------
  struct data_node *head = NULL;  // first node of the data linked list
  struct data_node *node;         // node of the linked list used for iteration  
  unsigned char buf[bufSize];
  int count;  
	char s2[15];
  double doubles ;
  //reception du nbr de noeuds--------------------------------------------------
  * nbrDeNoeuds = 0 ;
  recv (new_fd, nbrDeNoeuds,sizeof( * nbrDeNoeuds),0);
  printf ("received nbr of node from client : %d\n",  *nbrDeNoeuds);
  // -----------------RECEPTION-------------------------------------------------
  receiveDataU(new_fd,buf,bufSize); //reception head  
  unpack(buf, "h20sd", &count, s2,&doubles);
  head = node = create_node(count, s2,doubles);
  //reception des autres noeuds
  for (int i = 1; i < * nbrDeNoeuds ; i++){
    receiveDataU(new_fd,buf,sizeof(buf));    
    unpack(buf, "h20sd", &count, s2,&doubles);
      node->next = create_node(count, s2,doubles);
      node = node->next;
  }
  //print_list(head,* nbrDeNoeuds);
  //printf("end of receive .\n");
  return head ; 
  //free_list(head); 
  }