#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int main(int argc, char *argv[])
{
    int	sd, portno;
    struct	sockaddr_in server;
    struct  hostent *hp;

    if (argc < 3) {
         fprintf(stderr,"ERROR, no server IP & port provided\n");
         exit(1);
    }

    sd = socket (AF_INET,SOCK_DGRAM,0);

    server.sin_family = AF_INET;
    hp = gethostbyname(argv[1]);
    bcopy ( hp->h_addr, &(server.sin_addr.s_addr), hp->h_length);
    portno = atoi(argv[2]);
    server.sin_port = htons(portno);

    for (;;) {
        sendto(sd, "HI",2, 0, (const sockaddr*)&server, sizeof(server));
        sleep(2);
    }
}
