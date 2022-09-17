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
using namespace std;

int main(){
    //Booting up (Only while starting):
    int Aportnum = 30533;
    int sck = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(Aportnum);
    hint.sin_addr.s_addr = INADDR_ANY;
    int bindvalue = ::bind(sck,(sockaddr*)&hint,sizeof(hint));
    cout << "Server is up and running using UDP on port <" << Aportnum << ">" << endl;

    struct sockaddr_in hintM;
    hintM.sin_family = AF_INET;
    hintM.sin_port = htons(31533);
    inet_pton(AF_INET,"127.0.0.1",&hintM.sin_addr);
    char buf[1024];
    int sendMain;

    vector<string> sv;
    fstream file;
    string line;
    file.open("a.txt",ios::in);
    if (file.is_open()){
        while (getline(file,line)){  
            line.pop_back();
            sv.push_back(line);
        }
    }

    file.close();
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
  
    memset(buf, 0, 1024);
    for(string result:sv){
        sendMain = sendto(sck,result.c_str(),result.size()+1,0,(sockaddr*)&hintM,sizeof(hintM));
    }
       
    close(sck);
}