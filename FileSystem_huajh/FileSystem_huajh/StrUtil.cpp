#include <iostream>
#include <fstream>
#include <time.h>
#include "UnixStruct.h"
#include "UnixFunction.h"
using namespace std;

int _filter(const char * dirpath,char*** dirList)
{
	int size = 0;
	unsigned int len = strlen(dirpath);
	if(*(dirpath) == '-' || *(dirpath) == '/') { return 0;}
	char buffer[200] ;
	unsigned int count  = 0;
	char c;
	int flag = 0; // 0ǰһ������ '/'  1 ǰһ����'/'
	for(int i=0;i<len;i++)
	{
		c = *(dirpath+ i);
		if(c!='/')
		{
			buffer[count++] = c;			
			flag = 0;
		}else {
			if(flag == 0) {
				buffer[count] = '\0';
				*(*(dirList)+size) = (char*) malloc(count*sizeof(char));
				memcpy(*(*(dirList)+size) ,buffer,strlen(*(*(dirList)+size)));
				count=0;
				size ++;
			}
			flag = 1;
		}
	}
	if(flag ==0)
	{
		buffer[count] = '\0';
		*(*(dirList)+size) = (char*) malloc(count*sizeof(char));
		memcpy(*(*(dirList)+size) ,buffer,strlen(*(*(dirList)+size)));
		count=0;
		size ++;
	}
	return size;
}

int frag_str(const char * cmd,char*** cmdList) // �ָ��ַ���
{
	int len = strlen(cmd)-1;  //��ȥ���һ��'\n'
	char buffer[100];
	int size = 0;
	for(int i= 0;i<len;i++)
	{
		while(cmd[i]==' ' && i<len) i++;
		int count = 0;
		while(cmd[i]!=' '&& i<len)
			buffer[count++] = cmd[i++];
		buffer[count] = '\0';
		*(*(cmdList)+size) = (char*) malloc(count*sizeof(char));
		memcpy(*(*(cmdList)+size) ,buffer,strlen(*(*(cmdList)+size)));
		size ++;
	}
	return size;
}