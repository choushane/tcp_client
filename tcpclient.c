/* 
 * tcpclient.c - A simple TCP client
 * usage: tcpclient <host> <port>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define BUFSIZE 1024
#define MAXNO(__A__, __B__) ((__A__ > __B__) ? __A__ : __B__)

/* 
 * error - wrapper for perror
 */
void error(char *msg) {
    perror(msg);
    exit(0);
}

int connectServer( char *hostname, int portno ){
    int sockfd;
    struct sockaddr_in serveraddr;
    struct hostent *server;

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        exit(0);
    }

    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
	  (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);

    /* connect: create a connection with the server */
    if (connect(sockfd, &serveraddr, sizeof(serveraddr)) < 0) 
      error("ERROR connecting");
    return sockfd;
}


int main(int argc, char **argv) {
    int n;
    int sockfd = -1;
    char buf[BUFSIZE];
    fd_set fds;
    int maxfd = 0;

    /* check command line arguments */
    if (argc != 3) {
       fprintf(stderr,"usage: %s <hostname> <port>\n", argv[0]);
       exit(0);
    }

    while(1){

	if(sockfd < 0) {
	    sockfd = connectServer(argv[1], atoi(argv[2]));
	}

    	FD_ZERO(&fds);
    	maxfd = 0;
    	FD_SET(sockfd, &fds);
    	maxfd = MAXNO(sockfd, maxfd);

	if(!select(maxfd+1, &fds, NULL, NULL, NULL)) continue;

	if(FD_ISSET(sockfd, &fds)){	
            bzero(buf, BUFSIZE);
            n = read(sockfd, buf, BUFSIZE);
            if (n <= 0){
          	error("ERROR reading from socket");
		sockfd = -1;
	    }
    	    printf("Echo from server: %s\n", buf);
	}
    }
    close(sockfd);
    return 0;
}
