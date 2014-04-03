#pragma once
#include "afxcmn.h"


// CDlgTaskStatus dialog

class CDlgTaskStatus : public CDialog
{
	DECLARE_DYNAMIC(CDlgTaskStatus)

public:
	CDlgTaskStatus(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTaskStatus();
	void GetStatusStrByCmd(char cmd,char *pdes);
	void GenExampleListData();

	

// Dialog Data
	enum { IDD = IDD_DLG_TASK_STATUS };

protected:
		virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_tasklist;
	afx_msg void OnBnClickedBtnRefresh();
};
