/*
 * @Description: Lab4
 * @Date: 2019-11-18 21:25:17
 * @Author: I-Hsien
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2019-11-18 22:30:07
 */
#include <cstdlib>
#include <cstdio>
#include <string.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
using namespace std;
//Ref:https://blog.csdn.net/qq_37858386/article/details/78702435
void printdir(char *dir, int depth)
{
	DIR *dip;
	struct dirent *entry;
    /*
    struct dirent { 
    #ifndef __USE_FILE_OFFSET64
    __ino_t  d_ino; //索引节点号   
    __off_t  d_off; //在目录文件中的偏移 
        #else
        __ino64_t  d_ino;
        __off64_t   d_off;
    #endif
    unsigned   short   int   d_reclent;     //文件名的长度
    unsigned   char   d_type;     //d_name所指的文件类型   
    char   d_name[256];     //文件名 }; 
    */
	struct stat statbuf;
    /*
    struct stat 
    {
	    unsigned long  st_dev;	// 文件所属的设备
        unsigned long  st_ino;	// 文件相关的inode 
	    unsigned short st_mode;	// 文件的权限信息和类型信息: S_IFDIR, S_IFBLK, S_IFIFO, S_IFLINK
        unsigned short st_nlink;	//硬连接的数目
        unsigned short st_uid;	// 文件所有者的ID
        unsigned short st_gid;	//文件所有者的组ID 
        unsigned long  st_rdev;	//设备类型
        unsigned long  st_size;	//文件大小
        unsigned long  st_blksize;	//块大小
        unsigned long  st_blocks;	//块数
        unsigned long  st_atime;	// 文件最后访问时间
        unsigned long  st_atime_nsec;
        unsigned long  st_mtime;	// 最后修改内容的时间
        unsigned long  st_mtime_nsec;
        unsigned long  st_ctime;	// 文件最后修改属性的时间
        unsigned long  st_ctime_nsec;
        unsigned long  __unused4;
        unsigned long  __unused5;
    };
    */
		if ((dip = opendir(dir)) == NULL)	//return dir handle;is failed
	{
		cout<<"Opendir failed!"<<endl;
		return;
	}
	if ( chdir(dir) == -1 )
	{
		cout<<"Chdir failed!"<<endl;
		return;
	}
	
	while ( (entry = readdir(dip)) != NULL )		//is dir
	{
		lstat(entry->d_name, &statbuf) == -1;	//Get info
		if ( (statbuf.st_mode&S_IFMT) == S_IFDIR)
		{
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
				continue;
			cout<<"DEPTH: "<<depth<<"    DIRNAME: "<<entry->d_name<<"    INODE: "<<statbuf.st_ino<<"    UID: "<<statbuf.st_uid<<"    GID: "<<statbuf.st_gid<<"    SIZE: "<<statbuf.st_size<<"    BLOCKSIZE: "<<statbuf.st_blksize<<"    ATIME: "<<strtok(ctime(&statbuf.st_atime)+4, "\n")<<endl;  //statbuf.st_atime
			printdir( (entry->d_name), depth+4);
		}
		
		else//is file
			cout<<"DEPTH: "<<depth<<"    DIRNAME: "<<entry->d_name<<"    INODE: "<<statbuf.st_ino<<"    UID: "<<statbuf.st_uid<<"    GID: "<<statbuf.st_gid<<"    SIZE: "<<statbuf.st_size<<"    BLOCKSIZE: "<<statbuf.st_blksize<<"    ATIME: "<<strtok(ctime(&statbuf.st_atime)+4, "\n")<<endl;
	}
	chdir("..");	
	closedir(dip);	
	return;
}

int main(int argc, char *argv[])
{   
    if(argc!=2)
    {
        printf("Usage:<path>\n");
        return 0;
    }
	printdir(argv[1], 0);
	return 0;
}