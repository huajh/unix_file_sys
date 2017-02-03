#include <iostream>
#include <fstream>
#include <time.h>
#include<Windows.h>
#include "UnixStruct.h"
#include "UnixFunction.h"
#include   "conio.h " 
#include "time.h"
using namespace std;

void main()	
{
//	freopen("output.txt","w",stdout);
//	format();
	install();
	load();
	int str_capacity = 100;
	char str[100];
	char uid[20],pwd[20];
	while(1)
	{
		bool first =true;
		do
		{
			if(!first) cout<<"用户名或密码错误！"<<endl;
			first =false;
			printf("input uid: ");
			scanf("%s",&uid);
			printf("input password: ");
			scanf("%s",&pwd);
		}while(!login(uid,pwd));
		getchar();
		char *p = new char,*s = new char ;
		cout<<endl<<uid<<"@huajh-PC  ";lsDir();
		cout<<endl<<"$ ";
		while(fgets(str,str_capacity,stdin)!=NULL)
		{
			if(strcmp(str,"\n")==0 ||strcmp(str,"\r")==0  ) {
				cout<<endl<<uid<<"@huajh-PC  ";lsDir();
				cout<<endl<<"$ ";
				continue;  //回车 或换行
			}
			char **p = new char*;
			int size = frag_str(str,&p);
			if(size ==1)
			{
				if(strcmp(*p,"exit")==0) {
					save();
					puts("正在退出系统....");
					Sleep(500);
					return;
				}
				onecmd(p);
			}
			else if(size ==2)
				twocmd(p);
			else if(size==3)
				threecmd(p);
			else {
				if(**(p+1)=='-')
					printf("base:  %s: %s invalid option \n",*p,*(p+1));
				else
					printf("base:  %s:No such command  \n",*p);
			}
			if(curUser.u_uid==0)
				break;
			cout<<endl<<"huajh@huajh-PC  ";lsDir();
			cout<<endl<<"$ ";
		}	
	}
	getchar(); 
}

