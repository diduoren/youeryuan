#pragma once

#include "BtnST.h"
#include <afxcview.h>
#include "afxcmn.h"
#include <list>
#include <vector>

// RemoteExplorer form view

class RemoteExplorer : public CFormView
{
	DECLARE_DYNCREATE(RemoteExplorer)

protected:
	RemoteExplorer();           // protected constructor used by dynamic creation
	virtual ~RemoteExplorer();

public:
	enum { IDD = IDD_FORMVIEW1 };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);

	//鼠标双击某个item
	afx_msg void OnDblclkItem(NMHDR* pNMHDR, LRESULT* pResult);
	//鼠标右键选中某个item
	afx_msg void OnRclickItem(NMHDR* pNMHDR, LRESULT* pResult); 
	//回退到上一级父目录
	afx_msg void OnUpDir();
	//刷新当前目录
	afx_msg void OnRefreshDir();
	//改变下拉框的路径设置
	afx_msg void OnSelChangeComBox();

	//打开目录
	afx_msg void OnOpenDir();
	//创建目录
	afx_msg void OnCreateDir();
	//搜索
	afx_msg void OnSearchFile();
	//重命名
	afx_msg void OnRenameFile();
	//删除
	afx_msg void OnDeleteFile();
	//远程运行
	afx_msg void OnExecFile();
	//复制
	afx_msg void OnCopyFile();
	//粘贴
	afx_msg void OnPasteFile();
	//下载
	afx_msg void OnDownloadFile();
	//上传
	afx_msg void OnUploadFile();
	//上传
	afx_msg void OnUploadDir();

	typedef struct ICONINFO
	{
		char ext[5];
		int index;
	}ICONINFO;

public:
	//添加ITEM列表
	bool InsertItem(WPARAM wp, LPARAM lp);

	//文件操作成功
	void OnFileOperationCompleted(unsigned short cmd);
	//文件操作失败
	void OnFileOperationFailed(unsigned short cmd, unsigned short resp);

private:
	//设置LIST表头
	void SetColumn(bool isDisk);
	//插入单个驱动器
	bool InsertDrive(void* p);
	//插入单个文件/目录
	bool InsertFileOrDir(void* p);
	//寻找合适的文件图标
	void FindImageIconId(LPCTSTR ItemName,int &nImage);
	//当前选择的文件
	CString GetSelectedFile();
	//执行打开目录或者文件操作
	void OpenDirOrExecuteFile();

private:
	CButtonST	m_btnUpDir;
	CButtonST	m_btnRefresh;
	CButtonST	m_btnSearch;
	CComboBox	m_comDir;
	CListCtrl	m_listCtrl;
	CImageList	m_imgList;
	std::list<ICONINFO> m_list;
	std::vector<CString>	m_Drives;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg); 

	DECLARE_MESSAGE_MAP()
};


