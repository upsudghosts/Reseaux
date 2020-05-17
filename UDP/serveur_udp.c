/* SERVEUR UDP */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>

#define BUFLEN 512
#define PORT 9930

void err(char *str)
{
    perror(str);
    exit(1);
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
    struct sockaddr_in my_addr, cli_addr;
    int sockfd, i;
    socklen_t slen=sizeof(cli_addr);
    char buf[BUFLEN];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)// création d'une socket udp 
      err("socket");
    else
      printf("Serveur : succès de la création de la Socket \n");
	
//initialisation de la structure utiliser dans le bind de ma socket udp 
    bzero(&my_addr, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(PORT);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (bind(sockfd, (struct sockaddr* ) &my_addr, sizeof(my_addr))==-1)
      err("bind");
    else
      printf("Serveur : succès du bind\n");

    char hostbuffer[256];
    char *IPbuffer;
    struct hostent *host_entry;
    int hostname;
    
    // récupération des informations de l'hôte du serveur  pour ensuite les affichers 
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
        if (recvfrom(sockfd, buf, BUFLEN, 0, (struct sockaddr*)&cli_addr, &slen)==-1) // réception des données par le serveur 
            err("recvfrom()");
        printf("Reçu par %s:%d\nDonné: %s\n\n",
               inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), buf);
    }

    close(sockfd);
    return 0;
}
