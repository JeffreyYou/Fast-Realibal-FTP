//********************************************************************
//
// Name: Vignesh Sivanandha Rao
// Computer Networks
// October 27, 2018
//
//********************************************************************

#include <stdio.h>
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> 
#include <stdio.h> 

#include <stdbool.h>

struct Map{
	int key;
	char val[100];
} map;
struct Map indexToFile[1000];
struct Map localindexToFile[1000];


void listClientFiles(){
  
  DIR *d;
  struct dirent *dir;
  d = opendir(".");
  if (d) {
  	int count = 1;
    while ((dir = readdir(d)) != NULL) {
      if (dir->d_type == DT_REG)
	  {
	     printf("%d. %s\n", count, dir->d_name);
	     localindexToFile[count - 1].key = count;
 		strcpy(localindexToFile[count - 1].val, dir->d_name);

	     count++;
	  }
    }
    closedir(d);
  }
}

void printAndMakedir(char* str){
  char * pch;
  pch = strtok (str," ");
  int count = 1;
  memset(indexToFile, 0, sizeof(indexToFile));
  while (pch != NULL)
  {
    printf ("%d. %s\n",count, pch);
    indexToFile[count - 1].key = count;
 	strcpy(indexToFile[count - 1].val, pch);
    pch = strtok (NULL, "  ");
    count++;
  }
}

bool SendFileOverSocket(int socket_desc, char* file_name){

	struct stat	obj;
	int		file_desc,
			file_size;

	stat(file_name, &obj);
	file_desc = open(file_name, O_RDONLY);
	file_size = obj.st_size;
	printf("sending filesize %d\n", file_size);
	send(socket_desc, &file_size, sizeof(int), 0);
	sendfile(socket_desc, file_desc, NULL, file_size);
	printf("ftp>File %s uploaded successfully %d bytes sent \n", file_name, file_size );

	return true;
}

int main(int argc , char **argv)
{
	int 	socket_desc;
	struct 	sockaddr_in server;
	char 	request_msg[BUFSIZ], reply_msg[BUFSIZ];
	char SERVER_IP[100], FILENAME[100];
	if(argc<3){
		printf("usage ./client <SERVER_IP> <SERVER_PORT>\n");
		exit(0);
	}
	strcpy(SERVER_IP, argv[1]);
	int SERVER_PORT = atoi(argv[2]);
	printf("%s %d\n", SERVER_IP, SERVER_PORT);
	// Variables for the file being received
	int	file_size,
		file_desc;
	char	*data;
		
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_desc == -1)
	{
		perror("Could not create socket");
		return 1;
	}

	server.sin_addr.s_addr = inet_addr(SERVER_IP);
	server.sin_family = AF_INET;
	server.sin_port = htons(SERVER_PORT);

	// Connect to server
	if (connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		perror("Connection failed");
		return 1;
	}

	printf("Connection successfully established with the ftp server\n");

	while(1){
		printf("ftp>");
		scanf ("%[^\n]%*c", request_msg);
		bzero(reply_msg, sizeof(reply_msg));

		if(request_msg[0] == 'l' && request_msg[1]=='s'){
			char *file_name = strchr(request_msg, ' ');
			// printf("%s\n", file_name);
			if(strcmp(file_name+1,"client") == 0){
				listClientFiles();
			}
			else if(strcmp(file_name+1,"server") == 0){
				write(socket_desc, request_msg, strlen(request_msg));
				recv(socket_desc, reply_msg, sizeof(reply_msg), 0);
				// printf("%s\n", reply_msg );
				printAndMakedir(reply_msg);
				// printf("%s\n",indexToFile[0].val );
			}
		}
		else if(request_msg[0] == 'd'){
			char* str = strchr(request_msg, ' ');
			int index = atoi(str+1);
			// printf ("file to download %s\n",indexToFile[index - 1].val);
			strcpy(FILENAME, indexToFile[index - 1].val);
			// Get a file from server
			strcpy(request_msg, "Get ");
			strcat(request_msg, FILENAME);

			write(socket_desc, request_msg, strlen(request_msg));
			recv(socket_desc, reply_msg, 2, 0);

			// Start receiving file
			if (strcmp(reply_msg, "OK") == 0) {

				recv(socket_desc, &file_size, sizeof(int), 0);
				data = malloc(file_size);
				file_desc = open(FILENAME, O_CREAT | O_EXCL | O_WRONLY, 0666);
				recv(socket_desc, data, file_size, 0);
				write(file_desc, data, file_size);
				printf("ftp>File %s downloaded successfully %d bytes received \n", FILENAME, file_size );
				close(file_desc);
			}
			else {

				fprintf(stderr, "Bad request\n");
			}
			bzero(FILENAME, sizeof(FILENAME));

		}
		else if(request_msg[0] == 'b' && request_msg[1] == 'y' && request_msg[2] == 'e'){
			printf("Closing the connection\n");
			close(socket_desc);
			return 0;
		}
		else if(request_msg[0] == 'u'){
			char* str = strchr(request_msg, ' ');
			int index = atoi(str+1);
			// printf ("file to download %s\n",indexToFile[index - 1].val);
			strcpy(FILENAME, localindexToFile[index - 1].val);
			strcpy(request_msg, "u ");
			strcat(request_msg, FILENAME);

			write(socket_desc, request_msg, strlen(request_msg));
			// write(socket_desc, request_msg, strlen(request_msg));
			SendFileOverSocket(socket_desc, FILENAME);

		}

	}
	

	return 0;
}
