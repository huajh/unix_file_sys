#ifndef  UNIXSTRUCT
#define  UNIXSTRUCT

// 32M
#define  BLOCKSIZE 512  // 块大小 512bytes
#define BLOCKNUM 64*1024 // 65536块个数  
#define INODEBLOCKNUM  512    // i 结点块个数  1%
#define INODEMAX 32  // 最大内存i结点个数
//#define  INDEXMAX  INODEBLOCKNUM //索引表
#define USERMAX 10  // 最大用户数目
#define  OPENFILEMAX 50
#define DATABLOCKNUM BLOCKNUM - DATA_OFFSET  //数据块大小

#define DIFILE  0x0001   // 文件
#define DIDIR  0x0002   //   目录

#define GROUPSIZE 50  //组大小

// 　u 文件主 
// 	g 同组用户 
// 	o 其他用户 
// 	a 所有用户 

#define RUNTIMEMODFUID 0X4000 　//　4000 运行时可改变UID 
#define RUNTIMEMODFGID 0X2000	//　2000 运行时可改变GID 

#define  DEFAULTMODE 0X0000  //默认模式

// 文件主权限
#define UEXEC 0X0100  //  exicute 执行文件
#define UREAD 0X0200  // read   读文件
#define UWRITE 0X0400  // write  写文件
#define  UDELETE 0X0800 // delete 删除文件
// 同组权限
#define GEXEC 0X0010  //  exicute 执行文件
#define GREAD 0X0020  // read   读文件
#define GWRITE 0X0040  // write  写文件
#define  GDELETE 0X0080 // delete 删除文件
// 其它权限
#define OEXEC 0X0001  //  exicute 执行文件
#define OREAD 0X0002  // read   读文件
#define OWRITE 0X0004  // write  写文件
#define  ODELETE 0X0008 // delete 删除文件

#define   LOCK 0X01// 加锁标志
#define  UNLOCK 0X00 // 未加锁

#define MODIFIED   0x01//被修改标志
#define UNMODEIFIED  0x00// 未被修改

#define READONLY 0X01 //  只读
#define  CANWRITE 0X02//  可写

// 文件系统类型
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

#define INODE_OFFSET 2      // i结点块号偏移量
#define  MFD_OFFSET 514 
#define USER_OFFSET 515    //用户表偏移量
#define DATA_OFFSET 517   //数据块块号偏移量

#define NOFILE 10  // 用户打开文件表，default 10
#define DIRSIZE 31  // 一块 目录的长度

#include <time.h>

//////////////////////////////////////////////////////
//											#0-511			#512														#0- #65018
//	#0 ,					#1 			#2 - #513		#514				# 515 - #516 	 	#517-#65535
//磁盘引导块		超级块		磁盘i结点		MFD主目录		用户表				   		数据块（目录，free_stack，数据）
//
//////////////////////////////////////////////////////

/// #0 磁盘引导块
/*
超级块
unix : 成组链接法
每100块为单位进行分组
filsys  ： 安装后常驻内存

456B

分配 512B
#1
*/
struct SuperBlock
{
	unsigned int s_isize; // i 结点区总块数   4B
	unsigned int s_fsize; // 文件数据区总块数  4B

	unsigned int s_tfree ; // 空闲数据区总块数  4B
	unsigned int s_tinode; // 空闲i结点总数  4B

	unsigned int  s_nfree; // 直接管理的空闲块数,栈顶  4B
	unsigned int s_free[GROUPSIZE]; // 空闲块号栈  unsigned int 2^32 =  4G   200B

	unsigned int s_ninode; // 直接管理的空闲i结点数,栈顶   4B
	unsigned int s_inode[GROUPSIZE]; // 空闲i结点栈   200B

	char s_flock ; // 处理空闲块表时加锁标志   1B
	char s_ilock ; // 处理空闲索引结点表时加锁标志  1B

	char   s_fmod;    // flag: 超级块被修改标志   1B
	char   s_ronly;   //flag: file system is read only 1B
	time_t   s_time;    // time of last superblock update   8B

	char s_fname[6]; //  文件系统名称  6B

	long s_type; // 文件系统类型  4B

};

/*磁盘索引结点 
磁盘i结点 
文件控制块 ：FCB  = BFD + SFD
基本文件目录表：BFD
每个分配内存64B
62B
ls -i
64  * 8 = 512 
#2 - #513
共 512* 8 =4096  个i结点
*/

struct dinode
{
	unsigned short i_uid; //属主的 UID			  2B
	unsigned short i_gid; // 属主的组 ID (GID)  2B   文件主：0x01H,同组用户：0x02h,其它用户：0x04h
	unsigned short i_type; //文件类型				  2B
	unsigned short i_mode; // 文件存取权限    2B   
	unsigned short i_nlink; // 文件共享连接数     2B

	time_t  CreateTime; // 文件创建时间  di_ctime   8B
	//	time_t  LastModified; //   最近一次修改时间di_mtime	8B
	time_t  LastAccessed ; // 最近一次访问时间 di_atime		8B

	unsigned short i_size; // 文件长度  2B 
	unsigned short i_addr[13]; // 26B，文件最大为8G  ，若为目录，则存放目录的SFD物理块号
//0-9 ：  0 -9 
// 10 ： 512/2 = 256 ： 10-265 
// 11 ： 256*256= 65536 ： 266 - 65801
// 12 : 256*256*256 = 16777216 : 65802 - 16843017
// total 10 +256 + 256*256 + 256*256*256  = 16843018
};

/*  
内存i节点
82B   
sizeof ： 88B ? 
*/
struct inode
{
	struct dinode _base;
	// add
	unsigned short i_flag; //内存结点状态  2B
	unsigned short i_dev;   // 设备号  2B
	unsigned short i_Index; // 索引i结点号  2B
	unsigned short i_count; // 引用计数。当文件打开时，其引用计数加1，关闭时引用计数减1   2B
};

//  文件目录项：  directory ,Catalog
//  512B = 16B * 32
// SFD signal file directory
//  16B

struct SFD{
	unsigned short d_ino; // 文件内部标识 2B : 2^16 = 64K个 i结点
	char d_name[14] ; // 文件符号名
};

//目录
// 16*31  
// 500  分配一块512B，可有31条目录项
// 
struct dir 
{
	struct SFD SFD[DIRSIZE];
	unsigned int size; //目录项个数
};

//
// 一块
// 256*2 =  512
//
struct Indir  // 间接索引表
{
	unsigned short index[256];
};

///用户表
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
	unsigned int u_ofile[NOFILE];   // 用户打开文件表，default 10
	unsigned int p_file; // 打开文件个数
};

// 51*4 = 204B < 512B
// 
struct  free_stack
{
	unsigned int s_nfree; 
	unsigned int s_free[50]; 
};

// 系统打开表
// 13B
struct file
{
	char f_mode; // 打开方式  ，读r 还是写w  还wr 1B	
	unsigned int f_count; // 描述父子进程的共享计数  4B
	unsigned int f_offset;  // 读写位置指针  4B
	struct inode * f_inode; // 指向内存i结点指针  4B
};

#endif


extern struct SuperBlock filsys;  // 常驻内存超级块

extern struct file sys_file[OPENFILEMAX]; // 系统文件表
extern unsigned short p_file; // 系统打开文件个数

extern unsigned short p_inode;// 活动inode个数
extern struct  inode inodeList[INODEMAX];  //内存i结点表 活动inode .打开的文件表

extern unsigned short inodeIndex[INODEMAX] ; //活动inode index
//extern unsigned short cur_inode_p; // 当前活动i结点指针，指向inodeList

extern inode cur_inode;   //当前活动i结点
extern struct user curUser; // 当前用户
extern FILE *fd;
extern char** curDir;   // 当前目录
extern unsigned short dirLen; // 目录长度
extern unsigned short d_inoList[50]; // 记录当前路径下的各目录的物理块号
extern unsigned short p_ino; //头指针
//extern const char * _command[15]={"mkdir","install","format","ls","cd","rmdir","create","open","write","rename","read","help"};

extern void lsDir();