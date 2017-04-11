#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>

using namespace std;

/**
 * Function used to determine if a file exits or not
 * @param fileName the path name of the file in question
 * @return bool true if the file exists, false otherwise
 */
bool doesExist(string fileName)
{
	//open() will return -1 if the file does not already exist
	int success = open(fileName.c_str(), O_RDWR);
	if(success == -1)
		return false;
	return true;
}

/**
 * main method of a function used to create links and symbolic links on files
 * @param argc the number of command line arguments passed
 * @param argv an array storing the command line arguments
 * @return int the exit status of the program
 */
int main(int argc, char* argv[])
{
	//change to unbuffered output
	setvbuf(stdout, NULL, _IONBF, 0);
	cerr << unitbuf;

	int opt;
	bool symLink = false;
	//determines and handles the -s argument being passed
	while((opt = getopt(argc, argv, "s:")) != -1)
	{
		if(opt == 's')
			symLink = true;
	}

	//handles case where no files are passed
	if(argc == 1 || (argc == 2 && symLink))
	{
		cerr << "./ln: missing file operand" << endl;
		return -1;
	}

	//handles the creation of hard links and special cases
	if(!symLink)
	{
		int pos = 2;

		if(argc == 2)
			pos--;

		//handles case where the target file does not exist
		if(!doesExist(argv[argc-pos]))
		{
			cerr << "./ln: accessing `" << argv[argc-pos] << "': No such file or directory" << endl;
			return -1;
		}

		//creates a hard link from target file to target file
		if(argc == 2 && doesExist(argv[argc-pos]))
		{
			cerr << "./ln: creating hard link `" << argv[argc-pos] << "': File exists" << endl;
			link(argv[argc-pos], argv[argc-pos]);
			return 1;
		}
	}

	//handles expected case links
	if(symLink)
	{
		symlink(argv[argc-2], argv[argc-1]);
	} else {
		link(argv[argc-2], argv[argc-1]);
	}

	return 1;
}//main
