// ObjViewPane.h: interface for the CObjViewPane class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJVIEWPANE_H__2BC263B0_ED42_4E74_9203_B7151D1E5016__INCLUDED_)
#define AFX_OBJVIEWPANE_H__2BC263B0_ED42_4E74_9203_B7151D1E5016__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DockPaneWnd.h"
#include "WorkTree.h"

class SEU_QQwry;

//树型结构中叶子结点的起始值，以后增加结点值递增
#define		MIN_LEAF_NODE	2000

enum Status{ONLINE, OFFLINE, DESCRIPTION, NOP};
class CObjViewPane : public CWnd  
{
// Construction
public:
	CObjViewPane();
	virtual ~CObjViewPane();


// Attributes
public:
	BOOL	CreateImageList(CImageList& il, UINT nID);

	//插入新的树形节点
	HTREEITEM InsertItem(STANDARDDATA* info);
	
	//改变树形节点的名称
	int ChangeText(STANDARDDATA* info);

	//删除一个树形节点 
	int RemoveItem(STANDARDDATA* info);

	bool RemoveItem(HTREEITEM item);

	//获取最近被选中的叶子结点的值
	int GetValidObjData();

	int FindGroup();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickWorkTree(NMHDR* pNMHDR, LRESULT* pResult);
	
	//获取目标机属性
	afx_msg void OnGetObjMachineInfo();
	
	//关机
	afx_msg void OnShutdownObjMachine();

	//重启
	afx_msg void OnRebootObjMachine();

	//重命名
	afx_msg void OnRenameObjMachine();

	//更新所有的插件到服务器
	afx_msg void OnUnloadPlugin();

	//卸载服务器的所有插件
	afx_msg void OnUpdatePlugin();

	//卸载服务器
	afx_msg void OnRemoveService();

	DECLARE_MESSAGE_MAP()

private:
	int		m_iTempObj;
	HANDLE	m_hSelfDeleteThread;		
	unsigned int m_dwSelfDeleteThreadId;			
	CFont m_Font;
	CDockPaneWnd<CWorkTree> m_wndObjView;
	CImageList m_ilClassView;
	HTREEITEM	m_hItemNew;		//当前选中的树型结点
	HTREEITEM	m_hItemOld;		//刚才选中的树型结点
	int m_iObjData;
	int m_iGroupData;
	HTREEITEM m_root;		//目标机树型结构根节点句柄
	HTREEITEM m_hti;
};

#endif // !defined(AFX_OBJVIEWPANE_H__2BC263B0_ED42_4E74_9203_B7151D1E5016__INCLUDED_)
