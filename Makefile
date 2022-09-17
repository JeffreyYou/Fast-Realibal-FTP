all:	Server

Server: 
		g++ -pthread ftp_Server.cpp -o ftp_Server