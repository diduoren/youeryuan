#pragma once

#include "iPlug.h"

class RemotePlugin : public IPlugIn
{
public:
	RemotePlugin();
	virtual ~RemotePlugin();

	// 插件初始化
	virtual BOOL	OnInitalize();
	// 卸载插件
	virtual BOOL	OnUninitalize();

	// 创建文档模板
	virtual void OnCreateDocTemplate();

	// 创建框架菜单
	virtual void OnCreateFrameMenu(CMenu* pMainMenu);

	// 创建框架工具条
	virtual	void OnCreateFrameToolBar(ToolBarData*,UINT& count);

	// 创建停靠窗口
	virtual void OnCreateDockPane(DockPane* ,UINT& count);

	// 菜单、工具条操作
	virtual void OnCommand(UINT resId);
	virtual void OnCommandUI(CCmdUI* pCmdUI);

	// 收到通知，通知号，参数1，参数2
	virtual void OnNotify(UINT,WPARAM,LPARAM);

	//改变目录，定向到某个目录
	static void	OnRedirectDir(const char*);

	//文件传送
	int CreateUpDownFileThread(WPARAM);

	//执行某个文件操作
	static void OnFileOperation(unsigned short cmd, void* body, int len);
	static int DetachPathAndFile(const char* fullName, char* path, char* name);


private:
	CMultiDocTemplate* m_docTemp;
};
