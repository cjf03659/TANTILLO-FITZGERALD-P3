#include <stdio.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

#define BUFSIZE 1000

using namespace std;

/**
 * main method of program used to print the first n lines of a file
 * @param argc the number of command line arguments passed
 * @param argv an array of the arguments passed
 * @return int the exit status of the program
 */
int main(int argc, char * argv[])
{
	int nLines = 10;

	//create constants for different argument senarios
	const int NO_SIZE_NO_FILE = 1;
	const int SIZE_NO_FILE = 2;
	const int NO_SIZE_FILE = 3;
	int senario = 0;

	//handle error message for if over three arguments are passed
	if(argc > 3) {
		cerr << "Too many arguments: ./head [-n] [file] " << endl;
		return 1;
	}

	//determine if size option or file is passed for when only one argument is passed
	if(argc == 2)
	{
		string temp = argv[1];
		if(temp.compare("-") == 0)
		{
			senario = NO_SIZE_NO_FILE;
		}

		if(temp.at(0) != '-') senario = NO_SIZE_FILE;
	}

	//handle case where size and a '-' as the file are passed
	if(argc == 3)
	{
		string temp = argv[argc - 1];
		if(temp.compare("-") == 0) senario = SIZE_NO_FILE;
	}

	//handle case where no arguments are included
	if(argc == 1) senario = NO_SIZE_NO_FILE;

	
	if(argc >= 2 && senario != NO_SIZE_NO_FILE && senario != NO_SIZE_FILE){
	string numString = argv[1];
	//ensure that a '-' before the n argument is passed
	if(numString.find('-') != 0) {
		cerr << "Illegal format. Include \"-\" when specifying n" << endl;
		return 1;
	}

	numString.erase(numString.begin());

	for(int i = 0; i < (int)numString.length(); i++)
	{
		//ensure that only positive decimal integers are passed
		if(!(isdigit(numString.at(i))))
		{
			cerr << "Positive decimal integers for number option-argument only" << endl;
			return 1;
		}
	}

		//convert the string storing the n argument to an int
		try {
			nLines = stoi(numString, nullptr, 0);
		} catch (exception& e) {
			cerr << e.what() << " on -n option argument" << endl;
			return 1;
		}
		
		if(argc == 2) senario = SIZE_NO_FILE;	
	}

	//handle the opening and reading of the file
	char fBuf[BUFSIZE];
	int readSuccess;
	if(senario == NO_SIZE_NO_FILE || senario == SIZE_NO_FILE)
	{
		readSuccess = read(STDIN_FILENO, fBuf, BUFSIZE);
	} else {
		int fd = open(argv[argc-1], O_RDWR);
		readSuccess = read(fd, fBuf, BUFSIZE);
	}

	if(readSuccess < 0)
	{
		cerr << "Could not open and read file" << endl;
		return 1;
	}

	string lineBuf = "";
	int linesPrinted = 0;
	//print lines from file one at a time until n is reached or end of file
	for(int i = 0; i < BUFSIZE; i++)
	{
		if(fBuf[i] != '\n')
		{
			lineBuf += fBuf[i];
		} else {
			cout << lineBuf << endl;
			lineBuf = "";
			if(++linesPrinted >= nLines) break;
		}
	}
	return 0;
}











