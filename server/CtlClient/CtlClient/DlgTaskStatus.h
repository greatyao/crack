#pragma once
#include "afxcmn.h"
#include <string>
#include <vector>

using namespace std;

// CDlgTaskStatus dialog

class CDlgTaskStatus : public CDialog
{
	DECLARE_DYNAMIC(CDlgTaskStatus)

public:
	CDlgTaskStatus(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTaskStatus();

	void GenExampleListData();

// Dialog Data
	enum { IDD = IDD_DLG_TASK_STATUS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:	
	vector<string> m_TaskList;//保存任务


    CToolTipCtrl m_toolTip;//提示信息
	CListCtrl m_ListStatus;
	afx_msg void OnNMDblclkListTask(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnStart();
	virtual BOOL OnInitDialog();
	BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedBtnRefresh();
	void GetStatusStrByCmd(char cmd,char *pdes);
	afx_msg void OnBnClickedBtnPause();
	afx_msg void OnBnClickedBtnDelete();
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
};
