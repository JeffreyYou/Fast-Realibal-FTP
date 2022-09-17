all: server client

server: 
	g++ -std=c++11 -o server UDPserver.cpp 

client:
	g++ -std=c++11 -o client UDPclient.cpp

clean:
	rm server client