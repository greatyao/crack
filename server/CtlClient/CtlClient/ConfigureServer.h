#pragma once
#include "afxwin.h"
#include "PackManager.h"

// CConfigureServer �Ի���

class CConfigureServer : public CDialog
{
	DECLARE_DYNAMIC(CConfigureServer)

public:
	CConfigureServer(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CConfigureServer();

// �Ի�������
	enum { IDD = IDD_DIALOG_SERVER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CEdit m_nPort;
	CEdit m_sServer;
	CEdit m_user;
	CEdit m_passwd;
	afx_msg void OnBnClickedOk();
};
