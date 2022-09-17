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


static int currenttoken;
static ofstream out;
mutex mtx;

void FileHandler(string s,int token,int sck,sockaddr_in hintM){
    cout << "Hello " << token << endl;
    while(1){
        mtx.lock();
        string t = to_string(currenttoken);
        if(token > currenttoken){
            int sendMain = sendto(sck,t.c_str(),t.size()+1,0,(sockaddr*)&hintM,sizeof(hintM));
            mtx.unlock();
            usleep(200);
        } else if (token < currenttoken){   
            //drop 
            mtx.unlock();
            break;
        } else{
            currenttoken++;
            if(out.is_open()){
                out << s <<endl;
                out.flush();
            }else{
                cout << "Not opening" <<endl;
            }
            mtx.unlock();
            break;
        }
    }

   
}

int main(){
    currenttoken = 0;

    //create a socket
    int port = 31533;
    int sck = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    hint.sin_addr.s_addr = INADDR_ANY;

    struct sockaddr_in hintM;
    hintM.sin_family = AF_INET;
    hintM.sin_port = htons(30533);
    inet_pton(AF_INET,"127.0.0.1",&hintM.sin_addr);

    int test1 = ::bind(sck,(sockaddr*)&hint,sizeof(hint));
    if (test1 == -1){
        cout << "test1" << "\n";
    }
    cout << "Client is up and running using UDP on port <" << port << ">" << endl;

    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    out.open("b.txt", ios_base::app);

    char buf[66000]; 

    while(true){
        memset(buf, 0, 66000);
        int bin = recvfrom(sck,buf,1024,0,(struct sockaddr*) &client,&len);
        string s(buf);

        // token number ???
        char t = s[0];
        int token = t - '0'; 
        cout << "Token " << token <<endl;
        thread client_thread(FileHandler,s,token,sck,hintM);

        // thread client_thread(FileHandler,out);
        // client_thread.join();
         
    }

    out.close();
    close(sck);

}

