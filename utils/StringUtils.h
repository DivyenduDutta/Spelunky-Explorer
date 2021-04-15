#pragma once
#include <string>
#include <vector>

using namespace std;

/*
String helper funtions
*/

inline vector<string> splitString(const char* stringToSplit, char splitChar = ' ') {
	vector<string> splitData;
	string stringPart = "";

	int i = 0;
	while (i < strlen(stringToSplit)) {
		
		if (stringToSplit[i] != splitChar) {
			stringPart += stringToSplit[i];
		}
		else if(stringPart.length() != 0){
			splitData.push_back(stringPart);
			stringPart = "";
		}
		i++;
	}

	if (stringPart.length() != 0) {
		splitData.push_back(stringPart);
	}
	return splitData;
}

inline bool stringEndsWith(const string& str1, const string& str2) {
	if (!str1.compare(str1.length() - str2.length(), str2.length(), str2)) {
		return true;
	}
	return false;
}

inline bool stringStartsWith(const string& str1, const string& str2) {
	if (!str1.compare(0, str2.length(), str2)) {
		return true;
	}
	return false;
}