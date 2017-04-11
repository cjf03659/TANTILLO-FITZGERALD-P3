#include <stdio.h>
#include <string.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <math.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <vector>
#include <algorithm>

using namespace std;

#define BUFSIZE 20480

int main(int argc, char *argv[])
{

	int opt;
	bool f = false;
	bool c = false;
	bool n = false;
	bool noFile = false;
	int numArgs = 0;
	int num = 10;
	while((opt = getopt(argc, argv, "fc:n:")) != -1)
	{
		
		if(opt == 'f')
		{
			f = true;
			numArgs++;
		}
		if(opt == 'c')
		{
			c = true;
			numArgs += 2;
			try {
			num = stoi(optarg, nullptr, 0);
			} catch (exception& e) {
				cerr << e.what() << " on size option argument" << endl;
				return -1;
			}
		}
		if(opt == 'n')
		{
			n = true;
			numArgs += 2;
			try {
			num = stoi(optarg, nullptr, 0);
			} catch (exception& e) {
				cerr << e.what() << " on size option argument" << endl;
				return -1;
			}
		}

	}

	if(!n && !c)
	{
		n = true;
		num = 10;
	}

	if(numArgs >= (argc-1))
	{
		noFile = true;
	}

	//handle the opening and reading of the file
	char buffer[BUFSIZE];
	int readSuccess;
	int fd;
	if(noFile)
	{
		readSuccess = read(STDIN_FILENO, buffer, BUFSIZE);
		f = false;
	} else {
		fd = open(argv[argc-1], O_RDWR);
		readSuccess = read(fd, buffer, BUFSIZE);
	}

	if(readSuccess < 0)
	{
		cerr << "Could not open and read file" << endl;
		return 1;
	}

	if(f)
	{
		struct stat buf;
		stat(argv[argc-1], &buf);

		//determines type of file
		switch (buf.st_mode & S_IFMT) 
		{
			case S_IFIFO:  break;
			case S_IFREG:  break;
			default:       f = false; break;
		}
	}		
	

	vector<string> pData;

	string lineBuf = "";
	int lines = 0;
	int bytes = 0;
	//print lines from file one at a time until n is reached or end of file
	for(int i = readSuccess; i >= 0; i--)
	{
		if(n)
		{
			if(buffer[i] == '\n')
			{
				pData.insert(pData.begin(), lineBuf);
				lineBuf = "";
				//lineBuf.insert(lineBuf.begin(), buffer[i])
				if(++lines > num) break;
			}
		}

		if(c)
		{
			if(buffer[i] == '\n')
			{
				pData.insert(pData.begin(), lineBuf);
				lineBuf = "";
			}
			if(bytes++ > num)
			{
				pData.insert(pData.begin(), lineBuf);
				break;
			}
		}
		lineBuf.insert(lineBuf.begin(), buffer[i]);

		if(i == 0)
		{
			pData.insert(pData.begin(), lineBuf);
		}
	}

	for(int i = 0; i < pData.size(); i++)
	{
		cout << pData.at(i);
	}

	if(f)
	{
		char buf[1];
		int bytesRead = 0;
		lseek(fd, 0, SEEK_END);
		while(bytesRead = read(fd, buf, 1))
		{
			if(bytesRead == 1)
				cout << buf;
		}
	}

	return 0;
}








