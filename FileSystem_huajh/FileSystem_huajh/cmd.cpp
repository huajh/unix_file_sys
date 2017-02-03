#include <iostream>
#include <fstream>
#include <time.h>
#include "UnixStruct.h"
#include "UnixFunction.h"
using namespace std;
void onecmd(char **p)
{
	char *c = *p;
	if(strcmp(c,"ls")==0){
		ls();
	}else if(strcmp(c,"--help")==0 || strcmp(c,"help")==0 || strcmp(c,"-help")==0)
	{
		help();
	}
	else if(strcmp(c,"install") ==0)
	{
		install();
	}
	else if(strcmp(c,"logout") ==0)
	{
		logout();
	}
	else if(strcmp(c,"exit") ==0)
	{
		exit();
	}
	else {
		printf("base:  %s:command not found \n",c);
	}
}

void twocmd(char **p)
{
	char *c =*p;
	char *one = *(p+1);
	if(strcmp(one,"--help")==0)
	{
		int t=  help(c);
		if(t == -1)
			printf("bash: help :no help topics math '%s'. \n",one);
	}
	else if(strcmp(c,"cd")==0)
	{
		int t = cd(one);
		if(t==-1) 
			printf("bash: cd %s: No such file or directory\n",one);
		else if(t==-2) 
			printf("bash: cd %s:No such host or network path\n",one);
		else if(t==-3)
			printf(" bash: cd : %s : Not a directory",one);
	}else if(strcmp(c,"mkdir")==0)
	{
		int t = mkdir(one,0); // 不可多级
		if(t==-1)
			printf("bash:  %s:No such Directory \n",c);
		else if (t==-2)
			printf("%s:cannot create directory '%s' : No such file or directory \n",c,one);
		else if(t==-3)
			printf("%s:cannot create directory '%s' : Such file or directory has existed \n",c,one);
		else if(t== -4)
			printf("base:  %s:System error : overflow ! \n",c);

	}else if(strcmp(c,"rmdir")==0)
	{
		int t = rmdir(one,0);
		if(t==-1)
			printf("bash:  %s:No such directory \n",c);
		else if(t==-2)
			printf("bash:  %s:Directory not empty \n",c);
		else if(t==-3)
			printf("bash:  %s:is a file , not directory\n",c);
	}else if(strcmp(c,"create")==0) 
	{
		int t = _create(one);
		if(t == -1) printf("%s:cannot create file '%s' : Such file or directory has existed \n",c,one);
	}else if(strcmp(c,"del")==0)
	{
		int t = _delete(one);
		if(t==-1) printf("bash: %s : '%s' : cannot delete file : No such file \n",c,one);
	}else if(strcmp(c,"close")==0)
	{
		int t = _close(one);
		if(t==-1) printf("bash: %s : '%s' : cannot close file : You have not open file \n",c,one);	
	}else if(strcmp(c,"lsattr")==0){
		int t = lsattr(one);
	}else if(strcmp(c,"write")==0){
		int t = _write(one);
		// return -1   No such file
		// return -2  not a file but a directory
		// return -3  not open
		// return -4  not write mode 
		if(t==-1) printf("bash: %s : '%s' : cannot write file  : No such directory \n",c,one);
		if(t==-2) printf("bash: %s : '%s' : cannot write file  : not a file but a directory\n",c,one);
		if(t==-3) printf("bash: %s : '%s' : cannot write file  : you have not open the file \n",c,one);
		if(t==-4) printf("bash: %s : '%s' : cannot write file  : the current mode is read only . \n",c,one);

	}else if(strcmp(c,"read")==0)
	{
		int t = read(one);
		// return -1   No such Directory
		// return -2  not a file but a directory
		// return -3  not open
		// return -4 nothing it the file
		// return 1;
		if(t==-1) printf("bash: %s : '%s' : cannot read file  : No such directory \n",c,one);
		if(t==-2) printf("bash: %s : '%s' : cannot read file  : not a file but a directory\n",c,one);
		if(t==-3) printf("bash: %s : '%s' : cannot read file  : you have not open the file \n",c,one);
		if(t==-4) printf("bash: %s : '%s'  nothing in the file . \n",c,one);
	}
	else 
		printf("bash:  %s:command not found \n",c);
}

void threecmd(char **p)
{
	char *c = *p;
	char  * one = *(p+1);
	char * two = *(p+2);
	if(strcmp(c,"rename")==0)
	{
		int t = _rename(one,two);
		if(t==-1) 
		{
			printf(" %s: cannot stat '%s' :No such file or directory .\n",c,one);
		}else if(t== -2){
			printf(" %s: cannot rename '%s' :'%s' file or directory has existed . \n",c,one,two);
		}
	}else if(strcmp(c,"mkdir")==0)
	{
		if(strcmp(one,"-p")==0) // 
		{
			int t = mkdir(two,1);
			if(t==-1)
				printf("bash:  %s:No such Directory \n",c);
			else if(t==-3)
				printf("%s:cannot create directory '%s' : Such file or directory has existed \n",c,one);
			else if(t== -4)
				printf("bash:  %s:System error : overflow ! \n",c);
		}
		else if(strcmp(one,"-s")==0){ 
			int t = mkdir(two,2);
		}else
		{
			if(one[0]=='-')
				printf("%s: invalid option %s \n Try '%s --help' for more information'\n",c,one,c);
			else 
			{
				int  t =	mkdir(one,0);
				if(t==-1)
					printf("bash:  %s:No such Directory \n",c);
				else if (t==-2)
					printf("%s:cannot create directory '%s' : No such file or directory \n",c,one);
				else if(t==-3)
					printf("%s:cannot create directory '%s' : Such file or directory has existed \n",c,one);
				else if(t== -4)
					printf("bash:  %s:System error : overflow ! \n",c);
			}
			
		}
	}else if(strcmp(c,"rmdir")==0)
	{
		if(strcmp(one,"-p")==0)
		{
			int t = rmdir(two,1);
			if(t==-1)
				printf("bash:  %s:No such directory \n",c);
			else if(t==-2)
				printf("bash:  %s:Directory not empty \n",c);
			else if(t==-3)
				printf("bash:  %s:is a file , not directory\n",c);
		}
		else {
			if(one[0]=='-')
				printf("%s: invalid option %s \n Try '%s --help' for more information'\n",c,one,c);
			else 
			{
				int  t = rmdir(one,0);
				if(t==-1)
					printf("bash:  %s:No such directory \n",c);
				else if(t==-2)
					printf("bash:  %s:Directory not empty \n",c);
				else if(t==-5)
					printf("%s : cannot create directory '%s' : Not a directory",c,two);
			}
		}
	}else if(strcmp(c,"copy")==0) // copy
	{
		//  
		// return -1 destination 地址有误
		// return -2 destination 必须为目录
		// return -3 source地址有误
		// return -4 are the same file
		// return  1 success
		// cp -i a/1 a/2 c/  //文件的拷贝 ： 文件拷贝到c/目录下
		// cp -r a c   // 目录的拷贝 ： a目录copy到c目录下
		// 针对当前目录
		int t= copy(two,one);  //
		if(t == -1) 	printf("%s : cannot stat '%s' : No such file or directory",c,two);
		if(t == -2) 	printf("%s :  '%s ' destination should be directory ",c,two);
		if(t == -3) 	printf("%s: cannot stat '%s' : No such file or directory ",c,one);
		if(t == -4) 	printf("%s:  '%s'  and '%s' are the same file ",c,two,two);
	}else if(strcmp(c,"mv")==0)
	{
		int t= _move(one,two);  //
		if(t == -1) 	printf("%s : cannot stat '%s' : No such file or directory",c,two);
		if(t == -2) 	printf("%s :  '%s ' destination should be directory ",c,two);
		if(t == -3) 	printf("%s: cannot stat '%s' : No such file or directory ",c,one);
		if(t == -4) 	printf("%s:  '%s'  and '%s' are the same file ",c,two,two);
	}
	else if(strcmp(c,"open")==0)
	{
		char mode ;
		if(strcmp(one,"-w") ==0)  mode ='w';
		else if(strcmp(one,"-r")==0) mode ='r';
		else {
			printf("%s: invalid option %s \n Try '%s --help' for more information'\n",c,one,c);
			return;
		}			
		int t = open(two,mode);	
		// return 0 success
		// return -1 No such Directory
		// return -2  not a file but a directory
		// return -4 文件已经打开
		// return -5 文件已经被其它用户已write方式打开
		// mode -r  read
		// mode -w write
		// mode -wr read and write
		// 若文件已经打开，并且为write mode下，则不能重新打开
		if(t==-1) printf("%s : cannot open file '%s' : Not a file or directory",c,two);
		if(t==-2) printf("%s : cannot open file '%s' : Not a file but  directory",c,two);
		if(t==-4) printf("%s : cannot open file again '%s' : You have open the file ",c,two);
		if(t==-5) printf("%s : cannot open file '%s' : has been opened by other user  ",c,two);
	}else {
		printf("bash:  %s:command not found \n",c);
	}
}


// return -1 : no such file or drectory
// return -2 : no such host or network path
// return -3 : Not a directory
// return  0 后退
// return 前进步数 

int cd(const char* dirname)
{	
	if(strcmp(dirname,"..")==0)
	{// 后退
		if(p_ino>0) p_ino--;
		fstream FILE("VirtualDisk", ios::binary|ios::in|ios::out); 
		FILE.seekp(BLOCKSIZE*(INODE_OFFSET+d_inoList[p_ino]),ios::beg);			
		FILE.read((char *)&cur_inode._base,sizeof(cur_inode._base));  // update		
		//	free(*(curDir+dirLen));
		//	delete *(curDir+dirLen);
		dirLen --;
		return 0;
	}
	char **dirList = new char*;
	int size;
	if(*(dirname) =='/') 
		size = _filter(dirname+1,&dirList); // 除去第一个字符
	else 
		size = _filter(dirname,&dirList);
	if(size==0)  return -2;
	int curindex = d_inoList[p_ino], subindex; 
	int *tmpIndex = new int(size);
	for(int i=0;i<size;i++)   // -p
	{
		char * filename = *(dirList+i);
		dinode newinode ;		 
		subindex = find(curindex,filename,NULL);
		if(subindex==-1)    // 文件不存在
			return -1;
		dinode node = getinode(subindex);
		if(node.i_type == DIFILE) //文件类型
			return -3;
		*(tmpIndex+i) = subindex;
		curindex = subindex;
	}
	//success
	for(int i=0;i<size;i++){
		d_inoList[++p_ino] = *(tmpIndex+i);
		*(curDir+dirLen) = (char*) malloc(sizeof(*(dirList+i)));
		//	*(curDir+dirLen)  = new char(sizeof(*(dirList+i)));
		strcpy(*(curDir+dirLen),*(dirList+i));
		dirLen++;
	}
	fstream FILE("VirtualDisk", ios::binary|ios::in|ios::out); 
	FILE.seekp(BLOCKSIZE*(INODE_OFFSET+d_inoList[p_ino]),ios::beg);			
	FILE.read((char *)&cur_inode._base,sizeof(cur_inode._base));  // update		
	return  size;
}

void lsDir()
{
	if(dirLen == 1) printf("/");
	else 
		for(int i=1;i<dirLen;i++)
			printf("/%s",*(curDir+i));
//	printf("  \t%d\t%d",p_ino,d_inoList[p_ino]);
}

void ls(){  // 文件目录遍历

	//	cout<<"ls"<<endl;
	int size = cur_inode._base.i_size;
	unsigned short i_addr[13];
	memcpy(i_addr,cur_inode._base.i_addr,sizeof(i_addr));
	fstream FILE("VirtualDisk", ios::binary|ios::in|ios::out); 
	int len1 =  size <10 ? size : 10;
	for(int i=0;i<len1;i++)
	{
		dir dirs;
		FILE.seekp(BLOCKSIZE*(DATA_OFFSET+i_addr[i]),ios::beg);			
		FILE.read((char *)&dirs,sizeof(dirs)); 
		for(int i=0;i<dirs.size;i++)
		{
			printf("%-14s",dirs.SFD[i].d_name);
			if((i+1)%5==0) puts("");
		}
		puts("");
	}
	cout<<endl;
	//.... .. 
	//
	FILE.close();
}
// return -1 destination 地址有误
// return -2 destination 必须为目录
// return -3 source地址有误
// return -4 are the same file
// return  1 success
// 针对当前目录
int _move(const char* source,const char *destination)
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
	} //找到对应的文件
	dinode node = getinode(curindex);
	if(node.i_type == DIFILE) return -2;// 目标文件必须为目录

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
	if(prev ==curindex)  // 在目标文件夹中
		return -4;
//	dinode node2 = getinode(curindex2);
//	dinode newnode;
//	if(node2.i_type == DIFILE)  //文件拷贝
//	{
//		newnode = Filecopy(node2);
//	}else  {  // 目录递归拷贝
//		newnode = DirCopy(node2 );
//	}
//	int newindex = _alloc_inode();
//	fstream FILE("VirtualDisk", ios::binary|ios::in|ios::out); 
//	FILE.seekp(BLOCKSIZE*(INODE_OFFSET+newindex),ios::beg);		
//	FILE.write((char *)&newnode,sizeof(newnode));
	_del_from_parent(prev,dirList2[len2-1]);
	addsubfile(curindex,curindex2,dirList2[len2-1]);
	return 1;

}

// -p mode = 1  
// 此时若路径中的某些目录尚不存在，加上此选项后，系统将自动建立好那些尚不存在的目录，即一次可以建立多个目录。
// ' ' mode = 0  不能建立多个目录，目录不能存在
// return -1 表示创建失败,目录不合法
// return -2 表示创建单目录失败
// return -3 文件已经存在
// return -4 overflow
// return -5 目录不能为文件类型
// return  1 创建成功

int mkdir(const char* dirname,unsigned short mode)
{
	char ** dirList = new char*;
	int size = _filter(dirname,&dirList);
	//	cout<<"_filter size :"<<size<<endl;
	if(size == 0 )  return -1;
	int curindex = d_inoList[p_ino];
	if(mode ==0)  
	{
		if(size !=1)  return -2;
		char * filename = *dirList;
		int index2 = find(curindex,filename,NULL);
		if(index2 !=-1)  return -3;  //文件已经存在
		dinode newinode ;
		newinode.i_uid = curUser.u_uid;
		newinode.i_gid = curUser.u_gid;
		newinode.i_type = DIDIR; // 目录
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
		if(ss == -1)  return -4;
	}else{
		int subindex;		
		for(int i=0;i<size;i++)   // -p
		{
			char * filename = *(dirList+i);
			subindex = find(curindex,filename,NULL);
			if(i<size-1)
			{
				dinode node = getinode(subindex);
				if(node.i_type == DIFILE)
					return -5; //目录不能为文件类型
			}
			if(subindex==-1)    // 文件不存在
			{				
				dinode newinode ;
				newinode.i_uid = curUser.u_uid;
				newinode.i_gid = curUser.u_gid;
				newinode.i_type = DIDIR; // 目录
				newinode.i_mode = DEFAULTMODE;
				time(&newinode.CreateTime);
				time(&newinode.LastAccessed);
				newinode.i_size = 0;
				subindex = _alloc_inode();
				fstream FILE("VirtualDisk", ios::binary|ios::in|ios::out); 
				FILE.seekp(BLOCKSIZE*(subindex+INODE_OFFSET), ios::beg);				
				FILE.write((char*)&newinode,sizeof(newinode));		 
				FILE.close();
				int ss = addsubfile(curindex,subindex,filename);
				if(ss == -1) return -4;
			}
			curindex = subindex;
		}
	}
	return 1;
}

// mode
// 0 none  目录为空则删除
//  1 -p    删除所有已经为空的父目录
//  2 -s     当使用-p 选项时，出现错误不提示 
// return -1 No such Directory
// return -2 Directory not empty
// return -3 is a file , not directory
// return 1 成功

int rmdir(const char* dirname,unsigned short mode)
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
		if(subindex==-1)    return -1;// 文件不存在
		curindex = subindex;
		*(IndexList+i+1) = curindex;
	}
	dinode _cur = getinode(curindex);
	if(_cur.i_type==DIFILE)  return -3;  //不是目录
	if(_cur.i_size>0)  return -2;  //目录不为空
	_release_inode(*(IndexList+size));
	// 父目录中删除	
	int parent =  *(IndexList+size-1);
	int tt = _del_from_parent(parent,dirList[size-1]);
	if(mode >0)  // 删除空的父目录 
	{
		dinode node = getinode(parent);
		int cc = size -2;
		while(node.i_size==0 && cc >=0){
			parent = *(IndexList+cc);
			_del_from_parent(parent,dirList[cc]);
			cc--;			
		}
	}
	return 1;
}


// 重命名
//return -1 源文件不存在
// return -2 文件名已存在
// return 1 success

int _rename(const char * oldFilename, const char * newFilename) 
{
	int dd[5];
	memset(dd,-1,sizeof(dd));
	int oldf = find(d_inoList[p_ino],oldFilename,dd);
	if(oldf==-1) return -1;
	int newf = find(d_inoList[p_ino],newFilename,NULL);
	if(newf != -1) return -2;
	int cc = 5;
	while(cc--)
		if(dd[cc]!=-1)
			break;
	dir dirs;
	fstream FILE("VirtualDisk", ios::binary|ios::in|ios::out); 
	FILE.seekp(BLOCKSIZE*(DATA_OFFSET+dd[cc-1]),ios::beg);			
	FILE.read((char *)&dirs,sizeof(dirs));  
	strcpy(dirs.SFD[dd[cc]].d_name,newFilename);		
	FILE.seekp(BLOCKSIZE*(DATA_OFFSET+dd[cc-1]),ios::beg);			
	FILE.write((char *)&dirs,sizeof(dirs));  
	FILE.close();
	return 1;
}

int help()
{
	cout<<"mkdir: \n"<<"\t[语法]: mkdir  [-p] 目录名  "<<endl;
	cout<<"\t"<<" [说明]: 本命令用于建立目录，要求对其父目录具有写权限 "<<endl;
	cout<< "\t"<<"-p 建立目录时建立其所有不存在的父目录 "<<endl;

	cout<<"rmdir: \n"<<"\t[语法]: rmdir [-p]  目录名 "<<endl;
	cout<<"\t"<<"[说明]: 本命令用于删除目录 "<<endl;
	cout<<"\t"<<"-p 删除所有已经为空的父目录 "<<endl;
	cout<<"\t"<<"-s 当使用-p 选项时，出现错误不提示 "<<endl;

	cout<<"cd: \n"<<"\tcd 回到注册进入时的目录 "<<endl;
	cout<<"\t"<<"cd /tmp 进入 /tmp 目录 "<<endl;
	cout<<"\t"<<"cd ../ 进入上级目录 "<<endl;

	cout<<"rename:\n "<<"\t重命名： rename 目录名/文件名   新目录名/文件名"<<endl;
	cout<<"copy: \n"<<"\t拷贝： copy 目录名/文件名   新目录名/文件名"<<endl;
	cout<<"mv:\n "<<"\t移动： mv 目录名/文件名   新目录名/文件名"<<endl;
	cout<<"create:\n"<<"\t创建文件： create  文件名"<<endl;
	cout<<"del:\n"<<"\t删除文件：del 文件名"<<endl;
	cout<<"open:\n"<<"\t打开文件： open [-w][-r] 文件名"<<endl;
	cout<<"\t"<<"[说明]：-w 写方式打开，-r 只读方式打开   "<<endl;
	cout<<"\t"<<"写文件内容：write：文件名"<<endl;
	cout<<"write:\n"<<"\t写文件内容：read：文件名"<<endl;
	cout<<"read:\n"<<"\t读文件内容：read：文件名"<<endl;
	cout<<"close :\n "<<"\t关闭文件：close：文件名"<<endl;
	cout<<"lsattr:\n"<<"\t查看文件信息：lsattr：目录名/文件名"<<endl<<endl;	
	return 0;
}

int help(char *cmd)
{
	if(strcmp(cmd,"mkdir")==0)
	{
		cout<<"mkdir: \n"<<"\t[语法]: mkdir  [-p] 目录名  "<<endl;
		cout<<"\t"<<" [说明]: 本命令用于建立目录，要求对其父目录具有写权限 "<<endl;
		cout<< "\t"<<"-p 建立目录时建立其所有不存在的父目录 "<<endl;
	}
	else if(strcmp(cmd,"rmdir")==0)
	{
		cout<<"rmdir: \n"<<"\t[语法]: rmdir [-p]  目录名 "<<endl;
		cout<<"\t"<<"[说明]: 本命令用于删除目录 "<<endl;
		cout<<"\t"<<"-p 删除所有已经为空的父目录 "<<endl;
		cout<<"\t"<<"-s 当使用-p 选项时，出现错误不提示 "<<endl;
	}
	else if(strcmp(cmd,"cd")==0)
	{
		cout<<"cd: \n"<<"\tcd 回到注册进入时的目录 "<<endl;
		cout<<"\t"<<"cd /tmp 进入 /tmp 目录 "<<endl;
		cout<<"\t"<<"cd ../ 进入上级目录 "<<endl;
	}
	else if(strcmp(cmd,"rename")==0)
	{
		cout<<"rename:\n "<<"\t重命名： rename 目录名/文件名   新目录名/文件名"<<endl;
	}
	else if(strcmp(cmd,"copy")==0)
	{
		cout<<"copy: \n"<<"\t拷贝： copy 目录名/文件名   新目录名/文件名"<<endl;
	}
	else if(strcmp(cmd,"mv")==0)
	{
		cout<<"mv:\n "<<"\t移动： mv 目录名/文件名   新目录名/文件名"<<endl;
	}
	else if(strcmp(cmd,"create")==0)
	{
		cout<<"create:\n"<<"\t创建文件： create  文件名"<<endl;
	}
	else if(strcmp(cmd,"del")==0)
	{
		cout<<"del:\n"<<"\t删除文件：del 文件名"<<endl;
	}
	else if(strcmp(cmd,"open")==0)
	{
		cout<<"open:\n"<<"\t打开文件： open [-w][-r] 文件名"<<endl;
		cout<<"\t"<<"[说明]：-w 写方式打开，-r 只读方式打开   "<<endl;
		cout<<"\t"<<"写文件内容：write：文件名"<<endl;
	}
	else if(strcmp(cmd,"write")==0)
	{
		cout<<"write:\n"<<"\t写文件内容：read：文件名"<<endl;
	}
	else if(strcmp(cmd,"read")==0)
	{
		cout<<"read:\n"<<"\t读文件内容：read：文件名"<<endl;
	}
	else if(strcmp(cmd,"close")==0)
	{
		cout<<"close :\n "<<"\t关闭文件：close：文件名"<<endl;
	}
	else if(strcmp(cmd,"lsattr")==0)
	{
		cout<<"lsattr:\n"<<"\t查看文件信息：lsattr：目录名/文件名"<<endl<<endl;	
	}else {
			printf("bash:  %s:command not found \n",cmd);
	}
	return 0;
}

