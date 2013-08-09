// MasterPlugin.h: interface for the MasterPlugin class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MASTERPLUGIN_H__9A868FBA_F91C_4EAD_892A_AA7AC94281EA__INCLUDED_)
#define AFX_MASTERPLUGIN_H__9A868FBA_F91C_4EAD_892A_AA7AC94281EA__INCLUDED_

#include "ObjViewPane.h"
#include "OutputPane.h"

class MasterPlugin : public IPlugIn  
{
public:
	MasterPlugin();
	virtual ~MasterPlugin();

public:
	COutputPane* GetOutputPane();
	CObjViewPane* GetObjViewPane();

	virtual void OnCreateFrameMenu(CMenu* pMainMenu);
	virtual void OnCreateDockPane(DockPane* ,UINT& count);
	virtual void OnCreateFrameToolBar(ToolBarData*,UINT& count);
	virtual void OnCreateDocTemplate();

	virtual BOOL OnInitalize();
	virtual BOOL OnUninitalize();

	virtual void OnNotify(UINT,WPARAM,LPARAM);
	virtual void OnCommand(UINT resId);
	virtual void OnCommandUI(CCmdUI* pCmdUI);

	void ProcessResp(unsigned short cmd, unsigned short resp, void* body);

	//生成可以注入的木马程序
	void MakeInjectFile();

	//弹出木马上线提示
	void ShowTrojanOnlineMsgPop(const char* msg);

private:
	COutputPane* m_pOutputPane;
	CObjViewPane* m_pObjViewPane;
};

#endif // !defined(AFX_MASTERPLUGIN_H__9A868FBA_F91C_4EAD_892A_AA7AC94281EA__INCLUDED_)
