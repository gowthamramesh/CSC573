#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>

void * serverthread(void * parm);       /* thread function prototype    */

pthread_mutex_t  mut;

#define PROTOPORT         8222          /* default protocol port number */
#define QLEN              6             /* size of request queue        */


/*************************************************************************
 *  Program:        concurrent server
 *
 *   Purpose:        allocate a socket and then repeatedly execute the folllowing:
 *                             (1) wait for the next connection from a client
 *                                                       (2) create a thread to handle the connection
 *                                                                                 (3) go back to step (1)
 *
 *                                                                                                  The server thread will
 *                                                                                                                            (1) save the received text in its own file
 *                                                                                                                                                      (2) send a short message to the client
 *                                                                                                                                                                                (3) close the connection
 *
 *                                                                                                                                                                                 Syntax:         server [ port ]
 *
 *                                                                                                                                                                                                             port  - protocol port number to use
 *
 *                                                                                                                                                                                                              Note:           The port argument is optional. If no port is specified,
 *                                                                                                                                                                                                                                      the server uses the default given by PROTOPORT.
 *
 *                                                                                                                                                                                                                                      **************************************************************************
 *                                                                                                                                                                                                                                      */

main (int argc, char *argv[])
{
     struct   hostent   *ptrh;     /* pointer to a host table entry */
     struct   protoent  *ptrp;     /* pointer to a protocol table entry */
     struct   sockaddr_in sad;     /* structure to hold server's address */
     struct   sockaddr_in cad;     /* structure to hold client's address */
     int      sd, sd2;             /* socket descriptors */
     int      port;                /* protocol port number */
     int      alen;                /* length of address */
     pthread_t  tid;             /* variable to hold thread ID */

     pthread_mutex_init(&mut, NULL);
     memset((char  *)&sad,0,sizeof(sad)); /* clear sockaddr structure   */
     sad.sin_family = AF_INET;            /* set family to Internet     */
     sad.sin_addr.s_addr = INADDR_ANY;    /* set the local IP address */

     /* Check  command-line argument for protocol port and extract      */
     /* port number if one is specfied.  Otherwise, use the default     */
     /* port value given by constant PROTOPORT                          */

     if (argc > 1) {                        /* if argument specified     */
                     port = atoi (argv[1]); /* convert argument to binary*/
     } else {
                      port = PROTOPORT;     /* use default port number   */
     }
     if (port > 0)                          /* test for illegal value    */
                      sad.sin_port = htons((u_short)port);
     else {                                /* print error message and exit */
                      fprintf (stderr, "bad port number %s/n",argv[1]);
                      exit (1);
     }

     /* Map TCP transport protocol name to protocol number */

     if ( ((int)(ptrp = getprotobyname("tcp"))) == 0)  {
                     fprintf(stderr, "cannot map \"tcp\" to protocol number");
                     exit (1);
     }

     /* Create a socket */
     sd = socket (PF_INET, SOCK_STREAM, ptrp->p_proto);
     if (sd < 0) {
                       fprintf(stderr, "socket creation failed\n");
                       exit(1);
     }

     /* Bind a local address to the socket */
     if (bind(sd, (struct sockaddr *)&sad, sizeof (sad)) < 0) {
                        fprintf(stderr,"bind failed\n");
                        exit(1);
     }

     /* Specify a size of request queue */
     if (listen(sd, QLEN) < 0) {
                        fprintf(stderr,"listen failed\n");
                         exit(1);
     }

     alen = sizeof(cad);

     /* Main server loop - accept and handle requests */
     printf("Server up and running.\n");
     while (1) {

         printf("SERVER: Waiting to accept ...\n");

         if (  (sd2=accept(sd, (struct sockaddr *)&cad, &alen)) < 0) {
                              fprintf(stderr, "accept failed\n");
                              exit (1);
         }
         printf("New client! Spawning thread...\n");
         pthread_create(&tid, NULL, serverthread, (void *) &sd2 );
         printf("Spawned thread %d for socket %d; continuing\n", (int)tid, sd2);
     }
     close(sd);
}


void * serverthread(void * parm)
{
   FILE *fp;
   int tot, n, newsock;
   char message_to_archive[1001]; /* buffer for data to receive and archive */
   char header[500], footer[200]; /* store captured data */
   char ack[100];                 /* buffer for string the server sends */
   char arch_filename[50];
   struct   sockaddr_in cad;      /* structure to hold client's address */
   int ad_len;
   struct timeval tv;
   struct tm nowtm;

   newsock = *(int *)parm;
   printf ("Thread listening to socket %d\n", newsock);

   sprintf (arch_filename, "csc573_hw3_tcp.txt");
   ad_len = sizeof (cad);

   bzero (message_to_archive, 1001);
   tot = 0;
   do {
        tot += (n = recv (newsock, message_to_archive+tot, 1000-tot, 0));
        if (n < 0) {
           fprintf (stderr, "ERROR reading from socket: %d", n);
           printf ("ERROR reading from socket: %d", n);
           close (newsock);
           pthread_exit(0);
        }
   } while (message_to_archive[tot-1] > 0);
           message_to_archive[tot] = 0;
           gettimeofday(&tv, NULL);
           nowtm = *(localtime (& (tv.tv_sec)));

           getpeername (newsock, (struct sockaddr *) &cad, &ad_len);
           pthread_mutex_lock (&mut);
             sprintf (header, "--------------------------\nReceived from host IP %s port %d at %s :\n", inet_ntoa (cad.sin_addr), ntohs(cad.sin_port), asctime (&nowtm));
             sprintf (footer, "\n--------------------------\n");
             printf ("%s%s%s\n", header, message_to_archive, footer);
             fp = fopen (arch_filename, "a");
             fprintf (fp, "%s%s%s\n", header, message_to_archive, footer);
             fclose (fp);
           pthread_mutex_unlock (&mut);
           sprintf (ack, "Server received and archived your transmitted data");
           send (newsock, ack, strlen (ack), 0);

           printf ("Done with socket %d; exiting thread\n", newsock);

   close (newsock);
   pthread_exit(0);
}

