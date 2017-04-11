#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <wchar.h>

#define BUFSIZE 10000

using namespace std;

int main(int argc, char * argv[])
{

	bool c = false;
	bool m = false;
	bool l = false;
	bool w = false;
	int opt, numArgs;
	while((opt = getopt(argc, argv, "cmlw")) != -1)
	{
		
		if(opt == 'c')
		{
			c = true;
			numArgs++;
		}
		if(opt == 'm')
		{
			m = true;
			numArgs++;
		}
		if(opt == 'l')
		{
			l = true;
			numArgs++;
		}
		if(opt == 'w')
		{
			w = true;
			numArgs++;
		}

	}

	if(w)
	{
		cout << "yes w File: " << argv[argc-1] << endl;
	}

	//fix this
	bool noFile = false;

	int nBytes = 0;
	int fd;
	if(c)
	{
		fd = open(argv[argc-1], O_RDWR);
		lseek(fd, 0, SEEK_END);
		off_t endPos = lseek(fd, 0, SEEK_CUR);
		nBytes = endPos;
		close(fd);
		string outBuf = "";
		outBuf += to_string(nBytes);
		outBuf += " ";
		outBuf += argv[argc-1];
		outBuf += '\n';
		printf(outBuf.c_str());
	}

	int fd2;
	if(l)
	{
		//handle the opening and reading of the file
		char buf[BUFSIZE];
		int readSuccess;
		if(noFile)
		{
			readSuccess = read(STDIN_FILENO, buf, BUFSIZE);
		} else {
			fd2 = open(argv[argc-1], O_RDWR);
			readSuccess = read(fd, buf, BUFSIZE);
		}
	
		if(readSuccess < 0)
		{
			cerr << "Could not open and read file" << endl;
			return 1;
		}
	
		int lines = 0;
		//find number of new line characters in file
		for(int i = 0; i < BUFSIZE; i++)
		{
			if(buf[i] == '\n')
			{
				lines++;
			}
			//charTemp += buf[i];
		}

		
		//printf(lines);
		string buffer = "";
		buffer += to_string(lines);
		buffer += " ";
		buffer += argv[argc-1];
		buffer += '\n';
		printf(buffer.c_str());
	}

	int words = 0;
	int fd3;
	if(w)
	{
		wchar_t buf[BUFSIZE];
		wchar_t c;
		wchar_t b;
		int readSuccess;
		//if(noFile)
		//{
		//	readSuccess = read(STDIN_FILENO, buf, BUFSIZE);
		//} else {
			fd3 = open(argv[argc-1], O_RDWR);
			readSuccess = read(fd3, buf, BUFSIZE);
		//}
	
		if(readSuccess < 0)
		{
			cerr << "Could not open and read file" << endl;
			return 1;
		}
	
		for(int i = 1; i < BUFSIZE; i++)
		{
			c = buf[i];
			b = buf[i-1];
			if(iswspace(c))
			{
				if(iswspace(b))
				{
					//words++;
				} else {
					words++;
				}
			}
		}

		
		cout << "Words : " << to_string(words) << endl;
	}
	close(fd);
	close(fd2);
	close(fd3);

	return 1;
}





