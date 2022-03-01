
README: EE450 Programming Assignment 1
======================================

NAME        : PARTH SHRIVASTAVA
USCUserName : parthshr
USC ID      : 1824596991
EMAIL       : parthshr@usc.edu


About the assignment
--------------------
Assignment is divided in three parts. 
For part 1, data is read from dataTx.txt line by line to perform CRC12. I have used the same CRC12 generator to perform CRC. 

For part 2, data is read from dataRx.txt line by line to perform CRC12. I have used the same CRC12 generator to perform CRC.

For part 3, data is read from dataVs.txt line by line. Each line in the file have two segments, i.e. the data bits and error bits. For the data bits, we first performs CRC12 and append the remainder to the data bits and then xor the resultant bits with the error bits. Since the data bits is of size 64 bit and 12 bits of remainder that is to be appended, we will get 76 bit length. However, the error bits length is 72. So I have just appended 4 extra 0s at the end of error bits.
We further perform CRC12 on the resultant bits to find if there is an error or not.
In case of checksum, we took the first 8 bits and next 8 bits to perform bitwise addition. The resultant is again added to the next 8 bits. This is performed for all set of 8 bits in the data. Once we have the final 8 bits, we just take their compliment and append these 8 bits to the original data and then xor the resultant bits with the error bits.
We further perform checksum on the resultant bits to find if there is an error or not.  

Reading Files : Please use the same directory to insert the input text files.    


Details of the code files
-------------------------
1. crc_tx.cpp
2. crc_rx.cpp
3. crc_vs_checksum.cpp


If you have any questions, please feel free to contact me over email.


