#include <iostream>
#include <fstream>
#include <Windows.h>
#include <time.h>
#include "UnixStruct.h"
using namespace std;

void format()
{
	cout << "���ڸ�ʽ���ļ�ϵͳ ..." << endl;
	fstream FILE("VirtualDisk", ios::binary|ios::in|ios::out); //������̿ռ�
	if (!FILE)
	{
		cerr << "���̿ռ��޷�����!" << endl;
		return;
	}

	//ios::beg  ios::cur   ios::end
	FILE.seekp(0, ios::beg);
	//����ռ� , ������Block_Num�����̿� 
	char *buf = (char*) malloc(BLOCKNUM*BLOCKSIZE*sizeof(char));
	if (buf==NULL)
	{
		printf ("\nVirtualDisk create failed! \n");
		exit(0);
	}
	FILE.write(buf, BLOCKNUM*BLOCKSIZE*sizeof(char));  //33554432B = 32768KB =32MB
	FILE.close();
	cout << "��ʽ���ɹ�" << endl;
}

