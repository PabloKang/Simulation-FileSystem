//================================================================================
// Name        : project1_main.cpp
// Author      : Pablo Kang, Marissel Llavore, Kunal Pathak, Nicholas Steven Popov
// Version     :
// Copyright   : Your copyright notice
// Description : Project 1, Part 1
//================================================================================

#include "FileSystem53.h"
#include <iostream>
#include <string>
using namespace std;



int main() {
	// CURRENT MAIN() CODE IS FOR TESTING PURPOSES ONLY
	FileSystem53 testFS(0,0,"");
	testFS.format();
	
	testFS.save();
	testFS.restore();

	char** test = new char*[6];
	test[0] = new char[5];

	test[0][0] = (char)192;
	test[0][1] = (char)'-';

	int a = test[0][0];

	cout << a << endl << test[0][1];

	// Holds the console open unil user input.
	int i = 0;
	cin >> i;

	return 0;
}
