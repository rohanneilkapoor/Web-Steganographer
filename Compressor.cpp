#include "provided.h"
#include <string>
#include <vector>
#include "HashTable.h"
//#include "substituteHashTable.h"
#include <iostream>
using namespace std;

//compute hash function for the string
unsigned int computeHash(const string s)
{
    unsigned hash = 0;
    for(int i = 0; i < s.size(); i++)
    {
        hash = hash * 83  +  s[i]; //uses a prime number to prevent excessive collisions
    }
    return hash;
}
//comput hash function for the key
unsigned int computeHash(unsigned short key)
{
    return key%10;
}

// compresses a string to produce the compressed result as a series of unsigned short integers. 
void Compressor::compress(const string& s, vector<unsigned short>& numbers)
{
    cout << "compress" << endl;
    //creates a hash table that maps strings to unsigned shorts... the capacity is the smaller of N/2 + 512 (using integer division) and 16384... the load factor is .5
    unsigned int C;
    unsigned int temp1 = s.size()/2 + 512;
    unsigned int temp2 = 16384;
    if(temp1 < temp2)
        C = temp1;
    else
        C = temp2;
    unsigned int B = C*2;
    HashTable <string, unsigned short> H(B,C);
    
    //loop to add 256 associations to the hash table for each ascii character.
    for(int j = 0; j < 256; j++)
    {
        string key(1,static_cast<char>(j));
        H.set(key,j,true);
    }
    int nextFreeID = 256;
    string runSoFar = "";
    vector<unsigned short> v;
    
    
    for(unsigned short i = 0; i < s.size(); i++)
    {
        //if the expandedRun string is already contained in H, then (this longer string was already added to the hash table, so let’s keep expanding our run) set runSoFar equal to expandedRun, increasing the length of the current run by one character and continue
        unsigned short j = 0;
        string expandedRun = runSoFar + s[i];
        unsigned short x;
        unsigned short cv;
        if(H.get(expandedRun,j))
        {
            runSoFar = expandedRun;
            continue;
        }
        
        //Otherwise our new string expandedRun was not found in the hash table, so look up runSoFar in hash table H4 and determine what unsigned short it maps to.
        
        H.get(runSoFar,x);
        v.push_back(x);
        H.touch(runSoFar);
//        Reset runSoFar to the empty string.
//        Lookup c in hash table H and determine what unsigned short it maps to.
//        Call this mapped-to value cv.
//        Append cv to the vector V of unsigned shorts.
//        Now it’s time to add our new expandedRun sequence to our hash table. If
//        the table is not yet full to capacity
        runSoFar = "";
        string c(1,static_cast<char>(s[i]));
        H.get(c,cv);
        v.push_back(cv);
//        If the hash table isn't full, associate expandedRun in hash table H with the current value of
//        nextFreeID.
//        ii. Increment nextFreeID by one.
        if(!H.isFull())
        {
            H.set(expandedRun, nextFreeID);
            nextFreeID++;
        }
//        Otherwise, the table is at capacity, and we need to discard the least- recently-written item in the table and then add our new item to the hash table, reusing the existing ID from the discarded item:
//        i. Use the hash table’s discard() method to discard the least recently written non-permanent item in the table. Make sure to get the unsigned short value that the just-discarded string was mapped to.
//        ii. Add a new association to the hash table, mapping expandedRun to the just-discarded association’s unsigned short value.
        else
        {
            string k;
            unsigned short v;
            H.discard(k,v);
            H.set(expandedRun,v);
        }
    }
//    After you have completed step 6 for all the characters in the input string, if the runSoFar variable is not empty, then:
//         Look up runSoFar in hash table H and determine what unsigned short it maps to. Call this mapped-to value x.
//         Append x to the vector V of unsigned shorts.
//         Finally, append the hash table’s capacity C to vector V as the last item. This
//        enables the decompressor to know how big to make its hash table when decompressing the message, since it’s critical that its table have the same capacity as that of the compressor.
    if(runSoFar.size() > 0)
    {
        unsigned short x;
        H.get(runSoFar,x);
        v.push_back(x);
    }
    v.push_back(C);
    numbers = v;
    
}




//Decompress a previously-created vector of unsigned shorts to produce the string that was compressed.
bool Compressor::decompress(const vector<unsigned short>& numbers, string& s)
{
//    Create a hash table H that maps strings to unsigned shorts. The hash table must have a capacity of C items, where C is the smaller of N/2 + 512 (using integer division) and 16384.
//    The maximum load factor of your hash table must be 0.5. You must determine the number of buckets in the hash table based on the capacity C and this required maximum load factor.
    unsigned int C = numbers[numbers.size()-1];
    unsigned int B = C*2;
    HashTable <unsigned short, string> H(B,C);
    //Next, use a loop to add 256 associations to your hash table. For the jth association, 0 <= j <= 255, associate a C++ string that contains the single character static_cast<char>(j), with an unsigned short with the same value j.
    for(int j = 0; j < 256; j++)
    {
        string key(1,static_cast<char>(j));
        H.set(j,key,true);
    }
    int nextFreeID = 256;
    string runSoFar = "";
    string output = "";
    //For each character c in the input string that you’re compressing
    for(int i = 0; i < numbers.size()-1; i++)
    {
//        Create a string named expandedRun and set it equal to the runSoFar string plus the new character c appended on the end.
        unsigned int us = numbers[i];
//        If the expandedRun string is already contained in H, then (this longer string was already added to the hash table, so let’s keep expanding our run)
//        Set runSoFar equal to expandedRun, increasing the length of the current run by one character and continue
        if(us <= 255)
        {
//            Append x to the vector V of unsigned shorts.
//            e. Use the hash table’s touch() method to record the runSoFar entry in the
//            hash table as if it were the most recently written association (if it wasn’t a
//                                                                            permanent association).
//                f. Reset runSoFar to the empty string.
//                g. Lookup c in hash table H and determine what unsigned short it maps to.
//                Call this mapped-to value cv.
//                h. Append cv to the vector V of unsigned shorts.
//                i. Now it’s time to add our new expandedRun sequence to our hash table. If
//                the table is not yet full to capacity, then:
//                i. Associate expandedRun in hash table H with the current value of
//                nextFreeID.
//                ii. Increment nextFreeID by one.
            string k;
            H.get(us,k);
            output += k;
            if(runSoFar.size() == 0)
            {
                runSoFar += k;
                continue;
            }

            string expandedRun = runSoFar + k;
            if(!H.isFull())
            {
                H.set(nextFreeID,expandedRun);
                nextFreeID++;
            }
//            Otherwise, the table is at capacity, and we need to discard the least- recently-written item in the table and then add our new item to the hash table, reusing the existing ID from the discarded item:
//            i. Use the hash table’s discard() method to discard the least recently written non-permanent item in the table. Make sure to get the unsigned short value that the just-discarded string was mapped to.
//            ii. Add a new association to the hash table, mapping expandedRun to the just-discarded association’s unsigned short value.
            else
            {
                cout << "dis" << endl;
                string ke;
                unsigned short va;
                H.discard(va,ke);
                H.set(va,expandedRun);
            }
            runSoFar = "";
            continue;
        }
//        If the runSoFar variable is not empty, then:
//            a. Look up runSoFar in hash table H and determine what unsigned short it maps to. Call this mapped-to value x.
//            b. Append x to the vector V of unsigned shorts.
//            8. Finally, append the hash table’s capacity C to vector V as the last item. This
//            enables the decompressor to know how big to make its hash table when decompressing the message, since it’s critical that its table have the same capacity as that of the compressor.
        string k;
        if(H.get(us,k) == false)
        {
            cout << "Error";
            return false;
        }
        H.touch(us);
        output += k;
        runSoFar = k;
    }
    s = output;
    return true;
}


