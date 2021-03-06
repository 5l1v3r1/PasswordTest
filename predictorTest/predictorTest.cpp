#include "stdafx.h"
#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include <chrono>
//#include<thread>
//Myheaders
#include"FileHandling.h"
#include"DataComparison.h"
#include"DataPatterns.h"


using namespace std;


void programFlow() {
	vector<string> filePaths;
	vector<string> patternStringsVector;
	vector<string>::iterator rawIterator;
	FileHandling::getAllFilesRecursive(filePaths, "Input/");
	vector<string>::iterator file = filePaths.begin();
	DataComparison::readResources();
	cout << "\n Names, Dob, Locations and Commons files are readed \n";

	while (file != filePaths.end()) {
		cout << "\n" << " Starting file:" << *file << " \n";
		DataComparison::readRawDataList(*file);
		DataComparison::lowerCaseRawDataList();
		DataComparison::extractEmailAndPassword(":");
		DataComparison::extractEmailNamesAndWebsites();
		rawIterator = DataComparison::passwordsList.begin();
		while (rawIterator != DataComparison::passwordsList.end())
		{
			DataComparison::nameListComparison(*rawIterator);
			DataComparison::emailNameListComparison(*rawIterator, rawIterator);
			DataComparison::websiteListComparison(*rawIterator, rawIterator);
			DataComparison::dobListComparison(*rawIterator);
			DataComparison::locationListComparison(*rawIterator);
			DataComparison::commonWordListComparison(*rawIterator);
			DataPatterns::rawResults = DataComparison::getResults();
			DataComparison::results.clear();  DataComparison::results.shrink_to_fit();
			patternStringsVector.push_back(DataPatterns::patterns(*rawIterator));
			rawIterator++;
		}
		DataComparison::passwordsList.clear(); DataComparison::passwordsList.shrink_to_fit();
		DataComparison::emailNamesList.clear(); DataComparison::emailNamesList.shrink_to_fit();
		DataComparison::websitesList.clear(); DataComparison::websitesList.shrink_to_fit();
		FileHandling::writeFile(patternStringsVector, "Output/0.txt");
		file++;
	}
	patternStringsVector.clear(); patternStringsVector.shrink_to_fit();
	//sad
	cout << "\n\nProcess is Done";
}



void timeElasped() {
	auto start = std::chrono::high_resolution_clock::now();
	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = finish - start;
	std::cout << "Elapsed time: " << elapsed.count() << " s\n";
}



int main() {
	programFlow();
	return 0;
}
