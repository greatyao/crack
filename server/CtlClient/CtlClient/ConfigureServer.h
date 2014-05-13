#pragma once
#include "afxwin.h"
#include "PackManager.h"

// CConfigureServer 对话框

class CConfigureServer : public CDialog
{
	DECLARE_DYNAMIC(CConfigureServer)

public:
	CConfigureServer(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CConfigureServer();

// 对话框数据
	enum { IDD = IDD_DIALOG_SERVER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CEdit m_nPort;
	CEdit m_sServer;
	CEdit m_user;
	CEdit m_passwd;
	int m_logtype;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
};
