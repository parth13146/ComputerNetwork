#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <map>
#include <iterator>
#include <algorithm>
using namespace std;
#define PORT "33991"  // the port users will be connecting to
#define BACKLOG 10   // how many pending connections queue will hold
#define MAXDATASIZE 100
string returnStateName(map< string, vector<string> >& state_city, string city){
  string result = "NA";
  for (map<string, vector<string> >::const_iterator it = state_city.begin();it != state_city.end(); ++it ) {
   vector <string> inVect = it->second;
   for (unsigned j=0; j<inVect.size(); j++){
       if(city.compare(inVect[j])==0) {
           return it->first;
       }
   }
  }
  return result;
}
void printAllState(map< string, vector<string> >& state_city){
 int count = 0;
  for (map<string, vector<string> >::const_iterator it = state_city.begin();it != state_city.end(); ++it ) {
      if(count!=state_city.size()-1){
          cout << it->first << " ,";
       }else{
           cout << it->first;
       }
       count++;  
  }
  cout<< endl;
}
void sigchld_handler(int s){
  int saved_errno = errno;
  while(waitpid(-1, NULL, WNOHANG) > 0);
  errno = saved_errno;
}
 
void *get_in_addr(struct sockaddr *sa){
  if (sa->sa_family == AF_INET) {
   return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
int main(int argc, const char * argv[]) {
  cout << "Main server is up and running." << endl;
  map< string, vector<string> > state_city;
  map< string, vector<string> >::iterator itr;
  string line;
  string state;
  string cities;
  ifstream myfile ("list.txt");
  if (myfile.is_open()){
   int i = 0;
   
   while (getline (myfile,line)){
       if(i%2==0){
           state = line;
       }else{
           vector<string> cityVector;
           cities = line;
           string s = cities;
           string delimiter = ",";
           size_t pos = 0;
           string token;
           while ((pos = s.find(delimiter)) != string::npos) {
               token = s.substr(0, pos);
               cityVector.push_back(token);
               s.erase(0, pos + delimiter.length());
           }
           cityVector.push_back(s);
           sort( cityVector.begin(), cityVector.end() );
           cityVector.erase( unique( cityVector.begin(), cityVector.end() ), cityVector.end() );
           
           state_city.insert(pair<string, vector<string> >(state,cityVector));
       }
       i++;
   }
   myfile.close();
  }
  cout << "Main server has read the state list from list.txt." << endl;
  for (itr = state_city.begin(); itr != state_city.end(); ++itr) {
   vector <string> inVect = (*itr).second;
   cout << itr->first <<" : "<<endl;
   for (unsigned j=0; j<inVect.size(); j++){
       cout << inVect[j] <<" " ;
   }
   cout << endl;
  }
   
  int numbytes;
  int sockfd, new_fd; 
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_storage their_addr;
  socklen_t sin_size;
  struct sigaction sa;
  int yes=1;
  char s[INET6_ADDRSTRLEN];
  int rv;
  unsigned int myPort;   
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
   return 1;
  }
  if ((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype,servinfo->ai_protocol)) == -1) {
      perror("server: socket");
  }
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) {
      perror("setsockopt");
      exit(1);
  }
  if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
      close(sockfd);
      perror("server: bind");
  }
  freeaddrinfo(servinfo);
  if (servinfo == NULL)  {
      fprintf(stderr, "server: failed to bind\n");
      exit(1);
  }
  if (listen(sockfd, BACKLOG) == -1) {
      perror("listen");
      exit(1);
  }
  sa.sa_handler = sigchld_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
      perror("sigaction");
      exit(1);
  }
  int cID = 0;   
  while(1) {
      sin_size = sizeof their_addr;
      new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
      if (new_fd == -1) {
          perror("accept");
          continue;
       }else{
           cID++;
       }
      
       inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr*)&their_addr),s, sizeof s);
  
   while (!fork()) {
       char buf[MAXDATASIZE];
       if ((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1) {
           perror("recv");
           exit(1);
       }
       char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
       getnameinfo((struct sockaddr *)&their_addr, sin_size,hbuf,sizeof(hbuf),sbuf,sizeof(sbuf),NI_NUMERICHOST | NI_NUMERICSERV);
      
  
       buf[numbytes] = '\0';
       string receivedCity = buf;
      
       cout << "Main Server has received the request on city "<<buf <<" from client"<< cID<<" using TCP over port "<<sbuf<< endl;
       string returnState = returnStateName(state_city,receivedCity);
       if(returnState.compare("NA")==0){
           cout <<receivedCity<< " does not show up in states ";
           printAllState(state_city);
           cout << "The Main Server has sent "<<buf <<" : Not found "<< " to client"<< cID<<" using TCP over port "<<sbuf<< endl;   
       }else{
           cout <<buf <<" is associated with the state "<< returnState << endl;
       }
       char message[returnState.length()];
       int i;
       for(i=0;i<sizeof(returnState);i++){
           message[i] = returnState[i];
       }
       if (send(new_fd, message, strlen(message), 0) == -1)
           perror("send");
       cout << "Main Server has sent searching result to client"<< cID<<" using TCP over port "<<PORT<< endl;
       }
   
  }
 
  return 0;
}
 
 
 


