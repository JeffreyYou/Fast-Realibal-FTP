#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>

#include <iostream>
#include <fstream>
using namespace std;

#define SERVER_PORT 9999
#define CLIENT_PORT 8888

int main(int argc, char const *argv[]){

    char send_buffer[] = "get ./data.txt";
    char recv_buffer[2000];
    memset(recv_buffer, 0, sizeof(recv_buffer));

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

    //recv file name
    numbytes = recvfrom(server_sockfd, recv_buffer, 100, 0, NULL, 0);
    cout<<"the name of the file: \""<< recv_buffer <<"\""<< endl;
    //create file
    ofstream create_file;
    create_file.open("data_client.txt", ios::out | ios::trunc);
    create_file.close();
    
    memset(recv_buffer, 0, sizeof(recv_buffer));
    numbytes = recvfrom(server_sockfd, recv_buffer, 100, 0, NULL, 0);
    cout<<"the total packet to be sent: \""<< recv_buffer <<"\""<< endl;
    memset(recv_buffer, 0, sizeof(recv_buffer));


    int i = 27;
    ofstream write_file;
    write_file.open("./data_client.txt");
    int token_num;
    while(i>0){
        numbytes = recvfrom(server_sockfd, recv_buffer, 2000, 0, NULL, 0);
        token_num = 0;
        for(int j =0;j<6;j++){
            token_num = token_num * 10 + (recv_buffer[j]-'0');
        }

        cout<< "Receive packet: "<<token_num <<endl;
        write_file<< recv_buffer+6;
        memset(recv_buffer, 0, sizeof(recv_buffer));
        i--;
    }
    write_file.close();
    //cout<<"Server starts sending file \""<< recv_buffer <<"\""<< endl;
    close(server_sockfd);

    return 0;

}

// void ClientHandler(string s){
//     out.open("myfile.txt", ios::app);
//     out << str;
// }

