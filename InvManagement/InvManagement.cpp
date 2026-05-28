
/* Latest changes
	- Improved file functionality
	- Added dynamic file names
	- You can now add items to inventories
	- Added extra username and password security
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

vector<vector<string>>* targetVec = &wellington;

string storeString{}, fileName, formats[3] = { "name", "amount", "price" }, employeeFormats[3] = { "name", "pay", "role" };

int storeNum{};
bool loggedIn = true;
bool adminAccount = true;

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
static void dynamicNaming(int index) {

	switch (index) {
	case 0:
		fileName = "wellington.txt";
		targetVec = &wellington;
		break;

	case 1:
		fileName = "christchurch.txt";
		targetVec = &christchurch;
		break;

	case 2:
		fileName = "auckland.txt";
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
		dynamicNaming(i);
		fstream file(fileName, ios::app);
		file.close();
	}

}

// Convert the text files to vectors
static void fileToVector() {

	fstream file;
	int index{};

	for (index = 0; index < vectorCount; index++) {

		dynamicNaming(index);

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

	switch (storeNum) {
	case 1:
		storeString = "Wellington";
		targetVec = &wellington;
		break;

	case 2:
		storeString = "Christchurch";
		targetVec = &christchurch;
		break;

	case 3:
		storeString = "Auckland";
		targetVec = &auckland;
		break;
	}

}

// Handles displaying the inventories
static void viewInventory() {

	pickStore();

	for (int i = 0; i < (*targetVec).size(); i++) {
		cout << format("Item {}\n", i + 1);
		for (int j = 0; j < (*targetVec)[i].size(); j++) {
			if (j == 2) {
				cout << format("{}: $", formats[j]);
			}
			else {
				cout << format("{}: ", formats[j]);
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
			value += format("{}: $", formats[i]);
		}
		else {
			value += format("{}: ", formats[i]);
		}
		value += format("{}\n", (*targetVec)[index][i]);
	}
	value += "\n";




	return value;
}
//code is 
// Handles the actual editing part of the inventories
static void editingStock(int& max) {

	int index{}, newValue{}, answer{};
	string newName{}, prevItem{}, displayItem{};

	cout << "Pick item by ID: ";
	validateInput(index, 1, max);
	index -= 1;

	displayItem = displaySpecificItem(displayItem, index);
	cout << displayItem;

	cout << "1. Edit name\n2. Edit amount\n3. Edit price\n\n";
	cout << "Select an option: ";
	validateInput(answer, 1, 3);
	answer -= 1;

	cout << format("Enter new {}: ", formats[answer]);
	if (answer == 0) {
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		getline(cin, newName);
	}
	else {
		validateInput(newValue);
	}


	prevItem = (*targetVec)[index][answer];
	if (answer == 0) {
		(*targetVec)[index][answer] = newName;
	}
	else {
		(*targetVec)[index][answer] = to_string(newValue);
	}

	//switch (storeNum) {
	//case 1:
	//	prevItem = wellington[index][answer];
	//	if (answer == 0) {
	//		wellington[index][answer] = newName;
	//	}
	//	else {
	//		wellington[index][answer] = to_string(newValue);
	//	}
	//	break;

	//case 2:
	//	prevItem = christchurch[index][answer];
	//	if (answer == 0) {
	//		christchurch[index][answer] = newName;
	//	}
	//	else {
	//		christchurch[index][answer] = to_string(newValue);
	//	}
	//	break;

	//case 3:
	//	prevItem = auckland[index][answer];
	//	if (answer == 0) {
	//		auckland[index][answer] = newName;
	//	}
	//	else {
	//		auckland[index][answer] = to_string(newValue);
	//	}
	//	break;
	//}

	displayItem = displaySpecificItem(displayItem, index);

	if (answer == 2) {
		cout << format("Successfully changed item {}'s {} from ${} to ${} inside {}'s inventory\n", index + 1, formats[answer], prevItem, newValue, storeString);
	}
	else {
		cout << format("Successfully changed item {}'s {} from {} ", index + 1, formats[answer], prevItem);
		if (answer == 0) {
			cout << format("to {} ", newName);
		}
		else {
			cout << format("to {} ", newValue);
		}
		cout << format("inside {}'s inventory", storeString);
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

		//switch (storeNum) {
		//case 1:
		//	wellington.erase(wellington.begin() + index);
		//	break;

		//case 2:
		//	christchurch.erase(christchurch.begin() + index);
		//	break;

		//case 3:
		//	auckland.erase(auckland.begin() + index);
		//	break;
		//}

		cout << format("Successfully deleted item {} of {}'s inventory\n", index + 1, storeString);
	}
	else {
		cout << "Deletion cancelled\n";
	}



}

// Handles adding an item to an inventory
static void addItem() {

	string name{};
	int amount{};
	double price{};
	fstream file;

	//for (int i = 0; i < (sizeof(formats) / sizeof(formats[0]) +1); i++) {
	//	dynamicNaming(i);
	//	cout << format("Enter {}: ", formats[i]);
	//	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	//	cin >> formats[i];
	//}

	cout << "Enter a name: ";
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	getline(cin, name);

	cout << "Enter an amount: ";
	validateInput(amount, 1);

	cout << "Enter a price: ";
	validateInput(price, 1);

	(*targetVec).push_back({name, to_string(amount), format("{:.2f}", (price))});

	file.open(fileName, ios::app);
	file << format("{}\n{}\n{}\n\n", name, amount, price);

	//switch (storeNum) {
	//case 1:
	//	wellington.push_back({ name, to_string(amount), format("{:.2f}", (price)) });

	//	file.open("wellington.txt", ios::app);
	//	file << format("{}\n{}\n{}\n\n", name, amount, price);
	//	break;

	//case 2:
	//	christchurch.push_back({ name, to_string(amount), format("{:.2f}", (price)) });

	//	file.open("christchurch.txt", ios::app);
	//	file << format("{}\n{}\n{}\n\n", name, amount, price);
	//	break;

	//case 3:
	//	auckland.push_back({ name, to_string(amount), format("{:.2f}", (price)) });

	//	file.open("auckland.txt", ios::app);
	//	file << format("{}\n{}\n{}\n\n", name, amount, price);
	//	break;
	//}

	cout << format("Successfully added {} to {}'s inventory\n", name, storeString);

}

// Handles all the selecting stuff in the "edit inventory" menu
static void editInventory() {

	int max{}, answer{};

	viewInventory();

	cout << "1. Add item\n2. Edit item\n3. Delete item\n\n";
	cout << "Please choose an option: ";
	validateInput(answer, 1, 3);

	max = (*targetVec).size();

	//switch (storeNum) {
	//case 1:
	//	max = wellington.size();
	//	break;

	//case 2:
	//	max = christchurch.size();
	//	break;

	//case 3:
	//	max = auckland.size();
	//	break;
	//}

	switch (answer) {
	case 1:
		addItem();
		break;

	case 2:
		editingStock(max);
		break;

	case 3:
		deleteInventory(max);
	}
}

//employee management
static void editEmployees() {
	
	int answer{}, employeeID{}, newValue{}, index{}, max = employees.size();
	double pay{};
	string name{}, role{}, newName{}, prevItem{}, displayItem{};;
	fstream file;
	cout << "\n\n";

	//display all employees 

	for (int i = 0; i < employees.size(); i++) {
		cout << format("Employee {}\n", i + 1);
		for (int j = 0; j < employees[i].size(); j++) {
			if (j == 1) {
				cout << format("{}: $", employeeFormats[j]);
			}
			else {
				cout << format("{}: ", employeeFormats[j]);
			}
			cout << format("{}\n", employees[i][j]);
		}
		cout << "\n";
	}



	cout << "1. Add employee\n2. Edit employee\n3. Delete employee\n\n";
	cout << "Please choose an option: ";
	validateInput(answer, 1, 3);

	switch (answer) {
	case 1:
		//add employee to employees vector, then export to text file
		cout << "Enter employee name: ";
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		getline(cin, name);

		cout << "Enter pay per hour: ";
		validateInput(pay, 1);

		cout << "Enter employee role: ";
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		getline(cin, role);

		employees.push_back({ name, to_string(pay), role});

		file.open("employees.txt", ios::app);
		file << format("{}\n{}\n{}\n\n", name, format("{:.2f}", (pay)), role);
		break;

	case 2:
		//edit employee from employees vector, then export to text file
		cout << "Pick employee by ID: ";
		validateInput(index, 1, max);
		index -= 1;

		break;

	case 3:
		
		break;
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
	string username{}, password{};

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
			cout << "1. View Inventory\n2. Edit Inventory\n3. Edit Employees\n4. Edit Roster\n5. Logout\n6. Exit";
			cout << "\n\nSelect an option: ";
			validateInput(userChoice, 1, 6);
			switch (userChoice) {

			case 1:
				viewInventory();
				break;


			case 2:
				editInventory();
				break;

			case 3:
				// add and delete employees from the accounts vector, then export the vector to the text file
				editEmployees(); 
				break;

			case 4:
				cout << "Edit roster"; // add and delete shifts from the roster vector, then export the vector to the text file
				break;

			case 5:
				loggedIn = false;
				adminAccount = false;
				break;

			case 6:
				exit(0);
				break;

			}
		}
		else {

			cout << "1. View inventory\n2. Order product\n3. Logout\n4. Exit\n\n";
			cout << "Select an option: ";
			validateInput(userChoice, 1, 4);

			switch (userChoice) {
			case 1:
				viewInventory();
				break;

			case 2:
				break;

			case 3:
				loggedIn = false;
				break;

			case 4:
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