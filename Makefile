all:	clear Server Client run
clear:
		clear
Server: 
		g++ -std=c++11 -g -pthread ftp_Server.cpp -o server
Client:
		g++ -std=c++11 -g -pthread ftp_Client.cpp -o client

run:	
		./server
