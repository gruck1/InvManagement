
/* Latest changes
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

using namespace std;
using namespace filesystem;

int vectorCount = 5;
vector<vector<string>>wellington = {};
vector<vector<string>>christchurch = {};
vector<vector<string>>auckland = {};
vector<vector<string>>accounts = {};
vector<vector<string>>employees = {};
// Don't add any vectors to vectorCount below here

vector<string> inventoryFormats = { "name", "amount", "price" };
vector<string> employeeFormats = { "name", "role", "salary" };

vector<string>* targetFormats = &inventoryFormats;
vector<vector<string>>* targetVec = &wellington;

string storeString{}, fileName{}, username{}, password{};;

int storeNum{};
bool loggedIn = false, adminAccount = false;

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

/*
Changes fileName to fit txt format so dynamic names can be used.
Example:
	fstream file("wellington.txt"); this is hard coded

	fstream file(fileName); this uses dynamic file names
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
		targetVec = &employees;
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

	for (index = 0; index < vectorCount; index++) { // 1 is placeholder

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
	cout << "Please choose a store: ";
	validateInput(storeNum, 1, 3);

	getFileInfo(storeNum - 1);

}

// Handles displaying the inventories
static void viewDetails() {

	if (fileName != "employees.txt") {
		pickStore();
	}

	for (int i = 0; i < (*targetVec).size(); i++) {
		cout << format("Item {}\n", i + 1);
		for (int j = 0; j < (*targetVec)[i].size(); j++) {
			if (j == 2) {
				cout << format("{}: $", (*targetFormats)[j]);
			}
			else {
				cout << format("{}: ", (*targetFormats)[j]);
			}
			cout << format("{}\n", (*targetVec)[i][j]);
		}
		cout << "\n";
	}


}

// Gets the specific item that the user wants to interact with it so it can be used easily
static string displaySpecificItem(string& value, int& index) {

	value = "";

	value += format("Item {}\n", index + 1);
	for (int i = 0; i < (*targetVec)[index].size(); i++) {
		if (i == 2) {
			value += format("{}: $", (*targetFormats)[i]);
		}
		else {
			value += format("{}: ", (*targetFormats)[i]);
		}
		value += format("{}\n", (*targetVec)[index][i]);
	}
	value += "\n";

	return value;
}

// Handles the actual editing part of either inventories or list of employees
static void editingItem(int& max) {

	int index{}, newAmount{}, answer{};
	string newName{}, newRole, prevItem{}, displayItem{};
	double newMoney{};

	cout << "Pick item by ID: ";
	validateInput(index, 1, max);
	index -= 1;

	displayItem = displaySpecificItem(displayItem, index);
	cout << displayItem;

	for (int i = 0; i < (*targetFormats).size(); i++) {
		cout << format("{}. Edit {}\n", i + 1, (*targetFormats)[i]);
	}
	cout << "\nSelect an option: ";
	validateInput(answer, 1, 3);
	answer -= 1;

	cout << format("Enter new {}: ", (*targetFormats)[answer]);
	
	switch (answer) {
	case 0:
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		getline(cin, newName);
		break;

	case 1:
		if (fileName != "employees.txt") {
			validateInput(newAmount);
		}
		else {
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			getline(cin, newRole);
		}
		break;

	case 2:
		validateInput(newMoney);
	}

	prevItem = (*targetVec)[index][answer];
	switch (answer) {
	case 0:
		(*targetVec)[index][answer] = newName;
		break;

	case 1:
		if (fileName != "employees.txt") {
			(*targetVec)[index][answer] = to_string(newAmount);
		}
		else {
			(*targetVec)[index][answer] = newRole;
		}
		break;

	case 2:
		(*targetVec)[index][answer] = format("{:.2f}", newMoney);
	}

	vectorToFile();

	displayItem = displaySpecificItem(displayItem, index);

	if (fileName != "employees.txt") {
		switch (answer) {
		case 0:
			cout << format("Successfully changed item {}'s {} from {} to {} inside {}'s inventory\n", index + 1, (*targetFormats)[answer], prevItem, newName, storeString);
			break;

		case 1:
			cout << format("Successfully changed item {}'s {} from {} to {} inside {}'s inventory\n", index + 1, (*targetFormats)[answer], prevItem, newAmount, storeString);
			break;

		case 2:
			cout << format("Successfully changed item {}'s {} from ${} to ${:.2f} inside {}'s inventory\n", index + 1, (*targetFormats)[answer], prevItem, newMoney, storeString);
		}
	}
	else {
		switch (answer) {
		case 0:
			cout << format("Successfully changed employee {}'s {} from {} to {}\n", index + 1, (*targetFormats)[answer], prevItem, newName);
			break;

		case 1:
			cout << format("Successfully changed item {}'s {} from {} to {}\n", index + 1, (*targetFormats)[answer], prevItem, newRole);
			break;

		case 2:
			cout << format("Successfully changed item {}'s {} from ${} to ${:.2f}\n", index + 1, (*targetFormats)[answer], prevItem, newMoney);
		}
	}

	cout << format("\nDetails:\n\n{}", displayItem);
}

// Handles deleting items from the inventories
static void deleteInventory(int& max) {

	int index{};

	cout << "Choose the ID of the item you want to delete: ";
	validateInput(index, 1, max);
	index -= 1;

	if (confirm(format("WARNING: You are about to delete item {} of {}'s inventory\nDo you wish to proceed?\n", index + 1, storeString))) {
		(*targetVec).erase((*targetVec).begin() + index);

		vectorToFile();
		cout << format("Successfully deleted item {} of {}'s inventory\n", index + 1, storeString);
	}
	else {
		cout << "Deletion cancelled\n";
	}

}

static void deleteEmployee(int& max) {

	int index{};

	cout << "Choose the ID of the item you want to delete: ";
	validateInput(index, 1, max);
	index -= 1;

	if (confirm(format("WARNING: You are about to delete item {} from list of employees\nDo you wish to proceed?\n", index + 1, storeString))) {
		(*targetVec).erase((*targetVec).begin() + index);

		vectorToFile();
		cout << format("Successfully deleted item {} from list of employees\n", index + 1);
	}
	else {
		cout << "Deletion cancelled\n";
	}

}

static void deleteAccount() {

	if (confirm("WARNING: You are about to delete your account. Do you wish to proceed?\n")) {

		for (int i = 0; i < accounts.size(); i++) {
			if (accounts[i][0] == username && accounts[i][1] == password) {
				accounts.erase(accounts.begin() + i);

				vectorToFile();
				cout << format("Your account {} has been successfully deleted\n", username);
				loggedIn = false;
			}
		}
	}
	else {
		cout << "Deletion cancelled\n";
	}

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
		(*targetVec).push_back({ name, to_string(amount), format("{:.2f}", (price)) });
		file << format("{}\n{}\n{}\n\n", name, amount, price);

		cout << format("\nSuccessfully added {} to {}'s inventory\n", name, storeString);
	}
	else {
		(*targetVec).push_back({ name, role, format("{:.2f}", (price)) });
		file << format("{}\n{}\n{}\n\n", name, role, price);

		cout << format("\nSuccessfully added {} to list of employees\n", name);
	}

}

// Handles all the selecting stuff in the "edit inventory" menu
static void editInventory() {

	int max{}, answer{};

	viewDetails();

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

//employee management
static void editEmployees() {
	
	int answer{}, employeeID{}, newAmount{}, index{}, max = employees.size();

	fileName = "employees.txt";
	targetVec = &employees;
	viewDetails();

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


// Checks if password uses best password practices
static string checkPassword(string& password) {

	cin >> password;

	while (password.length() < 12) {
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cout << "Error: password must be over 12 characters. Try again: ";
		checkPassword(password);
	}
	while (password.find_first_of("0123456789") == string::npos) {
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cout << "Error: password must contain at least one number. Try again: ";
		checkPassword(password);
	}
	while (password.find_first_of("`~!@#$%^&*()_-=+[]{}\\|;:'\"<,.>/?") == string::npos) {
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cout << "Error: password must contain at least one special character. Try again: ";
		checkPassword(password);
	}

	return password;

}

// The actual program, this needed to be in its own function so createFiles and fileToVector would only run once
static void program() {

	fstream file;

	int userChoice{};

	while (!loggedIn) {
		//log in or sign up
		cout << "\n\n===================Inventory Management System===================\n\n";
		cout << "1. Log in\n2. Sign up\n3. Exit\n\nSelect an option: ";
		validateInput(userChoice, 1, 3);

		switch (userChoice) {
		case 1:
			cout << "Please enter your username: ";
			cin >> username;
			cout << "Please enter your password: ";
			cin >> password;

			if (checkLogin(username, password)) {
				cout << "Login successful!\n";
				loggedIn = true;
			}
			else {
				cout << "Login failed. Please try again.\n";
				continue;
			}

			break;
			//add new account to list of accounts and export to text file
		case 2:

			cout << "Please choose a username: ";
			cin >> username;

			if (username.length() < 5) {
				cout << "Error: username must be over 5 characters";
				continue;
			}

			if (checkLogin(username, password, "create")) {
				cout << "Error: username already exists. Please pick a different one";
				continue;
			}

			//check password security
			cout << "Please enter a password (Password must be over 12 characters,\ncontain at least one number, and one special character): ";
			checkPassword(password);

			//save username and password without admin access
			accounts.push_back({ username, password, "false" });

			file.open("accounts.txt", ios::app);
			file << format("{}\n{}\n{}\n\n", username, password, "false");
			file.close();

			cout << "Successfully created account!";
			break;

		case 3:
			exit(0);

		}
	}

	while (loggedIn) {

		int userChoice{};

		cout << "===================Inventory Management System===================\n\n";
		if (adminAccount) {
			cout << "Logged in as admin\n\n";
			cout << "1. View and Edit Inventory\n2. View and Edit Employees\n3. Edit Roster\n4. Logout\n5. Exit";
			cout << "\n\nSelect an option: ";
			validateInput(userChoice, 1, 6);
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
				cout << "Edit roster"; // add and delete shifts from the roster vector, then export the vector to the text file
				break;

			case 4:
				loggedIn = false;
				adminAccount = false;
				break;

			case 5:
				exit(0);
				break;

			}
		}
		else {

			cout << "1. View inventory\n2. Order product\n3. Logout\n4. Delete account\n5. Exit\n\n";
			cout << "Select an option: ";
			validateInput(userChoice, 1, 4);

			switch (userChoice) {
			case 1:
				viewDetails();
				break;

			case 2:
				break;

			case 3:
				loggedIn = false;
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