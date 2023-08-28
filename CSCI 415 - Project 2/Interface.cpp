#include "Interface.h"

#define PRETTY(x) setw(x) << left

vector<Interface::UserData> Interface::userData;

const vector<Interface::Commands> Interface::commands
{
	{HELP,		"?",				"Display a list of commands",		""},
	{EXIT,		"exit",				"Exit the program",					""},
	{CREATE,	"create",			"Create an account",				"create [id] [password]"},
	{LOGIN,		"login",			"Log into an account",				"login [id] [password]"},
	{ATTACK,	"attack",			"Perform rainbow attack on a user id or all user", "attack [id] | attack all"},
	{TEST,		"md5",				"Test the md5 algorithm",			"md5 [password]"},
	{CLEAR,		"clear",			"Clear the screen",					""},
	{DELETE,	"delete",			"Delete a user or all user",		"delete [id] | delete all"},
	{USER_INFO, "userinfo",			"Display a user or all user information", "userinfo [id] | userinfo all"},
	{SALT_SIZE, "saltsize",			"Set salt size",					"saltsize 6"}
};

void Interface::Start()
{
	// If MD5 is not initialized, return
	if (pMd5 == nullptr)
	{
		cout << "MD5 not initialized!" << endl;
		return;
	}

	cout << "Default salt size: 1, change using command: saltsize [size]" << endl;
	cout << "Type '?' for help" << endl;

	// Handle player inputs
	string input;
	while (input != "exit")
	{
		bool isValidCommand = false;
		cout << ">:";
		cin >> input;

		// Loop through commands...
		for (int i = 0; i < commands.size(); i++)
		{
			// If command found, call its respective function
			if (input == commands[i].cmd)
			{
				switch (i)
				{
				case HELP:		DisplayHelp();		break;
				case EXIT:		input = "exit";		break;
				case CREATE:	CreateAccount();	break;
				case LOGIN:		Login();			break;
				case ATTACK:	RainbowAttack();	break;
				case TEST:		MD5Test();			break;
				case CLEAR:		system("cls");		break;
				case DELETE:	Delete();			break;
				case USER_INFO: DisplayUserInfo();	break;
				case SALT_SIZE: SetSaltSize();		break;
				}

				isValidCommand = true;
			}
		}

		if (!isValidCommand)
			cout << "Invalid command!" << endl;

		cout << endl;
	}
}

void Interface::DisplayHelp()
{
	cout << PRETTY(16) << "[Command]"
		<< PRETTY(64) << "[Description]"
		<< PRETTY(32) << "[Example]" << endl;

	for (int i = 0; i < commands.size(); i++)
		cout << PRETTY(16) << commands[i].cmd
			<< PRETTY(64) << "- " + commands[i].description
			<< PRETTY(32) << commands[i].example << endl;
}

Interface::UserData* Interface::GetUserPtr(const int userId)
{
	for (int i = 0; i < userData.size(); i++)
		if (userData[i].userId == userId)
			return &userData[i];

	return nullptr;
}

const Interface::UserData Interface::GetUser(const int userId)
{
	UserData* user = GetUserPtr(userId);

	if (user != nullptr)
		return UserData(*user);

	return { -1 };
}

bool Interface::UserExist(const int userId)
{
	UserData temp = GetUser(userId);

	if (temp.userId != -1)
		return true;

	return false;
}

void Interface::CreateAccount()
{
	UserData newUser;
	string password;

	cin >> newUser.userId;
	cin >> password;
	//getline(cin, password);
	//password.erase(password.begin());

	if (UserExist(newUser.userId))
	{
		cout << "User id already exist!" << endl;
		return;
	}

	// Generate random salt
	newUser.salt = pMd5->GenerateRandomSalt();

	// Generate hash from password & salt
	newUser.hash = pMd5->Hash(password + newUser.salt);

	// Push to userData
	userData.push_back(newUser);

	cout << PRETTY(32) << "Account created!" << "Password + Salt: " << password << " + " << newUser.salt << " -> " << newUser.hash << endl;
}

void Interface::Login()
{
	UserData user;
	string password;

	cin >> user.userId;
	cin >> password;
	//getline(cin, password);
	//password.erase(password.begin());

	user = GetUser(user.userId);

	if (user.userId == -1)
	{
		cout << "User does not exist!" << endl;
		return;
	}

	// Get hash from password & salt
	string hash = pMd5->Hash(password + user.salt);

	// Compare hash with user's stored hash
	if (user.hash == hash)
		cout << "Logged in successfully!" << endl;
	else
		cout << "Failed to log in!" << endl;
}

void Interface::DisplayUserInfo()
{
	string str;
	cin >> str;

	if (userData.empty())
	{
		cout << "No user data!" << endl;
		return;
	}

	vector<UserData> userList;

	if (str == "all")
		userList = userData;
	else
		userList.push_back(GetUser(stoi(str)));

	if (userList[0].userId == -1 || userList.empty())
	{
		cout << "User does not exist!" << endl;
		return;
	}

	for (auto& user : userList)
	{
		cout << "User id: " << PRETTY(16) << user.userId
			<< "Salt: " << PRETTY(16) << user.salt
			<< "Hash: " << PRETTY(16) << user.hash << endl;
	}
}

void Interface::RainbowAttack()
{
	string str;
	cin >> str;

	if (userData.empty())
	{
		cout << "No user data!" << endl;
		return;
	}

	vector<UserData> userList;

	if (str == "all")
		userList = userData;
	else
		userList.push_back(GetUser(stoi(str)));

	vector<short> userCracked(userList.size(), 0);

	if (userList[0].userId == -1 || userList.empty())
	{
		cout << "User does not exist!" << endl;
		return;
	}

	ifstream iFile("rainbowtable.txt");

	if (!iFile.is_open())
	{
		cout << "Failed to read file: rainbowtable.txt" << endl;
		return;
	}

	string hash;
	clock_t start = clock();

	int userCrackedCounter = 0;
	// While there are still hash in file and all users are not cracked...
	while (userCrackedCounter < userList.size() && iFile >> hash)
	{
		// For each uncracked user, compare user's hash with file hash
		for (int i = 0; i < userList.size(); i++)
		{
			// If user is not cracked and two hash match...
			if (!userCracked[i] && hash == userList[i].hash)
			{
				// Set true for cracked
				userCrackedCounter++;
				userCracked[i] = true;

				break;
			}
		}
	}

	cout << "Rainbow attack time elapsed: " << (double)(clock() - start) / CLOCKS_PER_SEC << "s" << endl;

	// Display result for specified user(s)
	for (int i = 0; i < userCracked.size(); i++)
	{
		cout << "Rainbow attack on user ID: " << PRETTY(16) << userList[i].userId;

		if (userCracked[i] > 0)
			cout << "- Success | Hash: " << userList[i].hash << endl;
		else
			cout << "- Failed" << endl;
	}
}

void Interface::MD5Test()
{
	string str;
	cin >> str;
	//getline(cin, str);
	//str.erase(str.begin());

	string hash = pMd5->Hash(str);
	cout << str << " -> " << hash << endl;
}

void Interface::SaveUserData(const string& fileName)
{
	ofstream oFile(fileName);

	if (!oFile.is_open())
	{
		cout << "Failed to open file for writing: " << fileName << endl;
		return;
	}

	for (int i = 0; i < userData.size(); i++)
	{
		UserData& user = userData[i];
		string salt = user.salt == "" ? "." : user.salt;
		oFile << user.userId << " " << salt << " " << user.hash << endl;
	}

	oFile.close();
	cout << "File saved successfully!" << endl;
}

void Interface::LoadUserData(const string& fileName)
{
	ifstream iFile(fileName);

	if (!iFile.is_open())
	{
		cout << "Failed to open file for reading: " << fileName << endl;
		return;
	}

	UserData temp;
	int userCounter = 0;
	string line;

	while (getline(iFile, line))
	{
		stringstream ss(line);

		if (ss.good())
			ss >> temp.userId >> temp.salt >> temp.hash;
		else
			break;

		if (temp.salt == ".")
			temp.salt = "";

		userData.push_back(temp);
		userCounter++;
	}

	iFile.close();
	cout << userCounter << " account(s) read from file: " << fileName << endl;
}

void Interface::Delete()
{
	string userId;
	cin >> userId;

	if (userId == "all")
	{
		userData.clear();
		cout << "Deleted all user data!" << endl;
		return;
	}

	int tempId = stoi(userId);

	UserData* user = GetUserPtr(tempId);
	if (user != nullptr)
	{
		swap(*user, userData[userData.size() - 1]);
		userData.pop_back();
		cout << "Successfully deleted user!" << endl;
	}
	else
	{
		cout << "User does not exist!" << endl;
	}

	return;
}

void Interface::GenerateCombinations(string str, const unsigned short length, ofstream& outputFile)
{
	const static string alphanumeric = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

	if (length == 0) 
	{
		outputFile << pMd5->Hash(str) << endl;
		return;
	}

	for (int i = 0; i < alphanumeric.length(); i++)
	{
		string newStr = str + alphanumeric[i];
		GenerateCombinations(newStr, length - 1, outputFile);
	}
}

void Interface::GenerateRainbowTableHashes(const string& writeFile)
{
	ofstream oFile(writeFile);

	if (!oFile.is_open())
	{
		cout << "Failed to open file for writing: " << writeFile << endl;
		return;
	}

	cout << "Generating combinations..." << endl;

	clock_t start = clock();

	// Generate alphanumeric combinations
	for (unsigned short i = 1; i <= combinationLength; i++)
	{
		GenerateCombinations("", i, oFile);
		cout << "Generated all " << i << " alphanumeric combinations. " << i << "/" << combinationLength << endl;
	}

	clock_t end = ((double)(clock() - start)) / CLOCKS_PER_SEC;

	cout << "Generation time: " << end << "s" << endl;

	oFile.close();
}

void Interface::SetMD5(MD5* ptr)
{
	pMd5 = ptr;
}

void Interface::SetCombinationLength(const unsigned short length)
{
	combinationLength = length;
}

void Interface::SetSaltSize(const int size)
{
	pMd5->SetSaltSize(size);
}

void Interface::SetSaltSize()
{
	int size;
	cin >> size;
	pMd5->SetSaltSize(size);

	cout << "Salt size set to: " << size << endl;
}