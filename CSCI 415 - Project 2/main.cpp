#include "Interface.h"
#include <chrono>

int main()
{
	srand(time(NULL));

	MD5 md5;
	// 64 * 15 = 1000 iterations
	// Set 1 for default MD5 algorithm
	md5.SetIteration(15);	

	Interface interface;
	interface.SetMD5(&md5);

	/*
	// Should NOT be used unless you need to recreate the rainbow table

	// Set the max length for generating password combinations
	// WARNING: Takes a very long time to generate, especially at value larger than 4
	// For ex. Max length 5 took 116 minutes to generate + 31 GB of space
	interface.SetCombinationLength(4);

	// Generate rainbow table hashes
	interface.GenerateRainbowTableHashes("rainbowtable.txt");
	*/

	// Set salt size, adjustable in menu
	interface.SetSaltSize(1);

	// Load user data
	interface.LoadUserData("userdata.txt");

	// Initialize menu
	interface.Start();

	// Save user data
	interface.SaveUserData("userdata.txt");
}