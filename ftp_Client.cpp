#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <map>
#include <fstream>
using namespace std;

#define SERVER_PORT 9999
#define CLIENT_PORT 8888

int main(int argc, char const *argv[]){

    char send_buffer[] = "get /ect/network/interfaces";
    char recv_buffer[100];

    struct sockaddr_in client_recv_address, client_send_address;

    //create socket to send message to server
    int server_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&client_send_address, sizeof(client_send_address));

    client_send_address.sin_family = AF_INET;
    client_send_address.sin_port = htons(SERVER_PORT);//9999
    client_send_address.sin_addr.s_addr = INADDR_ANY;
    //inet_pton(AF_INET, "127.0.0.1", &client_send_address.sin_addr); 
 

    int numbytes = sendto(server_sockfd, send_buffer, sizeof(send_buffer), 0, (struct sockaddr*)&client_send_address, sizeof(client_send_address));
    if(numbytes < 0){
        cout<<"sendto error"<<endl;
    }
    cout << "port: "<<ntohs(client_send_address.sin_port) <<endl;
    while(1){
        numbytes = recvfrom(server_sockfd, recv_buffer, 100, 0, NULL, 0);
        if(numbytes > 0){
            break;
        }
    }
    

    cout<< recv_buffer << endl;
    close(server_sockfd);

    return 0;

}

// void ClientHandler(string s){
//     out.open("myfile.txt", ios::app);
//     out << str;
// }

