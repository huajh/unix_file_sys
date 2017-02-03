#include <iostream>
#include <fstream>
#include <Windows.h>
#include <time.h>
#include "UnixStruct.h"
using namespace std;

void format()
{
	cout << "正在格式化文件系统 ..." << endl;
	fstream FILE("VirtualDisk", ios::binary|ios::in|ios::out); //申请磁盘空间
	if (!FILE)
	{
		cerr << "磁盘空间无法创建!" << endl;
		return;
	}

	//ios::beg  ios::cur   ios::end
	FILE.seekp(0, ios::beg);
	//申请空间 , 共申请Block_Num个磁盘块 
	char *buf = (char*) malloc(BLOCKNUM*BLOCKSIZE*sizeof(char));
	if (buf==NULL)
	{
		printf ("\nVirtualDisk create failed! \n");
		exit(0);
	}
	FILE.write(buf, BLOCKNUM*BLOCKSIZE*sizeof(char));  //33554432B = 32768KB =32MB
	FILE.close();
	cout << "格式化成功" << endl;
}

