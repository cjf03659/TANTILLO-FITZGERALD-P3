#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <stdio.h>
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>
#include <iomanip>

using namespace std;

/**
 * Method to handle the conversion of a octal representation of the permission to a human-readable
 * series of chars and dashes representation
 * @param perms the files current permissions in octal
 * @return void
 */
void handleperm(unsigned long perms)
{
	//mask the permission
	int permission = (perms & 00777);
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
				cout << "r";
				continue;
			}
			if((i+1) % 2 == 0)
			{
				cout << "w";
				continue;
			}
			cout << "x";
		} else {
			cout << "-";
		}
	}
	//handles sticky bits
	int sticky = perms & S_ISVTX;
	if(sticky > 0)
	{
		cout << "t";
	}
}

/**
 * Main method that runs the stat program
 * @param argc the number of command line arguments
 * @param argv array that stores the arguments
 * @return int returns exit status
 */
int main(int argc, char* argv[])
{	
	//setup unbuffered outputs
	cout << unitbuf;
	setvbuf(stdout, NULL, _IONBF, 0);

	int arg = 1;
	//handles case where no arguments are passed
	if(argc < 2)
	{
		*argv[0] = '-';
		arg = 0;
	}
	do
	{
	//stores whether or not to print the device type
	bool device = false;
	//handles file identifier in permissions
	char idPerm = '-';
	//stat struct that holds the file's information
	struct stat buf;
	//handles case where '-' is passed as file name
	if((char)*argv[arg] == '-')
	{
		int fd = 0;
		fstat(fd, &buf);
		device = true;
	} else {
	stat(argv[arg], &buf);
	}

	

	int access = (buf.st_mode & (07777));

	cout << "  File: `" << argv[arg] << "'" << endl;
	cout << "  Size: " << buf.st_size << "\t\t   Blocks: " << buf.st_blocks
	<< "\tIO Block: " << buf.st_blksize << "   ";

	//determines type of file
	switch (buf.st_mode & S_IFMT) 
	{
		case S_IFBLK:  printf("block device\n"); device = true; break;
		case S_IFCHR:  printf("character device\n"); idPerm = 'c'; device = true; break;
		case S_IFDIR:  printf("directory\n"); idPerm = 'd';  break;
		case S_IFIFO:  printf("FIFO/pipe\n"); break;
		case S_IFLNK:  printf("symlink\n"); idPerm = 'l'; break;
		case S_IFREG:  printf("regular file\n"); break;
		case S_IFSOCK: printf("socket\n"); break;
		default:       printf("unknown?\n"); break;
	}		

	cout << "Device: "; printf("%04x",(unsigned int) buf.st_dev); cout << "h/" << buf.st_dev << "d    Inode: " << buf.st_ino << "    Links: " << buf.st_nlink;
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
	} else {
		printf("%s)", pwd->pw_name);
	}
	
	cout << "    Gid: ("; printf("%ld/", (long) buf.st_gid);
	//struct to hold group id
	struct group *grp;
	grp = getgrgid(buf.st_gid);
	if(grp == NULL)
	{
		perror("getgrgid");
	} else {
		printf("%s)\n", grp->gr_name);
	}
	
	//prints timestamps
	cout << "Access: "; printf("%s", ctime(&buf.st_atime));
	cout << "Modify: "; printf("%s", ctime(&buf.st_mtime));
	cout << "Change: "; printf("%s", ctime(&buf.st_ctime));

	cout << endl;
	arg++;
	} while (arg < argc);


	return 0;	

}
