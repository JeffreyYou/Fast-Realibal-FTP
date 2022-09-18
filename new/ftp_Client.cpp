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
#include <map>
#include <iostream>
#include <thread>
#include <fstream>
#include <mutex>

using namespace std;

int server_sockfd;
#define SERVER_PORT 9999
map<int,string> packet_map;
int current_token=0;

char recv_buffer[2000];
char file_name[100];
char packet_num[100];
char write_buffer[2000];

bool isFinish = false;
int total_sotred_packet=0;
int total_packetNum = 0;
int tok_len = 6;

bool* check_p;  
struct sockaddr_in client_recv_address, client_send_address;


mutex mtx;
void set_token(int token_, char* thistok){
        //convert token to char array of 00xxxx format
        string count_str;
        count_str = to_string(token_);
        tok_len = tok_len-count_str.length();
        while(tok_len != 0){
            count_str = "0" + count_str;
            tok_len = tok_len - 1;
        }
        tok_len = 6;
        strcpy(thistok, count_str.c_str());
}
int resend_packet(int number){
        char resend_buffer[6];
        set_token(number, resend_buffer);
        mtx.lock();
        sendto(server_sockfd, resend_buffer, sizeof(resend_buffer), 0, (struct sockaddr*)&client_send_address, sizeof(client_send_address));
        mtx.unlock();

        return 0;
}

int store_packet_in_map(){
        int before=0;
        int now=0;  

        while(total_sotred_packet!=total_packetNum){

            memset(recv_buffer, 0, sizeof(recv_buffer));
            int numbytes = recvfrom(server_sockfd, recv_buffer, 2000, 0, NULL, 0);
            int token_num = 0;
            for(int j =0;j<6;j++){
                token_num = token_num * 10 + (recv_buffer[j]-'0');
            }
            cout<<"store packet: "<<token_num<<endl;
            now = token_num;
            // if((now-before)>1){ //some packet lost
            //     //resend before+1

            //     thread requset_packet(resend_packet,before+1);
            //     //usleep();

            // }
            if(check_p[token_num] == false){ //not flase means drop this packet
                mtx.lock();
                check_p[token_num] == true;
                string str(recv_buffer);
                pair<int,string> p(token_num, str);
                packet_map.insert(p);
                total_sotred_packet++;
                before = token_num;
                mtx.unlock();
            }

        }
        return 0;
}
int write_packet_func(){
        ofstream write_file;
        write_file.open("./data_client.txt");

        while(current_token != total_packetNum){

            if(packet_map.count(current_token)>0){
                memset(write_buffer, 0, sizeof(write_buffer));

                //write_buffer = packet_map[current_token].c_str();
                write_file<< packet_map[current_token].substr(6);
                mtx.lock();
                packet_map.erase(current_token);

                mtx.unlock();

                cout<<"write packet: "<<current_token<<endl;
                current_token++;

            }else{
                //thread requset_packet(resend_packet,current_token);
                //requset_packet.join();
                //usleep(100);
                //resend
                cout<<"resend: "<<current_token<<endl;
                resend_packet(current_token);
                usleep(200);
            }
        }
        isFinish = true;
        return 0;
}
   

    // int i = 27;
    // ofstream write_file;
    // write_file.open("./data_client.txt");
    // int token_num;
    // while(i>0){
    //     numbytes = recvfrom(server_sockfd, recv_buffer, 2000, 0, NULL, 0);
    //     token_num = 0;
    //     for(int j =0;j<6;j++){
    //         token_num = token_num * 10 + (recv_buffer[j]-'0');
    //     }

    //     cout<< "Receive packet: "<<token_num <<endl;
    //     write_file<< recv_buffer+6;
    //     memset(recv_buffer, 0, sizeof(recv_buffer));
    //     i--;
    // }
    // write_file.close();

int main(int argc, char const *argv[]){

    char send_buffer[] = "get ./data.txt";
    

    string fileName;
    int packetNum;

    memset(recv_buffer, 0, sizeof(recv_buffer));


    //create socket to send message to server
    server_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
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
    memset(file_name, 0, sizeof(file_name));
    numbytes = recvfrom(server_sockfd, file_name, 100, 0, NULL, 0);
    //store the file name
    fileName = file_name; 
    cout<<"the name of the file: \""<< fileName <<"\""<< endl;
    //create file
    ofstream create_file;
    create_file.open("data_client.txt", ios::out | ios::trunc);
    create_file.close();
    
    memset(packet_num, 0, sizeof(packet_num));
    numbytes = recvfrom(server_sockfd, packet_num, 100, 0, NULL, 0);
    //store the packet number
    for(int s=0; s<strlen(packet_num); s++){
        total_packetNum = total_packetNum*10 + (packet_num[s]-'0');
    }
    cout<<"the total packet to be sent: \""<< total_packetNum <<"\""<< endl;

    bool recv_check[packetNum];
    for(int i = 0;i<packetNum;i++){
        recv_check[i] = false;
    }
    check_p = recv_check;

    thread store_packet(store_packet_in_map);
    thread write_packet(write_packet_func);

    store_packet.join();
    write_packet.join();
    char finish_buffer[] = {'9','9','9','9','9','9','\0'};
    cout<<"finish_buffer: "<< finish_buffer << endl;

    if(isFinish){
       int c = sendto(server_sockfd, finish_buffer, strlen(finish_buffer), 0, (struct sockaddr*)&client_send_address, sizeof(client_send_address));
        if(c<0){
        cout<<"send fail"<<endl;
        }
    }
    close(server_sockfd);
    
    return 0;

}

// void ClientHandler(string s){
//     out.open("myfile.txt", ios::app);
//     out << str;
// }

