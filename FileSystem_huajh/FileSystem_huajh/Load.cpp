#include <iostream>
#include <fstream>
#include <Windows.h>
#include <time.h>
#include "UnixStruct.h"
#include "UnixFunction.h"
using namespace std;

struct SuperBlock filsys;
char** curDir;
unsigned short dirLen; // Ŀ¼����
// ���50��
unsigned short d_inoList[50]; // ��ǰ·���µ�inode
unsigned short p_ino; //ͷָ��

inode inodeList[INODEMAX]; 
unsigned short p_inode;   

struct inode cur_inode;

struct file sys_file[OPENFILEMAX]; // ϵͳ���ļ���
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
	d_inoList[p_ino] = MFD_OFFSET-INODE_OFFSET;  // ��Ŀ¼,����inode��ƫ����
	curDir = new char*;
	*curDir =  (char*) malloc(5*sizeof(char));
	strcpy(*curDir," "); // ��Ŀ¼
	dirLen = 1;

	// ��ʼ���ڴ�i���� 
	p_inode = 0;

	////init ϵͳ���ļ���
	p_file = 0;

	// ��ʼ���û�
	curUser.p_file = 0;
	curUser.u_uid = 1;
	curUser .u_gid = 1;
	cout<<"load: "<<curUser.p_file<<endl;
	cout << "load success !" << endl;
}