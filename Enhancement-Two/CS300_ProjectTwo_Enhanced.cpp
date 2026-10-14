/*
 * ProjectTwo.cpp
 *
 *  Created on: Oct 18, 2025
 *      Author: Jack Markham
 */

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

// Course record

struct Course {
	string number;
	string title;
	vector<string> prerequisites;
};

static inline string trim(const string& s) {
	size_t i = 0, j = s.size();
	while (i < j && isspace(static_cast<unsigned char>(s[i]))) ++i;
	while (j > i && isspace(static_cast<unsigned char>(s[j]))) --j;
	return s.substr(i, j - i);
}

static inline string toUpperCopy(string s) {
	for (char& c : s) c = static_cast<char>(toupper(static_cast<unsigned char>(c)));
	return s;
}

// HashTable with separate chaining

class HashTable {
public:
	explicit HashTable(size_t cap = 199) : buckets(cap) {}

	vector<string> getAllKeys() const { return keys; }

	void clear() {
		for (auto& b : buckets) b.clear();
		keys.clear();
	}

	void insert(const Course& c) {
		size_t idx = hashKey(c.number);
		for (auto& item : buckets[idx]) {
			if (sameKey(item.number, c.number)) {
				item = c;
				return;
			}
		}
		buckets[idx].push_back(c);
		keys.push_back(c.number);
	}

	bool find(const string& courseNo, Course& out) const {
		size_t idx = hashKey(courseNo);
		for (const auto& item : buckets[idx]) {
			if (sameKey(item.number, courseNo)) {
				out = item;
				return true;
			}
		}
		return false;
	}

private:
	vector<list<Course>> buckets;
	vector<string> keys;

	size_t hashKey(const string& keyRaw) const {
		string key = toUpperCopy(keyRaw);
		unsigned long long h = 0, p = 131;
		for (char c : key) {
			h = h * p + static_cast<unsigned char>(c);
		}
		return static_cast<size_t>(h % buckets.size());
	}

	static bool sameKey(const string& a, const string& b) {
		return toUpperCopy(a) == toUpperCopy(b);
	}
};

// Loader helpers

static vector<string> splitCSV(const string& line) {
	vector<string> out;
	string token;
	stringstream ss(line);
	while (getline(ss, token, ',')) {
		out.push_back(trim(token));
	}
	return out;
}

static string addCourseFromFields(const vector<string>& fields, HashTable& table) {
	if (fields.size() < 2)
		return "Each row needs at least courseNumber and courseTitle.";

	if (trim(fields[0]).empty())
		return "Course number cannot be empty.";

	if (trim(fields[1]).empty())
		return "Course title cannot be empty.";

	Course c;
	c.number = fields[0];
	c.title = fields[1];

	for (size_t i = 2; i < fields.size(); ++i) {
		if (!fields[i].empty())
			c.prerequisites.push_back(fields[i]);
	}

	table.insert(c);
	return "";
}

// Check that every prerequisite refers to a course that exists

static bool validatePrerequisites(const HashTable& table, string& errorOut) {
	vector<string> ids = table.getAllKeys();

	for (const auto& id : ids) {
		Course c;

		if (table.find(id, c)) {
			for (const auto& prereq : c.prerequisites) {
				Course prereqCourse;

				if (!table.find(prereq, prereqCourse)) {
					errorOut = "Course " + c.number +
						" has an invalid prerequisite: " + prereq;
					return false;
				}
			}
		}
	}

	return true;
}

static bool loadFromFile(const string& filename, HashTable& table, string& errorOut) {
	ifstream file(filename.c_str());

	if (!file.is_open()) {
		errorOut = "Unable to open file: " + filename;
		return false;
	}

	table.clear();
	string line;
	size_t lineNo = 0;

	while (getline(file, line)) {
		++lineNo;
		string raw = trim(line);

		if (raw.empty() || raw[0] == '#') continue;

		vector<string> fields = splitCSV(raw);
		string err = addCourseFromFields(fields, table);

		if (!err.empty()) {
			table.clear();
			ostringstream oss;
			oss << "Parse error on line " << lineNo << ": " << err;
			errorOut = oss.str();
			return false;
		}
	}

	// Make sure all prerequisites exist after the courses are loaded
	if (!validatePrerequisites(table, errorOut)) {
		table.clear();
		return false;
	}

	return true;
}

static void loadSampleData(HashTable& table) {
	table.clear();

	vector<string> r;

	r.clear(); r.push_back("CS100"); r.push_back("Intro to CS");
	addCourseFromFields(r, table);

	r.clear(); r.push_back("CS200"); r.push_back("Data Structures"); r.push_back("CS100");
	addCourseFromFields(r, table);

	r.clear(); r.push_back("CS250"); r.push_back("Algorithms 1"); r.push_back("CS200");
	addCourseFromFields(r, table);

	r.clear(); r.push_back("CS260"); r.push_back("Computer Architecture"); r.push_back("CS200");
	addCourseFromFields(r, table);

	r.clear(); r.push_back("CS300"); r.push_back("Advanced Algorithms"); r.push_back("CS250");
	addCourseFromFields(r, table);

	r.clear(); r.push_back("MATH210"); r.push_back("Discrete Math");
	addCourseFromFields(r, table);
}

// Printing helpers

static void printCourse(const Course& c) {
	cout << c.number << ", " << c.title << '\n';
	cout << "Prerequisites: ";

	if (c.prerequisites.empty()) {
		cout << "None\n";
	}
	else {
		for (size_t i = 0; i < c.prerequisites.size(); ++i) {
			cout << c.prerequisites[i];
			if (i + 1 < c.prerequisites.size()) cout << ", ";
		}
		cout << '\n';
	}
}

// Follow the complete prerequisite chain

static void printPrerequisiteChain(
	const HashTable& table,
	const string& courseNumber,
	vector<string>& visited,
	int level = 0) {

	string current = toUpperCopy(courseNumber);

	// Stop if a circular prerequisite is found
	if (find(visited.begin(), visited.end(), current) != visited.end()) {
		cout << string(level * 2, ' ')
			<< "Circular prerequisite detected: "
			<< courseNumber << '\n';
		return;
	}

	visited.push_back(current);

	Course course;

	if (!table.find(courseNumber, course)) {
		cout << string(level * 2, ' ')
			<< courseNumber << " (course not found)\n";
		visited.pop_back();
		return;
	}

	for (const string& prereq : course.prerequisites) {
		Course prereqCourse;

		cout << string(level * 2, ' ');

		if (table.find(prereq, prereqCourse)) {
			cout << prereqCourse.number << ", "
				<< prereqCourse.title << '\n';

			printPrerequisiteChain(table, prereq, visited, level + 1);
		}
		else {
			cout << prereq << " (course not found)\n";
		}
	}

	visited.pop_back();
}

static void printCourseListAlpha(const HashTable& table) {
	vector<string> ids = table.getAllKeys();

	if (ids.empty()) {
		cout << "No courses loaded yet.\n";
		return;
	}

	sort(ids.begin(), ids.end(),
		[](const string& a, const string& b) {
			return toUpperCopy(a) < toUpperCopy(b);
		});

	cout << "Course List (Alphanumeric):\n";

	for (const auto& id : ids) {
		Course c;

		if (table.find(id, c)) {
			cout << c.number << ", " << c.title << '\n';
		}
	}
}

// Menu & Program

static void printMenu() {
	cout << "\n======= Advising Assistance ======\n";
	cout << "1. Load Data Structure\n";
	cout << "2. Print Course List\n";
	cout << "3. Print Course\n";
	cout << "9. Exit\n";
	cout << "Choose an option: ";
}

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	HashTable table(199);
	bool loaded = false;

	while (true) {
		printMenu();
		string choice;

		if (!getline(cin, choice)) break;

		if (choice == "1") {
			cout << "Enter filename or type SAMPLE: ";
			string filename;

			if (!getline(cin, filename)) {
				cout << "Input error. Try again.\n";
				continue;
			}

			filename = trim(filename);

			if (filename.empty()) {
				cout << "Error: filename cannot be empty.\n";
				continue;
			}

			if (toUpperCopy(filename) == "SAMPLE") {
				loadSampleData(table);
				loaded = true;
				cout << "Loaded built-in sample data.\n";
			}
			else {
				string err;

				if (loadFromFile(filename, table, err)) {
					loaded = true;
					cout << "Data loaded successfully from '" << filename << "'.\n";
				}
				else {
					loaded = false;
					cout << "Load failed: " << err << "\n";
				}
			}

		}
		else if (choice == "2") {

			if (!loaded) {
				cout << "Please load the data structure first.\n";
				continue;
			}

			printCourseListAlpha(table);

		}
		else if (choice == "3") {

			if (!loaded) {
				cout << "Please load the data structure first.\n";
				continue;
			}

			cout << "Enter a course number: ";
			string query;

			if (!getline(cin, query)) {
				cout << "Input error. Try again.\n";
				continue;
			}

			query = trim(query);

			if (query.empty()) {
				cout << "Error: course number cannot be empty.\n";
				continue;
			}

			Course c;

			if (table.find(query, c)) {
				printCourse(c);

				cout << "\nFull Prerequisite Chain:\n";

				if (c.prerequisites.empty()) {
					cout << "None\n";
				}
				else {
					vector<string> visited;
					printPrerequisiteChain(table, c.number, visited);
				}

			}
			else {
				cout << "Course '" << query
					<< "' was not found. Check the course number and try again.\n";
			}

		}
		else if (choice == "9") {

			cout << "Goodbye!\n";
			break;

		}
		else {

			cout << "Invalid option. Please choose 1, 2, 3, or 9.\n";
		}
	}

	return 0;
}

