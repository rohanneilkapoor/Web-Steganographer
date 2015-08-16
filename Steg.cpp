#include "provided.h"
#include <string>
#include <iostream>
using namespace std;


//function that takes in the text of a url and encodes the message in the page
bool Steg::hide(const string& hostIn, const string& msg, string& hostOut) 
{
    hostOut = "";
	if(hostIn.size() == 0)
        return false;
    
    //BREAKS UP HOST IN INTO SEPARATE LINES by adding each broken up section into a vector
    vector<string> substringTracker;
    int beg = 0;
    int end = 0;
    
    //iterates through the string
    for(int i = 0; i < hostIn.size(); i++)
    {
        //if it sees a \n or \r\n indicating a new line, it updates the beginning and end values according to that substring
        if(hostIn[i] == '\n')
        {
            if(i > 0 && hostIn[i-1] == '\r')
            {
                end = i-1;
            }
            else
                end = i;
            //pushes the substring into the vector
            string vectorEle = hostIn.substr(beg,end-beg);
            substringTracker.push_back(vectorEle);
            //if there are multple \ns or \r\ns next to eachother after the push, it updates i accordingly
            while(i < hostIn.size()-2 && ((hostIn[i+1] == '\r' && hostIn[i+2] == '\n') || (hostIn[i+1] == '\n')))
            {
                //pushes an empy string to the back of the vector for every \n or \r\n
                substringTracker.push_back("");
                if(hostIn[i+1] == 'r')
                    i += 2;
                else
                    i++;
            }
            beg = i + 1;
        }
        //if it's at the end of the string, it pushes the remaining letters to the back of the vector
        else if(i == hostIn.size()-1)
        {
            string vectorEle = hostIn.substr(beg,i+1-beg);
            substringTracker.push_back(vectorEle);
        }
    }
    //GETS RID OF SPACES AND TABS AT THE END OF EACH VECTOR ELEMENT
    for(int i = 0; i < substringTracker.size(); i++)
    {
        string fixedString = "";
        int lastLetter = -1;
        //iterates through a specific element and stores the last non space or tab variable
        for(int j = 0; j < substringTracker[i].size(); j++)
        {
            if(substringTracker[i][j] != ' ' && substringTracker[i][j] != '\t')
                lastLetter = j;
        }
        //gets the substring from the beginning to the last non space or tabl variable and swaps that string with the current string in the vector
        fixedString = substringTracker[i].substr(0,lastLetter+1);
        substringTracker[i].swap(fixedString);
    }
    
    //COMPRESSION by using the msg value passed in and storing the result into the numbers vector
    vector<unsigned short> numbers;
    Compressor::compress(msg, numbers);
    string encoded = BinaryConverter::encode(numbers); // encode that vector
    unsigned long L = encoded.size();
    unsigned long N = substringTracker.size();
    vector<string> substrings; //create a new vector for the substrings
    unsigned long firstSubstrs = L%N;
    unsigned long count = 0;
    //iterates through the substring tracker and pushes each encoded substring into the substrings vector
    for(int i = 0; i < substringTracker.size(); i++)
    {
        unsigned long x;
        
        if(i < firstSubstrs)
            x = (L/N) + 1;
        else
            x = (L/N);
        substrings.push_back(encoded.substr(count,x));
        count += x;
    }
    //append substringTracker's ith element witht the substring element
    for(int i = 0; i < substringTracker.size(); i++)
    {
        substringTracker[i] += substrings[i];
    }
    //append the hostOut element with the specific substringTracker element + a new line characer
    for(int i = 0; i < substringTracker.size(); i++)
    {
        hostOut += substringTracker[i] + "\n";
    }
    return true;
}

//funcition that reveals a hidden message given the string of the webpage
bool Steg::reveal(const string& hostIn, string& msg)
{
    //BREAKS UP HOST IN INTO SEPARATE LINES
    //same code as used at the top of hide (see lines 14-51)
    vector<string> substringTracker;
    int beg = 0;
    int end = 0;
    for(int i = 0; i < hostIn.size(); i++)
    {
        if(hostIn[i] == '\n')
        {
            if(i > 0 && hostIn[i-1] == '\r')
            {
                end = i-1;
            }
            else
                end = i;
            string vectorEle = hostIn.substr(beg,end-beg);
            substringTracker.push_back(vectorEle);
            while(i < hostIn.size()-2 && ((hostIn[i+1] == '\r' && hostIn[i+2] == '\n') || (hostIn[i+1] == '\n')))
            {
                substringTracker.push_back("");
                if(hostIn[i+1] == 'r')
                    i += 2;
                else
                    i++;
            }
            beg = i + 1;
        }
        else if(i == hostIn.size()-1)
        {
            string vectorEle = hostIn.substr(beg,i+1-beg);
            substringTracker.push_back(vectorEle);
        }
    }
    //GATHER TRAILING TABS AND SPACES
    string decodeThis = "";
    for(int i = 0; i < substringTracker.size(); i++)
    {
        int lastLetter = -1;
        //iterates through a specific element and stores the last non space or tab variable
        for(int j = 0; j < substringTracker[i].size(); j++)
        {
            if(substringTracker[i][j] != ' ' && substringTracker[i][j] != '\t')
                lastLetter = j;
        }
        decodeThis += substringTracker[i].substr(lastLetter+1,substringTracker[i].size());
    }
    vector<unsigned short> numbers;
    //call decode and then decompress on this new string and an empty vector using msg... return accordingly
    if(BinaryConverter::decode(decodeThis,numbers))
    {
        if(Compressor::decompress(numbers,msg))
        {
            return true;
        }
    }
	return false;  
}
