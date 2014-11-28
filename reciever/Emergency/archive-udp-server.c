#include <stdio.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>


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

   sprintf (arch_filename, "csc573_hw3_udp.txt");
   sprintf (ack, "Server received and archived your transmitted data");
   if (argc > 1) {
      proto_port = atoi (argv[1]);
   }

   sockfd=socket(AF_INET,SOCK_DGRAM,0);

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
      gettimeofday(&tv, NULL);
      nowtm = *(localtime (& (tv.tv_sec)));
      //sprintf (header, "--------------------------\nReceived %d bytes from host IP %s port %d at %s :\n", n, inet_ntoa (cad.sin_addr), ntohs(cad.sin_port), asctime (&nowtm));
      sprintf (footer, "\n--------------------------\n");
      printf ("%s%s%s\n", header, message_to_archive, footer);
      fp = fopen (arch_filename, "a");
      fprintf (fp, "%s%s%s\n", header, message_to_archive, footer);
      fclose (fp);

      //sendto(sockfd,ack,strlen(ack)+1,0,(struct sockaddr *)&cad,sizeof(cad));
   }
}

