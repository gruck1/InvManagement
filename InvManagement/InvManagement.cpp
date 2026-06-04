
/* Latest changes
	- Added comma formatting
*/

#include <iostream>
#include <sstream>
#include <format>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <filesystem>
#include <cstdlib>
#include <locale>

using namespace std;
using namespace filesystem;

int vectorCount = 6;
vector<vector<string>> wellington = {};
vector<vector<string>> christchurch = {};
vector<vector<string>> auckland = {};
vector<vector<string>> accounts = {};
vector<vector<string>> employees = {};
vector<vector<string>> roster = {};
// Don't add any vectors to vectorCount below here

vector<string> inventoryFormats = { "name", "amount", "price" };
vector<string> employeeFormats = { "name", "role", "salary" };
vector<string> rosterFormats = { "name", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };

vector<string>* targetFormats = &inventoryFormats;
vector<vector<string>>* targetVec = &wellington;

string storeString{}, fileName{}, username{}, password{};;

int storeNum{}, fileInfoNum{};
bool loggedIn = true, adminAccount = false, lowStockOnly = false;

// Formats stuff to commas (1,234,567,890)
static string commaFormat(auto& value, bool decimals = true) {
	stringstream ss;
	ss.imbue(locale(""));

	if (decimals) {
		ss << fixed << setprecision(2) << value;
	}
	else {
		ss << value;
	}
	return ss.str();
}

// Handles invalid input and ranges
static int validateInput(auto& validator, int lower = -1000000000, int higher = 1000000000) {

	cin >> validator;

	while (cin.fail()) {
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cout << "Please input a valid integer: ";
		cin >> validator;
	}

	while (validator < lower || validator > higher) {
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cout << format("Please input a number between {} and {}: ", commaFormat(lower), commaFormat(higher));
		validateInput(validator, lower, higher);
	}

	return validator;

}

// Handles pauses
static void waitForInput() {

	cout << "Press Enter to continue...";
	
	if (cin.rdbuf()->in_avail() > 0) {							  // if there are any leftover characters in the input buffer, clear them.
		cin.ignore(std::numeric_limits<streamsize>::max(), '\n'); // This is needed since the program uses getline which gets the whole line (no buffers left over)
	}															  // and cin which leaves characters in the input buffer;

	cin.get();
	system("cls");

}

/*
Changes info to fit correct format so dynamic names can be used.
Example:
	fstream file("wellington.txt"); this is hard coded
	cout << wellington[0][1]; this is hard coded

	fstream file(fileName); this uses dynamic names
	cout << (*targetVec)[0][1]; this also uses dynamic names
*/
static void getFileInfo(int index) {

	switch (index) {
	case 0:
		fileName = "wellington.txt";
		storeString = "Wellington";
		targetVec = &wellington;
		break;

	case 1:
		fileName = "christchurch.txt";
		storeString = "Christchurch";
		targetVec = &christchurch;
		break;

	case 2:
		fileName = "auckland.txt";
		storeString = "Auckland";
		targetVec = &auckland;
		break;

	case 3:
		fileName = "accounts.txt";
		targetVec = &accounts;
		break;

	case 4:
		fileName = "employees.txt";
		storeString = "employees";
		targetVec = &employees;
		break;

	case 5:
		fileName = "roster.txt";
		storeString = "roster";
		targetVec = &roster;
		break;
	}
}

// If no files exist, create them
static void createFiles() {

	for (int i = 0; i < vectorCount; i++) {
		getFileInfo(i);
		fstream file(fileName, ios::app);
		file.close();
	}

}

// Convert the text files to vectors
static void fileToVector() {

	fstream file;
	int index{};

	for (index = 0; index < vectorCount; index++) {

		getFileInfo(index);

		file.open(fileName, ios::in);

		string line{};
		int i{};

		if (file_size(fileName) > 0) {
			int maxLine{}, currentLine{};

			while (getline(file, line)) {
				maxLine++;
			}
			// maxLine -= 1;

			targetVec->push_back({});

			file.clear();
			file.seekg(0);
			while (getline(file, line)) {
				currentLine++;
				if (currentLine == maxLine) {
					break;
				}

				if (line == "") {
					targetVec->push_back({});
					i++;

				}
				else {
					(*targetVec)[i].push_back(line);
				}

			}
		}
		file.close();
	}

	fileName = "";

}

// Converts the vectors back to files, used when an item is deleted or edited
static void vectorToFile() {

	fstream file;
	int index{}, lineCount{};
	string line{};

	for (index = 0; index < vectorCount; index++) {

		getFileInfo(index);
		file.open(fileName, ios::in);

		if (file_size(fileName) > 0) {

			resize_file(fileName, 0);
			file.close();

			file.open(fileName, ios::app);
			for (int i = 0; i < (*targetVec).size(); i++) {

				for (int j = 0; j < (*targetVec)[i].size(); j++) {
					file << (*targetVec)[i][j];
					file << "\n";
				}
				file << "\n";

			}
		}
	}

	getFileInfo(fileInfoNum);

}

//check if the username and password match any of the accounts in the accounts vector. if it does, return true, if not return, false
//also give the user admin access if the account is an admin account
static bool checkLogin(string username, string password, string mode = "login") {

	// If the account vector is empty then no accounts have been made so there's obviously no way anything can match
	if (accounts.size() == 0) {
		return false;
	}

	// If the user is logging in, make sure the username and password are correct. If admin account, apply admin
	if (mode == "login") {
		for (int i = 0; i < accounts.size(); i++) {
			if (accounts[i][0] == username && accounts[i][1] == password) {
				if (accounts[i][2] == "true") {
					adminAccount = true;
				}
				return true;
			}
		}
		return false;
	}

	// If user is creating account, make sure the username doesn't already exist
	else if (mode == "create") {

		for (int i = 0; i < accounts.size(); i++) {
			if (accounts[i][0] == username) {
				return true;
			}
		}
		return false;
	}

	return false;
}

// Handles confirming options the user has chosen E.G deleting an item
static bool confirm(string text) {

	int answer{};

	cout << text;
	cout << "\n1. Yes, 2. No: ";
	validateInput(answer, 1, 2);

	system("cls");
	cout << "\n";

	if (answer == 1) {
		return true;
	}
	else {
		return false;
	}

}

// Handles choosing a store
static void pickStore() {

	cout << "1. Wellington\n2. Christchurch\n3. Auckland\n\n";
	cout << "Please choose a store: ";
	validateInput(storeNum, 1, 3);

	fileInfoNum = storeNum - 1;
	getFileInfo(storeNum - 1);

}

// Handles displaying the inventories
static bool viewDetails() {

	int count{};
	string amountStr{};

	if ((*targetVec).size() == 0) {
		cout << format("There are no items inside {}\n\n", storeString);
		return true;
	}

	for (int i = 0; i < (*targetVec).size(); i++) {

		amountStr = (*targetVec)[i][1];
		erase(amountStr, ',');

		if (lowStockOnly == true && (*targetFormats)[1] == "amount") {

			while (stoi(amountStr) > 5) {
				i++;
				count++;

				if (i == (*targetVec).size()) {
					break;
				}
				else {
					amountStr = (*targetVec)[i][1];
					erase(amountStr, ',');
				}
			}
			if (i == (*targetVec).size()) {
				break;
			}
		}

		cout << format("ID {}\n", i + 1);
		for (int j = 0; j < (*targetVec)[i].size(); j++) {
			cout << format("{}: {}\n", (*targetFormats)[j], (*targetVec)[i][j]);
		}
		cout << "\n";
	}

	if (count == (*targetVec).size()) {
		cout << format("There are no items under 5 stock inside {}\n\n", storeString);
		waitForInput();
		return false;
	}

	return true;

}

// Gets the specific item that the user wants to interact with it so it can be used easily
static string displaySpecificItem(int& index) {

	string value = "";
	value += format("ID: {}\n", index + 1);
	for (int i = 0; i < (*targetVec)[index].size(); i++) {
		value += format("{}: {}\n", (*targetFormats)[i], (*targetVec)[index][i]);
	}
	value += "\n";

	return value;
}

// Handles checking for duplicate entries
static bool checkItem(string& value, int index) {

	for (int i = 0; i < (*targetVec).size(); i++) {
		if ((*targetVec)[i][0] == value && i != index) {
			return true;
		}
	}

	return false;
}

// Handles the actual editing part of the roster
static void editingRoster(int& index, int day) {

	int answer{};
	double startingHoursInt{}, endingHoursInt{};
	string startingHoursString{}, endingHoursString{}, len{}, prevHours{};

	if (day != roster[index].size()) {
		cout << "1. Assign hours to day\n2. Clear hours\n3. Back\n\n";
		cout << "Please choose an option: ";
		validateInput(answer, 1, 3);
	}
	else {
		answer = 1;
	}

	system("cls");
	cout << "\n";

	switch (answer) {
	case 1: {

		cout << "Use 24 hour time format, use a . for minutes e.g. 12.30\nType 0 in both prompts to cancel\n\n";

		cout << "Enter starting hours";
		if (day != roster[index].size()) {
			cout << format(" for {}: ", rosterFormats[day]);
		}
		else {
			cout << ": ";
		}
		validateInput(startingHoursInt, 0, 24);

		len = format("{:05.2f}", startingHoursInt);
		for (int i = 0; i < len.length(); i++) {
			if (len[i] == '.') {
				startingHoursString += ":";
			}
			else {
				startingHoursString += len[i];
			}
		}

		cout << "Enter ending hours";
		if (day != roster[index].size()) {
			cout << format(" for {}: ", rosterFormats[day]);
		}
		else {
			cout << ": ";
		}
		validateInput(endingHoursInt, startingHoursInt, 24);

		len = format("{:05.2f}", endingHoursInt);
		for (int i = 0; i < len.length(); i++) {
			if (len[i] == '.') {
				endingHoursString += ":";
			}
			else {
				endingHoursString += len[i];
			}
		}

		if (startingHoursInt == 0 && endingHoursInt == 0) {
			system("cls");
			cout << "\nOperation cancelled\n\n";
			waitForInput();
			return;
		}

		/*
		startingHoursString[3] gets the ASCII index rather than the actual number. 0 is 48 so you can take away 0's ASCII index away from the first index to get the original number.
		Example:
			startingHoursString[3] = 54 (6's ASCII index)
			startingHoursString[4] = 51 (3's ASCII index)
			54 - 48 = 6 (number we want that represents the 10's collumn of minutes)
			6 * 10 = 60 (we now have the minutes in the 10's collumn)
			51 - 48 = 3 (number we want that represents the 1's collumn of minutes)
			60 + 3 = 63 (exceeds the maximum minutes on a clock)
			
		Now we can just check them both in an "if or" clause to see if any of them go over the maximum minutes.
		If they do, don't accept the input
		*/ 
		int startingMinutes = (startingHoursString[3] - '0') * 10 + (startingHoursString[4] - '0'); 
		int endingMinutes = (endingHoursString[3] - '0') * 10 + (endingHoursString[4] - '0');

		if (startingMinutes >= 60 || endingMinutes >= 60) {
			system("cls");
			cout << "\nError: input exceeds 60 minutes\n\n";
			waitForInput();
			return;
		}

		if (endingHoursInt - startingHoursInt < 3) {
			system("cls");
			cout << "\nError: a minimum of 3 hours are required a day\n\n";
			waitForInput();
			return;
		}

		if (day != roster[index].size()) {
			day = 2;
		}

		system("cls");
		cout << "\n";

		for (int i = 1; i < day; i++) {
			prevHours = roster[index][i];
			roster[index][i] = format("{} - {}", startingHoursString, endingHoursString);
			cout << format("Successfully updated {}'s hours from {} to {} on {}\n", roster[index][0], prevHours, roster[index][i], rosterFormats[i]);
		}
		cout << "\n";

		vectorToFile();
		waitForInput();

		break;
	}

	case 2:

		prevHours = roster[index][day];
		roster[index][day] = "unset";
		vectorToFile();

		cout << format("Successfully cleared {}'s hours on {}\n\n", roster[index][0], rosterFormats[day]);
		waitForInput();

		break;

	case 3:
		return;
	}
}

// Converts strings to title case (Hello How Are You)
static void convertToTitle(string& value) {

	bool convert = true;

	for (char &character : value) {

		if (convert) {
			// unsigned char is a safety mechanism that stops negative characters (ASCII index)
			character = static_cast<unsigned char>(toupper(static_cast<unsigned char>(character)));
			convert = false;

		}
		else if (isupper(character)) {
			character = static_cast<unsigned char>(tolower(static_cast<unsigned char>(character)));
		}

		if (static_cast<unsigned char>(character == ' ')) {
			convert = true;
		}
	}
}

// Handles the actual editing part of inventories, list of employees, and roster
static void editingItem(int& max) {

	int index{}, newAmount{}, answer{};
	string newName{}, newRole, prevItem{}, displayItem{}, displayNewValue{};
	double newMoney{};

	if ((*targetVec).size() == 0) {
		cout << format("There are no items to edit inside {}\n\n", storeString);
		waitForInput();
		return;
	}

	cout << "Pick item by ID (type 0 to cancel): ";
	validateInput(index, 0, max);

	if (index == 0) {
		return;
	}

	index -= 1;

	system("cls");
	cout << "\n";

	displayItem = displaySpecificItem(index);
	cout << displayItem;

	int ids{};

	for (int i = 0; i < (*targetFormats).size(); i++) {

		ids++;

		if (fileName == "roster.txt" && i == 0) { // skip being able to edit the name when editing roster, only hours can be edited
			i++;
		}

		if (fileName == "roster.txt") { // no need for the "i + 1" here because since the first item (name) is skipped, it can use normal i
			cout << format("{}. Edit {}\n", i, (*targetFormats)[i]);
		}
		else {
			cout << format("{}. Edit {}\n", i + 1, (*targetFormats)[i]);
		}
	}

	cout << format("{}: Back\n", ids + 1);

	cout << "\nSelect an option: ";
	if (fileName == "roster.txt") { // there is no need for the extra + 1 here because "back" is added as a choice and the first item of each nest in roster is skipped anyway
		validateInput(answer, 1, (*targetFormats).size());
	}
	else {
		validateInput(answer, 1, (*targetFormats).size() + 1); // since there is an extra option added (back), we need to add one more to the total size
	}

	if (answer == ids + 1) {
		return;
	}

	if (fileName == "roster.txt") {
		editingRoster(index, answer);
		return;
	}

	answer -= 1;

	cout << format("Enter new {} (type 0 to cancel): ", (*targetFormats)[answer]);
	bool convert = true;
	switch (answer) {
	case 0:
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		getline(cin, newName);

		convertToTitle(newName);

		displayNewValue = newName;

		if (newName == "0") {
			return;
		}

		if (newName.length() < 3) {
			cout << "Error: name must be 3 characters or more\n\n";
			waitForInput();
			return;
		}

		if (checkItem(newName, index)) { // Set index to entry ID so the function will overlook the same name so it can actually be changed
			cout << format("Error: item already exists inside {}\n\n", storeString);
			waitForInput();
			return;
		}
		break;

	case 1:
		if (fileName != "employees.txt") { // since inventory uses an integer input (amount) and employees use a string input (role), they need to be separated
			validateInput(newAmount);
			displayNewValue = format("{}", commaFormat(newAmount), false);

			if (newAmount == 0) {
				return;
			}
		}

		else {
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			getline(cin, newRole);
			displayNewValue = newRole;

			if (newRole == "0") {
				return;
			}

		}
		break;

	case 2:
		validateInput(newMoney);
		displayNewValue = format("{}", commaFormat(newMoney));

		if (newMoney < 29.90 && fileName == "employees.txt") {
			cout << "Error: salary must be equal to or above minimum wage ($29.90)\n\n";
			waitForInput();
			return;
		}
	}
	//cout << "shlooby";
	prevItem = (*targetVec)[index][answer];
	switch (answer) {
	case 0:

		if (prevItem == newName) {
			cout << "Error: New name cannot be the same as old name\n\n";
			waitForInput();
			return;
		}

		(*targetVec)[index][answer] = newName;
		if (fileName == "employees.txt") { // if the employee name is edited, make sure to update the name in the roster as well
			for (int i = 0; i < roster.size(); i++) {
				if (roster[i][0] == prevItem) {
					roster[i][0] = newName;
					break;
				}
			}
		}
		break;

	case 1:
		if (fileName != "employees.txt") { // same thing applies here, inventory uses an integer (amount) and employees uses a string (role) so they need to be separated

			if (prevItem == to_string(newAmount)) {
				cout << "Error: New amount cannot be the same as old amount\n\n";
				waitForInput();
				return;
			}

			(*targetVec)[index][answer] = format("{}", commaFormat(newAmount));
		}
		else {

			if (prevItem == newRole) {
				cout << "Error: New role cannot be the same as old role\n\n";
				waitForInput();
				return;
			}

			(*targetVec)[index][answer] = newRole;
		}
		break;

	case 2:
		if (prevItem == format("${:.2f}", newMoney)) {
			cout << format("Error: New {} cannot be the same as old {}\n\n", (*targetFormats)[answer], (*targetFormats)[answer]);
			waitForInput();
			return;
		}

		(*targetVec)[index][answer] = format("${}", commaFormat(newMoney));
	}

	vectorToFile();

	displayItem = displaySpecificItem(index);

	if (answer == 2) {
		cout << format("Successfuly changed item {}'s {} from {} to ${}", index + 1, (*targetFormats)[answer], prevItem, displayNewValue);
	}
	else {
		cout << format("Successfuly changed item {}'s {} from {} to {}", index + 1, (*targetFormats)[answer], prevItem, displayNewValue);
	}

	if (fileName != "employees.txt") {
		cout << format(" inside {}'s inventory\n", storeString);
	}
	else {
		cout << "\n";
	}

	cout << format("\nDetails,\n{}", displayItem);

	waitForInput();

}

// Handles deleting items from the inventories
static void deleteInventory(int& max) {

	int index{};

	if ((*targetVec).size() == 0) {
		cout << format("There are no items to delete inside {}\n\n", storeString);
		waitForInput();
		return;
	}

	cout << "Choose the ID of the item you want to delete: ";
	validateInput(index, 1, max);
	index -= 1;

	system("cls");
	cout << "\n";

	if (confirm(format("WARNING: You are about to delete item {} of {}'s inventory\nDo you wish to proceed?\n", index + 1, storeString))) {
		(*targetVec).erase((*targetVec).begin() + index);

		vectorToFile();
		getFileInfo(fileInfoNum);
		cout << format("Successfully deleted item {} of {}'s inventory\n\n", index + 1, storeString);
	}
	else {
		cout << "Deletion cancelled\n\n";
	}

	waitForInput();

}

// Handles deleting employees
static void deleteEmployee(int& max) {

	int index{};

	if ((*targetVec).size() == 0) {
		cout << format("There are no items to delete inside {}\n\n", storeString);
		waitForInput();
		return;
	}

	cout << "Choose the ID of the item you want to delete: ";
	validateInput(index, 1, max);
	index -= 1;

	system("cls");
	cout << "\n";

	if (confirm(format("WARNING: You are about to delete item {} from list of employees\nDo you wish to proceed?\n", index + 1, storeString))) {
		(*targetVec).erase((*targetVec).begin() + index);
		roster.erase(roster.begin() + index);

		vectorToFile();
		cout << format("Successfully deleted item {} from list of employees\n\n", index + 1);
	}
	else {
		cout << "Deletion cancelled\n\n";
	}

	waitForInput();

}

// Handles deleting your account
static void deleteAccount() {

	if (confirm("WARNING: You are about to delete your account. Do you wish to proceed?\n")) {

		for (int i = 0; i < accounts.size(); i++) {
			if (accounts[i][0] == username && accounts[i][1] == password) {
				accounts.erase(accounts.begin() + i);

				vectorToFile();
				loggedIn = false;
				cout << format("Your account {} has been successfully deleted\n\n", username);
			}
		}
	}
	else {
		cout << "Deletion cancelled\n\n";
	}

	waitForInput();

}

// Handles adding an item to either inventory or list of employees
static void addItem() {

	string name{}, role{};
	int amount{};
	double price{};
	fstream file;

	system("cls");
	cout << "\n";

	cout << "Type 0 at any stage to cancel adding an item\n\n";

	for (int i = 0; i < (*targetFormats).size(); i++) {
		cout << format("Enter {}: ", (*targetFormats)[i]);
		switch (i) {
		case 0:
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			getline(cin, name);

			convertToTitle(name);

			if (name == "0") {
				return;
			}

			if (name.length() < 3) {
				cout << "Error: name must be 3 characters or more\n\n";
				waitForInput();
				return;
			}

			if (checkItem(name, (*targetVec).size() + 1)) { // setting index to an impossible value so the function will check every entry and not skip anything
				cout << format("Error: name already exists inside {}\n\n", storeString);
				waitForInput();
				return;
			}
			break;

		case 1:
			if (fileName != "employees.txt") {
				validateInput(amount, 0);

				if (amount == 0) {
					return;
				}
			}
			else {
				getline(cin, role);

				if (role == "0") {
					return;
				}
			}
			break;

		case 2:
			validateInput(price, 0);

			if (price == 0) {
				return;
			}

			if (price < 29.90 && fileName == "employees.txt") {
				cout << "Error: salary must be equal to or above minimum wage ($29.90)\n\n";
				waitForInput();
				return;
			}
		}
	}

	file.open(fileName, ios::app);
	if (fileName != "employees.txt") {
		(*targetVec).push_back({ name, format("{}", commaFormat(amount, false)), format("${}", commaFormat(amount)) });
		file << format("{}\n{}\n{}\n\n", name, format("{}", commaFormat(amount, false)), format("${}", commaFormat(price)));

		cout << format("\nSuccessfully added {} to {}'s inventory\n\n", name, storeString);
	}
	else {
		(*targetVec).push_back({ name, role, format("${}", commaFormat(price)) });
		roster.push_back({ name, "unset", "unset", "unset", "unset", "unset", "unset", "unset", });

		file << format("{}\n{}\n${:.2f}\n\n", name, role, price);
		file.close();

		file.open("roster.txt", ios::app);
		file << format("{}\nunset\nunset\nunset\nunset\nunset\nunset\nunset\n\n", name);

		cout << format("\nSuccessfully added {} to list of employees\n\n", name);
	}

	waitForInput();

}

// Handles all the selecting stuff in the "edit inventory" menu
static void editInventory() {

	pickStore();

	while (true) {

		system("cls");
		cout << "\n";

		if (!viewDetails()) { // only applies when lowStockMode is set to true and there is nothing under 5 stock
			return;
		}

		int max{}, answer{};

		cout << format("Current Store: {}\n\n", storeString);
		cout << "1. Add item\n2. Edit item\n3. Delete item\n4. Back\n\n";
		cout << "Please choose an option: ";
		validateInput(answer, 1, 4);

		max = (*targetVec).size();

		switch (answer) {
		case 1:
			addItem();
			break;

		case 2:
			editingItem(max);
			break;

		case 3:
			deleteInventory(max);
			break;

		case 4:
			return;
		}
	}
}

//Handles employee management
static void editEmployees() {

	while (true) {

		system("cls");
		cout << "\n";

		fileInfoNum = 4;
		getFileInfo(4);
		viewDetails();

		int answer{}, employeeID{}, newAmount{}, index{}, max = employees.size();

		cout << "1. Add employee\n2. Edit employee\n3. Delete employee\n4. Back\n\n";
		cout << "Please choose an option: ";
		validateInput(answer, 1, 4);

		switch (answer) {
		case 1:
			addItem();
			break;

		case 2:
			editingItem(max);
			break;

		case 3:
			deleteEmployee(max);
			break;

		case 4:
			fileName = "";
			return;
		}
	}
}

// Handles setting all hours back to "unset" for one employee
static void clearHours(int& max) {

	int index{};

	cout << "Pick employee by ID: ";
	validateInput(index, 1, max);
	index -= 1;

	system("cls");
	cout << "\n";

	if (confirm(format("WARNING: You are about to clear the entire schedule of employee {}.\nDo you wish to proceed?\n", roster[index][0]))) {
		for (int i = 1; i < roster[index].size(); i++) {
			roster[index][i] = "unset";
		}

		vectorToFile();

		cout << format("Successfully cleared {}'s schedule\n\n", roster[index][0]);
	}
	else {
		cout << "Task cancelled\n\n";
	}

	waitForInput();

}

// Handles setting the entire roster back to "unset" for each employee
static void clearRoster() {

	system("cls");
	cout << "\n";

	if (confirm("WARNING: You are about to clear the entire roster. Do you wish to proceed?\n")) {
		
		for (int i = 0; i < roster.size(); i++) {

			for (int j = 1; j < roster[i].size(); j++) {
				roster[i][j] = "unset";
			}

		}

		vectorToFile();

		cout << "Successfully cleared roster\n\n";
	}
	else {
		cout << "Task cancelled\n\n";
	}

	waitForInput();

}

// Handles editing roster
static void editRoster() {

	if (roster.size() == 0) {
		cout << "There are no employees to manage\n\n";
		waitForInput();
		return;
	}

	while (true) {

		system("cls");
		cout << "\n";

		fileInfoNum = 5;
		getFileInfo(5);
		viewDetails();

		int answer{}, employeeID{}, newAmount{}, index{}, max = roster.size();

		cout << "1. Assign hours to employee\n2. Assign hours to employee for all days\n3. Clear employee's schedule\n4. Clear roster\n5. Back\n\n";
		cout << "Please choose an option: ";
		validateInput(answer, 1, 5);

		switch (answer) {
		case 1:
			editingItem(max);
			break;

		case 2:

			cout << "Pick employee by ID (type 0 to go back): ";
			validateInput(index, 0, roster.size());

			if (index == 0) {
				break;
			}

			index--;

			editingRoster(index, roster[index].size());
			break;

		case 3:
			clearHours(max);
			break;

		case 4:
			clearRoster();
			break;

		case 5:
			fileName = "";
			system("cls");
			return;
		}
	}
}

// Checks if password uses best password practices
static string checkPassword(string& password) {

	while (password.length() < 12) {
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cout << "Error: password must be over 12 characters. Try again: ";
		cin >> password;
		checkPassword(password);
	}

	// string::npos means target was not found, we can use this to check for any of the following characters
	while (password.find_first_of("0123456789") == string::npos) {
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cout << "Error: password must contain at least one number. Try again: ";
		cin >> password;
		checkPassword(password);
	}

	while (password.find_first_of("`~!@#$%^&*()_-=+[]{}\\|;:'\"<,.>/?") == string::npos) {
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cout << "Error: password must contain at least one special character. Try again: ";
		cin >> password;
		checkPassword(password);
	}

	return password;

}

// Handles purchasing products
static void purchaseProduct() {
	int productID{}, amount{};
	string priceStr{}, amountStr, displayItem;
	double price{}, totalPrice{};

	pickStore();
	system("cls");
	cout << "\n";

	viewDetails();

	cout << "Enter the ID of the product you want to order (type 0 to cancel): ";
	validateInput(productID, 0, (*targetVec).size());
	
	if (productID == 0) {
		return;
	}

	productID -= 1;

	displayItem = displaySpecificItem(productID);

	system("cls");
	cout << "\n";

	if ((*targetVec)[productID][1] == "0") {
		cout << "This item has no stock. Please choose another\n\n";
		waitForInput();
		return;
	}

	cout << format("{}Enter the amount you want to order (type 0 to cancel): ", displayItem);
	validateInput(amount, 0);

	if (amount == 0) {
		return;
	}

	amountStr = (*targetVec)[productID][1];
	erase(amountStr, ',');

	priceStr = (*targetVec)[productID][2];
	erase(priceStr, '$');
	erase(priceStr, ',');
	price = stod(priceStr);

	totalPrice = price * amount;
	priceStr = format("{}", commaFormat(totalPrice));

	if (amount > stoi(amountStr)) {
		cout << "\nError: not enough stock to order that amount\n\n";
		waitForInput();
		return;
	}

	(*targetVec)[productID][1] = to_string(stoi(amountStr) - amount);

	cout << format("\nYou have successfully ordered {} {} from {}'s inventory, costing ${}\n\n", commaFormat(amount), (*targetVec)[productID][0], storeString, commaFormat(priceStr));

	vectorToFile();
	waitForInput();
}

// The actual program, this needed to be in its own function so createFiles and fileToVector would only run once
static void program() {

	fstream file;

	int userChoice{};

	while (!loggedIn) {

		system("cls");

		// Login or sign up
		cout << "\n===================Aotearoa Treasures' Inventory Management System===================\n\n";
		cout << "1. Login\n2. Sign up\n3. View reminders\n4. Exit\n\nSelect an option: ";
		validateInput(userChoice, 1, 3);

		system("cls");
		cout << "\n";

		switch (userChoice) {
		case 1:
			cout << "Please enter your username (type 0 to cancel): ";
			cin >> username;
			if (username == "0") {
				break;
			}

			cout << "Please enter your password (type 0 to cancel): ";
			cin >> password;
			if (password == "0") {
				break;
			}

			if (checkLogin(username, password)) {
				cout << "Login successful!\n\n";
				loggedIn = true;
				waitForInput();
			}
			else {
				cout << "Login failed. Please try again.\n\n";
				waitForInput();
				continue;
			}

			break;
			//add new account to list of accounts and export to text file
		case 2:

			cout << "Please choose a username (type 0 to go back): ";
			cin >> username;
			if (username == "0") {
				break;
			}

			if (username.length() < 5) {
				cout << "Error: username must be over 5 characters\n\n";
				waitForInput();
				continue;
			}

			if (checkLogin(username, password, "create")) {
				cout << "Error: username already exists. Please pick a different one\n\n";
				waitForInput();
				continue;
			}

			//check password security
			cout << "Please enter a password - password must be over 12 characters,\ncontain at least one number, and one special character ";
			cout << "(type 0 to go back): ";
			cin >> password;
			if (password == "0") {
				break;
			}

			checkPassword(password);

			//save username and password without admin access
			accounts.push_back({ username, password, "false" });

			file.open("accounts.txt", ios::app);
			file << format("{}\n{}\n{}\n\n", username, password, "false");
			file.close();

			cout << "Successfully created account!\n\n";
			waitForInput();
			break;

		case 3:
			cout << "Don't forget,\nUsername: admin\nPassword: password\n\n";
			waitForInput();
			break;

		case 4:
			exit(0);

		}
	}

	while (loggedIn) {

		int userChoice{};

		system("cls");
		cout << "\n===================Aotearoa Treasures' Inventory Management System===================\n\n";
		if (adminAccount) {
			cout << "Logged in as admin\n\n";
			cout << "1. View and edit inventory\n2. View and edit employees\n3. View and edit roster\n4. View and edit accounts\n";
			cout << format("5. Toggle \"low stock only\" mode (currently {})\n6. Logout\n7. Exit", lowStockOnly);
			cout << "\n\nSelect an option: ";
			validateInput(userChoice, 1, 6);

			system("cls");
			cout << "\n";
			switch (userChoice) {
			case 1:
				// View and edit all inventories
				targetFormats = &inventoryFormats;
				editInventory();
				break;

			case 2:
				// add and delete employees from the account's vector, then export the vector to the text file
				targetFormats = &employeeFormats;
				editEmployees(); 
				break;

			case 3:
				// add and delete shifts from the roster vector, then export the vector to the text file
				targetFormats = &rosterFormats;
				editRoster();
				break;

			case 4:

				break;

			case 5:
				if (lowStockOnly) {
					lowStockOnly = false;
					cout << "\"Low Stock Only\" mode disabled. All items are visible to you\n\n";
				}
				else {
					lowStockOnly = true;
					cout << "\"Low Stock Only\" mode enabled. Only items with 5 or less stock are visible to you\n\n";
				}
				waitForInput();
				break;

			case 6:
				loggedIn = false;
				adminAccount = false;
				lowStockOnly = false;

				cout << "Successfully logged out\n\n";
				waitForInput();
				break;

			case 7:
				exit(0);
				break;

			}
		}
		else {

			cout << "1. View Inventory\n2. Order Product\n3. Logout\n4. Delete Account\n5. Exit\n\n";
			cout << "Select an option: ";
			validateInput(userChoice, 1, 5);

			system("cls");
			cout << "\n";
			switch (userChoice) {
			case 1:
				pickStore();
				system("cls");
				cout << "\n";

				viewDetails();
				waitForInput();
				break;

			case 2:
				purchaseProduct();
				break;

			case 3:
				loggedIn = false;
				cout << "Successfully logged out\n\n";
				waitForInput();
				break;

			case 4:
				deleteAccount();
				break;

			case 5:
				exit(0);
			}
		}

		program();

	}

}

// When code starts, run these functions then loop program forever
int main() {

	createFiles();
	fileToVector();

	program();

}