all: server client

server: 
	g++ -pthread -std=c++11 -o server server.cpp 

client:
	g++ -pthread -std=c++1 client client.cpp

clean:
	rm server client