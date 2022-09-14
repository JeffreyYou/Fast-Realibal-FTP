//********************************************************************
//
// Name: Vignesh Sivanandha Rao 
// Computer Networks 
// October 27, 2018
//
//********************************************************************

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <arpa/inet.h> 

#include <dirent.h> 
#include <stdio.h>
#define SERVER_PORT 12000

void* ConnectionHandler(void* socket_desc);


char* GetFilenameFromRequest(char* request);
bool SendFileOverSocket(int socket_desc, char* file_name);

void listClientFiles(char* server_response){
  char all_files[1000];
  DIR *d;
  struct dirent *dir;
  d = opendir(".");
  if (d) {
  	int count = 1;
    while ((dir = readdir(d)) != NULL) {
    	if (dir->d_type == DT_REG)
	  {
	  	strcat(server_response, dir->d_name);
	  	strcat(server_response," ");
	    count++;
	  }
    }
    closedir(d);
  }

}

int main(int argc, char **argv)
{
	int     socket_desc, 
		socket_client, 
		*new_sock, 
		c = sizeof(struct sockaddr_in);
	struct  sockaddr_in	server, client;

	//Socket Creation 
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_desc == -1)
	{
		perror("Could not create socket");
		return 1;
	}
	if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0)
    	perror("setsockopt(SO_REUSEADDR) failed");

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(SERVER_PORT);

	if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		perror("Bind failed");
		return 1;
	}
	//Setup listening socket
	listen(socket_desc , 3);
	while (socket_client = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c))
	{
		pthread_t sniffer_thread, client_thread;
		new_sock = malloc(1);
		*new_sock = socket_client;        
		
		pthread_create(&sniffer_thread, NULL,  ConnectionHandler, (void*) new_sock);
		pthread_join(sniffer_thread, NULL);
	}
	 
	if (socket_client<0)
	{
		perror("Accept failed");
		return 1;
	}

	return 0;
}


void *UploadHandler(int socket, char* file_name)
{
	   
	char	server_response[BUFSIZ],
		client_request[BUFSIZ];
	int	file_size,
		file_desc;
	char	*data;
	recv(socket, &file_size, sizeof(int), 0);
	printf("%d\n",file_size );
	data = malloc(file_size);
	file_desc = open(file_name, O_CREAT | O_EXCL | O_WRONLY, 0666);
	recv(socket, data, file_size, 0);
	write(file_desc, data, file_size);
	close(file_desc);
  
	return 0;
}

void *ConnectionHandler(void *socket_desc)
{
	int	socket = *(int*)socket_desc;   
	char	server_response[BUFSIZ],
		client_request[BUFSIZ],
		file_name[BUFSIZ];

	while(1){
		bzero(client_request, sizeof(client_request));
		if(recv(socket, client_request, sizeof(client_request), 0) == 0){
			return 0;	
		}
		bzero(server_response, BUFSIZ);
		if(client_request[0] == 'l' && client_request[1]=='s'){
			listClientFiles(server_response);
			printf("received ls from client, sending list of files in current dir\n");
			write(socket, server_response, strlen(server_response));
			
			continue;
		}
		else if(client_request[0] == 'u'){
			strcpy(file_name, GetFilenameFromRequest(client_request));
			printf("received u\n");\
			UploadHandler(socket, file_name);
			continue;
		}
		strcpy(file_name, GetFilenameFromRequest(client_request));
		printf("Reqest received: %s\n", client_request );
		// If requested file exists, notify the client and send it
		if (access(file_name, F_OK) != -1){
			
			strcpy(server_response, "OK");
			write(socket, server_response, strlen(server_response));
			SendFileOverSocket(socket, file_name);
		}
		else {
			// Requested file does not exist, notify the client
			strcpy(server_response, "NO");
			write(socket, server_response, strlen(server_response)); 
		}
	}
	free(socket_desc);   
	return 0;
}

char* GetFilenameFromRequest(char* request){

	char *file_name = strchr(request, ' ');
	return file_name + 1;
}

bool SendFileOverSocket(int socket_desc, char* file_name){

	struct stat	obj;
	int		file_desc,
			file_size;

	stat(file_name, &obj);
	file_desc = open(file_name, O_RDONLY);
	file_size = obj.st_size;
	send(socket_desc, &file_size, sizeof(int), 0);
	sendfile(socket_desc, file_desc, NULL, file_size);

	return true;
}
