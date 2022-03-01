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

bool checkAlpha(string &str)
{
    for (int i=0;i<str.size();i++) {
        if (isalpha(str[i]) == 0) return false;
    }
    return true;
}

void printUserID(map< string, vector<vector<string> > >& state_city){
//cout << "Print Method is called......."<<endl;
  for (map<string, vector<vector<string> > >::const_iterator it = state_city.begin();it != state_city.end(); ++it ) {
   vector<vector <string> > inVect = it->second;
   for(int i = 0; i < inVect.size(); ++i)
    {
        cout << it->first << endl;
        for(int j = 0; j < inVect[i].size(); ++j)
            cout << " " << inVect[i][j] << " " ;
        cout << endl;
    }
  }
}

vector<string> usersForParticularState(map< string, vector<vector<string> > >& state_city,string state, string userId){
//cout << "usersForParticularState Method is called......."<<endl;
  vector<string> result;
  for (map<string, vector<vector<string> > >::const_iterator it = state_city.begin();it != state_city.end(); ++it ) {
   vector<vector <string> > inVect = it->second;
   for(int i = 0; i < inVect.size(); ++i)
    {
        string mapKey = it->first;
        if(state.compare(mapKey)==0){
            for(int j = 0; j < inVect[i].size(); ++j){
                if(count(inVect[i].begin(), inVect[i].end(), userId)){
                    //cout << " " << inVect[i][j] << " " ;
                    if(userId.compare(inVect[i][j])!=0){
                        result.push_back(inVect[i][j]);
                    }
                }
            }
        }
    }
  }
  sort( result.begin(), result.end() );
  result.erase( unique( result.begin(), result.end() ), result.end() );
  return result;
}

string convertVectorToString(map< string, vector<vector<string> > >& state_city,string state, string userId){
    string result = "";
    vector<string> abcd = usersForParticularState(state_city,state,userId);
    if(abcd.size()>=1){
        for(int i = 0; i < abcd.size(); ++i){
            if(i==0){
                result = result + abcd[i];
            }else{
                result = result+","+ abcd[i];
            }
        }
    }else{
        result = "NA";
    }
    return result;
}

string IntToString(int a)
{
    ostringstream temp;
    temp << a;
    return temp.str();
}

void printAllState(map< string, vector<vector<string> > >& state_city){
 int count = 0;
  for (map<string, vector<vector<string> > >::const_iterator it = state_city.begin();it != state_city.end(); ++it ) {
      if(count!=state_city.size()-1){
          cout << it->first << " ,";
       }else{
           cout << it->first;
       }
       count++;  
  }
  cout<< endl;
}

string sendAllState(map< string, vector<vector<string> > >& state_city){
 int count = 0;
 stringstream ss;
 string result="";
  for (map<string, vector<vector<string> > >::const_iterator it = state_city.begin();it != state_city.end(); ++it ) {
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

void *get_in_addr(struct sockaddr *sa){
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int main(void){
    cout << "Server B is up and running using UDP on port "<<serverBPort<< endl;

    map< string, vector<vector<string> > > state_city;
    map< string, vector<vector<string> > >::iterator itr;
    vector<vector<string> > listVector;
    string line;
    string state;
    ifstream myfile ("dataB.txt");
    if (myfile.is_open()){
    int i = 0;
    int flag = 0;
    while (getline (myfile,line)){
        //cout << line << endl;

        if(checkAlpha(line)){
            if(i!=0){
                state_city.insert(pair<string, vector<vector<string> > >(state,listVector));
                listVector.clear();
            }
            state = line;
        }else{
            vector<string> cityVector;
            string s = line;
            string delimiter = ",";
            size_t pos = 0;
            string token;
            while ((pos = s.find(delimiter)) != string::npos) {
                token = s.substr(0, pos);
                cityVector.push_back(token);
                s.erase(0, pos + delimiter.length());
            }
            cityVector.push_back(s);
            listVector.push_back(cityVector);
        }
         i++;     
    }
    state_city.insert(pair<string, vector<vector<string> > >(state,listVector));
    //printUserID(state_city);

    //vector<string> abcd = usersForParticularState(state_city,"Ohio","481822692");
    // for(int i = 0; i < abcd.size(); ++i){
    //     cout << " " << abcd[i] << endl;
    // }


    myfile.close();
    }

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
        int flag=0;
        int l=0;
        int temp=0;
        char queriedState[MAXBUFLEN];
        char queriedUserID[MAXBUFLEN];
        memset(queriedState, 0, MAXBUFLEN);
        memset(queriedUserID, 0, MAXBUFLEN);
        for(temp=0;temp<strlen(buf);temp++){
            if(flag==0){
                if(buf[temp] == '*'){
                    flag=1;
                }else{
                queriedState[temp] = buf[temp];
                }
            }else{
                queriedUserID[l] = buf[temp];
                l++;
            }
        }
        cout << "Server B has received a request for finding possible friends of User " <<queriedUserID << " in "<< queriedState<<endl;

        string temp2 = convertVectorToString(state_city,queriedState,queriedUserID);
        if(temp2.compare("NA")==0){
            cout << "User " << queriedUserID <<" does not show up in "<< queriedState << endl;
            memset(message, 0, MAXBUFLEN);
            int z=0;
            for(z=0;z<temp2.length();z++){
                message[z] = temp2[z];
            }
            sendBytes = sendto(sockfd, message, temp2.length(), 0, (struct sockaddr *)&their_addr, addr_len);
            if (sendBytes == -1) {
                perror("listener: sendto");
                exit(1);
            }
            cout << "Server B has sent User " << queriedUserID<<" not found to Main Server" << endl;
        }else{
            cout << "Server B found the following possible friends for User " << queriedUserID <<" in " << queriedState <<" : "<<temp2<<endl;
            memset(message, 0, MAXBUFLEN);
            int z=0;
            for(z=0;z<temp2.length();z++){
                message[z] = temp2[z];
            }
            sendBytes = sendto(sockfd, message, temp2.length(), 0, (struct sockaddr *)&their_addr, addr_len);
            if (sendBytes == -1) {
                perror("listener: sendto");
                exit(1);
            }
            cout << "Server B has sent the result to Main Server" << endl;
        }     
    }
    freeaddrinfo(servinfo);

    close(sockfd);

    return 0;
}