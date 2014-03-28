// CtlClientDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "dlguploadtask.h"
#include "dlgtaskstatus.h"
#include "dlgclientstatus.h"


// CCtlClientDlg dialog
class CCtlClientDlg : public CDialog
{
// Construction
public:
	CCtlClientDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CTLCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_tabmain;
	CDlgUploadTask m_dlgupload;
	CDlgTaskStatus m_dlgtask;
	CDlgClientStatus m_dlgclient;
	afx_msg void OnTcnSelchangeTabmain(NMHDR *pNMHDR, LRESULT *pResult);
};
