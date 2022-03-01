#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <netdb.h>
#include <string>
#include <cstring>
#include <map>
#include <iterator>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>

#define serverAPort "30991" 
#define serverBPort "31991" 
#define serverMainPort "32991"
#define PORT "33991"  // the port users will be connecting to
#define BACKLOG 10   // how many pending connections queue will hold
#define LOCALHOST "127.0.0.1"
#define MAXDATASIZE 2048

using namespace std;
#define MAXBUFLEN 2048

map<string, int> state_backend_mapping;
map<string, int> state_backend_mappingA;
map<string, int> state_backend_mappingB;

bool stateInServer(string state,map<string, int> state_backend_mapping){
    for (map<string, int >::const_iterator it = state_backend_mapping.begin();it != state_backend_mapping.end(); ++it ) {
      string mapKey = it->first;
      if(state.compare(mapKey)==0){
          return true;
      }
  }
  return false;
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

string stateMappingToServer(string state){
    if(stateInServer(state,state_backend_mappingA)){
        return "serverA";
    }else if(stateInServer(state,state_backend_mappingB)){
        return "serverB";
    }else{
        return "NA";
    }
}

void storeCharMessageToMap(char buf[],int value){

    vector<string> cityVector;
    string s = buf;
    string delimiter = ",";
    size_t pos = 0;
    string token;
    while ((pos = s.find(delimiter)) != string::npos) {
        token = s.substr(0, pos);
        if(value==0){
            state_backend_mappingA.insert(pair<string,int>(token,value));
        }else{
            state_backend_mappingB.insert(pair<string,int>(token,value));
        }
        state_backend_mapping.insert(pair<string,int>(token,value));
        s.erase(0, pos + delimiter.length());
    }
    state_backend_mapping.insert(pair<string,int>(s,value));
    if(value==0){
        state_backend_mappingA.insert(pair<string,int>(s,value));
    }else{
        state_backend_mappingB.insert(pair<string,int>(s,value));
    }
}

void printCharMessageToMap(){
    map<string,int>::iterator it;
    for (it = state_backend_mapping.begin(); it != state_backend_mapping.end(); it++) {
        cout <<it->first<<" "<< it->second << " ";
    }
    cout<<endl;
}
void printCharMessageToMapA(){
    map<string,int>::iterator it;
    cout<<"Server A" << endl;
    for (it = state_backend_mappingA.begin(); it != state_backend_mappingA.end(); it++) {
        cout <<it->first<<endl;
    }
    cout<<endl;
}
void printCharMessageToMapB(){
    map<string,int>::iterator it;
    cout<<"Server B" << endl;
    for (it = state_backend_mappingB.begin(); it != state_backend_mappingB.end(); it++) {
        cout <<it->first<<endl;
    }
    cout<<endl;
}

int main(int argc, char *argv[])
{
    cout << "Main server is up and running."<< endl;
    int sockfd;
    //int sockfdB;
    struct addrinfo hints, *servinfo, *p;
    struct addrinfo hintsB, *servinfoB, *pB;

    socklen_t addr_len=0;
    struct sockaddr_storage their_addr;
    

    memset(&hints, 0, sizeof hints);
    memset(&hintsB, 0, sizeof hintsB);

    hints.ai_family = AF_INET; 
    hints.ai_socktype = SOCK_DGRAM;
    hintsB.ai_family = AF_INET; 
    hintsB.ai_socktype = SOCK_DGRAM;

    int rv = getaddrinfo(LOCALHOST, serverMainPort, &hints, &servinfo);
    if (rv != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if (sockfd == -1) {
        perror("talker: socket");
    }
    int bindVariable = bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);
    if (bindVariable == -1) {
        close(sockfd);
        perror("listener: bind");
    }


    int rvA = getaddrinfo(LOCALHOST, serverAPort, &hints, &servinfo);
    
    string returnState = "CALIFORNIA";
    char message[returnState.length()];
       int i;
       for(i=0;i<returnState.length();i++){
           message[i] = returnState[i];
       }
      
    int numbytes = sendto(sockfd, message, returnState.length(), 0, servinfo->ai_addr, servinfo->ai_addrlen);
    if (numbytes == -1) {
        perror("talker: sendto1");
        exit(1);
    }
    char buf[MAXBUFLEN];
    addr_len = sizeof their_addr; 
    int numbytes2 = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0, (struct sockaddr *)&their_addr, &addr_len);
    if (numbytes2 == -1) {
        perror("talker: recvfrom1");
        exit(1);
    }
    cout << "Main server has received the state list from server A using UDP over port " << serverMainPort << endl;
    storeCharMessageToMap(buf,0);

    int rvB = getaddrinfo(LOCALHOST, serverBPort, &hintsB, &servinfoB);
    memset(buf, 0, MAXBUFLEN);
    int numbytesB = sendto(sockfd, message, returnState.length(), 0, servinfoB->ai_addr, servinfoB->ai_addrlen);
    if (numbytesB == -1) {
        perror("talker: sendto2");
        exit(1);
    }

    addr_len = sizeof their_addr; 
    int numbytes2B = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,(struct sockaddr *)&their_addr, &addr_len);
    if (numbytes2B == -1) {
        perror("talker: recvfrom2");
        exit(1);
    }
    cout << "Main server has received the state list from server B using UDP over port " << serverMainPort << endl;

    storeCharMessageToMap(buf,1);
    //freeaddrinfo(servinfoB);

    printCharMessageToMapA();
    printCharMessageToMapB();

    int numbytes1;
    int sockfd1, new_fd; 
    struct addrinfo hints1, *servinfo1, *p1;
    struct sockaddr_storage their_addr1;
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv1;
    unsigned int myPort;   
    memset(&hints1, 0, sizeof hints1);
    hints1.ai_family = AF_UNSPEC;
    hints1.ai_socktype = SOCK_STREAM;
    hints1.ai_flags = AI_PASSIVE;
    if ((rv1 = getaddrinfo(NULL, PORT, &hints1, &servinfo1)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv1));
    return 1;
    }
    if ((sockfd1 = socket(servinfo1->ai_family, servinfo1->ai_socktype,servinfo1->ai_protocol)) == -1) {
        perror("server: socket");
    }
    if (setsockopt(sockfd1, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }
    if (bind(sockfd1, servinfo1->ai_addr, servinfo1->ai_addrlen) == -1) {
        close(sockfd1);
        perror("server: bind");
    }
    freeaddrinfo(servinfo1);
    if (servinfo1 == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }
    if (listen(sockfd1, BACKLOG) == -1) {
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

    while(1){
      sin_size = sizeof their_addr1;
      new_fd = accept(sockfd1, (struct sockaddr *)&their_addr1, &sin_size);
      if (new_fd == -1) {
          perror("accept");
          continue;
       }else{
           cID++;
       }
      
      inet_ntop(their_addr1.ss_family,get_in_addr((struct sockaddr*)&their_addr1),s, sizeof s);

      while (!fork()) {

       // receiving tcp   
       char buf[MAXDATASIZE];
       if ((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1) {
           perror("recv");
           exit(1);
       }
       char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
       getnameinfo((struct sockaddr *)&their_addr1, sin_size,hbuf,sizeof(hbuf),sbuf,sizeof(sbuf),NI_NUMERICHOST | NI_NUMERICSERV);
    
       buf[numbytes] = '\0';
       string receivedQuery = buf;

       string str1 = "*";
       size_t found = receivedQuery.find(str1);

       string recState = receivedQuery.substr(0, found); 
       string userId = receivedQuery.substr(found+1,receivedQuery.length());

       cout << "Main server has received the request on User "<<userId<<" in "<<recState<<" from client "<<cID<< " using TCP over port "<<sbuf<< endl;

       char message4[100] ;
      string query = recState +"*"+ userId;
      string receivedState = stateMappingToServer(recState);
      if(receivedState.compare("NA")==0){
          cout << recState<< " does not show up in server A&B." <<endl;

       returnState = "Not Found"; 
       char message[MAXBUFLEN];
       memset(message, 0, MAXBUFLEN);
       int i;
       for(i=0;i<returnState.length();i++){
           message[i] = returnState[i];
       }
       if (send(new_fd, message, returnState.length(), 0) == -1)
           perror("send");
          cout << "Main Server has sent "<< recState<<": Not found to client "<<cID<<" using TCP over port "<<sbuf<< endl;
      }else{
          if(receivedState.compare("serverA")==0){
              cout << recState<< " shows up in server A." <<endl;
              int var=0;
              for(var=0;var<query.length();var++){
                message4[var] = query[var];
              }
              
              int numbyte = sendto(sockfd, message4, query.length(), 0, servinfo->ai_addr, servinfo->ai_addrlen);
              if (numbyte == -1) {
                perror("talker: sendto");
                exit(1);
              }
              cout << "Main Server has sent request of User "<< userId <<" to server A using UDP over port "<< serverMainPort << endl;
              memset(buf, 0, MAXBUFLEN);
              int numbytes3 = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0, (struct sockaddr *)&their_addr, &addr_len); 
              if (numbytes3 == -1) {
                perror("talker: recvfrom");
                exit(1);
              }
              string userIds = buf;
              if(userIds.compare("NA")==0){
                cout << "Main server has received User "<< userId<< " : Not found from server A" << endl;
                char message[MAXBUFLEN];
                memset(message, 0, MAXBUFLEN);
                int i;
                for(i=0;i<userIds.length();i++){
                    message[i] = userIds[i];
                }
                if (send(new_fd, message, userIds.length(), 0) == -1)
                    perror("send");
                cout << "Main Server has sent message to client "<<cID<<" using TCP over port "<< PORT<< endl;

              }else{
                cout << "Main server has received searching result of User "<<userId<<" from server A" << endl; 
                char message[MAXBUFLEN];
                memset(message, 0, MAXBUFLEN);
                int i;
                for(i=0;i<userIds.length();i++){
                    message[i] = userIds[i];
                }
                if (send(new_fd, message, userIds.length(), 0) == -1)
                    perror("send");
                cout << "Main Server has sent searching result(s) to client "<<cID<<" using TCP over port "<< PORT<< endl;
              }
          }else{
              cout << recState<< " shows up in server B." <<endl;
              int var=0;
              for(var=0;var<query.length();var++){
                message4[var] = query[var];
              }
              
              int numbytesB = sendto(sockfd, message4, query.length(), 0, servinfoB->ai_addr, servinfoB->ai_addrlen);
              if (numbytesB == -1) {
                perror("talker: sendto237");
                exit(1);
              }
              cout << "Main Server has sent request of User "<< userId <<" to server B using UDP over port "<< serverMainPort << endl;
              memset(buf, 0, MAXBUFLEN);
              int numbytes3B = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0, (struct sockaddr *)&their_addr, &addr_len);   
              if (numbytes3B == -1) {
                perror("talker: recvfrom");
                exit(1);
              }
              string userIds = buf;
              if(userIds.compare("NA")==0){
                cout << "Main server has received User "<< userId<< " : Not found from server B" << endl;
                char message[MAXBUFLEN];
                memset(message, 0, MAXBUFLEN);
                int i;
                for(i=0;i<userIds.length();i++){
                    message[i] = userIds[i];
                }
                if (send(new_fd, message, userIds.length(), 0) == -1)
                    perror("send");
                cout << "Main Server has sent message to client "<<cID<<" using TCP over port "<< PORT<< endl;
              }else{
                cout << "Main server has received searching result of User "<<userId<<" from server B" << endl;
                char message[MAXBUFLEN];
                memset(message, 0, MAXBUFLEN);
                int i;
                for(i=0;i<userIds.length();i++){
                    message[i] = userIds[i];
                }
                if (send(new_fd, message, userIds.length(), 0) == -1)
                    perror("send");
                cout << "Main Server has sent searching result(s) to client "<<cID<<" using TCP over port "<< PORT<< endl; 
              }
          }
      }
      
    }
    }
    close(sockfd);
    return 0;
}
