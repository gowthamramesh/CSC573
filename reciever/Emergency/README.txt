****************************************
README File for CSC573 Internet Protocols
Q5 of HW3
****************************************
Name: Mitul Panchal
Student id: 200047044
Date: Sept 08, 2014


Platform: Linux (preferably Ubuntu 14.04 OS)
Language: C

*********************
Directory structure:
*********************

3------Q5-------client.c
   |        |     
   |         ---archive-tcp-server.c
   |        |
   |         ---archive-udp-server.c
   |        |
   |         ---Makefile
   |        |
   |         ---README.txt
   |        
   |
   |---Q8------hello_world.c
   |         |
   |          --Makefile
   |         |
   |          --README.txt
   |
    ---README

******************************************************************************
Programming question 5:
******************************************************************************

The program to send a specified information to a server at specified IP:port.

Language: C
Platform: Ubuntu OS 14.04 (Tested)
          Any Linux distribution with gcc installed and kernel 2.6.32 or greater
Source: 
   client.c (Program to send specified data to UDP and TCP server)
   archive-udp-server.c (Server code for testing udp locally) - Taken from Resources link on course website
   archive-tcp-server.c (Server code for testing tcp locally) - Taken from Resources link on course website

Output:
   client.o ( executable to send data to server)
   archive-tcp-server.o (executable for tcp server)
   archive-udp-server.o (executable for udp server) 

Steps to build:
1. To Build only client program
   - make q5
2. To Build client program as well as server programs provided along with HW3
   (Only for local testing)
   - make
3. To clean 
   - make clean

How to run:
make q5 will generate client.o in same folder.

./client.o <server IP> <server port> <client IP> <client port>

server IP  : 152.46.17.183 (As provided in question 5)
server port: 8222 (As provided in question 5)

client IP : IP address of the machine from where the client is run
client port: client port number (Recommended to select non reserved port usually greater than 1024)

Output (Snapshot):

mpancha@vclv99-129:/afs/unity.ncsu.edu/users/m/mpancha/IP/3$ ./client.o 152.46.17.183 8222 152.7.99.129 12345
UDP Client: Sent message to udp server 152.46.17.183:8222
UDP Client: Recieved ACK: Server received and archived your transmitted data
TCP Client: Sent message to tcp server 152.46.17.183:8222
TCP Client: Received Ack:Server received and archived your transmitted data


