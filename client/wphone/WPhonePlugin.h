#pragma once

#include "iPlug.h"

class WPhonePlugin :public IPlugIn
{
public:
	WPhonePlugin();
	virtual ~WPhonePlugin();

	// 插件初始化
	virtual BOOL OnInitalize();
	// 卸载插件
	virtual BOOL OnUninitalize();

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
	virtual void OnNotify(UINT, WPARAM, LPARAM);

	//发送短消息
	static int SendMessage(const char* to, const char* body);

	//发送电子邮件
	static int SendEmail(const char* to, const char* subject, const char* body);

	//打电话
	static int CallNumber(const char* to);

	//浏览网页
	static int BrowseWeb(const char* to);

	//打开摄像头
	static int CaptureCamera();

	//转到某个页面
	int NavigateToSMS(const char* to);
	int NavigateToCall(const char* to);
	int NavigateToEmail(const char* to);

private:
	CWnd* GetActiveWnd(CMultiDocTemplate* doc);

	void ProcessResp(void* s, int cmd, int resp, unsigned short seq, void* data, int len);

private:
	CMultiDocTemplate* m_contact_docTemp;
	CMultiDocTemplate* m_sms_docTemp;
	CMultiDocTemplate* m_call_docTemp;
	CMultiDocTemplate* m_email_docTemp;
};
