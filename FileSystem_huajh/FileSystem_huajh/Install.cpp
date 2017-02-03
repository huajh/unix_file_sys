#include <iostream>
#include <fstream>
#include <Windows.h>
#include <time.h>
#include "UnixStruct.h"
#include "UnixFunction.h"
using namespace std;

//��װ
void install()
{
	cout << "���ڰ�װ�ļ�ϵͳ... ..." << endl;
	fstream FILE("VirtualDisk", ios::binary|ios::in|ios::out); 

	SuperBlock super;

	// #2-#513
	super.s_isize = INODEBLOCKNUM;
	//#517-#65535
	super.s_fsize = DATABLOCKNUM;

	super.s_tfree = super.s_fsize;
	super.s_tinode = super.s_isize;

	free_stack Fstack;

	// init ��ʼ������inode�� ,50��Ϊһ��
	//��һ��49��
	// #2-50,#51-100,#101-150,#151-200,#201-250 ,..

	int groupcount =(INODEBLOCKNUM +1)/GROUPSIZE;	  //����  ��
	if((INODEBLOCKNUM +1)%GROUPSIZE!=0)
		groupcount++;
	int cursor =  -1;
//	cout<<"inode group: "<<groupcount<<endl;
	for(int i=1;i<=groupcount;i++){
		Fstack.s_nfree = GROUPSIZE;
		if(i==groupcount && ( INODEBLOCKNUM +1 )%GROUPSIZE !=0)  ///���һ��
			Fstack.s_nfree =( INODEBLOCKNUM +1 )%GROUPSIZE;

		for(int j=0;j<Fstack.s_nfree;j++)
			Fstack.s_free[j] = cursor ++;
		if(i==1)
			Fstack.s_free[0]= -1;  // ��
		if(i!=groupcount){
			FILE.seekp(BLOCKSIZE*(cursor+INODE_OFFSET), ios::beg);
			FILE.write((char*)&Fstack,sizeof(Fstack));
		}else{   //���һ�� д�볬����
			super.s_ninode = Fstack.s_nfree;
			memcpy(super.s_inode,Fstack.s_free,sizeof(Fstack.s_free));
		}
	}

	// init ��ʼ������datablock�� ,50��Ϊһ��
	//��һ��49��
	// #517-565,#566-615 ,..

	groupcount =(DATABLOCKNUM +1)/GROUPSIZE;	  //����  ��
	if((DATABLOCKNUM +1)%GROUPSIZE!=0)
		groupcount++;
	cursor =  -1;
//	cout<<"datablock group: "<<groupcount<<endl;
	for(int i=1;i<=groupcount;i++){
		Fstack.s_nfree = GROUPSIZE;
		if(i==groupcount && ( DATABLOCKNUM +1 )%GROUPSIZE !=0)  ///���һ��
			Fstack.s_nfree =( DATABLOCKNUM +1 )%GROUPSIZE;

		for(int j=0;j<Fstack.s_nfree;j++)
			Fstack.s_free[j] = cursor ++;
		if(i==1)
			Fstack.s_free[0]= -1;  // ��
		if(i!=groupcount){
			FILE.seekp(BLOCKSIZE*(cursor+DATA_OFFSET), ios::beg);
			FILE.write((char*)&Fstack,sizeof(Fstack));
		}else{   //���һ�� д�볬����
			super.s_nfree = Fstack.s_nfree;
			memcpy(super.s_free,Fstack.s_free,sizeof(Fstack.s_free));
		}
	}

	super.s_flock =UNLOCK;
	super.s_ilock = UNLOCK;

	super.s_fmod = UNMODEIFIED;
	super.s_ronly = CANWRITE;

	time(&super.s_time);//��ȡ��ǰϵͳʱ��

	strcpy(super.s_fname,"super");
	super.s_type = USR;

	FILE.seekp(BLOCKSIZE, ios::beg); //��䳬������ 
	FILE.write((char*)&super, sizeof(super));

	struct dinode MFD;
	MFD.i_uid = 1;
	MFD.i_gid = 1;
	MFD.i_type =DIDIR; //Ŀ¼
	MFD.i_nlink = 0;
	MFD.i_mode = DEFAULTMODE;
	MFD.i_size = 0;
	time(&MFD.LastAccessed); 
	//	time(&MFD.LastModified); 
	time(&MFD.CreateTime);

	FILE.seekp(MFD_OFFSET*BLOCKSIZE, ios::beg); //
	FILE.write((char*)&MFD,sizeof(MFD));

	struct user admin;  //�����û�
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

	cout << "��װ�ɹ���" << endl;
}