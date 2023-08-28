#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <bitset>
#include <memory.h>

using namespace std;

class MD5
{
private:
	// Fixed variables
	static const unsigned int kA0 = 0x67452301;
	static const unsigned int kB0 = 0xefcdab89;
	static const unsigned int kC0 = 0x98badcfe;
	static const unsigned int kD0 = 0x10325476;

	// Precomputed table
	static const unsigned int K[64];

	// Shifts per round
    static const unsigned int s[64];

	// # of iteration
	int iteration = 1;

	// Salt size, 48-bits
	int saltSize = 6;

	// Left circular bitwise shift
	unsigned int LeftRotate(unsigned int value, const unsigned int shiftAmount);

	// Convert little endian to big endian
	unsigned int LittleEndianToBigEndian(const unsigned int value);

	// Convert ascii string to vector of bytes
	vector<unsigned char> AsciiToBytes(const string& str);

	// Convert hash to string
	string HashToString(const vector<unsigned int>& hash);

	// Convert ascii string to binary string
	string AsciiToBinary(const string& str);

	// Initialize message blocks for hash function
	void InitMessageBlocks(string str, vector<vector<unsigned int>>& messageBlocks);

public:

	MD5() {};
	
	// Set iteration
	void SetIteration(int n);

	// Hash function
	string Hash(string str);

	// Generate a random alphanumeric salt
	string GenerateRandomSalt();

	// Set salt size
	void SetSaltSize(int n);
};