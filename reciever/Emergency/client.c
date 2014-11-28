/*
 * Author: Mitul Panchal
 *
 * Description: This is the client program to send a message to specific server
 *              using TCP and UDP connection for CSC573 HW3 question 4.
 *
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE  1024

char message[BUFFER_SIZE]; // Server is not expecting message more than 1000 bytes anyway
char resp_msg[BUFFER_SIZE]; // Why shall we expect response more than 1000 bytes!

int main(int argc, char *argv[])
{
    int udp_socket, tcp_socket, src_port, dst_port, n_sent,n_recv, len;
    struct hostent *host;
    struct sockaddr_in server, local;
    struct tm date_time;
    time_t T;

    /* Argument parsing */
    if (argc < 4)
    {
        fprintf(stderr, "Usage: %s <server> <dstport> <client> <srcport> <proto>\n", argv[0]);
        return 1;
    }
   
    dst_port = atoi(argv[2]);
    src_port = atoi(argv[4]);

    /* Get Time and Date */
    time(&T);        
    date_time = *localtime(&T);  

    /* Construct Message */
    sprintf(message,"Mitul Panchal\n200047044\n%d\nCSC573 F2014\n%02d/%02d/%04d %02d:%02d\n",\
                  src_port, date_time.tm_mon+1, date_time.tm_mday, date_time.tm_year + 1900,\
                    date_time.tm_hour, date_time.tm_min);
    
    /* Populate server client structure */
    server.sin_family = AF_INET;
    server.sin_port = htons(dst_port);
    inet_aton(argv[1], &server.sin_addr);

    local.sin_family = AF_INET;
    local.sin_port = htons(src_port);
    inet_aton(argv[3], &local.sin_addr);

    /* Create udp socket */
    udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
	int opt_val = 150;
	setsockopt(udp_socket, IPPROTO_IP,IP_OPTIONS, &opt_val, sizeof(opt_val));    
    /* Bind socket to assign local port */
    if(bind(udp_socket, (struct sockaddr*)&local, sizeof(struct sockaddr_in))<0)
    {
        printf("UDP Client:bind failed\n");
        return 1;
    }
int i;
 for(i=0;i<100;i++){
    /* Send message to Server */
    if(sendto(udp_socket, message, strlen(message), 0, (struct sockaddr*)&server, sizeof(struct sockaddr_in))<0)
    {
        printf("UDP Client:sendto failed\n");
        return 1;
    }
    printf("UDP Client: Sent message to udp server %s:%d\n", argv[1], ntohs(server.sin_port)); 
}
    len = sizeof(struct sockaddr_in);
   
    /* Recieve ack message from server */
    if(recvfrom(udp_socket, resp_msg, BUFFER_SIZE,0, (struct sockaddr*)&local,&len)<0)
    {
        printf("UDP Client: recvfrom failed\n");
        close(udp_socket);
        return 1;
    }
    printf("UDP Client: Recieved ACK: %s\n",resp_msg);
    /* Close the socket */
    close(udp_socket); 

    /* TCP send */
    
    /* Reset the data structure to reuse */
    memset(&local,0,sizeof(local));
    memset(&resp_msg,0,sizeof(resp_msg));

    local.sin_family = AF_INET;
    local.sin_port = htons(src_port);
    inet_aton(argv[3], &local.sin_addr);

    /* Create socket for TCP */
    tcp_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(tcp_socket == -1)
    {
        printf("TCP Client: Socket create failed\n");
        return 1;
    }

    if(bind(tcp_socket, (struct sockaddr*)&local, sizeof(struct sockaddr_in))<0)
    {
        printf("TCP Client: Bind failed \n");
        close(tcp_socket); 
        return 1;
    }

    /* Connect to the TCP server */
    if(connect(tcp_socket, (struct sockaddr*)&server, sizeof(struct sockaddr_in))<0)
    {
        printf("TCP Client: Connect failed \n");
        close(tcp_socket); 
        return 1;
    }

    /* Send message to server */
    if(write(tcp_socket, message, sizeof(message))<0)
    {
        printf("TCP Client: Send failed\n");
        close(tcp_socket); 
        return 1;
    }
    printf("TCP Client: Sent message to tcp server %s:%d\n", argv[1], ntohs(server.sin_port)); 
    /* Recieve ack from the server */
    if(recvfrom(tcp_socket, resp_msg, BUFFER_SIZE, 0, (struct sockaddr*)&local,&len)<0)
    {
       printf("TCP Client: recv failed\n");
       close(tcp_socket); 
       return 1;
    }
    else
    {
       printf("\nTCP Client: Received Ack:%s\n", resp_msg);
    }

    /* Close the socket fd*/
    close(tcp_socket); 
    return 0;
}
