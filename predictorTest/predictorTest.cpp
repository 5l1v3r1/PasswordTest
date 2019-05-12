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
#include"DataComparison.h"
#include"DataPatterns.h"


using namespace std;


//Global Vectors
vector<string> raw_Data_Vector;//temporary data vector
vector<string> pattern_Strings_Vector;
//Global iterators
vector<string>::iterator raw_Vector_iterator;


void programFlow() {

	FileHandling f;
	DataCleansing d;
	DataComparison c;
	DataPatterns p;
	vector<string> filePaths;
	f.getAllFilesRecursive(filePaths, "Input/");
	vector<string>::iterator file = filePaths.begin();

	cout << "\n Names, Dob, Locations and Commons files are readed \n";

	while (file != filePaths.end()) {
		cout << "\n" << " Starting file:" << *file << " \n";
		c.readRawDataList(*file);
		c.lowerCaseRawDataList();
		c.extractEmailAndPassword(":");
		c.extractEmailNamesAndWebsites();
		raw_Vector_iterator = DataComparison::passwordsList.begin();
		while (raw_Vector_iterator != DataComparison::passwordsList.end())
		{
			c.nameListComparison(*raw_Vector_iterator);
			c.emailNameListComparison(*raw_Vector_iterator, raw_Vector_iterator);
			c.websiteListComparison(*raw_Vector_iterator, raw_Vector_iterator);
			c.dobListComparison(*raw_Vector_iterator);
			c.locationListComparison(*raw_Vector_iterator);
			c.commonWordListComparison(*raw_Vector_iterator);
			p.rawResults = c.getResults();
			c.DataComparison::results.clear(); c.DataComparison::results.shrink_to_fit();
			pattern_Strings_Vector.push_back(p.patterns(*raw_Vector_iterator));

			raw_Vector_iterator++;
		}
		c.passwordsList.clear(); c.passwordsList.shrink_to_fit();
		c.emailNamesList.clear(); c.emailNamesList.shrink_to_fit();
		c.websitesList.clear(); c.websitesList.shrink_to_fit();
		f.writeFile(pattern_Strings_Vector, "Output/0.txt");
		file++;
	}

	cout << "\n\nProcess is Done";
}






int main() {
	programFlow();
	return 0;
}
