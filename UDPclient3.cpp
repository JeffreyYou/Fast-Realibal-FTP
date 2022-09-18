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
#include <thread>
#include <chrono>
#include <mutex>
using namespace std;

int RTT = 0; //
int currenttoken = 0; //
int numoftoken = 0; //
bool* tokenlist; //
static ofstream out; //
bool notfinished = true;
int minkey = 100000000;
mutex mtx;

map<int,string> tkmap;

void CountThread(int sck, sockaddr_in hintM){
    // this thread will count the next 5 packets which are needed
    // it will send them to the server
    int s = 0;
    int z = 5;
    while(notfinished){
        mtx.lock();
        s = currenttoken;
        z = 5;
        if(currenttoken == numoftoken){
            notfinished = false;
            mtx.unlock();
            break;
        }

        string misspkt = "";
        while(z > 0 && s<numoftoken){
            if(tokenlist[s] == false){
                misspkt += to_string(s) + " ";
                z --;
            }
            s++;
        }
        int miss =  sendto(sck,misspkt.c_str(),misspkt.size()+1,0,(sockaddr*)&hintM,sizeof(hintM));
        mtx.unlock();
        usleep(RTT/5);
    }
}

void UnloadThread(){
    while(notfinished){
        mtx.lock();

        // check if it is empty
        if(tkmap.empty()){
            minkey = 100000000;
            mtx.unlock();
            continue;
        }
        else{   
            // if minkey has been added, update minkey
            if(currenttoken > minkey || tokenlist[minkey] == true){
                minkey = 100000000;
                for(auto i : tkmap){
                    if(i.first < minkey){
                        minkey = i.first;
                    }
                }
                mtx.unlock();
                continue;
            }
            
            // if minkey is larger than currenttoken, continue
            if(currenttoken < minkey){
                mtx.unlock();
                usleep(RTT/10);
            } 

            // if minkey is equal to the currenttoken, add it to the file
            // update currenttoken, tokenlist, delete the pairs
            if(currenttoken == minkey){
                if(out.is_open()){
                    out << tkmap[minkey];
                    out.flush();
                }else{
                    cout << "Serious Error! Not opening" <<endl;
                }
                tokenlist[minkey] = true;
                currenttoken ++;
                tkmap.erase(minkey);
                for(auto i : tkmap){
                    if(i.first < minkey){
                        minkey = i.first;
                    }
                }
                mtx.unlock();
                continue;
            }
        }
    } 
}

void LoadThread(int token,string s){
    mtx.lock();
    // if the token has been added into it, drop it
    // else add it into the thread, update the minkey
    if(currenttoken > token || tokenlist[token] == true){
        mtx.unlock();
    } else{
        pair<int,string> p(token,s);
        tkmap.insert(p);
        if(minkey>token){
            minkey = token;
        }
        mtx.unlock();
    }
}




int main(int argc, char *argv[]){
    //set RTT
    char* rttc = argv[1];
    string rtts = string(rttc);
    RTT = stoi(rtts);

    // create client socket
    int port = 31533;
    int sck = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    hint.sin_addr.s_addr = INADDR_ANY;
    int test1 = ::bind(sck,(sockaddr*)&hint,sizeof(hint));
    if (test1 == -1){
        cout << "test1" << "\n";
    }
    cout << "Client is up and running using UDP on port <" << port << ">" << endl;

    // server address
    struct sockaddr_in hintM;
    hintM.sin_family = AF_INET;
    hintM.sin_port = htons(30533);
    inet_pton(AF_INET,"127.0.0.1",&hintM.sin_addr);

    //buffer
    char buf[66000]; 
    memset(buf, 0, 66000);

    //received sock address
    struct sockaddr_in client;
    socklen_t len = sizeof(client);

    //open ofstream out
    out.open("b.txt", ios_base::app);

    // ask the server the file I need
    // receive the number of token
    string ask1 = "get";
    string ask2 = "oksend";
    int ask =  sendto(sck,ask1.c_str(),ask1.size()+1,0,(sockaddr*)&hintM,sizeof(hintM));
    int askreceived = recvfrom(sck,buf,1024,0,(struct sockaddr*) &client,&len);
    if(askreceived>0){
        string nt(buf);
        numoftoken = stoi(nt);
        int ask =  sendto(sck,ask2.c_str(),ask2.size()+1,0,(sockaddr*)&hintM,sizeof(hintM));
    }else{
        ask =  sendto(sck,ask1.c_str(),ask1.size()+1,0,(sockaddr*)&hintM,sizeof(hintM));
    }

    // create a checklist for all tokens
    bool tl[numoftoken];
    for(int i = 0;i<numoftoken;i++){
        tl[numoftoken] = false;
    }

    // create a count thread
    thread count_thread(CountThread,sck,hintM);

    // create a unload-thread
    thread unload_thread(UnloadThread);


    while(true){
    
        // buf problem ????
        memset(buf, 0, 66000);

        //????????? how many to recvfrom
        int bin = recvfrom(sck,buf,1550,0,(struct sockaddr*) &client,&len);
        if(bin<0){
            cout << "Received error" << endl;
        }
        string s(buf);
        
        // token number ???
        int token =0;
        for(int j =0;j<6;j++){
            token = token * 10 + (buf[j]-'0');
        }

        //create a writing thread
        thread load_thread(LoadThread,token,s);
      
    
        
    }
}