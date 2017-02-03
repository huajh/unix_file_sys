#include <iostream>
#include <fstream>
#include <Windows.h>
#include <time.h>
#include "UnixStruct.h"
#include "UnixFunction.h"
using namespace std;

// 分配一块空间
// return -1 没有可用空间
unsigned int _alloc_data()
{
//	cout<<"_alloc_data"<<endl;
	unsigned int block;
	if(filsys.s_tfree == 0) return -1;
	filsys.s_nfree--;
	filsys.s_tfree --;
	block = filsys.s_free[filsys.s_nfree];
	if(filsys.s_nfree == 0) // 第一个
	{
		if(filsys.s_free[0]== -1) // 最后一块 
			return 0;
		fstream FILE("VirtualDisk", ios::binary|ios::in|ios::out); 
		FILE.seekp(BLOCKSIZE*(DATA_OFFSET+filsys.s_free[0]),ios::beg);
		free_stack fstack;
		FILE.read((char *)&fstack,sizeof(fstack));
		filsys.s_nfree = fstack.s_nfree;
		memcpy(filsys.s_free,fstack.s_free,sizeof(fstack.s_free));
		FILE.close();
	}
	return block;
}
unsigned int  _alloc_inode()
{
//	cout<<"_alloc_inode"<<endl;
	unsigned int block;
	if(filsys.s_tinode == 0) return -1;
	filsys.s_ninode--;
	filsys.s_tinode --;
	block = filsys.s_inode[filsys.s_ninode];
	if(filsys.s_ninode == 0) // 第一个
	{
		if(filsys.s_inode[0]== -1) // 最后一块 
			return 0;
		fstream FILE("VirtualDisk", ios::binary|ios::in|ios::out); 
		FILE.seekp(BLOCKSIZE*(INODE_OFFSET+filsys.s_inode[0]),ios::beg);
		free_stack fstack;
		FILE.read((char *)&fstack,sizeof(fstack));
		filsys.s_ninode = fstack.s_nfree;
		memcpy(filsys.s_inode,fstack.s_free,sizeof(fstack.s_free));
		FILE.close();
	}
	return block;
}

void  _release_data(int index)
{
//	cout<<"_release_data"<<endl;
	filsys.s_tfree++;
	if(filsys.s_nfree<GROUPSIZE)
	{
		filsys.s_free[filsys.s_nfree++] = index;	
	}else
	{
		free_stack fstack;
		fstack.s_nfree = GROUPSIZE;
		memcpy(fstack.s_free,filsys.s_free,sizeof(fstack.s_free)); // 写入释放块中
		fstream FILE("VirtualDisk", ios::binary|ios::in|ios::out); 
		FILE.seekp(BLOCKSIZE*(index+DATA_OFFSET), ios::beg);
		FILE.write((char*)&fstack,sizeof(fstack));
		filsys.s_nfree = 1;
		filsys.s_free[0] = index;
		FILE.close();
	}
}
void _release_inode(int index)
{
//	cout<<"_release_inode"<<endl;
	filsys.s_tinode++;
	if(filsys.s_ninode<GROUPSIZE)
	{
		filsys.s_inode[filsys.s_ninode++] = index;	
	}else
	{
		free_stack fstack;
		fstack.s_nfree = GROUPSIZE;
		memcpy(fstack.s_free,filsys.s_inode,sizeof(fstack.s_free)); // 写入释放块中
		fstream FILE("VirtualDisk", ios::binary|ios::in|ios::out); 
		FILE.seekp(BLOCKSIZE*(index+INODE_OFFSET), ios::beg);
		FILE.write((char*)&fstack,sizeof(fstack));
		filsys.s_ninode = 1;
		filsys.s_free[0] = index;
		FILE.close();
	}
}
