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

string crcAtRx(string txdata, string generator){
    string result = "pass";
    string rem = crc(txdata,generator);
    for(int i =0;i<rem.length();i++){
        if(rem[i]=='1'){
            result = "not pass";
            break;
        }
    }
    return result;
}



string addition(string first,string second){

    string result="100000000";
    string carry = "0";
        for(int i = 7;i>=0;i--){
            if(carry=="1"){
                if(first[i]=='1' && second[i]=='1'){
                    carry="1";
                    result[i+1]='1';   
                }else if(first[i]=='1' || second[i]=='1'){
                    carry="1";
                    result[i+1]='0';                   
                }else{
                    carry="0";
                    result[i+1]='1';
                }
            }else{
                if(first[i]=='1' && second[i]=='1'){
                    carry="1";
                    result[i+1]='0';
                }else if(first[i]=='1' || second[i]=='1'){
                    carry="0";
                    result[i+1]='1';
                }else{
                    carry="0";
                    result[i+1]='0';
                }
            }
            
        }
        if(carry!="1"){
            result = result.substr(1,8);
        }
        return result;
}

string checkSum8Bits(string first8bits,string second8bits){
    string result="";
    if(!(first8bits.length()!=8 || second8bits.length()!=8)){
        result = addition(first8bits,second8bits); 
    }
    if(result[0]=='1'&&result.length()==9){
        result = result.substr(1,8);
        result = addition(result,"00000001");
    }
    return result;
}

string checkSum(string data){
    string result="";
    int dataLength = data.length();
    int loopLimit = dataLength/8; 
    int count=1;
    string first8bits = data.substr(0,8);
    int j=0; 
    while(count++<loopLimit){
        string second8bits = data.substr(8+j,8);
        first8bits = checkSum8Bits(first8bits,second8bits);
        j=j+8;
    }

    for(int i=0;i<first8bits.length();i++){
        if(first8bits[i]=='0'){
            first8bits[i]= '1';
        }else{
            first8bits[i] = '0';
        }
    }
    return first8bits;
}

// Part 3

string xorCheckSum(string a, string b){
    string result="";
    int length = a.length();
    int i = 0;
    while(i<length){
        if(a[i] == b[i])
            result+="0";
        else
            result+="1";
        i++;
    }
    return result;
}

string checkSumAtRx(string txdata){
    string result = "pass";
    string rem = checkSum(txdata);
    cout<<"checkSumAtRx "<<rem<<endl;
    for(int i =0;i<rem.length();i++){
        if(rem[i]=='1'){
            result = "not pass";
            break;
        }
    }
    return result;
}


string appendTrailingZeros(string input, int length){
    while(length-->0){
        input+="0";
    }
    return input;
}


int main() {
   string generator = "1100000001111";
   string line;
    ifstream myfile ("dataVs.txt");
    if (myfile.is_open())
    {
        while ( getline (myfile,line) )
        {
            int count = 0;
            for(int i=0;i<line.length();i++){
                if(line[i] == ' '){
                    count = i;
                    break;
                }
            }
            string data = line.substr(0,count);
            string errorBits =  line.substr(count+1,line.length()-count+1);

            string rem = crc(data,generator);
            string codeword = data+rem;
            int length = codeword.length() - errorBits.length();
            string errorBitsPadded = appendTrailingZeros(errorBits,length);
            string addedCRCError = xorCheckSum(codeword,errorBitsPadded);
            string resultCRC = crcAtRx(addedCRCError,generator);
            
            string checksum = checkSum(data);
            string checksumWithData = data + checksum;
            string addedError = xorCheckSum(checksumWithData,errorBits);
            string resultCheckSum = checkSumAtRx(addedError);

            cout<<"crc : "<<rem<<"\t result : "<<resultCRC<<endl;
            cout<<"checksum : "<<checksum<<"\t result : "<<resultCheckSum<<endl;
            
        }
        myfile.close();
    }else{
        cout << "Unable to open file";
    }

   return 0;
}
