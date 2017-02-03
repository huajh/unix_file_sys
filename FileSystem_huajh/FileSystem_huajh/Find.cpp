
#include <iostream>
#include <fstream>
#include <time.h>
#include "UnixStruct.h"
#include "UnixFunction.h"
using namespace std;

//index 为inode索引号，返回filename的索引号，-1表示没有找到
int find(int index,const char* filename,int dd[])
{
//	cout<<"find"<<endl;
//	cout<<index<<" "<<filename<<endl;
	if(dd==NULL) dd = new int(5);
	int foundindex = -1;
	unsigned short i_addr[13];
	int size;
	fstream FILE;
	if(index ==d_inoList[p_ino])  // 当前目录
	{
	//	cout<<"current dir"<<endl;
		size= cur_inode._base.i_size;
		memcpy(i_addr,cur_inode._base.i_addr,sizeof(i_addr));
	}else{
//		cout<<" not  current dir"<<endl;
		FILE.open("VirtualDisk", ios::binary|ios::in|ios::out); 
		FILE.seekp(BLOCKSIZE*(INODE_OFFSET+index),ios::beg);	
		dinode node;
		FILE.read((char *)&node,sizeof(node));
		size = node.i_size;
		memcpy(i_addr,node.i_addr,sizeof(i_addr));
		FILE.close();
	}

// 	cout<<size<<endl;
// 	for(int i=0;i<13;i++)
// 		cout<<i_addr[i]<<" ";
// 	cout<<endl;
	FILE.open("VirtualDisk", ios::binary|ios::in|ios::out); 
	int dirsize = size>10 ? 10 : size;
//	cout<<"//直接索引  0 -9 "<<endl;
	for(int i=0;i<dirsize;i++) //直接索引  0 -9 
	{		
		dir dir_cusor; 
		dd[0] = i_addr[i];
		FILE.seekp(BLOCKSIZE*(DATA_OFFSET+i_addr[i]),ios::beg);	
		FILE.read((char *)&dir_cusor,sizeof(dir_cusor));		
		foundindex = findInDir(dir_cusor,filename,&dd[1]);
		if(foundindex != -1){ FILE.close(); return foundindex; }
	}
	if(size<=10 ) {FILE.close();return -1;}
//	cout<<"//一次间接索引 10-265 "<<endl;
	Indir oneIndir;//一次间接索引 10-265 
	dd[0] = i_addr[10];
	FILE.seekp(BLOCKSIZE*(DATA_OFFSET+i_addr[10]),ios::beg);
	FILE.read((char *)&oneIndir,sizeof(oneIndir));
	int oneLen = size-10 > 256 ? 256 : size-10;
	for(int i=0;i<oneLen;i++) 
	{
		dd[1] = oneIndir.index[i];
		dir dir_cusor; 
		FILE.seekp(BLOCKSIZE*(DATA_OFFSET+oneIndir.index[i]),ios::beg);
		FILE.read((char *)&dir_cusor,sizeof(dir_cusor));
		foundindex = findInDir(dir_cusor,filename,&dd[2]);
		if(foundindex != -1){ FILE.close(); return foundindex; }
	}
	if(size-10 <= 256 ) return - 1;

//	cout<<"/二次间接索引  266 - 65801 "<<endl;
	Indir twoIndir; //二次间接索引  266 - 65801
	dd[0]=i_addr[11];
	FILE.seekp(BLOCKSIZE*(DATA_OFFSET+i_addr[11]),ios::beg);
	FILE.read((char *)&oneIndir,sizeof(oneIndir));
	int twoLen = size-10-256 >256*256  ? 256*256 : size-10-256;
	int firstLen  = twoLen/256+1;
	if(twoLen%256==0)  firstLen -- ;
	int secondlastlen= twoLen%256; 
	if(secondlastlen==0)  secondlastlen =256;
	for(int i=0;i<firstLen;i++) 
	{
		dd[1]= oneIndir.index[i];
		FILE.seekp(BLOCKSIZE*(DATA_OFFSET+oneIndir.index[i]),ios::beg);
		FILE.read((char *)&twoIndir,sizeof(twoIndir));	
		int secondlen = (i == firstLen -1) ? secondlastlen : 256;
		for(int j=0;j<secondlen;j++){
			dir dir_cusor; 
			dd[2] = twoIndir.index[i];
			FILE.seekp(BLOCKSIZE*(DATA_OFFSET+twoIndir.index[i]),ios::beg);
			FILE.read((char *)&dir_cusor,sizeof(dir_cusor));
			foundindex = findInDir(dir_cusor,filename,&dd[3]);
			if(foundindex != -1){ FILE.close(); return foundindex; }
		}
	}
	if(size-10-256 <=256*256  ) return - 1;

//	cout<<"//三次间接索引   65802 - 16843017 "<<endl;
	Indir threeIndir; //三次间接索引   65802 - 16843017
	FILE.seekp(BLOCKSIZE*(DATA_OFFSET+i_addr[12]),ios::beg);
	dd[0] = i_addr[12];
	FILE.read((char *)&oneIndir,sizeof(oneIndir));
	int threeLen = size-10-256 -256*256 ;
	firstLen  = threeLen/(256*256)+1;
	if(threeLen%(256*256)==0)  firstLen -- ;
	int  secondlastLen= (threeLen%(256*256))%256;
	if(secondlastLen==0)  secondlastlen =256;
	int thirdlastLen = threeLen%256;
	if(thirdlastLen ==0 ) thirdlastLen = 256;
	for(int i=0;i<firstLen;i++) 
	{
		dd[1]= oneIndir.index[i];
		FILE.seekp(BLOCKSIZE*(DATA_OFFSET+oneIndir.index[i]),ios::beg);
		FILE.read((char *)&twoIndir,sizeof(twoIndir));	
		int secondlen = (i == firstLen -1) ? secondlastlen : 256;
		for(int j=0;j<secondlen;j++){
			dd[2] = twoIndir.index[i];
			FILE.seekp(BLOCKSIZE*(DATA_OFFSET+twoIndir.index[i]),ios::beg);
			FILE.read((char *)&threeIndir,sizeof(threeIndir));	
			int thirdlen = (i == secondlen -1) ? thirdlastLen : 256;
			for(int j=0;j<thirdlen;j++){
				dir dir_cusor; 
				dd[3]= threeIndir.index[i];
				FILE.seekp(BLOCKSIZE*(DATA_OFFSET+threeIndir.index[i]),ios::beg);
				FILE.read((char *)&dir_cusor,sizeof(dir_cusor));
				foundindex = findInDir(dir_cusor,filename,&dd[4]);
				if(foundindex != -1){ FILE.close(); return foundindex; }
			}
		}
	}
	FILE.close();
	return -1;
}

int findInDir(dir dir_cusor,const char* filename,int * offset)
{
//	cout<<"findInDir"<<endl;
	for(int j=0;j<dir_cusor.size;j++)
	{
		if(strcmp(dir_cusor.SFD[j].d_name ,filename)==0) //找到
		{
			*offset = j;
			return  dir_cusor.SFD[j].d_ino;
		}

	}
	return -1;
}
