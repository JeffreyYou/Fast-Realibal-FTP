
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <thread>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

using namespace std;

char tok[6];
int tok_len = 6;
char MTU_1500[1472];
char MTU_9001[8973];
int token = 0;

void set_token(int token_){
    //convert token to char array of 00xxxx format
    string count_str;
    count_str = to_string(token_);
    tok_len = tok_len-count_str.length();
    while(tok_len != 0){
        count_str = "0" + count_str;
        tok_len = tok_len - 1;
    }
    tok_len = 6;
    strcpy(tok, count_str.c_str());
}


void send(char* filename, bool MTU, int socket, int token){
//int main(){
    int n = 0;

    int no_packet;
    int count = 0;
    int seek;
    ifstream file;
    file.open("data.bin");
    //seekg() locate position of tokens
    //MTU = 1, packet is 1500 byte
    if(MTU){
        //MTU = 1500
        //decide the size of file, set header for token to be 6 byte
        //no_packet = 1024^3 / (1472-6) + 1;
        while(!file.eof()){
            
            if(count != token){
                seek = token*(1472-6);
                
            }
            else{
                //count tracks current sequence of packet;
                seek = count*(1472-6);
                count++;
            }
            //seek the current location
                
            file.seekg(seek,ios::beg);

            //MTU_1500 = tok + packet;
            file.read(MTU_1500+6, sizeof(MTU_1500)-6);
            for(n=0; n<6; n++){
                MTU_1500[n] = tok[n];
            }
            //cout << MTU_1500 << endl;
            //send
            n = sendto(socket, MTU_1500, sizeof(MTU_1500));
            //reset
            memset(MTU_1500, 0, sizeof(MTU_1500));
            token = count;
        }
    }
    else{
        //MTU = 9001
        while(!file.eof()){
            
            if(count != token){
                seek = token*(8973-6);
                
            }
            else{
                //count tracks current sequence of packet;
                seek = count*(8973-6);
                count++;
            }
            //seek the current location
                
            file.seekg(seek,ios::beg);

            //MTU_9001 = tok + packet;
            file.read(MTU_9001+6, sizeof(MTU_9001)-6);
            for(n=0; n<6; n++){
                MTU_9001[n] = tok[n];
            }
            //cout << MTU_9001 << endl;
            //send
            n = sendto(socket, MTU_9001, sizeof(MTU_9001));
            //reset
            memset(MTU_9001, 0, sizeof(MTU_9001));
            token = count;
        }
    }
    
    file.close();
}