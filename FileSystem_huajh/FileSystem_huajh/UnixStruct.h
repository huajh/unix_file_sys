#ifndef  UNIXSTRUCT
#define  UNIXSTRUCT

// 32M
#define  BLOCKSIZE 512  // ���С 512bytes
#define BLOCKNUM 64*1024 // 65536�����  
#define INODEBLOCKNUM  512    // i �������  1%
#define INODEMAX 32  // ����ڴ�i������
//#define  INDEXMAX  INODEBLOCKNUM //������
#define USERMAX 10  // ����û���Ŀ
#define  OPENFILEMAX 50
#define DATABLOCKNUM BLOCKNUM - DATA_OFFSET  //���ݿ��С

#define DIFILE  0x0001   // �ļ�
#define DIDIR  0x0002   //   Ŀ¼

#define GROUPSIZE 50  //���С

// ��u �ļ��� 
// 	g ͬ���û� 
// 	o �����û� 
// 	a �����û� 

#define RUNTIMEMODFUID 0X4000 ��//��4000 ����ʱ�ɸı�UID 
#define RUNTIMEMODFGID 0X2000	//��2000 ����ʱ�ɸı�GID 

#define  DEFAULTMODE 0X0000  //Ĭ��ģʽ

// �ļ���Ȩ��
#define UEXEC 0X0100  //  exicute ִ���ļ�
#define UREAD 0X0200  // read   ���ļ�
#define UWRITE 0X0400  // write  д�ļ�
#define  UDELETE 0X0800 // delete ɾ���ļ�
// ͬ��Ȩ��
#define GEXEC 0X0010  //  exicute ִ���ļ�
#define GREAD 0X0020  // read   ���ļ�
#define GWRITE 0X0040  // write  д�ļ�
#define  GDELETE 0X0080 // delete ɾ���ļ�
// ����Ȩ��
#define OEXEC 0X0001  //  exicute ִ���ļ�
#define OREAD 0X0002  // read   ���ļ�
#define OWRITE 0X0004  // write  д�ļ�
#define  ODELETE 0X0008 // delete ɾ���ļ�

#define   LOCK 0X01// ������־
#define  UNLOCK 0X00 // δ����

#define MODIFIED   0x01//���޸ı�־
#define UNMODEIFIED  0x00// δ���޸�

#define READONLY 0X01 //  ֻ��
#define  CANWRITE 0X02//  ��д

// �ļ�ϵͳ����
#define BIN   0X0001
#define DEV  0X0002
#define ETC  0X0004
#define LIB    0X0008
#define MNT 0X0010 
#define SBIN 0X0020
#define TMP 0X0040
#define USR 0X0080
#define VAR 0X0100
#define HOME 0X0200

#define INODE_OFFSET 2      // i�����ƫ����
#define  MFD_OFFSET 514 
#define USER_OFFSET 515    //�û���ƫ����
#define DATA_OFFSET 517   //���ݿ���ƫ����

#define NOFILE 10  // �û����ļ���default 10
#define DIRSIZE 31  // һ�� Ŀ¼�ĳ���

#include <time.h>

//////////////////////////////////////////////////////
//											#0-511			#512														#0- #65018
//	#0 ,					#1 			#2 - #513		#514				# 515 - #516 	 	#517-#65535
//����������		������		����i���		MFD��Ŀ¼		�û���				   		���ݿ飨Ŀ¼��free_stack�����ݣ�
//
//////////////////////////////////////////////////////

/// #0 ����������
/*
������
unix : �������ӷ�
ÿ100��Ϊ��λ���з���
filsys  �� ��װ��פ�ڴ�

456B

���� 512B
#1
*/
struct SuperBlock
{
	unsigned int s_isize; // i ������ܿ���   4B
	unsigned int s_fsize; // �ļ��������ܿ���  4B

	unsigned int s_tfree ; // �����������ܿ���  4B
	unsigned int s_tinode; // ����i�������  4B

	unsigned int  s_nfree; // ֱ�ӹ���Ŀ��п���,ջ��  4B
	unsigned int s_free[GROUPSIZE]; // ���п��ջ  unsigned int 2^32 =  4G   200B

	unsigned int s_ninode; // ֱ�ӹ���Ŀ���i�����,ջ��   4B
	unsigned int s_inode[GROUPSIZE]; // ����i���ջ   200B

	char s_flock ; // ������п��ʱ������־   1B
	char s_ilock ; // ���������������ʱ������־  1B

	char   s_fmod;    // flag: �����鱻�޸ı�־   1B
	char   s_ronly;   //flag: file system is read only 1B
	time_t   s_time;    // time of last superblock update   8B

	char s_fname[6]; //  �ļ�ϵͳ����  6B

	long s_type; // �ļ�ϵͳ����  4B

};

/*����������� 
����i��� 
�ļ����ƿ� ��FCB  = BFD + SFD
�����ļ�Ŀ¼��BFD
ÿ�������ڴ�64B
62B
ls -i
64  * 8 = 512 
#2 - #513
�� 512* 8 =4096  ��i���
*/

struct dinode
{
	unsigned short i_uid; //������ UID			  2B
	unsigned short i_gid; // �������� ID (GID)  2B   �ļ�����0x01H,ͬ���û���0x02h,�����û���0x04h
	unsigned short i_type; //�ļ�����				  2B
	unsigned short i_mode; // �ļ���ȡȨ��    2B   
	unsigned short i_nlink; // �ļ�����������     2B

	time_t  CreateTime; // �ļ�����ʱ��  di_ctime   8B
	//	time_t  LastModified; //   ���һ���޸�ʱ��di_mtime	8B
	time_t  LastAccessed ; // ���һ�η���ʱ�� di_atime		8B

	unsigned short i_size; // �ļ�����  2B 
	unsigned short i_addr[13]; // 26B���ļ����Ϊ8G  ����ΪĿ¼������Ŀ¼��SFD������
//0-9 ��  0 -9 
// 10 �� 512/2 = 256 �� 10-265 
// 11 �� 256*256= 65536 �� 266 - 65801
// 12 : 256*256*256 = 16777216 : 65802 - 16843017
// total 10 +256 + 256*256 + 256*256*256  = 16843018
};

/*  
�ڴ�i�ڵ�
82B   
sizeof �� 88B ? 
*/
struct inode
{
	struct dinode _base;
	// add
	unsigned short i_flag; //�ڴ���״̬  2B
	unsigned short i_dev;   // �豸��  2B
	unsigned short i_Index; // ����i����  2B
	unsigned short i_count; // ���ü��������ļ���ʱ�������ü�����1���ر�ʱ���ü�����1   2B
};

//  �ļ�Ŀ¼�  directory ,Catalog
//  512B = 16B * 32
// SFD signal file directory
//  16B

struct SFD{
	unsigned short d_ino; // �ļ��ڲ���ʶ 2B : 2^16 = 64K�� i���
	char d_name[14] ; // �ļ�������
};

//Ŀ¼
// 16*31  
// 500  ����һ��512B������31��Ŀ¼��
// 
struct dir 
{
	struct SFD SFD[DIRSIZE];
	unsigned int size; //Ŀ¼�����
};

//
// һ��
// 256*2 =  512
//
struct Indir  // ���������
{
	unsigned short index[256];
};

///�û���
// 92B  //90B
// unsigned int  UserCount ; 4B 
//92 * 10 +4= 924 < 512*2 =1024
// USERMAX = 10 
// #522-#523

struct user
{
	char uid[20];   
	char pwd[20];
	unsigned short u_defalut_mode; // 
	unsigned short u_uid;
	unsigned short u_gid;
	unsigned int u_ofile[NOFILE];   // �û����ļ���default 10
	unsigned int p_file; // ���ļ�����
};

// 51*4 = 204B < 512B
// 
struct  free_stack
{
	unsigned int s_nfree; 
	unsigned int s_free[50]; 
};

// ϵͳ�򿪱�
// 13B
struct file
{
	char f_mode; // �򿪷�ʽ  ����r ����дw  ��wr 1B	
	unsigned int f_count; // �������ӽ��̵Ĺ������  4B
	unsigned int f_offset;  // ��дλ��ָ��  4B
	struct inode * f_inode; // ָ���ڴ�i���ָ��  4B
};

#endif


extern struct SuperBlock filsys;  // ��פ�ڴ泬����

extern struct file sys_file[OPENFILEMAX]; // ϵͳ�ļ���
extern unsigned short p_file; // ϵͳ���ļ�����

extern unsigned short p_inode;// �inode����
extern struct  inode inodeList[INODEMAX];  //�ڴ�i���� �inode .�򿪵��ļ���

extern unsigned short inodeIndex[INODEMAX] ; //�inode index
//extern unsigned short cur_inode_p; // ��ǰ�i���ָ�룬ָ��inodeList

extern inode cur_inode;   //��ǰ�i���
extern struct user curUser; // ��ǰ�û�
extern FILE *fd;
extern char** curDir;   // ��ǰĿ¼
extern unsigned short dirLen; // Ŀ¼����
extern unsigned short d_inoList[50]; // ��¼��ǰ·���µĸ�Ŀ¼��������
extern unsigned short p_ino; //ͷָ��
//extern const char * _command[15]={"mkdir","install","format","ls","cd","rmdir","create","open","write","rename","read","help"};

extern void lsDir();