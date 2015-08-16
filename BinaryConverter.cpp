#include "provided.h"
#include <string>
#include <iostream>
#include <vector>
using namespace std;

const size_t BITS_PER_UNSIGNED_SHORT = 16;

string convertNumberToBitString(unsigned short number);
bool convertBitStringToNumber(string bitString, unsigned short& number);

//encrypts the message using a vector of numbers
string BinaryConverter::encode(const vector<unsigned short>& numbers)
{
    string encodedString = ""; //creates an empty string
    
    //iterates through the vector and calls converNumberToBitString for each element
    for(int i = 0; i < numbers.size(); i++)
    {
        string preFixed = convertNumberToBitString(numbers[i]);
        string fixed = "";
        //appends a tab if the number is found to be 1 and appends a space if the number is found to be a 0 to fixed
        for(int j = 0; j < preFixed.size(); j++)
        {
            if(preFixed[j] == '1')
                fixed += '\t';
            else if(preFixed[j] == '0')
                fixed += ' ';
                
        }
        //appends fixed to the encoded string to be returned
        encodedString += fixed;
    }
    return encodedString;
}

//decrypts the specific string and an empty vector of number
bool BinaryConverter::decode(const string& bitString,
							 vector<unsigned short>& numbers)
{
    numbers.clear(); //clears the vector to make sure it's empty
    
    //checks to see if the string is valid (contains only spaces and tabs)
    for(int i = 0; i < bitString.size(); i++)
    {
        if(bitString[i] != ' ' && bitString[i] != '\t')
        {
            return false;
        }
    }
    
    //checks if the string is of correct length
    if(bitString.size()%16 != 0)
    {
        return false;
    }
    
    //begins the decryption process by iterating through each character in the string
    int beg = 0;
    int end = 16;
    while(end <= bitString.size())
    {
        //proceeds in iterations of 16 to add correctly to the numbers vector
        string nums = "";
        
        //iterates through a specific set of 16 and appends a 0 or 1 accordingly to the nums
        for(int i = beg; i < end; i++)
        {
            if(bitString[i] == ' ')
                nums += '0';
            else
                nums += '1';
        }
        unsigned short x;
        //calls a helper function and pushes back that set of 16 to the numbers vector
        if(convertBitStringToNumber(nums,x))
            numbers.push_back(x);
        //the beginning and end of the loop
        beg = end;
        end += 16;
    }
    return true;
}

string convertNumberToBitString(unsigned short number)
{
	string result(BITS_PER_UNSIGNED_SHORT, '0');
	for (size_t k = BITS_PER_UNSIGNED_SHORT; number != 0; number /= 2)
	{
		k--;
		if (number % 2 == 1)
			result[k] = '1';
	}
	return result;
}

bool convertBitStringToNumber(string bitString, unsigned short& number)
{
	if (bitString.size() != BITS_PER_UNSIGNED_SHORT)
		return false;
	number = 0;
	for (size_t k = 0; k < bitString.size(); k++)
	{
		number *= 2;
		if (bitString[k] == '1')
			number++;
		else if (bitString[k] != '0')
			return false;
	}
	return true;
}
