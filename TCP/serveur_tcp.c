/* SERVEUR TCP */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT "9034"   // le port utilisé

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


void checkHostName(int hostname)
{
    if (hostname == -1)
    {
        perror("gethostname");
        exit(1);
    }
}
  

void checkHostEntry(struct hostent * hostentry)
{
    if (hostentry == NULL)
    {
        perror("gethostbyname");
        exit(1);
    }
}
  

void checkIPbuffer(char *IPbuffer)
{
    if (NULL == IPbuffer)
    {
        perror("inet_ntoa");
        exit(1);
    }
}

int main(void)
{
    fd_set master;    //liste de descripteur de fichier 
    fd_set read_fds;  //list tempon de descripteur pour le select()
    int fdmax;        // le maximum de descripteur de fichier 

    int listener;     //descripteur de la socket 
    int newfd;        //descripteur de la nouvelle socket  
    struct sockaddr_storage remoteaddr; //client address
    socklen_t addrlen;

    char buf[256];    //buffer pour les donner reçu du client 
    int nbytes;

    char remoteIP[INET6_ADDRSTRLEN]; //

    int yes=1;        //valeur de l'option SO_REUSEADDR (cette option permet de réutiliser les adresses locales de la socket (cela permet de bind toute les sockets sauf celle déjà bind).
    int i, j, rv;

    struct addrinfo hints, *ai, *p;

    FD_ZERO(&master);    
    FD_ZERO(&read_fds);  //initialise les structures marte et read_fds

    //initialise la structure hint qui nous sert de filtre pour les sockets à rechercher
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }
    //mise en écoute de tout les sockets filtrés
    for(p = ai; p != NULL; p = p->ai_next)
    {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) {
            continue;
        }
        
        
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    // On entre dans cette boucle if si la fonction bind à échoué.
    if (p == NULL) {
        fprintf(stderr, "serveur selectionné: erreur de  bind\n");
        exit(2);
    }

    freeaddrinfo(ai); //free la structure ai
    
    puts("succès du binding!");
  
    if (listen(listener, 10) == -1)
    {
        perror("listen");
        exit(3);
    }
    puts(" server en écoute...");
    
    // ajoute l'écoutant à la liste master
    FD_SET(listener, &master);

    // On garde le maximum des descripteur ce qui nous permetera plus tard de les parcourirs
    fdmax = listener; //(il vaut 1 à la première itération)

    char hostbuffer[256];
    char *IPbuffer;
    struct hostent *host_entry;
    int hostname;
    
    // on cherche les informations de l'hôte du serveur
    hostname = gethostname(hostbuffer, sizeof(hostbuffer));
    checkHostName(hostname);
      
  
    host_entry = gethostbyname(hostbuffer);
    checkHostEntry(host_entry);
    
    
    IPbuffer = inet_ntoa(*((struct in_addr*)
                           host_entry->h_addr_list[0]));
    
    printf("Hostname: %s\n", hostbuffer);
    printf("Host IP: %s\n", IPbuffer);
    

    while(1)
    {
        read_fds = master; 
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) // surveille les descripteur de fichier de notre liste et attend qu'au moins l'un des fichiers soit prêt
        {
            perror("select");
            exit(4);
        }

        //parcour les conexions éxistantes pour lire des données
        for(i = 0; i <= fdmax; i++) 
        {
            if (FD_ISSET(i, &read_fds))// on cherche si le descripteur i est en écoute
            {   
                if (i == listener)
                {
                    
                    addrlen = sizeof remoteaddr;
                    newfd = accept(listener,
                        (struct sockaddr *)&remoteaddr,
                        &addrlen);

                    if (newfd == -1)// si accept ne fonctionne pas 
                    {
                        perror("accept");
                    }
                    else
                    {
                        FD_SET(newfd, &master); // met le noveau descripteur dans la liste des conexions en attente
                        if (newfd > fdmax) //on met à jour le plus grand descripteur
                        {   
                            fdmax = newfd;
                        }
                        printf("serveur sélectionné: nouvelle connexion de %s au"
                               "socket %d\n", inet_ntop(remoteaddr.ss_family,
                               get_in_addr((struct sockaddr*)&remoteaddr),
                               remoteIP, INET6_ADDRSTRLEN), newfd);
                    }
                }
                else
                {
                    // alors si on a des données 
                    if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0)
                    {   // si on a eu une erreur lors de la reception des données 
                        if (nbytes == 0)
                        {   // connection fermée
                            printf("serveur sélectionné: socket %d connexion fermée\n", i);
                        }
                        else
                        {
                            perror("recv");
                        }
                        close(i);  
                        FD_CLR(i, &master); // 
                    }
                    else
                    {
                        // si on a eu des donnée du client 
                        for(j = 0; j <= fdmax; j++)
                        {
                            // broadcast 
                            if (FD_ISSET(j, &master))
                            {
                                // sauf le descripteur en écoute et nous même
                                if (j != listener && j != i)
                                {
                                    if (send(j, buf, nbytes, 0) == -1) //envoi de donnée au client
                                    {
                                        perror("send");
                                    }
                                }
                            }
                        }
                    }
                }
            } 
        } 
    } 
    
    return 0;
}
