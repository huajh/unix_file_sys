#include <iostream>
#include <fstream>
#include <Windows.h>
#include <time.h>
#include "UnixStruct.h"
#include "UnixFunction.h"
using namespace std;

//安装
void install()
{
	cout << "正在安装文件系统... ..." << endl;
	fstream FILE("VirtualDisk", ios::binary|ios::in|ios::out); 

	SuperBlock super;

	// #2-#513
	super.s_isize = INODEBLOCKNUM;
	//#517-#65535
	super.s_fsize = DATABLOCKNUM;

	super.s_tfree = super.s_fsize;
	super.s_tinode = super.s_isize;

	free_stack Fstack;

	// init 初始化空闲inode块 ,50块为一组
	//第一组49块
	// #2-50,#51-100,#101-150,#151-200,#201-250 ,..

	int groupcount =(INODEBLOCKNUM +1)/GROUPSIZE;	  //组数  ？
	if((INODEBLOCKNUM +1)%GROUPSIZE!=0)
		groupcount++;
	int cursor =  -1;
//	cout<<"inode group: "<<groupcount<<endl;
	for(int i=1;i<=groupcount;i++){
		Fstack.s_nfree = GROUPSIZE;
		if(i==groupcount && ( INODEBLOCKNUM +1 )%GROUPSIZE !=0)  ///最后一组
			Fstack.s_nfree =( INODEBLOCKNUM +1 )%GROUPSIZE;

		for(int j=0;j<Fstack.s_nfree;j++)
			Fstack.s_free[j] = cursor ++;
		if(i==1)
			Fstack.s_free[0]= -1;  // 空
		if(i!=groupcount){
			FILE.seekp(BLOCKSIZE*(cursor+INODE_OFFSET), ios::beg);
			FILE.write((char*)&Fstack,sizeof(Fstack));
		}else{   //最后一组 写入超级块
			super.s_ninode = Fstack.s_nfree;
			memcpy(super.s_inode,Fstack.s_free,sizeof(Fstack.s_free));
		}
	}

	// init 初始化空闲datablock块 ,50块为一组
	//第一组49块
	// #517-565,#566-615 ,..

	groupcount =(DATABLOCKNUM +1)/GROUPSIZE;	  //组数  ？
	if((DATABLOCKNUM +1)%GROUPSIZE!=0)
		groupcount++;
	cursor =  -1;
//	cout<<"datablock group: "<<groupcount<<endl;
	for(int i=1;i<=groupcount;i++){
		Fstack.s_nfree = GROUPSIZE;
		if(i==groupcount && ( DATABLOCKNUM +1 )%GROUPSIZE !=0)  ///最后一组
			Fstack.s_nfree =( DATABLOCKNUM +1 )%GROUPSIZE;

		for(int j=0;j<Fstack.s_nfree;j++)
			Fstack.s_free[j] = cursor ++;
		if(i==1)
			Fstack.s_free[0]= -1;  // 空
		if(i!=groupcount){
			FILE.seekp(BLOCKSIZE*(cursor+DATA_OFFSET), ios::beg);
			FILE.write((char*)&Fstack,sizeof(Fstack));
		}else{   //最后一组 写入超级块
			super.s_nfree = Fstack.s_nfree;
			memcpy(super.s_free,Fstack.s_free,sizeof(Fstack.s_free));
		}
	}

	super.s_flock =UNLOCK;
	super.s_ilock = UNLOCK;

	super.s_fmod = UNMODEIFIED;
	super.s_ronly = CANWRITE;

	time(&super.s_time);//获取当前系统时间

	strcpy(super.s_fname,"super");
	super.s_type = USR;

	FILE.seekp(BLOCKSIZE, ios::beg); //填充超级块区 
	FILE.write((char*)&super, sizeof(super));

	struct dinode MFD;
	MFD.i_uid = 1;
	MFD.i_gid = 1;
	MFD.i_type =DIDIR; //目录
	MFD.i_nlink = 0;
	MFD.i_mode = DEFAULTMODE;
	MFD.i_size = 0;
	time(&MFD.LastAccessed); 
	//	time(&MFD.LastModified); 
	time(&MFD.CreateTime);

	FILE.seekp(MFD_OFFSET*BLOCKSIZE, ios::beg); //
	FILE.write((char*)&MFD,sizeof(MFD));

	struct user admin;  //超级用户
	strcpy(admin.uid,"huajh");
	strcpy(admin.pwd,"123");
	admin.u_defalut_mode = DEFAULTMODE;
	admin.u_uid =1;
	admin.u_gid =1;

	unsigned int UserCount = 1;

	FILE.seekp(USER_OFFSET*BLOCKSIZE, ios::beg); //
	FILE.write((char*)&UserCount,sizeof(UserCount));
	FILE.seekp(sizeof(UserCount), ios::cur); //
	FILE.write((char*)&admin, sizeof(admin));

	FILE.close();

	cout << "安装成功！" << endl;
}