#ifndef SVR_CMD_TABLE_H
#define SVR_CMD_TABLE_H

/** 
 数据写函数，由服务端总控提供 
 @param s TCP/UDP会话连接
 @param	header 应用包头数据
 @param buf 木马服务端写往监控客户端的数据
 @param n 数据长度
 @return 失败返回-1，否则返回写成功数据长度
*/
typedef int (*writeToClient)(conn s, struct control_header* header, const char* buf, int n);

/** 
 具体各个命令模块的执行，由各个插件分别提供
 @param s TCP/UDP会话连接
 @param buf 木马服务端从监控客户端接收的数据
 @param n 数据长度
 @param fn 回调函数
 @return 失败返回-1，否则返回0
*/
typedef int (*coreFunc)(conn s, char* buf , int n, writeToClient fn);


typedef int (*loadDll)();

/** 
 初始化dll
*/
typedef int (*onInitialize)(void*);

/** 
 清理dll的某些资源
*/
typedef int (*onUninitialize)(void*);


/**
 有关命令执行模块的结构体
*/
typedef struct cmd_table
{   
	unsigned short		cmdID;			//命令ID
	unsigned short		reserved;		//保留字段
	coreFunc 			func;			//核心函数地址
	onUninitialize		destroy;		//清理函数地址
	onInitialize		init;		//初始化函数地址
	char				name[32];		//实现该命令字的DLL名
}cmd_table;

/**
 所有命令执行模块的结构体，由服务端总控维护
*/
typedef std::map<unsigned short, cmd_table> CmdTables;


#endif