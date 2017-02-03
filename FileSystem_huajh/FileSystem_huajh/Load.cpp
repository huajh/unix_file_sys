#include <iostream>
#include <fstream>
#include <Windows.h>
#include <time.h>
#include "UnixStruct.h"
#include "UnixFunction.h"
using namespace std;

struct SuperBlock filsys;
char** curDir;
unsigned short dirLen; // 目录长度
// 最大50层
unsigned short d_inoList[50]; // 当前路径下的inode
unsigned short p_ino; //头指针

inode inodeList[INODEMAX]; 
unsigned short p_inode;   

struct inode cur_inode;

struct file sys_file[OPENFILEMAX]; // 系统打开文件表
 unsigned short p_file; 

void load()
{
	cout << "load... ..." << endl;
	fstream FILE("VirtualDisk", ios::binary|ios::in|ios::out); 
	FILE.seekp(BLOCKSIZE,ios::beg);	
	FILE.read((char *)&filsys,sizeof(filsys));
	FILE.seekp(MFD_OFFSET*BLOCKSIZE,ios::beg);
	FILE.read((char *)&cur_inode._base,sizeof(cur_inode._base));
	FILE.close();
	p_ino = 0;
	d_inoList[p_ino] = MFD_OFFSET-INODE_OFFSET;  // 根目录,关于inode的偏移量
	curDir = new char*;
	*curDir =  (char*) malloc(5*sizeof(char));
	strcpy(*curDir," "); // 根目录
	dirLen = 1;

	// 初始化内存i结点表 
	p_inode = 0;

	////init 系统打开文件表
	p_file = 0;

	// 初始化用户
	curUser.p_file = 0;
	curUser.u_uid = 1;
	curUser .u_gid = 1;
	cout<<"load: "<<curUser.p_file<<endl;
	cout << "load success !" << endl;
}