
/* Latest changes
	- Almost all admin related interactions are finished now
	- Combined a bunch of employee editing functions to save space using dynamic formats
	- You can now edit and delete employees
	- You can now delete your account when signed in as a non-admin
	- vectorToFile works now
*/

#include <iostream>
#include <format>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <filesystem>
#include <cstdlib>

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
bool loggedIn = true, adminAccount = true, lowStockOnly = false;

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
		cout << format("Please input a number between {} and {}: ", lower, higher);
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
	cout << "Please choose a store (type 0 to go back): ";
	validateInput(storeNum, 0, 3);

	if (storeNum == 0) {
		return;
	}

	fileInfoNum = storeNum - 1;
	getFileInfo(storeNum - 1);

}

// Handles displaying the inventories
static bool viewDetails() {

	int count{};

	if ((*targetVec).size() == 0) {
		cout << format("There are no items inside {}\n\n", storeString);
		return true;
	}

	for (int i = 0; i < (*targetVec).size(); i++) {
		if (lowStockOnly == true && (*targetFormats)[1] == "amount") {
			while (stoi((*targetVec)[i][1]) > 5) {
				i++;
				count++;

				if (i == (*targetVec).size()) {
					break;
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
static string displaySpecificItem(string& value, int& index) {

	value = "";
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


static void editingRoster(int& index, int& day) {

	int answer{};
	double startingHoursInt{}, endingHoursInt{};
	string startingHoursString{}, endingHoursString{}, len{}, prevHours{};

	cout << "1. Assign hours\n2. Clear hours\n3. Back\n\n";
	cout << "Please choose an option: ";
	validateInput(answer, 1, 3);

	switch (answer) {
	case 1:

		cout << "Use 24 hour time format, use a . for minutes e.g. 12.30\n\n";

		cout << format("Enter starting hours for {}: ", rosterFormats[day]);
		validateInput(startingHoursInt, 0, 24);

		len = format("{:.2f}", startingHoursInt);
		for (int i = 0; i < len.length(); i++) {
			if (len[i] == '.') {
				startingHoursString += ":";
			}
			else {
				startingHoursString += len[i];
			}
		}

		cout << format("Enter ending hours for {}: ", rosterFormats[day]);
		validateInput(endingHoursInt, startingHoursInt, 24);

		len = format("{:.2f}", endingHoursInt);
		for (int i = 0; i < len.length(); i++) {
			if (len[i] == '.') {
				endingHoursString += ":";
			}
			else {
				endingHoursString += len[i];
			}
		}

		prevHours = roster[index][day];
		roster[index][day] = format("{} - {}", startingHoursString, endingHoursString);
		vectorToFile();

		cout << format("Successfully updated {}'s hours from {} to {} on {}\n\n", roster[index][0], prevHours, roster[index][day], rosterFormats[day]);
		waitForInput();

		break;

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

	cout << "Pick item by ID: ";
	validateInput(index, 1, max);
	index -= 1;

	system("cls");
	cout << "\n";

	displayItem = displaySpecificItem(displayItem, index);
	cout << displayItem;

	for (int i = 0; i < (*targetFormats).size(); i++) {
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
	cout << "\nSelect an option: ";
	if (fileName == "roster.txt") { // since the first item of roster is skiwpped (the name), we need to take away 1 choice so only a day can be picked
		validateInput(answer, 1, (*targetFormats).size() - 1);
	}
	else {
		validateInput(answer, 1, (*targetFormats).size());
	}

	if (fileName == "roster.txt") {
		editingRoster(index, answer);
		return;
	}

	answer -= 1;

	cout << format("Enter new {}: ", (*targetFormats)[answer]);
	
	switch (answer) {
	case 0:
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		getline(cin, newName);
		displayNewValue = newName;

		if (checkItem(newName, index)) { // Set index to entry ID so the function will overlook the same name so it can actually be changed
			cout << format("Error: item already exists inside {}\n\n", storeString);
			waitForInput();
			return;
		}
		break;

	case 1:
		if (fileName != "employees.txt") { // since inventory uses an integer input (amount) and employees use a string input (role), they need to be separated
			validateInput(newAmount);
			displayNewValue = to_string(newAmount);
		}
		else {
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			getline(cin, newRole);
			displayNewValue = newRole;
		}
		break;

	case 2:
		validateInput(newMoney);
		displayNewValue = format("{:.2f}", newMoney);
	}
	//cout << "shlooby";
	prevItem = (*targetVec)[index][answer];
	switch (answer) {
	case 0:
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
			(*targetVec)[index][answer] = to_string(newAmount);
		}
		else {
			(*targetVec)[index][answer] = newRole;
		}
		break;

	case 2:
		(*targetVec)[index][answer] = format("${:.2f}", newMoney);
	}

	vectorToFile();

	displayItem = displaySpecificItem(displayItem, index);

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

	cout << format("\nDetails:\n\n{}", displayItem);

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

	for (int i = 0; i < (*targetFormats).size(); i++) {
		cout << format("Enter {}: ", (*targetFormats)[i]);
		switch (i) {
		case 0:
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			getline(cin, name);

			if (checkItem(name, (*targetVec).size() + 1)) { // setting index to an impossible value so the function will check every entry and not skip anything
				cout << format("Error: name already exists inside {}\n\n", storeString);
				waitForInput();
				return;
			}
			break;

		case 1:
			if (fileName != "employees.txt") {
				validateInput(amount, 1);
			}
			else {
				getline(cin, role);
			}
			break;

		case 2:
			validateInput(price, 1);
		}
	}

	file.open(fileName, ios::app);
	if (fileName != "employees.txt") {
		(*targetVec).push_back({ name, to_string(amount), format("${:.2f}", (price)) });
		file << format("{}\n{}\n{}\n\n", name, amount, price);

		cout << format("\nSuccessfully added {} to {}'s inventory\n\n", name, storeString);
	}
	else {
		(*targetVec).push_back({ name, role, format("${:.2f}", (price)) });
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

//employee management
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

	cout << "Input ID of the desired employee: ";
	validateInput(index, 1, max);
	index -= 1;

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

	if (confirm("WARNING: You are about to clear the entire roster. Do you wish to proceed?\n")) {
		
		for (int i = 0; i < roster.size(); i++) {

			for (int j = 1; j < roster[i].size(); j++) {
				roster[i][j] = "unset";
			}

		}

		vectorToFile();

		cout << "successfully cleared roster\n\n";
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

		fileInfoNum = 5;
		getFileInfo(5);
		viewDetails();

		int answer{}, employeeID{}, newAmount{}, index{}, max = roster.size();

		cout << "1. Assign hours to employee\n2. Clear employee's schedule\n3. Clear roster\n4. Back\n\n";
		cout << "Please choose an option: ";
		validateInput(answer, 1, 4);

		switch (answer) {
		case 1:
			editingItem(max);
			break;

		case 2:
			clearHours(max);
			break;

		case 3:
			clearRoster();
			break;

		case 4:
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

// user purchasing products
static void purchaseProduct() {
	int productID{}, amount{};
	string priceStr{};
	double price{};

	pickStore();
	system("cls");
	cout << "\n";

	viewDetails();
	cout << "\nEnter the ID of the product you want to order: ";
	validateInput(productID, 1, (*targetVec).size());
	//cin >> productID;
	productID -= 1;
	cout << "\nEnter the amount you want to order: ";
	//cin >> amount;
	validateInput(amount, 1);

	priceStr = (*targetVec)[productID][2];
	erase(priceStr, '$');
	price = stod(priceStr);

	if (amount > stoi((*targetVec)[productID][1])) {
		cout << "\nError: not enough stock to order that amount\n\n";
		waitForInput();
		return;
	}

	cout << format("\nYou have successfully ordered {} {} from {}'s inventory, costing ${}\n\n", amount, (*targetVec)[productID][0], storeString, (price * amount));
	(*targetVec)[productID][1] = to_string(stoi((*targetVec)[productID][1]) - amount);

	vectorToFile();
	waitForInput();
}



// The actual program, this needed to be in its own function so createFiles and fileToVector would only run once
static void program() {

	fstream file;

	int userChoice{};

	while (!loggedIn) {

		system("cls");

		//log in or sign up
		cout << "\n===================Aotearoa Treasures' Inventory Management System===================\n\n";
		cout << "1. Log in\n2. Sign up\n3. Exit\n\nSelect an option: ";
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
			exit(0);

		}
	}

	while (loggedIn) {

		int userChoice{};

		system("cls");
		cout << "\n===================Aotearoa Treasures' Inventory Management System===================\n\n";
		if (adminAccount) {
			cout << "Logged in as admin\n\n";
			cout << format("1. View and Edit Inventory\n2. View and Edit Employees\n3. Edit Roster\n4. Logout\n5. Toggle \"Low Stock Only\" Mode (Currently {})\n6. Exit", lowStockOnly);
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
				loggedIn = false;
				adminAccount = false;
				lowStockOnly = false;

				cout << "Successfully logged out\n\n";
				waitForInput();
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