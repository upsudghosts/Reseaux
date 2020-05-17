/* CLIENT UDP */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#define BUFLEN 512
#define PORT 9930

void err(char *s)
{
    perror(s);
    exit(1);
}

int main(int argc, char** argv)
{
    struct sockaddr_in serv_addr;
    int sockfd, i, slen=sizeof(serv_addr);
    char buf[BUFLEN];

    if(argc != 2)
    {
      printf("Usage : %s <Server-IP>\n",argv[0]);
      exit(0);
    }

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
        err("socket");
// initialisation de la strucyure d"envoie de donnée 
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_aton(argv[1], &serv_addr.sin_addr)==0)
    {
        fprintf(stderr, "fail de inet_aton() \n");
        exit(1);
    }

    printf ("Connecté au serbeur avec succès. ");
    while(1)
    {
        printf("\nEntrez les données à envoyer (Ecrivez exit et pressez entrée pour sortir) : ");
        scanf("%[^\n]",buf);// enregistre tout sauf le retour à la ligne 
        getchar();
        if(strcmp(buf,"exit") == 0)
          exit(0);

        if (sendto(sockfd, buf, BUFLEN, 0, (struct sockaddr*)&serv_addr, slen)==-1) // envoie les donées au serveur 
            err("sendto()");
    }

    close(sockfd);
    return 0;
}
