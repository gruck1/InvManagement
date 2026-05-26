
/* Latest changes
	- Formatted the vector printing better
	- You can now edit name, amount and price with proper formatting
	- GRUCK IF YOU CAN SEE THIS THE GITHUB TEST WORKED
*/

//hello daniel
// THIS IS SECOND EPIC TEST
//im grucking it

#include <iostream>
#include <format>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

vector<vector<string>>wellington = { {} };
vector<vector<string>>christchurch = { {} };
vector<vector<string>>auckland = { {} };
vector<vector<string>>accounts = { {} };

string username{}, password{}, formats[3] = { "name", "amount", "price" };

int userChoice{}, store{};
bool loggedIn = true; // Testing purposes
bool adminAccount = true; // Testing purposes

static int validateInput(auto& validator) {

	cin >> validator;

	while (cin.fail()) {
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cout << "Please input a valid integer: ";
		cin >> validator;
	}

	return validator;

}

// If no files exist, create them
static void createFiles() {

	fstream file1("wellington.txt", ios::app);
	fstream file2("christchurch.txt", ios::app);
	fstream file3("auckland.txt", ios::app);
	fstream file4("accounts.txt", ios::app);

}

// Convert the text files to vectors
static void fileToVector() {

	fstream file;
	int index{};

	/*
	The index opens the correct file and goes to the next number once everything is imported successfully.
	For example: index starts at 0 so wellington text file is edited. When everything is successfully imported into the wellington vector,
	the wellington text file is closed and index goes to the next number to open the next text file
	*/
	for (index = 0; index < 4; index++) {
		switch (index) {
		case 0:
			file.open("wellington.txt", ios::in);
			break;

		case 1:
			file.open("christchurch.txt", ios::in);
			break;

		case 2:
			file.open("auckland.txt", ios::in);
			break;

		case 3:
			file.open("accounts.txt", ios::in);
			if (file.is_open()) {
				//cout << "shlooby\n";
			}
			break;
		}

		string line{};
		int i{};

		/*
		i starts at 0, so the first nest will be edited. Once the first nest is full, create empty nest (vector.push_back({}) and change i by 1 so
		it can start editing the next nest without errors.
		If the new nest doesn't get created, it breaks because it can't access i.
		Example:
			vector = { {a, b, c } };
			i = 0;

			vector[i].push_back(d); the vector now looks like { {a, b, c, d} }

			change i by 1
			vector[i].push_back(e); vector = { {a, b, c, d}, i wants to be here };

		i is in the next nest but it doesn't exist so the code fails. With an empty nest created, this error won't happen.
		Example:
			vector = { {a, b, c } };
			i = 0;

			vector[i].push_back(d); the vector now looks like { {a, b, c, d} }

			vector.push_back({}); creating empty nest - vector now looks like { {a, b, c, d}, {} }

			change i by 1
			vector[i].push_back(e); vector = { {a, b, c, d}, {e} };

		Now it works because there is a new nest.
		*/
		while (getline(file, line)) {
			switch (index) {
			case 0:
				if (line == "") {
					wellington.push_back({});
					i++;
				}
				else {
					wellington[i].push_back(line);
				}
				break;

			case 1:
				if (line == "") {
					christchurch.push_back({});
					i++;
				}
				else {
					christchurch[i].push_back(line);
				}
				break;

			case 2:
				if (line == "") {
					auckland.push_back({});
					i++;
				}
				else {
					auckland[i].push_back(line);
				}
				break;

			case 3:
				if (line == "") {
					accounts.push_back({});
					i++;
				}
				else {
					accounts[i].push_back(line);
				}
				break;
			}

		}
		file.close();
	}
}
//test
//check if the username and password match any of the accounts in the accounts vector, if it does return true, if not return false
//also give the user admin access if the account is an admin account
static bool checkLogin(string& username, string& password) {
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

static void pickStore() {

	cout << "1. Wellington\n2. Christchurch\n3. Auckland\n\n";
	cout << "Please choose a store: ";
	validateInput(store);

}

static void viewInventory() {

	pickStore();

	switch (store) {
	case 1:
		for (int i = 0; i < wellington.size(); i++) {
			cout << format("Item {}\n", i + 1);
			for (int j = 0; j < wellington[i].size(); j++) {
				if (j == 2) {
					cout << format("{}: $", formats[j]);
				}
				else {
					cout << format("{}: ", formats[j]);
				}
				cout << format("{}\n", wellington[i][j]);
			}
			cout << "\n";
		}
		break;

	case 2:
		for (int i = 0; i < christchurch.size(); i++) {
			cout << format("Item {}\n", i + 1);
			for (int j = 0; j < christchurch[i].size(); j++) {
				if (j == 2) {
					cout << format("{}: $", formats[j]);
				}
				else {
					cout << format("{}: ", formats[j]);
				}
				cout << format("{}\n", christchurch[i][j]);
			}
			cout << "\n";
		}
		break;

	case 3:
		for (int i = 0; i < auckland.size(); i++) {
			cout << format("Item {}\n", i + 1);
			for (int j = 0; j < auckland[i].size(); j++) {
				if (j == 2) {
					cout << format("{}: $", formats[j]);
				}
				else {
					cout << format("{}: ", formats[j]);
				}
				cout << format("{}\n", auckland[i][j]);
			}
			cout << "\n";
		}
		break;
	}
}

static void editInventory() {

	int index{}, answer{}, newValue{};
	string newName{}, prevItem{};

	viewInventory();

	cout << "Pick item by ID: ";
	validateInput(index);
	index -= 1;

	switch (store) {
	case 1:
		cout << format("Item {}\n", index + 1);
		for (int i = 0; i < wellington[index].size(); i++) {
			if (i == 2) {
				cout << format("{}: $", formats[i]);
			}
			else {
				cout << format("{}: ", formats[i]);
			}
			cout << format("{}\n", wellington[index][i]);
		}
		cout << "\n";
		break;

	case 2:
		cout << format("Item {}\n", index + 1);
		for (int i = 0; i < christchurch[index].size(); i++) {
			if (i == 2) {
				cout << format("{}: $", formats[i]);
			}
			else {
				cout << format("{}: ", formats[i]);
			}
			cout << format("{}\n", christchurch[index][i]);
		}
		cout << "\n";
		break;

	case 3:
		cout << format("Item {}\n", index + 1);
		for (int i = 0; i < auckland[index].size(); i++) {
			if (i == 2) {
				cout << format("{}: $", formats[i]);
			}
			else {
				cout << format("{}: ", formats[i]);
			}
			cout << format("{}\n", auckland[index][i]);
		}
		cout << "\n";
		break;
	}

	cout << "1. Edit name\n2. Edit amount\n3. Edit price\n4. Delete item\n\n";
	cout << "Select an option: ";
	validateInput(answer);
	answer -= 1;

	switch (answer) {
	case 0:
	case 1:
	case 2:

		cout << format("Enter new {}: ", formats[answer]);
		if (answer == 0) {
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			getline(cin, newName);
		}
		else {
			validateInput(newValue);
		}

		switch (store) {
		case 1:
			prevItem = wellington[index][answer];
			wellington[index][answer] = newName;
			break;

		case 2:
			prevItem = christchurch[index][answer];
			christchurch[index][answer] = to_string(newValue);
			break;

		case 3:
			prevItem = auckland[index][answer];
			auckland[index][answer] = to_string(newValue);
			break;
		}

		if (answer == 2) {
			cout << format("Successfully changed item {}'s {} from ${}  to ${}\n", index + 1, formats[answer], prevItem, newValue);
		}
		else {
			cout << format("Successfully changed item {}'s {} from {} ", index + 1, formats[answer], prevItem);
			if (answer == 0) {
				cout << format("to {}\n", newName);
			}
			else {
				cout << format("to {}\n", newValue);
			}
		}
		break;

	case 3:
		break;

	}
}



int main() {

	createFiles();
	if (wellington.size() == 1) {
		fileToVector();
	}

	fstream file;
	while (!loggedIn) {
		//log in or sign up
		cout << "\n\n===================Inventory Management System===================\n\n";
		cout << "1. Log in\n2. Sign up\n3. Exit\n\nSelect an option: ";
		validateInput(userChoice);

		switch (userChoice) {
		case 1:
			cout << "Username: ";
			cin >> username;
			cout << "\n";
			cout << "Password: ";
			cin >> password;
			cout << "\n";

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
			cout << "Username: ";
			cin >> username;
			cout << "Password: ";
			cin >> password;
			accounts.push_back({ username, password, "false" });
			file.open("accounts.txt", ios::app);
			file << format("{}\n{}\n{}\n\n", username, password, "false");
			file.close();
			break;

		case 3:
			exit(0);

		default:
			cout << "Invalid option. Please try again.\n";
			break;

		}
	}

	while (loggedIn) {
		cout << "===================Inventory Management System===================\n\n";
		if (adminAccount) {
			cout << "1. View Inventory\n2. Edit Inventory\n3. Edit Employees\n4. Edit Roster\n5. Logout\n6. Exit";
			cout << "\n\nSelect an option: ";
			validateInput(userChoice);
			switch (userChoice) {

			case 1:
				viewInventory();
				break;


			case 2:
				editInventory();
				break;

			case 3:
				cout << "Edit employees"; // add and delete employees from the accounts vector, then export the vector to the text file
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

			default:
				cout << "Invalid option. Please try again.\n";
				break;
			}
		}
		else {
			cout << "1. View inventory\n2. Order product\n3. Logout\n4. Exit\n\n";
		}

		main();






















		//	for (const auto& nest : wellington) {
		//		for (const auto& item : nest) {
		//			cout << item << " ";
		//		}
		//		cout << "\n";
		//	}
		//	cout << "\n";

		//	for (const auto& nest : christchurch) {
		//		for (const auto& item : nest) {
		//			cout << item << " ";
		//		}
		//		cout << "\n";
		//	}
		//	cout << "\n";

		//	for (const auto& nest : auckland) {
		//		for (const auto& item : nest) {
		//			cout << item << " ";
		//		}
		//		cout << "\n";
		//	}
		//	cout << "\n";
	}
}