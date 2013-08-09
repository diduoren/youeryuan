#ifndef CONN_H
#define CONN_H

#include <vector>
#include <algorithm>

#define ERRCONNSETUP	1000
#define ERRNOSERVER		1001
#define ERRCONNREJ		1002
#define ERRSOCKFAIL		1003
#define ERRSECFAIL		1004
#define ERRCONNFAIL		2000
#define ERRCONNLOST		2001
#define ERRNOCONN		2002
#define ERRRESOURCE		3000
#define ERRTHREAD		3001
#define ERRNOBUF		3002
#define ERRFILE			4000
#define ERRINVRDOFF		4001
#define ERRRDPERM		4002
#define ERRINVWROFF		4003
#define ERRWRPERM		4004
#define ERRINVOP		5000
#define ERRBOUNDSOCK	5001
#define ERRCONNSOCK		5002
#define ERRINVPARAM		5003
#define ERRINVSOCK		5004
#define ERRUNBOUNDSOCK	5005
#define ERRNOLISTEN		5006
#define ERRRDVNOSERV	5007
#define ERRRDVUNBOUND	5008
#define ERRSTREAMILL	5009
#define ERRDGRAMILL		5010
#define ERRDUPLISTEN	5011
#define ERRLARGEMSG		5012
#define ERRINVPOLLID	5013
#define ERRASYNCFAIL	6000
#define ERRASYNCSND		6001
#define ERRASYNCRCV		6002
#define ERRPEERERR		7000
#define ERRIGNORE		8000
#define ERRUNKNOWN		-1


enum ConnOpt
{
	CONN_MSS,             // the Maximum Transfer Unit
	CONN_SNDTIMEO,        // send() timeout
	CONN_RCVTIMEO,        // recv() timeout
	CONN_SNDBUF,          // maximum buffer in sending queue
	CONN_RCVBUF,          // UDT receiving buffer size
	CONN_LINGER,          // waiting for unsent data when closing
	CONN_MSGTTL,          // time-to-live of a datagram message
	CONN_RENDEZVOUS,      // rendezvous connection mode
	CONN_REUSEADDR,	// reuse an existing port or create a new one
	CONN_MAXBW		// maximum bandwidth (bytes per second) that the connection can use
};

enum ConnType 
{ 
	CONN_TCP	= 1,
	CONN_UDP1	= 2,
	CONN_UDP2	= 3,
	CONN_HTTP_SERVER = 4,
	CONN_HTTP_CLIENT = 5
};

typedef void* conn;

extern int conn_error;

struct ws32_loadlibrary_func;

int conn_startup();
int conn_init(struct ws32_loadlibrary_func* func);

unsigned long conn_htonl(unsigned long);
unsigned short conn_htons(unsigned short);
unsigned long conn_ntohl(unsigned long);
unsigned short conn_ntohs(unsigned short);

char* conn_inet_ntoa(struct in_addr);
unsigned long conn_inet_addr(const char *);

/** 
 创建会话连接
 @param type 会话连接类型，支持TCP和UDP
 @return 成功返回连接会话，否则返回空
*/
conn conn_socket(int type);

/**
 得到会话连接类型
 @param s 会话连接P
 @return 成功返回连接会话类型 
*/
enum ConnType conn_socktype(conn s);

/**
 接受一个新的连接
 @return 新的会话连接
*/
conn conn_accept(conn s, struct sockaddr *addr, int *len);

/**
 将连接绑定到一个地址以及端口上
 @成功返回0，否则-1
*/
int conn_bind (conn s, const struct sockaddr *addr, int namelen);

/**
 关闭连接会话
*/
int conn_close(conn s);

/** 
 连接一个服务端
 @成功返回0，否则-1
*/
int conn_connect(conn s, const struct sockaddr *name, int namelen);

/** 
 使连接会话从主动连接变为被动连接
 @return 成功返回0，否则返回-1
*/
int conn_listen(conn s, int backlog);

/** 
 从连接会话中读取数据
 @成功返回读取的数据长度，否则返回-1
*/
int conn_read(conn s, char * buf, int len, int flags, char* rawHttp = NULL);

/** 
 往连接会话中写数据
 @成功返回写入的数据长度，否则返回-1
*/
int conn_write(conn s, const char * buf, int len, int flags);

/** 
 获得与本地连接会话相连接的另一端的的地址
*/
int conn_getpeername(conn s, struct sockaddr* name, int* namelen);

/** 
 获得本地连接会话的地址信息
*/
int conn_getsockname(conn s, struct sockaddr* name, int* namelen);

/** 
 获取连接会话的属性
*/
int conn_getsockopt(conn s, int level, ConnOpt optname, void* optval, int* optlen);

/** 
 设置连接会话的属性
*/
int conn_setsockopt(conn s, int level, ConnOpt optname, const void* optval, int optlen);


typedef std::vector<conn> conn_set;
//static inline void CONN_CLR(u, uset) ( (find((uset)->begin(), (uset)->end(), u) != (uset)->end()) ?(uset)->erase((uset)->find(u)):0)
#define CONN_SET(u, uset) ((uset)->push_back(u))
#define CONN_ZERO(uset) ((uset)->clear())
#define CONN_ISSET(u, uset) (find((uset)->begin(), (uset)->end(), u) != (uset)->end())

int conn_selectEx(const conn_set& fds, conn_set* readfds, conn_set* exceptfds, int msTimeOut);

static int udpPorts[] = {53, 137, 138, 1701, 1900, 2177, 3540, 3702};
static int tcpPorts[] = {139, 445, 1723, 2177, 3587, 5357, 5358, 10243};
static int httpPorts[] = {80, 3128, 8080};

enum {WRITE_ERROR = -1, DES_ERROR = -2, COMPRESS_ERROR = -3, WRITE_TOOLONG = -4 };

struct sock_funcs
{
	conn (*mySocket)(int);
	int  (*myConnect)(conn, const struct sockaddr*, int);
	int  (*myRead)(conn, char *, int, int, char*);
	int  (*myWrite)(conn, struct control_header*, const char*, int);
	int  (*myClose)(conn);
	unsigned long (*myInet_addr)(const char*);
	unsigned short (*myHtons)(unsigned short);
	int   myError;
};

#endif
