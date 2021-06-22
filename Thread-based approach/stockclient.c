


#include "csapp.h"

/* **********************************************
   stockclient.c is changed

   Rio_readlineb -> Rio_readnb
   If "exit", close clientfd
 * **********************************************/
int main(int argc, char **argv) 
{
    int clientfd;
    char *host, *port, buf[MAXLINE];
    rio_t rio;

    if (argc != 3) {
		fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
		exit(0);
    }
    host = argv[1];
    port = argv[2];

    clientfd = Open_clientfd(host, port);
    Rio_readinitb(&rio, clientfd);

    while (Fgets(buf, MAXLINE, stdin) != NULL) {
		Rio_writen(clientfd, buf, strlen(buf));
		Rio_readnb(&rio, buf, MAXLINE); //edit 
		if(strncmp(buf, "exit", 4) == 0) //edit
			break;
		Fputs(buf, stdout);
    }
    Close(clientfd); //line:netp:echoclient:close
    exit(0);
}
