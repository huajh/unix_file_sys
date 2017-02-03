#include <iostream>
#include <fstream>
#include <time.h>
#include "UnixStruct.h"
#include "UnixFunction.h"
using namespace std;

void save()
{
//	cout<<"save"<<endl;
	fstream FILE("VirtualDisk", ios::binary|ios::in|ios::out); 
	FILE.seekp(BLOCKSIZE,ios::beg);	
	FILE.write((char *)&filsys,sizeof(filsys));
	FILE.close(); 
}

int addsubfile(int curindex,int subindex,const char* filename)
{
	unsigned short  i_addr[13];
	int  size;
	fstream FILE("VirtualDisk", ios::binary|ios::in|ios::out); 
	dinode node;
	if(curindex ==d_inoList[p_ino])  // ��ǰĿ¼
	{
		size= cur_inode._base.i_size;
		memcpy(i_addr,cur_inode._base.i_addr,sizeof(i_addr));
		node  = cur_inode._base;

	}else{
		FILE.seekp(BLOCKSIZE*(INODE_OFFSET+curindex),ios::beg);	
		FILE.read((char *)&node,sizeof(node));
		size = node.i_size;
		memcpy(i_addr,node.i_addr,sizeof(i_addr));
	}

	int insertdir[4], curlen,depth = 0;   // Ŀ¼���
	if(size ==0) 
	{ 
		depth =0;
		dir newdir;  // �½�һ��Ŀ¼
		int newindex = _alloc_data();
		newdir.size = 1;
		newdir.SFD[0].d_ino = subindex;
		memcpy(newdir.SFD[0].d_name,filename,sizeof(newdir.SFD[0].d_name));
		FILE.seekp(BLOCKSIZE*(DATA_OFFSET+newindex),ios::beg);
		FILE.write((char *)&newdir,sizeof(newdir)); 
		node.i_addr[0] = newindex; //update
		node.i_size = 1;
		FILE.seekp(BLOCKSIZE*(INODE_OFFSET+curindex),ios::beg);
		FILE.write((char *)&node,sizeof(node));  // save
		if(curindex ==d_inoList[p_ino])
			cur_inode._base = node;  //save
		return 1;
	}
	else if(size <=10){
		insertdir[0] = size-1; depth =1;
	}
	else if(size-10<=256){
		curlen = size -10;
		insertdir[0]  = 10;
		insertdir[1] =curlen-1;
		depth =2;
	}else if(size-10-256<256*256){
		curlen = size -10 - 256;
		insertdir[0]  = 11;
		insertdir[1] = curlen/256;
		if(curlen%256==0) insertdir[1] --;
		insertdir[2] = (curlen-1)%256;
		depth =3;
	}else if(size -10 -256- 256*256 < 256*256*256){
		curlen = size -10 -256- 256*256;
		insertdir[0]  = 12;
		insertdir[1] = curlen/(256*256);
		if(curlen%(256*256)==0) insertdir[1] --;
		insertdir[2] = (curlen%(256*256))/256;
		if(curlen%256==0) insertdir[2] = (insertdir[2] -1+256)%256;
		insertdir[3] = (curlen-1)%256;
		depth =4;
	}
	Indir indirs[3];
	int block[4];
	block[0] = i_addr[insertdir[0]];
	for(int i=1;i<depth;i++)
	{
		FILE.seekp(BLOCKSIZE*(DATA_OFFSET+block[i-1]),ios::beg);
		FILE.read((char *)&indirs[i-1],sizeof(indirs[i-1]));  // ���Ŀ¼��
		block[i] = indirs[i-1].index[insertdir[i]];
	}
	dir dirs; // 
	FILE.seekp(BLOCKSIZE*(DATA_OFFSET+block[depth-1]),ios::beg);
	FILE.read((char *)&dirs,sizeof(dirs));  //�ҵ����һ��Ŀ¼
	if(dirs.size < DIRSIZE)   //δ��
	{
		dirs.SFD[dirs.size].d_ino = subindex;
		strcpy(dirs.SFD[dirs.size].d_name,filename);
		dirs.size++;
		FILE.seekp(BLOCKSIZE*(DATA_OFFSET+block[depth-1]),ios::beg);
		FILE.write((char *)&dirs,sizeof(dirs)); //save
		return  1;

	}else{  //����
		if(depth ==4 && insertdir[3] == 255)
		{
			return -1 ; //��� overflow
		}
		dir newdir;  // �½�һ��Ŀ¼
		int newDirIndex = _alloc_data();
		newdir.size = 1;
		newdir.SFD[0].d_ino = subindex;

		strcpy(newdir.SFD[0].d_name,filename);
		FILE.seekp(BLOCKSIZE*(DATA_OFFSET+newDirIndex),ios::beg);
		FILE.write((char *)&newdir,sizeof(newdir)); 

		// ����Ŀ¼

		// ����Ҫ����һ����Ŀ¼��
		if((depth==1 && insertdir[0]<10) ||
			depth==2 && insertdir[1]<255 ||
			depth==3 && insertdir[2]<255||
			depth==4 && insertdir[3]<255 )
		{
			if(depth ==1){
				node.i_addr[size] = newDirIndex;
			}
			node.i_size++;	
			FILE.seekp(BLOCKSIZE*(DATA_OFFSET+curindex),ios::beg);			
			FILE.write((char *)&node,sizeof(node)); 
			if(curindex ==d_inoList[p_ino])
				cur_inode._base = node;  //update
			if(depth!=1){
				indirs[depth-2].index[insertdir[depth-1]+1] = newDirIndex;
				FILE.seekp(BLOCKSIZE*(DATA_OFFSET+block[depth-1]),ios::beg);			
				FILE.write((char *)&indirs[depth-2],sizeof(indirs[depth-2])); 
			}
			FILE.close();
			return 1;
		}

		// ��Ҫ����һ����Ŀ¼��
		Indir newIndirs;
		int newIndirsindex = _alloc_data();
		newIndirs.index[0] = newDirIndex;
		FILE.seekp(BLOCKSIZE*(DATA_OFFSET+newIndirsindex),ios::beg);			
		FILE.write((char *)&newIndirs,sizeof(newIndirs)); 

		if(depth==1)
			node.i_addr[10] = newIndirsindex;
		else if(depth==2){  //��Ҫ���ٶ���
			Indir newIndirs2;
			int newIndirsindex2 = _alloc_data();
			newIndirs2.index[0] = newIndirsindex;
			node.i_addr[11] = newIndirsindex2;
			FILE.seekp(BLOCKSIZE*(DATA_OFFSET+newIndirsindex2),ios::beg);			
			FILE.write((char *)&newIndirs2,sizeof(newIndirs2)); 
		}else if(depth==3){ 
			if(insertdir[1]<255){  //
				indirs[0].index[insertdir[1]+1] = newIndirsindex;
				FILE.seekp(BLOCKSIZE*(DATA_OFFSET+block[0]),ios::beg);			
				FILE.write((char *)&indirs[0],sizeof(indirs[0])); 
			}else { //  //��Ҫ��������
				Indir newIndirs2;
				int newIndirsindex2 = _alloc_data();
				newIndirs2.index[0] = newIndirsindex;
				Indir newIndirs3;
				int newIndirsindex3 = _alloc_data();
				newIndirs3.index[0] = newIndirsindex2;
				node.i_addr[12] = newIndirsindex3;
				FILE.seekp(BLOCKSIZE*(DATA_OFFSET+newIndirsindex2),ios::beg);			
				FILE.write((char *)&newIndirs2,sizeof(newIndirs2)); 
				FILE.seekp(BLOCKSIZE*(DATA_OFFSET+newIndirsindex3),ios::beg);			
				FILE.write((char *)&newIndirs3,sizeof(newIndirs3)); 
			}
		}else // depth =4
		{
			if(insertdir[2]<255){  //
				indirs[1].index[insertdir[2]+1] = newIndirsindex;
				FILE.seekp(BLOCKSIZE*(DATA_OFFSET+block[1]),ios::beg);			
				FILE.write((char *)&indirs[1],sizeof(indirs[1])); 
			}else if(insertdir[1]<255){
				Indir newIndirs2;
				int newIndirsindex2 = _alloc_data();
				newIndirs2.index[0] = newIndirsindex;
				indirs[0].index[insertdir[1]+1] = newIndirsindex2;
				FILE.seekp(BLOCKSIZE*(DATA_OFFSET+newIndirsindex2),ios::beg);			
				FILE.write((char *)&newIndirs2,sizeof(newIndirs2)); 
				FILE.seekp(BLOCKSIZE*(DATA_OFFSET+block[0]),ios::beg);			
				FILE.write((char *)&indirs[0],sizeof(indirs[0])); 
			}
		}
		node.i_size ++;
		FILE.seekp(BLOCKSIZE*(INODE_OFFSET+curindex),ios::beg);
		FILE.write((char *)&node,sizeof(node));  // update
		if(curindex ==d_inoList[p_ino])
			cur_inode._base = node;  //update
	}
	FILE.close();
	return 1;
}

//
// return -1 ɾ��ʧ��
// return 1 ɾ���ɹ�
//

int _del_from_parent(int parent,const char * filename)
{
	int dd[5];
	memset(dd,-1,sizeof(dd));
	int child = find(parent,filename,dd);
	if(child ==-1) return -1;
	int cc = 4;
	while(dd[cc]==-1 && cc>=0) cc--;
	int offsetInDir =dd[cc];
	dir dirs;
	fstream FILE("VirtualDisk", ios::binary|ios::in|ios::out); 
	FILE.seekp(BLOCKSIZE*(DATA_OFFSET+dd[cc-1]),ios::beg);			
	FILE.read((char *)&dirs,sizeof(dirs));  
	if(dirs.size-1>0) // Ŀ¼δ�� 
	{		
		if(offsetInDir ==dirs.size-1) dirs.size--;
		else  //�����һ������ͷŵļ�¼��
		{	
			dirs.SFD[offsetInDir].d_ino = dirs.SFD[dirs.size-1].d_ino;
			strcpy(dirs.SFD[dd[cc]].d_name ,dirs.SFD[dirs.size-1].d_name);
			dirs.size--;
		}
		FILE.seekp(BLOCKSIZE*(DATA_OFFSET+dd[cc-1]),ios::beg); //update
		FILE.write((char *)&dirs,sizeof(dirs));  
		FILE.close();
	}
	else // �ѿ�,�ͷ�Ŀ¼
	{	
		dinode parentInode = getinode(parent);
		if(cc==1)  //ֱ��
		{
			for(int i=0;i<10;i++)
				if(dd[0]==parentInode.i_addr[i])
				{
					parentInode.i_addr[i]  = -1; // ��Ч
					break;
				}
		}		
		else if(cc>=2)  // �������� ,cc 
		{
			_release_data(dd[cc-1]); // �ͷż������
			Indir indirs;
			FILE.open("VirtualDisk", ios::binary|ios::in|ios::out);
			FILE.seekp(BLOCKSIZE*(DATA_OFFSET+dd[cc-2]),ios::beg);			   //update
			FILE.read((char *)&indirs,sizeof(indirs));  
			indirs.index[dd[cc-1]] = -1; //��ʾ�ÿ�
			// �ж�indirs �ĳ���   �� // �ṹ������������size
			// //
			// ?? ?
			//
		}
		parentInode.i_size--;
		FILE.seekp(BLOCKSIZE*(INODE_OFFSET+parent),ios::beg);			
		FILE.write((char *)&parentInode,sizeof(parentInode));  
		if(parent == d_inoList[p_ino])
			cur_inode._base = parentInode;
	}
	FILE.close();
	return 0;
}

// return -1 �ļ����Ѿ�����
// return  1 �����ɹ�
int _create(char *filename)
{
	int curindex = d_inoList[p_ino];
	int index = find(curindex,filename,NULL);
	if(index !=-1)  return -1; 
	dinode newinode ;
	newinode.i_uid = curUser.u_uid;
	newinode.i_gid = curUser.u_gid;
	newinode.i_type = DIFILE; // �ļ�����
	newinode.i_mode = DEFAULTMODE;
	time(&newinode.CreateTime);
	time(&newinode.LastAccessed);
	newinode.i_size = 0;
	int newindex = _alloc_inode();
	fstream FILE("VirtualDisk", ios::binary|ios::in|ios::out); 
	FILE.seekp(BLOCKSIZE*(newindex+INODE_OFFSET), ios::beg);
	FILE.write((char*)&newinode,sizeof(newinode));		 
	FILE.close();
	int ss = addsubfile(curindex,newindex,filename);
	return ss;
}

// return 0 success
// return -1 No such Directory
// return -2  not a file but a directory
// return -3  no such mode 
// return -4 �ļ��Ѿ���
// return -5 �ļ��Ѿ��������û���write��ʽ��
// mode -r  read
// mode -w write
// mode -wr read and write
// ���ļ��Ѿ��򿪣�����Ϊwrite mode�£��������´�

int open(char *dirname,char mode)
{
//	cout<<"open : "<<dirname<<"  "<<mode<<endl;
	char ** dirList = new char*;
	int size = _filter(dirname,&dirList); // 1
	if(size == 0 )  return -1;
	int curindex = d_inoList[p_ino];
	int *IndexList = new int(size+1);
	int subindex;
	*(IndexList) = curindex;
	for(int i=0;i<size;i++)   //
	{
		char * filename = *(dirList+i);
		subindex = find(curindex,filename,NULL);
		if(subindex==-1)    return -1;// �ļ�������
		curindex = subindex;
		*(IndexList+i+1) = curindex;
	}
	dinode _cur = getinode(curindex);
	if(_cur.i_type == DIDIR) return -2;  //is a directory not file
	for(int i=0;i<curUser.p_file;i++) // �Ƿ��Ѿ���
	{
		int t = curUser.u_ofile[i]; //
		if(sys_file[i].f_inode->i_Index ==curindex)
			return -4;
	}
	bool found   = false;
	for(int i=0;i<p_file;i++)
	{
		if(sys_file[i].f_mode ==mode && sys_file[i].f_inode->i_Index==curindex)
		{
			if(sys_file[i].f_mode=='w') return -5;
			found = true;
			sys_file[i].f_count ++;  // r ģʽ������
			curUser.u_ofile[curUser.p_file++] = i;
			break;
		}
	}
	if(!found) // �½�һ��inode
	{
		time(&_cur.LastAccessed);
		inodeList[p_inode]._base = _cur;
		inodeList[p_inode].i_count=1;
		inodeList[p_inode].i_Index = curindex; //  
	//	inodeList[p_inode].i_dev = 0; //  
	//	inodeList[p_inode].i_flag = 0; //  
		sys_file[p_file].f_count ++;	
		sys_file[p_file].f_mode = mode;
		sys_file[p_file].f_offset = 0;
		sys_file[p_file].f_inode = &inodeList[p_inode]; //ָ��
		curUser.u_ofile[curUser.p_file++] = p_file;
		p_file++;
		p_inode++;
		// update visited time
		fstream FILE("VirtualDisk", ios::binary|ios::in|ios::out); 
		FILE.seekp(BLOCKSIZE*(curindex+INODE_OFFSET), ios::beg);
		FILE.write((char*)&inodeList[p_inode]._base,sizeof(inodeList[p_inode]._base));		 
		FILE.close();
	}
	return 0;
}

//return -1 not open 
//return 1 success
int _close(char *dirname)
{
	char ** dirList = new char*;
	int size = _filter(dirname,&dirList); // 1
	if(size == 0 )  return -1;
	fstream FILE("VirtualDisk", ios::binary|ios::in|ios::out); 
	int curindex = d_inoList[p_ino];
	int *IndexList = new int(size+1);
	int subindex;
	*(IndexList) = curindex;
	for(int i=0;i<size;i++) 
	{
		char * filename = *(dirList+i);
		subindex = find(curindex,filename,NULL);
		if(subindex==-1)    return -1;// �ļ�������
		curindex = subindex;
		*(IndexList+i+1) = curindex;
	}
	dinode _cur = getinode(curindex);
	if(_cur.i_type == DIDIR) return -2;  //is a directory not file

	bool canwrite = false ,found =false;
	for(int i=0;i<curUser.p_file;i++)
	{
		int t = curUser.u_ofile[i];
		if(sys_file[t].f_inode->i_Index = curindex ) 
		{
			found  =true;
			if(curUser.p_file>1) // ��ֻһ��
				curUser.u_ofile[i] = curUser.u_ofile[curUser.p_file-1];
			curUser.p_file--;
				if(--sys_file[t].f_count) // 0
			{				
				sys_file[i] = sys_file[p_file-1];
				p_file--;
			}				
			break;
		}
	}
	if(!found)  return -1;
	return 1;
}

// return -1   No such Directory
// return -2  not a file but a directory
// return 1 success
int _delete(const char * dirname)
{

	char ** dirList = new char*;
	int size = _filter(dirname,&dirList); // 1
	if(size == 0 )  return -1;
	fstream FILE("VirtualDisk", ios::binary|ios::in|ios::out); 
	int curindex = d_inoList[p_ino];
	int *IndexList = new int(size+1);
	int subindex;
	*(IndexList) = curindex;
	for(int i=0;i<size;i++)   // -p
	{
		char * filename = *(dirList+i);
		subindex = find(curindex,filename,NULL);
		if(subindex==-1)    return -1;// �ļ�������
		curindex = subindex;
		*(IndexList+i+1) = curindex;
	}
	dinode _cur = getinode(curindex);
	if(_cur.i_type==DIDIR)  return -2;  //����Ŀ¼
	if(_cur.i_size>0)  return -2;  //Ŀ¼��Ϊ��
	_release_inode(*(IndexList+size));
	// ��Ŀ¼��ɾ��	
	int parent =  *(IndexList+size-1);
	int tt = _del_from_parent(parent,dirList[size-1]);

	return 1;
}

// return -1   No such Directory
// return -2  not a file but a directory
// return -3  not open
// return -4  not write mode 

int _write(const char *dirname)
{
	char ** dirList = new char*;
	int size = _filter(dirname,&dirList); // 1
	if(size == 0 )  return -1;
	fstream FILE("VirtualDisk", ios::binary|ios::in|ios::out); 
	int curindex = d_inoList[p_ino];
	int *IndexList = new int(size+1);
	int subindex;
	*(IndexList) = curindex;
	for(int i=0;i<size;i++) 
	{
		char * filename = *(dirList+i);
		subindex = find(curindex,filename,NULL);
		if(subindex==-1)    return -1;// �ļ�������
		curindex = subindex;
		*(IndexList+i+1) = curindex;
	}
	// 
	dinode _cur = getinode(curindex);
	if(_cur.i_type == DIDIR) return -2;  //is a directory not file

	bool canwrite = false ,found =false;
	for(int i=0;i<curUser.p_file;i++)
	{
		int t = curUser.u_ofile[i];
		if(sys_file[t].f_inode->i_Index = curindex ) 
		{
			found  =true;		
			if(sys_file[t].f_mode=='w')// ���Զ�
			{
				canwrite =true;break;
			}
				
		}
	}
	if(!found)  return -3;
	if(!canwrite)  return -4;
	return _writebuf(curindex);
}

//
// �����ļ����ݣ���#����
int _writebuf(int index)
{
	char buf[512];
	char *newbuf;
	unsigned int str_capacity = 1024+5;
	printf("input whatever you want  : \n");
	fgets(buf,str_capacity,stdin);
	newbuf = new char(strlen(buf)-1);
	for(int i =0;i<strlen(buf)-1;i++)
		newbuf[i] = buf[i];

	// ����ֻд��һ��
	//int size = strlen(newbuf)/512+1;
	int newindex = _alloc_data();
	fstream FILE("VirtualDisk", ios::binary|ios::in|ios::out); 
	FILE.seekp(BLOCKSIZE*(newindex+DATA_OFFSET), ios::beg);
	FILE.write((char*)&newbuf,sizeof(char)*512);		 
	cout<<"\njust for show :  "<<newbuf<<endl;
	dinode node = getinode(index);
	node.i_addr[0] = newindex;
	node.i_size ++;
	FILE.seekp(BLOCKSIZE*(index+INODE_OFFSET), ios::beg);
	FILE.write((char*)&node,sizeof(node));		 
	FILE.close();
// 	while(fgets(buf,str_capacity,stdin)!=NULL)
// 	{
// 		if(strcmp(buf,"#\n")==0 || strcmp(buf,"#\r") )
// 			break;
// 		else{
// 
// 		}
// 	}
	return 1;
}

// return -1   No such Directory
// return -2  not a file but a directory
// return -3  not open
// return -4 nothing it the file
// return 1;
int read(const char *dirname)
{
	char ** dirList = new char*;
	int size = _filter(dirname,&dirList); // 1
	if(size == 0 )  return -1;
	fstream FILE("VirtualDisk", ios::binary|ios::in|ios::out); 
	int curindex = d_inoList[p_ino];
	int *IndexList = new int(size+1);
	int subindex;
	*(IndexList) = curindex;
	for(int i=0;i<size;i++) 
	{
		char * filename = *(dirList+i);
		subindex = find(curindex,filename,NULL);
		if(subindex==-1)    return -1;// �ļ�������
		curindex = subindex;
		*(IndexList+i+1) = curindex;
	}
	// 
	dinode _cur = getinode(curindex);
	if(_cur.i_type == DIDIR) return -2;  //is a directory not file
	bool found =false;
	for(int i=0;i<curUser.p_file;i++)
	{
		int t = curUser.u_ofile[i];
		if(sys_file[t].f_inode->i_Index == curindex ) // ���Զ�
		{
				found  =true;
				break;
		}
	}
	if(!found) return -3;
	return _readit(curindex);
}
int  _readit(int index)
{
	dinode node = getinode(index);
	if(node.i_size ==0) return -4;
	int tt = node.i_addr[0];
	char *buf = (char*)malloc(BLOCKSIZE*sizeof(char));
	fstream FILE("VirtualDisk", ios::binary|ios::in|ios::out); 
	FILE.seekp(BLOCKSIZE*(tt+DATA_OFFSET), ios::beg);
	FILE.read((char*)&buf,sizeof(buf));
	cout<<"\ncontent :  "<<buf<<endl;
	FILE.close();
	return 1;
}

// return -1   No such Directory
// return 1;
//��ʾ�ļ�����
int lsattr(const char *dirname)
{
	char ** dirList = new char*;
	int size = _filter(dirname,&dirList); // 1
	if(size == 0 )  return -1;
	fstream FILE("VirtualDisk", ios::binary|ios::in|ios::out); 
	int curindex = d_inoList[p_ino];
	int *IndexList = new int(size+1);
	int subindex;
	*(IndexList) = curindex;
	for(int i=0;i<size;i++) 
	{
		char * filename = *(dirList+i);
		subindex = find(curindex,filename,NULL);
		if(subindex==-1)    return -1;// �ļ�������
		curindex = subindex;
		*(IndexList+i+1) = curindex;
	}
	dinode _cur = getinode(curindex);

	cout<<"filename : "<<dirList[size-1]<<endl;
	cout<<"uid : "<<_cur.i_uid<<endl;
	cout<<"gid : "<<_cur.i_gid<<endl;
	cout<<"CreateTime : "<<_cur.CreateTime<<endl;
	cout<<"LastAccessed : "<<_cur.LastAccessed<<endl;
	if(_cur.i_type==DIFILE)
		cout<<"Type : File"<<endl;
	else
		cout<<"Type : Directory "<<endl;
	cout<<"Size : "<<_cur.i_size<<endl;
	return 0;
}
// 1 success
// -1 failed
struct user curUser;
unsigned int userBlockID;
bool login(char *uid,char *pwd)
{	
	fstream FILE("VirtualDisk", ios::binary|ios::in|ios::out); 
	unsigned int usercount ;
	struct user userlist[USERMAX] ;
	FILE.seekp(USER_OFFSET*BLOCKSIZE,ios::beg);
	FILE.read((char*)&usercount,sizeof(usercount));
	FILE.seekp(sizeof(usercount),ios::cur);
	FILE.read((char*)&userlist,sizeof(userlist));
	FILE.close();
	for(int i=0;i<usercount;i++)
	{
		if(strcmp(userlist[i].uid,uid)==0 &&strcmp(userlist[i].pwd,pwd)==0  )
		{
			memcpy(&curUser,&userlist[i],sizeof(curUser));
			userBlockID = i;
			curUser.p_file = 0;
			return true;
		}
	}
	return false;
}

int logout()
{
	printf("ע���ɹ���\n");
	curUser.u_uid =0;
	curUser.u_gid = 0;
	//curUser.u_ofile[];
	curUser.p_file = 0;
	return 0;
}
void exit()
{

}
int Register()
{
	return 0 ;
}

struct dinode getinode(int index)
{
	dinode node;
	fstream FILE("VirtualDisk", ios::binary|ios::in|ios::out); 
	FILE.seekp((INODE_OFFSET+index)*BLOCKSIZE,ios::beg);
	FILE.read((char*)&node,sizeof(node));
	FILE.close();
	return node;
}

//  
// return -1 destination ��ַ����
// return -2 destination ����ΪĿ¼
// return -3 source��ַ����
// return -4 are the same file
// return  1 success
// cp -i a/1 a/2 c/  //�ļ��Ŀ��� �� �ļ�������c/Ŀ¼��
// cp -r a c   // Ŀ¼�Ŀ��� �� aĿ¼copy��cĿ¼��
// ��Ե�ǰĿ¼
int copy(char* destination, char* source)  // 
{
	char **dirList1 = new char*;
	int len1 = _filter(destination,&dirList1);
	if(len1==0)  return -1;
	int curindex = d_inoList[p_ino] ,subindex;
	for(int i=0;i<len1;i++)
	{
		subindex = find(curindex,dirList1[i],NULL); 
		if(subindex == -1) return -1;
		curindex = subindex;
	} //�ҵ���Ӧ���ļ�
	dinode node = getinode(curindex);
	if(node.i_type == DIFILE) return -2;// Ŀ���ļ�����ΪĿ¼

	char **dirList2 = new char*;
	int len2 = _filter(source,&dirList2);
	if(len2==0)  return -3;
	int curindex2 = d_inoList[p_ino] ,subindex2;
	int prev ;
	for(int i=0;i<len1;i++)
	{
		prev = curindex2;
		subindex2 = find(curindex2,dirList2[i],NULL); 
		if(subindex2 == -1) return -3;
		curindex2 = subindex2;
	}
	if(prev ==curindex)  // ��Ŀ���ļ�����
		return -4;
	dinode node2 = getinode(curindex2);
	dinode newnode;
	if(node2.i_type == DIFILE)  //�ļ�����
	{
		newnode = Filecopy(node2);
	}else  {  // Ŀ¼�ݹ鿽��
		newnode = DirCopy(node2 );
	}
	int newindex = _alloc_inode();
	fstream FILE("VirtualDisk", ios::binary|ios::in|ios::out); 
	FILE.seekp(BLOCKSIZE*(INODE_OFFSET+newindex),ios::beg);		
	FILE.write((char *)&newnode,sizeof(newnode));
	addsubfile(curindex,newindex,dirList2[len2-1]);
	return 1;
}
dinode DirCopy(dinode node )
{
	dinode newinode ;
	newinode.i_uid = curUser.u_uid;
	newinode.i_gid = curUser.u_gid;
	newinode.i_mode = DEFAULTMODE;
	time(&newinode.CreateTime);
	time(&newinode.LastAccessed);
	newinode.i_type = node.i_type;
	int size = node .i_size;
	newinode.i_size = size;

	fstream FILE;

	//ֱ��
	int size1 = size>10 ? 10 :size; // 
	for(int i=0;i<size1;i++)
	{
		int to = _alloc_data(); /// Ŀ¼������
		newinode.i_addr[i] = to;		
		dir fromdirs ,todirs;
		FILE.open("VirtualDisk", ios::binary|ios::in|ios::out); 
		FILE.seekp(BLOCKSIZE*(DATA_OFFSET+node.i_addr[i]),ios::beg);		
		FILE.read((char *)&fromdirs,sizeof(fromdirs));
		FILE.close();
		todirs.size = fromdirs.size;
		for(int j= 0;j<fromdirs.size;j++){
			strcpy(todirs.SFD[j].d_name , fromdirs.SFD[j].d_name);
			int i_Index = _alloc_inode();
			todirs.SFD[j].d_ino = i_Index;
			dinode subinode = getinode(fromdirs.SFD[j].d_ino);
			if(subinode.i_type == DIFILE)   //�ļ�
			{
				FILE.open("VirtualDisk", ios::binary|ios::in|ios::out); 
				dinode newsubnode = Filecopy(subinode);				
				FILE.seekp(BLOCKSIZE*(INODE_OFFSET+i_Index),ios::beg);		
				FILE.write((char *)&newsubnode,sizeof(newsubnode));
				FILE.close();
			}
			else{  //Ŀ¼
				dinode newDir = DirCopy(subinode );  //�ݹ�
				FILE.open("VirtualDisk", ios::binary|ios::in|ios::out); 
				FILE.seekp(BLOCKSIZE*(INODE_OFFSET+i_Index),ios::beg);		
				FILE.write((char *)&newDir,sizeof(newDir));
				FILE.close();
			}
		}
		FILE.open("VirtualDisk", ios::binary|ios::in|ios::out); 
		FILE.seekp(BLOCKSIZE*(DATA_OFFSET+to),ios::beg);		
		FILE.write((char *)&todirs,sizeof(todirs));
		FILE.close();
	}
	if(size <=10) return newinode;
	// һ�μ��...

	// ���μ��...

	// ���μ��...
	//return NULL;
}

dinode Filecopy(dinode node)  //�ļ�����
{
	dinode newinode ;
	newinode.i_uid = curUser.u_uid;
	newinode.i_gid = curUser.u_gid;
	newinode.i_mode = DEFAULTMODE;
	time(&newinode.CreateTime);
	time(&newinode.LastAccessed);
	newinode.i_type = node.i_type;
	int size = node .i_size;
	newinode.i_size = size;
	//ֱ��
	int size1 = size>10 ? 10 :size; // 
	for(int i=0;i<size1;i++)
	{
		int to = _alloc_data();
		newinode.i_addr[i] = to;
		blockTransfor(to,node.i_addr[i]);		
	}
	if(size <=10) return newinode;

	fstream FILE;
	// һ�μ��
	int size2 = size -10 >256 ? 256 : size - 10;
	Indir todirs , fromdirs; 
	FILE.open("VirtualDisk", ios::binary|ios::in|ios::out); 
	FILE.seekp(BLOCKSIZE*(DATA_OFFSET+node.i_addr[10]),ios::beg);		
	FILE.read((char *)&fromdirs,sizeof(fromdirs));
	for(int i=0;i<size2;i++)
	{
		int to = _alloc_data();
		blockTransfor(to,fromdirs.index[i]);
		todirs.index[i] = to;
	}
	newinode.i_addr[10] = _alloc_data();
	FILE.seekp(BLOCKSIZE*(DATA_OFFSET+newinode.i_addr[10]),ios::beg);		
	FILE.write((char *)&todirs,sizeof(todirs));
	if(size <=10+256) return newinode;

	//// ���μ��
	int len2 = (size -1)-10 - 256 > 256*256 ? 256*256 : (size -1)-10 - 256;
	int a = len2/256;
	int b = len2%256;
	FILE.open("VirtualDisk", ios::binary|ios::in|ios::out); 
	FILE.seekp(BLOCKSIZE*(DATA_OFFSET+node.i_addr[11]),ios::beg);		
	FILE.read((char *)&fromdirs,sizeof(fromdirs));
	for(int i=0;i<=a;i++)
	{
		Indir fromdir2,todirs2;
		FILE.seekp(BLOCKSIZE*(DATA_OFFSET+fromdirs.index[i]),ios::beg);
		FILE.write((char *)&fromdir2,sizeof(fromdir2));
		int lenb = (i==a) ?  b : 255;
		for(int j=0;j<=lenb;j++)
		{
			int to2 = _alloc_data();
			blockTransfor(to2,fromdir2.index[i]);
			todirs2.index[i] = to2;
		}
		int to = _alloc_data();
		todirs2.index[i] = to;
		FILE.seekp(BLOCKSIZE*(DATA_OFFSET+to),ios::beg);
		FILE.write((char *)&todirs2,sizeof(todirs2));
	}
	int to = _alloc_data();
	newinode.i_addr[11] = to;
	FILE.seekp(BLOCKSIZE*(DATA_OFFSET+to),ios::beg);
	FILE.write((char *)&todirs,sizeof(todirs));
	if(size <=10+256+256*256) return newinode;

	///  ���μ��


}

void blockTransfor(int to,int from)
{
	fstream FILE("VirtualDisk", ios::binary|ios::in|ios::out); 
	char  buf[256];
	FILE.seekp(BLOCKSIZE*(DATA_OFFSET+from),ios::beg);		
	FILE.read((char *)&buf,sizeof(buf));
	FILE.seekp(BLOCKSIZE*(DATA_OFFSET+to),ios::beg);		
	FILE.write((char *)&buf,sizeof(buf));
}


const char * _command[15]={"mkdir","install","format","ls","cd","rmdir","create","open","write","rename","read","help"};
