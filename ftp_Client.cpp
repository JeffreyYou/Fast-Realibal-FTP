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
#include <arpa/inet.h>
#include <condition_variable> 
using namespace std;

int server_sockfd;
#define SERVER_PORT 9999
map<int,char*> packet_map;
int current_token=0;

char recv_buffer[1472];
char file_name[100];
char packet_num[100];
char packet_size[100];
char write_buffer[2000];

bool isFinish = false;
int total_packetNum = 0;
int total_packetSize = 0;

bool* check_p;  
bool* c_p;  
bool* c_p_e;  

int number_store;

struct sockaddr_in client_recv_address, client_send_address;


mutex mtx;
void set_token(int token_, char* thistok){
        //convert token to char array of 00xxxx format
        int tok_len = 6;

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
void set_token_2(int token_, char* thistok){
        //convert token to char array of 00xxxx format
        int tok_len = 6;

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
        sendto(server_sockfd, resend_buffer, sizeof(resend_buffer), 0, (struct sockaddr*)&client_send_address, sizeof(client_send_address));

        return 0;
}
int resend_packet_write(int number){
        char resend_buffer[6];
        set_token_2(number, resend_buffer);
        sendto(server_sockfd, resend_buffer, sizeof(resend_buffer), 0, (struct sockaddr*)&client_send_address, sizeof(client_send_address));

        return 0;
}

//----------------------------Store-----------------------
int store_packet_in_map(){
          
	    bool is_map_full = false;
        int total_sotred_packet=0;

        while(total_sotred_packet != total_packetNum){
            int token_num;

            //cout<<"before recv: "<< token_num<<endl;
            //memset(recv_buffer, 0, sizeof(recv_buffer));
            bzero(recv_buffer, sizeof(recv_buffer));
            int numbytes = recvfrom(server_sockfd, recv_buffer, 1500, 0, NULL, 0);
            token_num = 0;
            for(int j =0;j<6;j++){
                token_num = token_num * 10 + (recv_buffer[j]-'0');
            }

            //number_store = token_num;

            if((total_sotred_packet%300 == 0) && total_sotred_packet>0){
    		      is_map_full = true;
            }
    	    if(is_map_full){
    		      char ok_buffer[] = {'9','9','9','9','9','8','\0'};
           		  int c = sendto(server_sockfd, ok_buffer, strlen(ok_buffer), 0, (struct sockaddr*)&client_send_address, sizeof(client_send_address));
    		      is_map_full = false;
            }

            if(check_p[token_num] == false){ //not flase means drop this packet
                check_p[token_num] = true;
                
                char* ptr = (char*) malloc(1466);
                for(int u=0; u<1466; u++){
                    ptr[u] = recv_buffer[u];
                }
                pair<int,char*> p(token_num, ptr);
                //cout<<"store: "<< token_num<<endl;

                //cout<<"recv_buffer: "<<recv_buffer<<endl;

                mtx.lock();
                packet_map.insert(p);
                //cout<<"total_sotred_packet: "<<total_sotred_packet<<endl;
                total_sotred_packet++;

                mtx.unlock();

               // cout<<"map key: "<<token_num<<",value: "<<packet_map.count(token_num)<<endl;
                

            }

        }

        //cout<<"store exit"<<endl;
        return 0;
}

// -----------------------Write---------------------------
int write_packet_func(){
        ofstream write_file;
        //write_file.open("./test.txt", ios::binary | ios::out);
        write_file.open("./data1.bin", ios::binary | ios::out);


        while(current_token != total_packetNum){
            //cout<<"go to if with current_token : "<<current_token<<endl;
            if(packet_map.count(current_token)>0){
                //memset(write_buffer, 0, sizeof(write_buffer));

		        //strcpy(write_buffer, packet_map[current_token].c_str());
                //write_buffer = packet_map[current_token].c_str();
                //write_file<< packet_map[current_token].substr(6);
                //char* pp = packet_map[current_token];

                //write_file<< packet_map[current_token]+6;
                if(current_token != (total_packetNum-1)){
                    write_file.write(packet_map[current_token]+6, 1466);
                }else{
                    write_file.write(packet_map[current_token]+6, total_packetSize-(total_packetNum-1)*1466);
                }
                //cout<<packet_map[current_token]<<endl;
                //cout<<"write packet: "<<current_token<< ", Map size: "<<packet_map.size()<<endl;

	            mtx.lock();
                packet_map.erase(current_token);
                mtx.unlock();

                current_token++;

                //cout<<"Next write: "<<current_token<< ", Value: "<<packet_map.count(current_token)<<endl;

            }else{
	    
                resend_packet(current_token);           
                usleep(1);
		
            }
        }

        isFinish = true;
        return 0;
}

int write_resend_func_start(){
    c_p = check_p;
    int start = current_token;
    int end = number_store;
    int iterator = start;

    while(current_token != (total_packetNum+1)){
        //if(isFinish) break;

        while(iterator < end){
            if(isFinish) return 0;
            
             int iterator = start;
            if(c_p[iterator] == false){
                resend_packet_write(iterator);
                resend_packet_write(iterator);
                resend_packet_write(iterator);
                resend_packet_write(iterator);
                resend_packet_write(iterator);
     
            }  
            iterator++;
            

        }
        start = current_token;
        end = number_store;
        iterator = start;
    }
    

    return 0;

}
int write_resend_func_middle_toend(){
    c_p = check_p;
    int start = current_token;
    int end = number_store;
    int iterator = (start+end)/2;

    while(current_token != (total_packetNum+1)){
       // if(isFinish) break;

        while(iterator < end){
            
            if(isFinish) return 0;
            
            int iterator = start;
            if(c_p[iterator] == false){
                resend_packet_write(iterator);
                resend_packet_write(iterator);
                resend_packet_write(iterator);
                resend_packet_write(iterator);
                resend_packet_write(iterator);
     
            }  
            iterator++;
        }
        start = current_token;
        end = number_store;
        iterator = start;
    }
    

    return 0;

}
int write_resend_func_tostart(){
    c_p_e = check_p;
    int start = number_store;
    int end = current_token;
    int iterator = (start+end)/2;

    while(current_token != (total_packetNum+1)){
        //if(isFinish) break;

        while(iterator > end){
            
            if(isFinish) return 0;
            
            int iterator = start;
            if(c_p_e[iterator] == false){
                resend_packet_write(iterator);
                resend_packet_write(iterator);
                resend_packet_write(iterator);
                resend_packet_write(iterator);
                resend_packet_write(iterator);
     
            }  
            iterator--;
        }
        start = number_store;
        end = current_token;
        iterator = start;
    }
    

    return 0;

}
int write_resend_func_end(){

    c_p_e = check_p;
    int start = number_store;
    int end = current_token;
    int iterator = start;

    while(current_token != (total_packetNum+1)){
        //if(isFinish) break;
        while(iterator > end){
            if(isFinish) return 0;
            
            int iterator = start;
            if(c_p_e[iterator] == false){
                resend_packet_write(iterator);
                resend_packet_write(iterator);
                resend_packet_write(iterator);
                resend_packet_write(iterator);
                resend_packet_write(iterator);
     
            }  
            iterator--;
        }
        start = number_store;
        end = current_token;
        iterator = start;
    }
    

    return 0;

}
int main(int argc, char const *argv[]){

    //char send_buffer[] = "get ./test.txt";
    char send_buffer[] = "get ./data1.bin";
    

    string fileName;
    int packetNum;

    //memset(recv_buffer, 0, sizeof(recv_buffer));


    //create socket to send message to server
    server_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&client_send_address, sizeof(client_send_address));

    client_send_address.sin_family = AF_INET;
    client_send_address.sin_port = htons(SERVER_PORT);//9999
    //client_send_address.sin_addr.s_addr = INADDR_ANY;
    //inet_pton(AF_INET, "10.0.1.108", &client_send_address.sin_addr); 
    inet_pton(AF_INET, "192.168.10.33", &client_send_address.sin_addr); 
 

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
    create_file.open(fileName, ios::binary | ios::out | ios::trunc);
    create_file.close();
    
    memset(packet_num, 0, sizeof(packet_num));
    numbytes = recvfrom(server_sockfd, packet_num, 100, 0, NULL, 0);
    //store the packet number
    for(int s=0; s<strlen(packet_num); s++){
        total_packetNum = total_packetNum*10 + (packet_num[s]-'0');
    }
    cout<<"the total packet to be sent: \""<< total_packetNum <<"\""<< endl;

    bool recv_check[total_packetNum];
    for(int i = 0;i<total_packetNum;i++){
        recv_check[i] = false;
    }
    check_p = recv_check;

    memset(packet_size, 0, sizeof(packet_size));
    numbytes = recvfrom(server_sockfd, packet_size, 100, 0, NULL, 0);
    for(int s=0; s<strlen(packet_size); s++){
        total_packetSize = total_packetSize*10 + (packet_size[s]-'0');
    }
    cout<<"the total packet size: \""<< total_packetSize <<"\""<< endl;


    thread store_packet(store_packet_in_map);
    thread write_packet(write_packet_func);
    //thread checker(write_resend_func_start);
    //thread checker_end(write_resend_func_end);
    //thread checker_middle_to_end(write_resend_func_middle_toend);
    //thread checker_middle_to_start(write_resend_func_tostart);



    store_packet.join();
    write_packet.join();

    //checker.join();
    //checker_end.join();
    //checker_middle_to_start.join();
    //checker_middle_to_end.join();

    char finish_buffer[] = {'9','9','9','9','9','9','\0'};
    cout<<"------------Trasmission Finish!----------------" << endl;

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
// 

