#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cstring>
using namespace std;

string xorOperation(string a, string b){
    string result = "";
    if(a[0]=='0'){
        return a.substr(1);
    }
    int length = a.length();
    int i = 0;
    while(i<length){
        if(a[i] == b[i])
            result+="0";
        else
            result+="1";
        i++;
    }
    return result.substr(1);
}

string crc(string data, string generator){
    int lengthGenerator = generator.length();

    // Appending zeroes 
    string transmitted = data;
    int i = 0;
    while(i++<lengthGenerator-1){
      transmitted += '0';
        
    }
    string dividend = transmitted.substr(0,lengthGenerator);
    string divisor = generator;

    int j = lengthGenerator;
    while(j<transmitted.length()){
        dividend = xorOperation(dividend,divisor);
        dividend = dividend + transmitted[j];
        j++;
    }
    string result = xorOperation(dividend,divisor);

    return result;
}

int main() {
   string generator = "1100000001111";

   string line;
   ifstream myfile ("dataTx.txt");
    if (myfile.is_open()){
        while ( getline (myfile,line) ){
            string rem = crc(line,generator);
            string codeword = line+rem;
            cout<<"codeword:"<<endl<< codeword <<endl;
            cout<<"crc:"<<endl<<rem<<endl; 
        }
        myfile.close();
    }

   return 0;
}
