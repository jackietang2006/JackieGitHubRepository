#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char *argv[])
{
   int   sd;
   struct   sockaddr_in server;
   char buf[512];
   int rc, portno;

   if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
   }
   portno = atoi(argv[1]);
   server.sin_family = AF_INET;
   server.sin_addr.s_addr = htonl(INADDR_ANY);
   server.sin_port = htons(portno);

   sd = socket (AF_INET,SOCK_DGRAM,0);

   bind (sd, (struct sockaddr *) &server, sizeof(server));
   
   for(;;){
      rc = recv (sd, buf, sizeof(buf), 0);
      buf[rc]= (char) NULL;
      printf("Received: %s\n", buf);
   }
}
