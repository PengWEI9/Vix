/* Copyright (c) ERG Transit Systems (HK) Ltd. All rights reserved.
**
** Simple logging facility to a tcp port (32000).  There are no credentials
** associated with it but it does not receive (read) any data.
**
** Written by John Dennis 6/9/11.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

int main(int argc, char **argv)
{
	int sockfd, n;
	struct sockaddr_in servaddr;
	char recvline[1001];

	if (argc != 2)
	{
		printf("usage:  log_client <IP address>\n");
		exit(1);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port = htons(32000);

	if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) == 0)
	{
        while (1)
        {
            memset(recvline, 0, sizeof(recvline));
            n = recvfrom(sockfd, recvline, 100, 0, NULL, NULL);
            if (n > 0)
            {
                fputs(recvline, stdout);
            }
            else if (n == 0)
            {
                printf("socket closed: exiting\n");
                break;
            }
            else
            {
                // close the socket?
                printf("got errorno %d (%s): exiting\n", errno, strerror(errno));
                break;
            }
        }
    	close(sockfd);
    }
    else
    {
        printf("connect failed\n");
    }
    return 0;
}
