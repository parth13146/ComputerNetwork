#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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
#define LOCALHOST "127.0.0.1"

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

    while(1){
      cout << "Enter state name: ";
      char message2[100] ;
      cin.getline(message2,sizeof(message2));
      string recState = message2;
      string receivedState = stateMappingToServer(recState);
      if(receivedState.compare("NA")==0){
          cout << recState<< " does not show up in server A&B." <<endl;
      }else{
          if(receivedState.compare("serverA")==0){
              cout << recState<< " shows up in server A." <<endl;
              
              int numbyte = sendto(sockfd, message2, recState.length(), 0, servinfo->ai_addr, servinfo->ai_addrlen);
              if (numbyte == -1) {
                perror("talker: sendto");
                exit(1);
              }
              cout << "The Main Server has sent request for " <<recState<< " to server A using UDP over port "<< serverMainPort << endl;
              memset(buf, 0, MAXBUFLEN);
              int numbytes3 = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0, (struct sockaddr *)&their_addr, &addr_len); 
              if (numbytes3 == -1) {
                perror("talker: recvfrom");
                exit(1);
              }
              cout << "The Main server has received searching result(s) of " << recState <<" from server A." << endl;
              //cout << "BUF:******************* "<<buf<<endl; 
              int flag=0;
              int l=0;
              int z=0;
              char cities[MAXBUFLEN];
              char distinctCount[3];
              memset(distinctCount, 0, 3);
              memset(cities, 0, MAXBUFLEN);
              for(z=0;z<strlen(buf);z++){
                  if(flag==0){
                      if(buf[z] == '*'){
                          flag=1;
                      }else{
                        distinctCount[z] = buf[z];
                      }
                  }else{
                      cities[l] = buf[z];
                      l++;
                  }
              }
              cout<<"There are " << distinctCount <<" distinct cities in "<< recState<< " : "<<cities<<endl;   
          }else{
              cout << recState<< " shows up in server B." <<endl;
              
              int numbytesB = sendto(sockfd, message2, recState.length(), 0, servinfoB->ai_addr, servinfoB->ai_addrlen);
              if (numbytesB == -1) {
                perror("talker: sendto237");
                exit(1);
              }
              cout << "The Main Server has sent request for " <<recState<< " to server B using UDP over port "<< serverMainPort << endl;
              memset(buf, 0, MAXBUFLEN);
              int numbytes3B = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0, (struct sockaddr *)&their_addr, &addr_len);   
              if (numbytes3B == -1) {
                perror("talker: recvfrom");
                exit(1);
              }
              cout << "The Main server has received searching result(s) of " << recState <<" from server B." << endl;
              //cout << "BUF:******************* "<<buf<<endl; 
              int flag=0;
              int l=0;
              int z=0;
              char cities[MAXBUFLEN];
              char distinctCount[3];
              memset(cities, 0, MAXBUFLEN);
              memset(distinctCount, 0, 3);
              for(z=0;z<strlen(buf);z++){
                  if(flag==0){
                      if(buf[z] == '*'){
                          flag=1;
                      }else{
                        distinctCount[z] = buf[z];
                      }
                  }else{
                      cities[l] = buf[z];
                      l++;
                  }
              }
              cout<<"There are " << distinctCount <<" distinct cities in "<< recState<< " : "<<cities<<endl;   
          }
      }
      cout <<"-----Start a new query-----"<<endl;
    }
    

    printf("talker: packet contains \"%s\"\n", buf);
    
    close(sockfd);

    return 0;
}
