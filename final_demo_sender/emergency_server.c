#include <stdio.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>

int prevSeq[3][15000];
int packetCounter =0;
int main(int argc, char**argv)
{
   int sockfd,n;
   struct sockaddr_in servaddr,cad;
   socklen_t len;
   char message_to_archive[1001];
   char header[500], footer[200];
   char ack[100];
   char arch_filename[50];
   FILE *fp;
   struct timeval tv;
   struct tm nowtm;
   short proto_port = 8222;

   /* Sequence identification parameters */
   char *host1="192.168.1.10";
   char *host2="192.168.3.10";
   char *host3="192.168.2.10";  
void updateSeq(char *sourceIP,char *message,int portNo)
{
   
    packetCounter++;
    int seqNo = 0;
    char *seq_no= (char *)malloc(8);
    strncpy(seq_no, message+40,7); 
    seq_no[7]='\0';	
    seqNo = atoi(seq_no);
    if(seqNo ==0)
    {
	return;
    }
    if(packetCounter%50 == 0)
    {
         printf("packets receiving. Curr packet- %d\n",packetCounter);
    }
    int index = 0;
    if(strcmp(sourceIP,host1)==0)
	index = 0;
    else if(strcmp(sourceIP,host2)==0)
        index = 1;
    else if(strcmp(sourceIP,host3)==0)
        index = 2;

    if(prevSeq[index][portNo] == (seqNo-1))
    {
        prevSeq[index][portNo] = seqNo;
    }
    else
    {
       int missedSeq = seqNo-1;
       int missedPackets = seqNo - prevSeq[index][portNo] - 1;
	if(missedPackets>0)
       printf("IP:%s, PortNo: %d, packets missed- %d, last seq- %d, Current Seq- %d \n",sourceIP,portNo,missedPackets,prevSeq[index][portNo],seqNo);
       prevSeq[index][portNo] = seqNo;
    }
    
}


   sprintf (arch_filename, "csc573_hw3_udp.txt");
   sprintf (ack, "Server received and archived your transmitted data");
   if (argc > 1) {
      proto_port = atoi (argv[1]);
   }

   sockfd=socket(AF_INET,SOCK_DGRAM,0);
   if(sockfd<0)
   {
      printf("sockfd faield\n");
      return;
   }
   bzero(&servaddr,sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
   servaddr.sin_port=htons(proto_port);
   bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

   while (1)
   {
      len = sizeof(cad);
      bzero (message_to_archive, 1001);
      //printf ("Waiting for client...\n");
      n = recvfrom (sockfd,message_to_archive,1000,0,(struct sockaddr *)&cad,&len);
      if (n < 0) {
           fprintf (stderr, "ERROR reading from socket: %d", n);
           printf ("ERROR reading from socket: %d", n);
           continue;
      }
      //printf("pkt recvd\n");
      //gettimeofday(&tv, NULL);
      //nowtm = *(localtime (& (tv.tv_sec)));
      //sprintf (header, "--------------------------\nReceived %d bytes from host IP %s port %d at %s :\n", n, inet_ntoa (cad.sin_addr), ntohs(cad.sin_port), asctime (&nowtm));
      //sprintf (footer, "\n--------------------------\n");
 
      updateSeq(inet_ntoa(cad.sin_addr),message_to_archive,ntohs(cad.sin_port));  

 
      //printf ("%s%s%s\n", header, message_to_archive, footer);
      // fp = fopen (arch_filename, "a");
      //fprintf (fp, "%s%s%s\n", header, message_to_archive, footer);
      //fclose (fp);

      //sendto(sockfd,ack,strlen(ack)+1,0,(struct sockaddr *)&cad,sizeof(cad));
   }
}
