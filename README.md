# Fast and Reliable File Transfer Protocol
## Why This

The traditional FTP protocol performs well under most circumstances.

However, when the network environment is poor, its performance deteriorates significantly. Since TCP-based FTP uses an exponential back-off mechanism, it decreases the transmission rate when it assumes the network link is congested,

This is a **UDP-based** file transfer protocol.

It uses multi-thread to reliably transfer files from server to client, with a very good performance under the high-congestion and high-loss conditions.

## Quick Start
### Server Setup
server.cpp:

- Description: The server waiting for the transmission.

- Compile: `g++ -std=c++11 -g -pthread ftp_Server.cpp -o server`

- Run: `./<target_file_name> <file_to_transfer> <host_port>`

- Makefile shortcut:

  `make server`

### Client Setup

client.cpp:

- Description: The client that establish connection with server and sends the file.

- Compile: `g++ -std=c++11 -g -pthread ftp_Client.cpp -o client`

- Run: `./<target_file_name> <path_to_store_file> <hostname> <host_port>`

- Makefile shortcut:

  `make client`

### File Integrity Check
MD5.py:
- Description: Generate the hash code of transferred file.
  
- Run: `python MD5.py`