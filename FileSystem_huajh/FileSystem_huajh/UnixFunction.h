
#ifndef UNIXFUNCTION
#define UNIXFUNCTION

// ��ʽ��
extern void format();
// ��������װ�ļ�ϵͳ
extern void install();
// ����
extern void load();
// ����
extern void save();

//��dinode
extern int find(int index,const char* filename,int dd[]);
extern int findInDir(dir dir_cusor,const char* filename,int * offset=NULL);

// ����һ��dataBlock
extern unsigned int  _alloc_data();
// ����һ��inodeBlock
extern unsigned int  _alloc_inode();
// �ͷ�һ��dataBlock
extern void _release_data(int index);
// �ͷ�һ��inodeBlock
extern void _release_inode(int index);
// fd 
// ���Ŀ¼
//�﷨��mkdir [ѡ��] dirname 
extern int mkdir(const char* dirname,unsigned short mode);
extern int addsubfile(int curindex,int subindex,const char* filename);

//ɾ��Ŀ¼
//�﷨��rmdir [ѡ��] dirname 
extern int rmdir(const char* dirname,unsigned short mode);
extern int _del_from_parent(int parent,const char * filename);

// �ƶ�Ŀ¼���ļ�
//�﷨��mv ��ѡ��� Դ�ļ���Ŀ¼ Ŀ���ļ���Ŀ¼
extern int _move(const char* source,const char *destination);

// Ŀ¼���ļ�������
extern int _rename(const char * oldFilename, const char * newFilename);

// �г�Ŀ¼�嵥
extern void ls();

// ����Ŀ¼
extern int cd(const char* dirname);

// ���һ���ļ�
extern int _create(char *filename);

//���ļ���Ŀ¼ ,�򿪷�ʽ
extern int open(char *filename,char mode);

//�ر��ļ�
extern int _close(char *filename);

// ɾ���ļ�
extern int _delete(const char * filename);

//  д���ļ�
extern  int _write(const char *dirname);
extern int _writebuf(int index);
// �����ļ�
extern int read(const char* filename);
extern int  _readit(int index);

//Ŀ¼���ļ��Ŀ���
extern int copy(char* destination, char* source);
extern dinode DirCopy(dinode node );
extern dinode Filecopy(dinode node);
extern void  blockTransfor(int to,int from);

// �鿴�ļ�����
// lsattr [-adRvV] [�ļ���Ŀ¼...]ѡ�����:
// -a: ��ʾ�����ļ���Ŀ¼�����������ļ�;
// -d: ��ʾĿ¼���ƣ�����������;
// -R: �ݹ鴦����ָ��Ŀ¼�µ������ļ�����Ŀ¼һ������;
// -v: ��ʾ�ļ���Ŀ¼�汾;
// -V: ��ʾ�汾��Ϣ;
extern int lsattr(const char *filename);

//��½
extern bool login(char *uid,char *pwd);
//ע��
extern int logout();
// ע��
extern int Register();
//����
extern int help(char *cmd);
extern int help();

extern void exit();
// ������������inode
extern struct dinode getinode(int index);

/// ���ļ������Ϸ� ����1�����򷵻���Ŀ¼�����������dirList ��
extern int _filter(const char * dirpath,char*** dirList);

//// �ָ��ַ���
extern int frag_str(const char * cmd,char*** cmdList) ;

extern void onecmd(char **p);
extern void twocmd(char **p);
extern void threecmd(char **p);

#endif