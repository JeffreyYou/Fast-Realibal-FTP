all:	clear Server Client run
clear:
		clear
Server: 
		g++ -pthread ftp_Server.cpp -o server
Client:
		g++ -pthread ftp_Client.cpp -o client

run:	
		./server