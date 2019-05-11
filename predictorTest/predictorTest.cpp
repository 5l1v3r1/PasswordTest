#include "stdafx.h"
#include<iostream>
#include<string>
#include<vector>
#include<algorithm>
#include<utility>
#include<fstream>
#include<filesystem>
#include<thread>
//Myheaders
#include"FileHandling.h"
#include"DataCleansing.h"


using namespace std;


//Global Vectors
vector<string> names;//vecor to store all common name list
vector<string> DOB;//vector to store all DOB combination
vector<string> locations;//vector to store all locations list
vector<string> commons;//vector to store all commons list
vector<string> raw_Data_Vector;//temporary data vector
vector<string> passwords;//vector to store passwords of single files
vector<string> emails;//vector to store emails of single files
vector<string> email_Names;//vector to store email_names extracted from emails vector
vector<string> websites;//vector to store websites extracted from emails vector
vector<string> mobiles;//vector to store mobile number list
vector<string> pattern_Strings_Vector;
struct pattern_Structure {
	string key;
	string value;
	int len;
	int loc;
};
vector<pattern_Structure> results;
vector<vector<pattern_Structure>> results_splits;


//Global iterators
vector<string>::iterator raw_Vector_iterator;

//will sort a vector of structure on length then location
void sort_Vector_Struct_OnLength(vector<pattern_Structure>& x) {
	sort(x.begin(), x.end(), [](pattern_Structure a, pattern_Structure b) -> bool
	{
		if (a.len != b.len) {
			return a.len > b.len;
		}
		else {
			return a.loc < b.loc;
		}
	});
}

//will sort a vector of structure on location then length 
void sort_Vector_Struct_OnLocation(vector<pattern_Structure>& x) {
	sort(x.begin(), x.end(), [](pattern_Structure a, pattern_Structure b) -> bool
		{
			if (a.loc != b.loc) {
				return a.loc < b.loc;
			}
			else {
				return a.len > b.len;
			}
		});
}

//Will extract passwords and emails seperately
void extract_Email_and_Passwrod(string delimeter) {
	size_t found, foundDelimeter;
	for (auto it = raw_Data_Vector.begin(); it != raw_Data_Vector.end(); it++) {
		found = it->find("@");
		if (found != string::npos) {
			foundDelimeter = it->find(delimeter, found + 1);
			if (foundDelimeter != string::npos) {
				emails.push_back(it->substr(0, foundDelimeter));
				passwords.push_back(it->substr(foundDelimeter + 1, it->length()-1));
				continue;
			}
		}
	}
}

//extract mail_Name and Website from email part
void extract_Email_Names_and_Website() {
	size_t found;
	for (auto it = emails.begin(); it != emails.end(); it++) {
		found = it->find("@");
		if (found != string::npos) {
			if (found != string::npos) {
				email_Names.push_back(it->substr(0, found));
				websites.push_back(it->substr(found + 1, it->length() - 1));
				continue;
			}
		}
	}
}

//checks if Name exists in Password
void name_List_Comparison(string pass) {
	vector<string>::iterator it = names.begin();
	while (it != names.end()) {
		size_t found = pass.find(*it);
		if (found != string::npos) {
			results.push_back({ "Name/", *it, static_cast<int>(it->length()), static_cast<int>(found)});
		}
		it++;
	}
}

//checks if email_name exists in Password
void email_Name_List_Comparison(string pass, vector<string>::iterator pass_it) {
	string temp = email_Names.at(distance(passwords.begin(), pass_it));
	size_t found = pass.find(temp);
	if (found != string::npos) {
		results.push_back({ "email_Name/", temp, static_cast<int>(temp.length()), static_cast<int>(found) });
	}
}

//checks if website exists in Password
void website_List_Comparison(string pass, vector<string>::iterator pass_it) {
	string temp = websites.at(distance(passwords.begin(), pass_it));
	size_t found = pass.find(temp);
	if (found != string::npos) {
		results.push_back({ "Website/", temp, static_cast<int>(temp.length()), static_cast<int>(found) });
	}
}

//checks if DOB exists in Password
void dob_List_Comparison(string pass) {
	vector<string>::iterator it = DOB.begin();
	while (it != DOB.end()) {
		size_t found = pass.find(*it);
		if (found != string::npos) {
			results.push_back({ "DOB/", *it, static_cast<int>(it->length()), static_cast<int>(found) });		
		}
		it++;
	}
}

//checks if location exists in Password
void location_List_Comparison(string pass) {
	vector<string>::iterator it = locations.begin();
	while (it != locations.end()) {
		size_t found = pass.find(*it);
		if (found != string::npos) {
			results.push_back({ "Location/", *it, static_cast<int>(it->length()), static_cast<int>(found) });
		}
		it++;
	}
}

//checks if Mobile Number exists in Password

//checks if Common words exists in Password
void common_Word_List_Comparison(string pass) {
	vector<string>::iterator it = commons.begin();
	string rawS1, rawS2;
	bool flag1 = false, flag2 = false, flag3 = false;
	while (it != commons.end()) {
		size_t found = pass.find(*it);
		if (found != string::npos && !flag1) {
			results.push_back({ "Common1/", *it, static_cast<int>(it->length()), static_cast<int>(found) });
			rawS1 = pass.substr(0, found);
			rawS2 = pass.substr(found + it->length(), pass.length() - 1);
			found = string::npos;
			flag1 = true;
		}
		found = rawS1.find(*it);
		if (found != string::npos && (flag1 && !flag2)) {
			results.push_back({ "Common2/", *it, static_cast<int>(it->length()), static_cast<int>(found) });
			found = string::npos;
			flag2 = true;
		}
		found = rawS2.find(*it);
		if ((found != string::npos && flag1) && (flag2 && !flag3)) {
			results.push_back({ "Common3/", *it, static_cast<int>(it->length()), static_cast<int>(found) });
			flag3 = true;
		}
		it++;
	}
	flag1 = false; flag2 = false; flag3 = false;
}

//will add undetected part into splited results
void undetected(string& p) {

	if (results_splits.size() != 0) {
		int p_size = p.length();
		vector<pattern_Structure> x_i_temp;
		for (size_t i = 0; i < results_splits.size(); i++) {
			int x_i_size = results_splits[i].size();
			if (results_splits[i][0].loc != 0) {
				x_i_temp.push_back({ "Undetected/", p.substr(0, results_splits[i][0].loc), results_splits[i][0].loc, 0 });
			}
			for (int j = 0; j < x_i_size - 1; j++) {
				if ((results_splits[i][j + 1].loc - (results_splits[i][j].loc + results_splits[i][j].len - 1) - 1) > 0) {
					x_i_temp.push_back({ "Undetected/", p.substr((results_splits[i][j].loc + results_splits[i][j].len), (results_splits[i][j + 1].loc - (results_splits[i][j].loc + results_splits[i][j].len - 1) - 1)), (results_splits[i][j + 1].loc - (results_splits[i][j].loc + results_splits[i][j].len - 1) - 1), (results_splits[i][j].loc + results_splits[i][j].len) });
				}
			}
			if ((results_splits[i][x_i_size - 1].loc + results_splits[i][x_i_size - 1].len) != p_size) {
				x_i_temp.push_back({ "Undetected/", p.substr((results_splits[i][x_i_size - 1].loc + results_splits[i][x_i_size - 1].len), (p_size - 1 - (results_splits[i][x_i_size - 1].loc + results_splits[i][x_i_size - 1].len))), (p_size - 1 - (results_splits[i][x_i_size - 1].loc + results_splits[i][x_i_size - 1].len)), (results_splits[i][x_i_size - 1].loc + results_splits[i][x_i_size - 1].len) });
			}
			results_splits[i].insert(results_splits[i].end(), x_i_temp.begin(), x_i_temp.end());
			x_i_temp.clear(); x_i_temp.shrink_to_fit();
		}
	}
}

//will devide overlapping patterns in results
void dividePatterns(vector<pattern_Structure>& x, string& p) {
	vector<pattern_Structure> temp;
	if (x.size() != 0) {
		for (size_t i = 0; i < x.size() - 1; ++i) {
			for (size_t j = i + 1; j < x.size(); ++j) {
				if ((x[j].loc >= x[i].loc) && (x[j].loc <= (x[i].loc + x[i].len - 1))) {
					temp.push_back(x[j]);
					x.erase(x.begin() + j);
					j--;
				}
			}
		}
		if (temp.size() == 0) {
			results_splits.push_back(x);
		}
		else {
			results_splits.push_back(x);
			return dividePatterns(temp, p);
		}
		undetected(p);
	}
	else
	{
		x.push_back({ "Undetected/", p, int(p.length()), 0 });
		results_splits.push_back(x);
	}
}



//Generate pattern by bool variables
string patterns(string pass) {
	string final_pattern, value_pattern;
	sort_Vector_Struct_OnLength(results);
	dividePatterns(results ,pass);
	results.clear(); results.shrink_to_fit();
	
	for (size_t i = 0; i < results_splits.size(); i++) {
		sort_Vector_Struct_OnLocation(results_splits[i]);
	}
	final_pattern += "Password: " + pass + "\n";
	for (size_t i = 0; i < results_splits.size(); i++)
	{

		final_pattern += "\t\t\tPattern" + to_string(i + 1) + ": "; 
		value_pattern += "\t\t\tValues" + to_string(i + 1) + ": ";
		for (size_t j = 0; j < results_splits[i].size(); j++)
		{
			final_pattern += results_splits[i][j].key;
			value_pattern += "\"" + results_splits[i][j].value + "\"" + " Length:" + to_string(results_splits[i][j].len) + " Position:" + to_string(results_splits[i][j].loc) + ", ";
		}
		final_pattern += "\n"; 
		value_pattern += "\n";
	}
	final_pattern += value_pattern; 
	for (size_t i = 0; i < results_splits.size(); i++) {
		results_splits[i].clear(); results_splits[i].shrink_to_fit();
	}
	results_splits.clear(); results_splits.shrink_to_fit();
	return final_pattern;
}

void programFlow() {

	FileHandling f;
	DataCleansing d;
	vector<string> filePaths;
	f.getAllFilesRecursive(filePaths, "Input/");
	vector<string>::iterator file = filePaths.begin();

	f.readFile(names, "datafiles/names.txt");
	f.readFile(DOB, "datafiles/dobs.txt");
	f.readFile(locations, "datafiles/locations.txt");
	f.readFile(commons, "datafiles/Common.txt");
	cout << "\n Names, Dob, Locations and Commons files are readed \n";

	while (file != filePaths.end()) {
		cout << "\n" << " Starting file:" << *file << " \n";
		f.readFile(raw_Data_Vector, *file);
		d.lowerCase(raw_Data_Vector);
		extract_Email_and_Passwrod(":");
		f.writeFile(emails, "Output/emails.txt");
		f.writeFile(passwords, "Output/paswords.txt");
		raw_Data_Vector.clear(); raw_Data_Vector.shrink_to_fit();
		extract_Email_Names_and_Website();
		f.writeFile(email_Names, "Output/email_names.txt");
		f.writeFile(websites, "Output/websites.txt");
		raw_Vector_iterator = passwords.begin();
		while (raw_Vector_iterator != passwords.end())
		{
			name_List_Comparison(*raw_Vector_iterator);
			email_Name_List_Comparison(*raw_Vector_iterator, raw_Vector_iterator);
			website_List_Comparison(*raw_Vector_iterator, raw_Vector_iterator);
			dob_List_Comparison(*raw_Vector_iterator);
			location_List_Comparison(*raw_Vector_iterator);
			common_Word_List_Comparison(*raw_Vector_iterator);
			pattern_Strings_Vector.push_back(patterns(*raw_Vector_iterator));

			raw_Vector_iterator++;
		}
		f.writeFile(pattern_Strings_Vector, "Output/0.txt");
		pattern_Strings_Vector.clear(); pattern_Strings_Vector.shrink_to_fit();
		passwords.clear(); passwords.shrink_to_fit();
		emails.clear(); emails.shrink_to_fit();
		email_Names.clear(); email_Names.shrink_to_fit();
		websites.clear(); websites.shrink_to_fit();


		file++;
	}

	cout << "\n\nProcess is Done";
}






int main() {
	programFlow();
	return 0;
}
