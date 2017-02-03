
#ifndef UNIXFUNCTION
#define UNIXFUNCTION

// 格式化
extern void format();
// 启动，安装文件系统
extern void install();
// 加载
extern void load();
// 保存
extern void save();

//在dinode
extern int find(int index,const char* filename,int dd[]);
extern int findInDir(dir dir_cusor,const char* filename,int * offset=NULL);

// 分配一块dataBlock
extern unsigned int  _alloc_data();
// 分配一块inodeBlock
extern unsigned int  _alloc_inode();
// 释放一块dataBlock
extern void _release_data(int index);
// 释放一块inodeBlock
extern void _release_inode(int index);
// fd 
// 添加目录
//语法：mkdir [选项] dirname 
extern int mkdir(const char* dirname,unsigned short mode);
extern int addsubfile(int curindex,int subindex,const char* filename);

//删除目录
//语法：rmdir [选项] dirname 
extern int rmdir(const char* dirname,unsigned short mode);
extern int _del_from_parent(int parent,const char * filename);

// 移动目录或文件
//语法：mv ［选项］ 源文件或目录 目标文件或目录
extern int _move(const char* source,const char *destination);

// 目录或文件重命名
extern int _rename(const char * oldFilename, const char * newFilename);

// 列出目录清单
extern void ls();

// 进入目录
extern int cd(const char* dirname);

// 添加一个文件
extern int _create(char *filename);

//打开文件或目录 ,打开方式
extern int open(char *filename,char mode);

//关闭文件
extern int _close(char *filename);

// 删除文件
extern int _delete(const char * filename);

//  写入文件
extern  int _write(const char *dirname);
extern int _writebuf(int index);
// 读出文件
extern int read(const char* filename);
extern int  _readit(int index);

//目录或文件的拷贝
extern int copy(char* destination, char* source);
extern dinode DirCopy(dinode node );
extern dinode Filecopy(dinode node);
extern void  blockTransfor(int to,int from);

// 查看文件属性
// lsattr [-adRvV] [文件或目录...]选项介绍:
// -a: 显示所有文件和目录，包括隐藏文件;
// -d: 显示目录名称，而非其内容;
// -R: 递归处理，将指定目录下的所有文件及子目录一并处理;
// -v: 显示文件或目录版本;
// -V: 显示版本信息;
extern int lsattr(const char *filename);

//登陆
extern bool login(char *uid,char *pwd);
//注销
extern int logout();
// 注册
extern int Register();
//帮助
extern int help(char *cmd);
extern int help();

extern void exit();
// 根据引索返回inode
extern struct dinode getinode(int index);

/// 若文件名不合法 返回1，否则返回子目录个数，存放在dirList 中
extern int _filter(const char * dirpath,char*** dirList);

//// 分割字符串
extern int frag_str(const char * cmd,char*** cmdList) ;

extern void onecmd(char **p);
extern void twocmd(char **p);
extern void threecmd(char **p);

#endif