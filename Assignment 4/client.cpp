#include <iostream>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <map>
#include <iterator>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
using namespace std;
#define PORT "33991"
#define MAXDATASIZE 2048
 
void *get_in_addr(struct sockaddr *sa){
  if (sa->sa_family == AF_INET) {
   return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
int main(int argc, const char * argv[]) {
  cout << "Client is up and running." << endl;
  int sockfd, numbytes;

  struct addrinfo hints, *servinfo, *p;
  int rv;
  char s[INET6_ADDRSTRLEN];
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  struct sockaddr_in client;
  socklen_t clientsz = sizeof(client);

  if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
      return 1;
  }
  // loop through all the results and connect to the first we can
  for(p = servinfo; p != NULL; p = p->ai_next) {
   if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) {
       perror("client: socket");
       continue;
   }
   if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
       close(sockfd);
       //perror("client: connect");
       continue;
   }
   break;
  }
  if (p == NULL) {
      fprintf(stderr, "client: failed to connect\n");
      return 2;
  }
  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof s);
  freeaddrinfo(servinfo); // all done with this structure
 
 
  while(1){
      cout << "Enter state name: ";
      char message2[MAXDATASIZE] ;
      cin.getline(message2,sizeof(message2));
      cout << "Enter user ID: ";
      char message3[MAXDATASIZE];
      cin.getline(message3,sizeof(message3));
      string recState = message2;
      string userId = message3;
      string query = recState +"*"+ userId;
      
      char message[MAXDATASIZE] ;
      memset(message, 0, MAXDATASIZE);
      int var=0;
      for(var=0;var<query.length();var++){
        message[var] = query[var];
      }
      if (send(sockfd, message, query.length(), 0) == -1)
           perror("send");
       //char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
       //getnameinfo((struct sockaddr *)p->ai_addr, p->ai_addrlen,hbuf,sizeof(hbuf),sbuf,sizeof(sbuf),NI_NUMERICHOST | NI_NUMERICSERV);
       getsockname(sockfd, (struct sockaddr *) &client, &clientsz);    
       cout << "Client has sent "<<message2<<" and User "<<message3<< " to Main Server using TCP over port "<<ntohs(client.sin_port)<<endl;
       char buf[MAXDATASIZE];
       memset(buf, 0, MAXDATASIZE);
       if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
           perror("recv");
           exit(1);
       }
   buf[numbytes] = '\0';
   
   string resultState = buf;       
   if(resultState.compare("Not Found")==0){
       cout << recState <<": Not found"<<endl;   
   }else if(resultState.compare("NA")==0){
       cout << "User "<<userId<<": Not found"<<endl;
   }else{
       cout << "User " << resultState<<" is/are possible friend(s) of User "<< userId<<" in " <<recState<< endl;
   }
   cout << "-----Start a new request-----" << endl;	
  }
  return 0;
}
 
 
 
 



