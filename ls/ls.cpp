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

char* thisDir;
int blockCount = 0;
int maxSize = 0;
std::vector<struct Line> printData;

struct Line {
	string fileName;
	string line;
};

string handleperm(unsigned long);

void longLn(string file)
{
	string buffer = "";
	struct stat buf;
	stat(file.c_str(), &buf);
	buffer += handleperm(buf.st_mode);
	//cout << ". " <<  buf.st_nlink << " ";
	buffer += ". " + to_string(buf.st_nlink) + " ";
 	blockCount += buf.st_blocks;
	//struct to hold user id
	struct passwd *pwd;
	pwd = getpwuid(buf.st_uid);
	if(pwd == NULL)
	{
		perror("getpwuid");
	} else {
		//printf("%s ", pwd->pw_name);
		//buffer += pwd->pw_name + " ";
		string tempstr(pwd->pw_name);
		buffer += tempstr + " ";
	}
	
	//cout << "    Gid: ("; printf("%ld/", (long) buf.st_gid);
	//struct to hold group id
	struct group *grp;
	grp = getgrgid(buf.st_gid);
	if(grp == NULL)
	{
		perror("getgrgid");
	} else {
		//printf("%s ", grp->gr_name);
		string tempstr(grp->gr_name);
		buffer += tempstr + " ";
	}

	//cout << buf.st_size << " ";
	int size = buf.st_size;
	int digits = 0;
	while(size != 0)
	{
		size = size / 10;
		digits++;
	}
	if(digits > maxSize) maxSize = digits;

	for(int i = 0; i < maxSize - digits; i++)
	{
		buffer += " ";
	}

	//if(buf.st_size % 1000 == buf.st_size) buffer += " ";
	//if(buf.st_size % 100 == buf.st_size) buffer += " ";
	buffer += to_string(buf.st_size) + " ";
	
	//prints timestamps
	//cout << "Access: "; printf("%s", ctime(&buf.st_atime));
	//cout << "Modify: "; printf("%s", ctime(&buf.st_mtime));
	//printf("%s", ctime(&buf.st_ctime));
	
	char temp[200];
	struct tm *tm;
	tm = localtime(&buf.st_ctime);
	strftime(temp, sizeof(temp), "%b %e %H:%M", tm);
	//cout << temp <(< " ";
	string tstring(temp);
	buffer += tstring;
	buffer += " ";

	printData.back().line = buffer;
	//delete [] temp;
}


/**
 * Method to handle the conversion of a octal representation of the permission to a human-readable
 * series of chars and dashes representation
 * @param perms the files current permissions in octal
 * @return void
 */
string handleperm(unsigned long perms)
{
	string buffer = "";
	//mask the permission
	int permission = (perms & 00777);
	
	switch (perms & S_IFMT) 
	{
		case S_IFDIR:  buffer += "d"; break;
		default:       buffer += "-"; break;
	}		
	//loops through the masked bits
	for(int i = 8; i >= 0; i--)
	{
		//determines if the bit is "on"
		if(permission - pow(2,i) >= 0)
		{
			//represents proper permission based on location in loop
			//knowing the group changes every three iterations
			permission -= pow(2,i);
			if((i+1) % 3 == 0)
			{
				buffer += "r";
				continue;
			}
			if((i+1) % 2 == 0)
			{
				buffer += "w";
				continue;
			}
			buffer += "x";
		} else {
			buffer += "-";
		}
	}
	//handles sticky bits
	int sticky = perms & S_ISVTX;
	if(sticky > 0)
	{
		buffer += "t";
	}
	return buffer;
}

void printLines(bool a, bool l)
{

	int maxLength = 0;

	if(l) cout << "total " << blockCount/2 << endl;
	for(int i = printData.size()-1; i >= 0; i--)
	{
		for(int x = 1; x <= i; x++)
		{
			string one = printData.at(x).fileName;
			string two = printData.at(x-1).fileName;
			if(one.length() > maxLength) maxLength = one.length();
			transform(one.begin(), one.end(), one.begin(), ::tolower);
			transform(two.begin(), two.end(), two.begin(), ::tolower);
			if(one.compare(two) < 0)
			{
				struct Line temp = printData.at(x);
				printData.at(x) = printData.at(x-1);
				printData.at(x-1) = temp;
			}
		}
	}

	for(int i = 0; i < printData.size(); i++)
	{
		if(l) cout << printData.at(i).line;
		cout << printData.at(i).fileName;
		//for(int x = 0; x < (maxLength - printData.at(i).fileName.length())+3; x++)
		//{
		//	cout << " ";
		//}
		//if(!l && i%3 == 0) cout << endl;
		if(l) cout << endl;
	}

	if(!l) cout << endl;
}

int main(int argc, char * argv[])
{

/**	struct Line line1;
	struct Line line2;
	struct Line line3;
	struct Line line4;
	struct Line line5;
	struct Line line6;

	line1.fileName = "AAAAAA";
	line2.fileName = "BBBBBB";
	line3.fileName = "CCCCCC";
	line4.fileName = "DDDDDD";
	line5.fileName = "EEEEEE";
	line6.fileName = "FFFFFF";

	printData.push_back(line3);
	printData.push_back(line4);
	printData.push_back(line2);
	printData.push_back(line5);
	printData.push_back(line1);
	printData.push_back(line6);

	printLines();
	return 0;
*/
	int opt;
	bool a = false;
	bool l = false;
	while((opt = getopt(argc, argv, "al")) != -1)
	{
		
		if(opt == 'a')
		{
			a = true;
			if(argv[optind-1][2] == 'l')
				l = true;
		}
		if(opt == 'l')
		{
			l = true;
			if(argv[optind-1][2] == 'a')
				a = true;
		}

	}

	if(argc == 1 || (argc == 2 && (a || l)))
	{
		const int size = 2;
		thisDir = new char[2];
		strcpy(thisDir, ".");
		//memcpy(nfile, nextFile.c_str(), newSize + 1);
		argv[argc - 1] = thisDir;
//		delete [] thisDir;
		
		cout << "last arg " << argv[argc-1] << endl;
	}

	//stat struct that holds the file's information
	struct stat buf;
	stat(argv[argc-1], &buf);

	int access = (buf.st_mode & (07777));
/**
	cout << "  File: `" << argv[arg] << "'" << endl;
	cout << "  Size: " << buf.st_size << "\t\t   Blocks: " << buf.st_blocks
	<< "\tIO Block: " << buf.st_blksize << "   ";
*/
	bool isDir = false;
	//determines type of file
	switch (buf.st_mode & S_IFMT) 
	{
		//case S_IFBLK:  printf("block device\n"); device = true; break;
		//case S_IFCHR:  printf("character device\n"); idPerm = 'c'; device = true; break;
		case S_IFDIR:  isDir = true; break;
		//case S_IFIFO:  printf("FIFO/pipe\n"); break;
		//case S_IFLNK:  printf("symlink\n"); idPerm = 'l'; break;
		//case S_IFREG:  printf("regular file\n"); break;
		//case S_IFSOCK: printf("socket\n"); break;
		default:        break;
	}		

	if(!isDir && !l)
	{
		cout << argv[argc-1] << endl;
	}

	if(isDir)
	{
		
		DIR *dir;
		struct dirent *dp;
		string dirName = argv[argc - 1];
		dirName += "/";
		string nextFile = argv[argc - 1];
		dir = opendir(argv[argc-1]);

		if(a)
		{
			struct Line temp1;
			temp1.fileName =  ".";
			printData.push_back(temp1);
			if(l) longLn(dirName);

			struct Line temp2;
			temp2.fileName = "..";
			printData.push_back(temp2);
			if(l) longLn(".");
		}

		//iterate through the elements stored in the directory
		while((dp = readdir(dir)) != NULL)
		{
			nextFile = dp->d_name;
			//nextFile += "/";
			//keeps from removing current and parent working directory
			if(nextFile.compare("..") == 0 || nextFile.compare(".") == 0) {
				//do nothing for these directories
			} else {
				//nextFile += dp->d_name;
				struct Line temp;
				temp.fileName = nextFile;
				printData.push_back(temp);
				if(l) 	longLn(dirName + nextFile);
			//	cout << nextFile << endl;
				//const int newSize = nextFile.size();
				//char *nfile = new char[newSize + 1];
				//memcpy(nfile, nextFile.c_str(), newSize + 1);
				//argv[argc - 1] = nfile;
				//recursive call passing one of the files within the directory
				//remove(argc, argv);
				//delete [] nfile;
			}
		}//while
		closedir(dir);
		printLines(a, l);
		//rmdir(dir_name);
	}
/**
	//cout << "Device: "; printf("%04x",(unsigned int) buf.st_dev); cout << "h/" << buf.st_dev << "d    Inode: " << buf.st_ino << "    Links: " << buf.st_nlink;
	if(device)
	{
		cout << "    Device type: "; printf("%x", (unsigned int) buf.st_rdev);
	}
	cout << endl;
	cout << "Access: ("; 
	printf("%lo",(unsigned long) access);
	cout << "/" << idPerm; handleperm(buf.st_mode); cout << ")  Uid: (";
	printf("%ld/", (long) buf.st_uid);

	//struct to hold user id
	struct passwd *pwd;
	pwd = getpwuid(buf.st_uid);
	if(pwd == NULL)
	{
		perror("getpwuid");
	if(isDir)
	{
		DIR *dir;
		struct dirent *dp;
		char * dir_name = argv[argc - 1];
		string nextFile = argv[argc - 1];
		dir = opendir(argv[argc-1]);
		//stop removal if the recursive option is not passed
		if(!recursive) {
			cerr << "Cannot remove `" << argv[argc-1] << "\": is a directory" << endl;
			return 0;
		}
		//iterate through the elements stored in the directory
		while((dp = readdir(dir)) != NULL)
		{
			nextFile = dir_name;
			nextFile += "/";
			//keeps from removing current and parent working directory
			if(!strcmp(dp->d_name, "..") || !strcmp(dp->d_name, ".")) {
				//do nothing for these directories
			} else {
				nextFile += dp->d_name;
				const int newSize = nextFile.size();
				char *nfile = new char[newSize + 1];
				memcpy(nfile, nextFile.c_str(), newSize + 1);
				argv[argc - 1] = nfile;
				//recursive call passing one of the files within the directory
				remove(argc, argv);
				delete [] nfile;
			}
		}//while
		closedir(dir);
		rmdir(dir_name);
	}
*/
	delete [] thisDir;
	return 0;
}
