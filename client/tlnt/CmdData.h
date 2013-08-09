// CmdData.h: interface for the CCmdData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CMDDATA_H__B39B42AE_D18B_41CF_9BA6_722DBBAC00DA__INCLUDED_)
#define AFX_CMDDATA_H__B39B42AE_D18B_41CF_9BA6_722DBBAC00DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct tagCmdData
{
	char strGUID[40];	//Ä¾ÂíµÄGUID
	char strCmd[256];	//ÊäÈëÃüÁî
}CmdData;


typedef std::vector<CmdData> CmdDataList;

extern CmdDataList g_CmdDataList;

#endif // !defined(AFX_CMDDATA_H__B39B42AE_D18B_41CF_9BA6_722DBBAC00DA__INCLUDED_)
