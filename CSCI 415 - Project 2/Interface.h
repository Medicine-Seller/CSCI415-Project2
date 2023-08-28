#pragma once
#include <fstream>
#include <time.h>
#include "MD5.h"

using namespace std;

class Interface
{
private:

	enum COMMANDS
	{
		HELP,
		EXIT,
		CREATE,
		LOGIN,
		ATTACK,
		TEST,
		CLEAR,
		DELETE,
		USER_INFO,
		SALT_SIZE,
	};

	struct UserData 
	{
		int userId;
		string salt;
		string hash;
	};

	struct Commands
	{
		COMMANDS id;
		string cmd;
		string description;
		string example;
	};

	// List of commands & descriptions
	static const vector<Commands> commands;

	// User database
	static vector<UserData> userData;

	// Length for generating password combinations
	unsigned short combinationLength = 0;

	// MD5 pointer
	MD5* pMd5 = nullptr;

	// Display all commands
	void DisplayHelp();

	// Create an account
	void CreateAccount();

	// Log into an account
	void Login();

	// Perform rainbow attack
	void RainbowAttack();

	// MD5Test the MD5 algorithm
	void MD5Test();

	// Delete an account
	void Delete();

	// Check if user exist
	bool UserExist(const int userId);
	
	// Get user by ID
	const UserData GetUser(const int userId);

	// Get user pointer by ID
	UserData* GetUserPtr(const int userId);

	// Generate hashed password combinations, ALPHANUMERIC only
	void GenerateCombinations(string str, const unsigned short length, ofstream& outputFile);
	
	// Display user information
	void DisplayUserInfo();

	// Set salt size
	void SetSaltSize();

public:

	Interface() {};

	// Handle player commands
	void Start();

	// Save user data to file
	void SaveUserData(const string& fileName);

	// Load user data from file
	void LoadUserData(const string& fileName);

	// Generate rainbow table, ALPHANUMERIC only
	void GenerateRainbowTableHashes(const string& outputFile);

	// Set length for password combinations
	void SetCombinationLength(const unsigned short length);

	// Initialize MD5
	void SetMD5(MD5* ptr);

	// Set salt size
	void SetSaltSize(const int size);
};