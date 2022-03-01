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
#define MAXDATASIZE 100
 
void *get_in_addr(struct sockaddr *sa){
  if (sa->sa_family == AF_INET) {
   return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
int main(int argc, const char * argv[]) {
  cout << "Client is up and running." << endl;
//    while(1){
//      cout << "Enter City Name: ";
//      string input;
  
//      // After sending the request to Server.
//      cout << "Client has sent city "<<input<<" to Main Server using TCP."<<endl;
//      //Receiving message from server.
//      string state;
//      cout << "Client has received results from Main Server: "<< endl;
//      cout << input << " is associated with state "<< state<<endl;
//      break;
//    }
  int sockfd, numbytes;
  char buf[MAXDATASIZE];
  struct addrinfo hints, *servinfo, *p;
  int rv;
  char s[INET6_ADDRSTRLEN];
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
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
      cout << "Enter City Name: ";
      char message[100] ;
      cin.getline(message,sizeof(message));
      if (send(sockfd, message, strlen(message), 0) == -1)
           perror("send");
       cout << "Client has sent city "<<message<<" to Main Server using TCP."<<endl;
       char buf[MAXDATASIZE];
       if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
           perror("recv");
           exit(1);
       }
   buf[numbytes] = '\0';
   string resultState = buf;       
   if(resultState.compare("NA")==0){
       cout << message <<" not found."<<endl;   
   }else{
       cout << "Client has received results from Main Server: "<< endl;
       cout << message << " is associated with state "<<buf<<"."<< endl;
   }
   cout << "-----Start a new query-----" << endl;	
  }
  return 0;
}
 
 
 
 



