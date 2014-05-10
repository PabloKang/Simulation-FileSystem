//================================================================================
// Name        : project1_main.cpp
// Author      : Pablo Kang, Marissel Llavore, Kunal Pathak, Nicholas Steven Popov
// Version     :
// Copyright   : Your copyright notice
// Description : Project 1, Part 1
//================================================================================


#include <iostream>
#include <string>
#include "FileSystem53.h"
using namespace std;



int main() {
	// CURRENT MAIN() CODE IS FOR TESTING PURPOSES ONLY
	FileSystem53 testFS(0,0,"");
	string cmd;
	cin >> cmd;
	while (cmd != "ex") // ex = exit?
	{
		if (cmd.size() == 2)
		{
			if (cmd == "cr")
			{
				string fname;
				cin >> fname;
				int result = testFS.create(fname);
				if (result == 0)
					cout << "file " << fname << " created" << endl;
				else
					cout << "error" << endl;
			}
			else if (cmd == "de")
			{
				string fname;
				cin >> fname;
				int result = testFS.deleteFile(fname);
				if (result == 0)
					cout << "file " << fname << " destroyed" << endl;
				else
					cout << "error" << endl;
			}
			else if (cmd == "op")
			{
				string fname;
				cin >> fname;
				int result = testFS.open(fname);
				if (result != -1 || result != -2)
					cout << "file " << fname << " opened, index = " << result << endl;
				else
					cout << "error" << endl;
			}
			else if (cmd == "cl")
			{
				int index;
				cin >> index;
				testFS.close(index);
				cout << "file with index " << index << " closed" << endl;
			}
			else if (cmd == "rd")
			{
				int index, count;
				cin >> index >> count;
				char* ma = new char[count];
				int result = testFS.read(index, ma, count);
				if (result != -1 || result != -2)
				{
					cout << count << " bytes read: ";
					for (int i = 0; i < count; i++)
					{
						std::cout << ma[i];
					}
				}
				else
					cout << "error";
			}
			else if (cmd == "wr")
			{
				int index, count;
				char c;
				cin >> index >> c >> count;
				int result = testFS.write(index, c, count);
				if (result != -1 || result != -2)
					cout << count << " bytes written" << endl;
				else
					cout << "error" << endl;
			}
			else if (cmd == "sk")
			{
				int index, position;
				cin >> index >> position;
				int result = testFS.lseek(index, position);
				if (result == 0)
					cout << "current position is " << position << endl;
				else
					cout << "error" << endl;
			}
			else if (cmd == "dr")
			{
				testFS.directory();
			}
			else if (cmd == "in")
			{
				if (std::ifstream("lDiskISO.txt")) // file exists
				{
					testFS.restore();
					cout << "disk restored" << endl;
				}
				else
				{
					FileSystem53 newFS(0, 0, "lDiskISO.txt");
					cout << "disk initialized" << endl;
				}
			}
			else if (cmd == "sv")
			{
				testFS.save();
				cout << "disk saved" << endl;
			}
		}
		else
			cout << "error" << endl;
		cin >> cmd;
	}
	testFS.format();
	
	testFS.save();
	testFS.restore();

	//char** test = new char*[6];
	//test[0] = new char[5];

	//test[0][0] = (char)1;

	//cout << '<' << test[0][0] << '>' << endl;
	char* h = new char[5];
	testFS.read(0, h, 0);

	// Holds the console open unil user input.
	int i = 0;
	cin >> i;

	return 0;
}
