// PluginApp.h: interface for the PluginApp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PluginApp_H__B006C998_96CF_4429_85B7_934A393F58A7__INCLUDED_)
#define AFX_PluginApp_H__B006C998_96CF_4429_85B7_934A393F58A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "iPlug.h"

class LIBPLUG_API PluginApp : public CWinApp  
{
public:
	PluginApp();
	virtual ~PluginApp();

	BOOL InitInstance();

	//加载所有可用的插件
	void LoadAllPlugins();

	//卸载插件
	void UnloadAllPlugins();	

	//创建所有的文档模板
	void RegisterAllDocTemplates();
	
	//命令触发事件
	void OnPlugCmd(UINT id);
	
	//命令菜单状态更新
	void OnPlugCmdUI(CCmdUI* pCmdUI);

	//注册码机制
	BOOL IsRegisterOK();

	static char* GetCFGFile();

	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_PluginApp_H__B006C998_96CF_4429_85B7_934A393F58A7__INCLUDED_)
