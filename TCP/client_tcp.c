/* CLIENT TCP */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT "9034" // le port utilisé

#define MAXDATASIZE 100 // le nombre maximum d'octet que nous pouvons avoir en une seul fois 
#define MAXNAMESIZE 25

//fonction du thread thread 
void *receive_handler(void *);

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    char message[MAXDATASIZE]; //buffer des données 
    char nickName[MAXNAMESIZE];
    int sockfd;//, numbytes;
    char sBuf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 2) {
        fprintf(stderr,"usage: ./client <hostname>\n");
        exit(1);
    }

	// initialisation du filtre 
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // parcour de de toute les sockets filtrées et connexion à la première socket trouvé
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connexion");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: erreur de  connexion\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connexion à %s\n", s);

    freeaddrinfo(servinfo); // free la structure service info 
    
    puts("Surnom:");
    memset(&nickName, sizeof(nickName), 0);
    memset(&message, sizeof(message), 0);  //initialise les buffers 
    fgets(nickName, MAXNAMESIZE, stdin);  
    puts(message);
  
    //création d'un thread pour recevoir les méssages 
    pthread_t recv_thread;
    
    if( pthread_create(&recv_thread, NULL, receive_handler, (void*)(intptr_t) sockfd) < 0)
    {   //On caste la structure sockfd pour éviter les warnings

        perror("erreur création du thread");
        return 1;
    }
    puts("Handler de réception synchrone reçu");
    
    //envoie un méssage au serveur server:
    puts("Connecté\n");
    puts("[Tapez '/quit' pour quitter] \n Vous pouvez désormer écrire des messages.");

    for(;;)
    {
        char temp[6];
        memset(&temp, sizeof(temp), 0);

        memset(&sBuf, sizeof(sBuf), 0); //initialise les buffers
        fgets(sBuf, 100, stdin); // prend le message ;

        if(sBuf[0] == '/' &&
           sBuf[1] == 'q' &&
           sBuf[2] == 'u' &&
           sBuf[3] == 'i' &&
           sBuf[4] == 't')
            return 1;
        
            
        int count = 0;
        while(count < strlen(nickName)) // on met le nom au début du méssage ( mise en forme du méssage) 
        {
            message[count] = nickName[count];
            count++;
        }
        count--;
        message[count] = ':';
        count++;
 
        for(int i = 0; i < strlen(sBuf); i++)
        {
            message[count] = sBuf[i];
            count++;
        }
        message[count] = '\0';
        puts(message);

      //envoi de données
        if(send(sockfd, message, strlen(message), 0) < 0)
        {
            puts("erreur du send");
            return 1;
        }
        memset(&sBuf, sizeof(sBuf), 0);
        

    }
    
    puts("Fermeture de la connection");
    pthread_join(recv_thread , NULL);
    close(sockfd);

    return 0;
}

//thread fonction qui gère la réception du méssage du serveur 
void *receive_handler(void *sock_fd)
{
    int sFd = (intptr_t) sock_fd;
    char buffer[MAXDATASIZE];
    int nBytes;
    
    for(;;)
    {
        if ((nBytes = recv(sFd, buffer, MAXDATASIZE-1, 0)) == -1)
        {
            perror("recv");
            exit(1);
        }
        else
            buffer[nBytes] = '\0';
        printf("%s", buffer);
    }
}
