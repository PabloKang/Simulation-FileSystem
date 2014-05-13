//================================================================================
// Name        : project1_main.cpp
// Author      : Pablo Kang, Marissel Llavore, Kunal Pathak, Nicholas Steven Popov
// Version     :
// Copyright   : Your copyright notice
// Description : Project 1, Part 1
//================================================================================

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "FileSystem53.h"

using namespace std;



int main() {

	FileSystem53 fileSystem(0, 0, "");

	fileSystem.shellCommandList();

	fileSystem.format();
	fileSystem.save();
	fileSystem.restore();

	string cmd = "porn";
	while (cmd != "sd"){

		int numberOfArgs = 0;

		std::string tempIn;
		std::string buffer;
		std::vector<std::string> userInput;			// user commands are ultimately stored here

		std::cout << "\nFS53> ";					// arbitrary command prompt

		getline(cin, tempIn);
		while (tempIn.size() == 0) {				// for empty input
			std::cout << "\nFS53> ";
			getline(cin, tempIn);
		}


		std::stringstream streamIn(tempIn);
		while (streamIn >> buffer) {				// adds each element of the user's input, separated by spaces, to the vector
			userInput.push_back(buffer);
			++numberOfArgs;
		}


		if (userInput.at(0) == "cr") {
			if (numberOfArgs != 2)
				std::cout << "\nYou included the wrong number of arguments.\nPlease try again.";
			else
				fileSystem.shellCr(userInput.at(1));
		}

		else if (userInput.at(0) == "de") {
			if (numberOfArgs != 2)
				std::cout << "\nYou included the wrong number of arguments.\nPlease try again.";
			else
				fileSystem.shellDe(userInput.at(1));
		}

		else if (userInput.at(0) == "op") {
			if (numberOfArgs != 2)
				std::cout << "\nYou included the wrong number of arguments.\nPlease try again.";
			else
				fileSystem.shellOp(userInput.at(1));
		}

		else if (userInput.at(0) == "cl") {
			if (numberOfArgs != 2)
				std::cout << "\nYou included the wrong number of arguments.\nPlease try again.";
			else
				fileSystem.shellCl(userInput.at(1));
		}

		else if (userInput.at(0) == "rd") {
			if (numberOfArgs != 3)
				std::cout << "\nYou included the wrong number of arguments.\nPlease try again.";
			else
				fileSystem.shellRd(userInput.at(1), userInput.at(2));
		}

		else if (userInput.at(0) == "wr") {
			if (numberOfArgs != 4)
				std::cout << "\nYou included the wrong number of arguments.\nPlease try again.";
			else
				fileSystem.shellWr(userInput.at(1), userInput.at(2), userInput.at(3));
		}

		else if (userInput.at(0) == "sk") {
			if (numberOfArgs != 3)
				std::cout << "\nYou included the wrong number of arguments.\nPlease try again.";
			else
				fileSystem.shellSk(userInput.at(1), userInput.at(2));
		}

		else if (userInput.at(0) == "dr") {
			if (numberOfArgs != 1)
				std::cout << "\nYou included the wrong number of arguments.\nPlease try again.";
			else
				fileSystem.shellDr();
		}

		else if (userInput.at(0) == "in") {
			if (numberOfArgs != 2)
				std::cout << "\nYou included the wrong number of arguments.\nPlease try again.";
			else
				fileSystem.shellIn(userInput.at(1));
		}

		else if (userInput.at(0) == "sv") {
			if (numberOfArgs != 1)
				std::cout << "\nYou included the wrong number of arguments.\nPlease try again.";
			else
				fileSystem.shellSv();
		}

		else if (userInput.at(0) == "sd") {							// exits the terminal
			cmd = "sd";
			std::cout << "\nGood bye!\n";
		}

		else if (userInput.at(0) == "hp") {							// displays the list of commands available
			fileSystem.shellCommandList();
		}

		else
			std::cout << "\nNot a valid command, please type hp for help.";

	}

	system("pause");
	return 0;
}