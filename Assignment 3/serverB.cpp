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
#include <vector>
#include <iterator>
#include <algorithm>
#include <fstream>
#include <cstdlib>
#include <iostream>
//#include <regex>
#include <sstream>

#define serverBPort "31991"
#define serverMainPort "32991" 
#define LOCALHOST "127.0.0.1"
#define MAXBUFLEN 2048
using namespace std;

string IntToString(int a)
{
    ostringstream temp;
    temp << a;
    return temp.str();
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

string sendAllState(map< string, vector<string> >& state_city){
 int count = 0;
 stringstream ss;
 string result="";
  for (map<string, vector<string> >::const_iterator it = state_city.begin();it != state_city.end(); ++it ) {
      if(count!=state_city.size()-1){
          result += it->first + ",";
          //cout<<it->first;
       }else{
           result += it->first;
           //cout<<it->first;
       }
       count++;  
  }
  //cout<<result;
  return result;
}

int returnCitiesCount(map< string, vector<string> >& state_city, string state){
  int result = 0;
  for (map<string, vector<string> >::const_iterator it = state_city.begin();it != state_city.end(); ++it ) {
      string mapKey = it->first;
      if(state.compare(mapKey)==0){
          vector <string> inVect = it->second;
          result = inVect.size();
      }
  }
  return result;
}
string citiesForState(map< string, vector<string> >& state_city, string state){
  string result = "";
  for (map<string, vector<string> >::const_iterator it = state_city.begin();it != state_city.end(); ++it ) {
      string mapKey = it->first;
      if(state.compare(mapKey)==0){
          int count=0;
          vector <string> inVect = it->second;
          for (unsigned j=0; j<inVect.size(); j++){
            if(count!=inVect.size()-1){
                result += inVect[j] + ",";
            }else{
                result += inVect[j];
            }
            count++;
        }
      }
  }
  return result;
}



void *get_in_addr(struct sockaddr *sa){
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int main(void){
    cout << "Server B is up and running using UDP on port "<<serverBPort<< endl;

    map< string, vector<string> > state_city;
    map< string, vector<string> >::iterator itr;
    string line;
    string state;
    string cities;
    ifstream myfile ("dataB.txt");
    if (myfile.is_open()){
    int i = 0;
   
    while (getline (myfile,line)){
        if(i%2==0){
            state = line.substr(0,line.length()-1);
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
    //printAllState(state_city);
    //string ss = sendAllState(state_city);
    
    //printAllState(state_city);

    //cout<<returnCitiesCount(state_city,"Maine")<<"YAY    "<<endl;

    int sockfd;
    struct addrinfo hints, hintClient, *servinfo, *p;
    struct sockaddr_storage their_addr;
    char buf[MAXBUFLEN];
    socklen_t addr_len;
    char s[INET_ADDRSTRLEN];
    memset(&hints, 0, sizeof hints);
    memset(&hintClient, 0, sizeof hintClient);
    hints.ai_family = AF_INET; 
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;


    // sending DS to MAIN Server 
    
    int rv = getaddrinfo(LOCALHOST, serverBPort, &hints, &servinfo);
    if (rv != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    sockfd = socket(servinfo->ai_family, servinfo->ai_socktype,servinfo->ai_protocol);
    if (sockfd == -1) {
        perror("listener: socket");
    }
    int bindVariable = bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);
    if (bindVariable == -1) {
        close(sockfd);
        perror("listener: bind");
    }
    string ss = sendAllState(state_city);
    char message[MAXBUFLEN];
    
    int i;
    for(i=0;i<ss.length();i++){
        message[i] = ss[i];
    }
    addr_len = sizeof their_addr;
     int numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0, (struct sockaddr *)&their_addr, &addr_len);
     if (numbytes == -1) {
         perror("recvfrom");
         exit(1);
     }

    int sendBytes = sendto(sockfd, message, ss.length(), 0, (struct sockaddr *)&their_addr, addr_len);
    if (sendBytes == -1) {
        perror("talker: sendto");
        exit(1);
    }
    cout << "Server B has sent a state list to Main Server."<<endl;    
    // end of sending DS to MAIN Server 


    
    while(1){
        
        memset(buf, 0, MAXBUFLEN);
        int numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0, (struct sockaddr *)&their_addr, &addr_len);
        if (numbytes == -1) {
            perror("recvfrom");
            exit(1);
        }
        cout << "Server B has received a request for "<< buf<< endl;

        cout << "Server B found "<< returnCitiesCount(state_city,buf) <<" distinct cities for "<< buf << endl;
        cout << citiesForState(state_city,buf) << endl;
        memset(message, 0, MAXBUFLEN);
        //string distinctCity = to_string(returnCitiesCount(state_city,buf)) +"*"+citiesForState(state_city,buf);
        int a = returnCitiesCount(state_city,buf);
        string s = IntToString(a);
        string distinctCity = s +"*"+citiesForState(state_city,buf);
        //cout<< "Parth: " << distinctCity << endl;
        int z=0;
        for(z=0;z<distinctCity.length();z++){
           message[z] = distinctCity[z];
        }
        sendBytes = sendto(sockfd, message, distinctCity.length(), 0, (struct sockaddr *)&their_addr, addr_len);
        if (sendBytes == -1) {
            perror("talker: sendto");
            exit(1);
        }
        cout << "Server B has sent the results to Main Server" << endl;
       

        // printf("listener: got packet from %s\n",
        //     inet_ntop(their_addr.ss_family,
        //         get_in_addr((struct sockaddr *)&their_addr),
        //         s, sizeof s));
        // // printf("listener: packet is %d bytes long\n", numbytes);
        // // buf[numbytes] = '\0';
        // printf("listener: packet contains \"%s\"\n", buf);
    }
    freeaddrinfo(servinfo);

    close(sockfd);

    return 0;
}