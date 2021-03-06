/*
Student ID : 20123426
Name : 최진성
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>

void display();


int peertcpSocket = -1;	// peer socket

int main(int argc, char **argv)
{
	
  int tcpServ_sock;
  
  struct sockaddr_in tcpServer_addr;
  struct sockaddr_in tcpClient_addr;
  struct sockaddr_in newTcp_addr;

  socklen_t clnt_len;
    
  fd_set reads, temps;
  int fd_max;

  char command[1024];


  char *tcpport = NULL;
  char *userid = NULL;

// NEED TO ADD SOME VARIABLES 
  char *token = NULL;
  char str[]=" \n";
  char message[100];
 
  if(argc != 3){
    printf("Usage : %s <tcpport> <userid>\n", argv[0]);
    exit(1);
  }
  
  tcpport=argv[1];
  userid=argv[2];

  display();
	

	// NEED TO CREATE A SOCKET FOR TCP SERVER
	tcpServ_sock = socket(PF_INET, SOCK_STREAM, 0);
  	memset(&tcpServer_addr, 0, sizeof(tcpServer_addr));
  	tcpServer_addr.sin_family=AF_INET;
	tcpServer_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	tcpServer_addr.sin_port=htons(atoi(tcpport));

	// NEED TO bind
	if(bind(tcpServ_sock, (struct sockaddr*) &tcpServer_addr, sizeof(tcpServer_addr))==-1)
		printf("bind() error\n");	

	// NEED TO listen
  	if(listen(tcpServ_sock, 5)==-1)
		printf("listen() error\n");

	// initialize the select mask variables and set the
	// mask with stdin and the tcp server socket
	FD_ZERO(&reads);
	FD_SET(fileno(stdin), &reads);
	FD_SET(tcpServ_sock, &reads);
	fd_max=tcpServ_sock;

  	printf("%s> \n", userid);

  while(1)
  {
    int nfound;
    char *hostname;
    char *portno;
    char tempCmd[1024];
    temps = reads;
    
    nfound = select(fd_max+1, &temps, 0, 0, NULL);
	
	if(FD_ISSET(fileno(stdin), &temps)) {
		// Input from the keyboard
		fgets(command, sizeof (command), stdin);
  		FD_CLR(fileno(stdin), &temps);
		
		// NEED TO IMPLEMENT for input from keyboard
		strcpy(tempCmd, command);
		token = strtok(tempCmd, str);
		if(!strcmp(tempCmd, "@quit"))
		{
			printf("Connection Closed %d\n", peertcpSocket);
			close(peertcpSocket);	
			peertcpSocket = socket(PF_INET, SOCK_STREAM, 0);
			exit(0);
		}
		else if(!strcmp(token, "@talk"))
		{
			token = strtok(NULL, str);
			if(!strcmp(token, "localhost"))
				hostname = "127.0.0.1";
			else
				strcpy(hostname, token);
			portno = strtok(NULL, str);
			
			// NEED TO CREATE A SOCKET FOR TCP CLIENT
			peertcpSocket = socket(PF_INET, SOCK_STREAM, 0);
  			memset(&tcpClient_addr, 0, sizeof(tcpClient_addr));
  			tcpClient_addr.sin_family=AF_INET;
			tcpClient_addr.sin_addr.s_addr=inet_addr(hostname);
			tcpClient_addr.sin_port=htons(atoi(portno));

			if(connect(peertcpSocket, (struct sockaddr*)&tcpClient_addr, sizeof(tcpClient_addr))==-1)
				printf("connect() error\n");
			else
			{
				FD_SET(peertcpSocket, &reads);
				if(fd_max<peertcpSocket)
					fd_max=peertcpSocket;
			}
		}
		else
		{
			write(peertcpSocket, command, strlen(command));			
		}
  		printf("%s> \n", userid);
	}
	else if(FD_ISSET(tcpServ_sock, &temps))
	{
		//connect request from a peer
		clnt_len=sizeof(newTcp_addr);
		peertcpSocket=accept(tcpServ_sock, (struct sockaddr*)&newTcp_addr, &clnt_len);
		FD_SET(peertcpSocket, &reads);
		if(fd_max<peertcpSocket)
			fd_max=peertcpSocket;
		printf("connection from host %s, port %d, socket %d\n", 
			inet_ntoa(newTcp_addr.sin_addr), ntohs(newTcp_addr.sin_port), peertcpSocket);
	}
	else if(FD_ISSET(peertcpSocket, &temps))
	{
		// message from a peer
		int str_len=read(peertcpSocket, message, 100);
		if(str_len==0)
		{
			FD_CLR(peertcpSocket, &reads);
			printf("Connection Closed %d\n", peertcpSocket);
			close(peertcpSocket);
			peertcpSocket = socket(PF_INET, SOCK_STREAM, 0);
		}
		else printf("%s\n", message);
	}

  }//while End
  close(tcpServ_sock);
  return 0;
}//main End

void display()
{
	printf("Student ID : 20123426 \n");  // 이 위치에 본인 학번을 기입할 것
	printf("Name : Jinsung \n");      // 이 위치에 본인 이름을 기입할 것
}



