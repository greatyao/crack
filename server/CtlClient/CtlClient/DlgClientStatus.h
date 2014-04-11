#pragma once
#include "afxcmn.h"


// CDlgClientStatus dialog

class CDlgClientStatus : public CDialog
{
	DECLARE_DYNAMIC(CDlgClientStatus)

public:
	CDlgClientStatus(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgClientStatus();

// Dialog Data
	enum { IDD = IDD_DLG_CLIENT_STATUS };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_clientlist;
	afx_msg void OnBnClickedBtnClient();
};
