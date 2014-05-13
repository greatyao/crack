// ConfigureServer.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CtlClient.h"
#include "ConfigureServer.h"
#include "CLog.h"

// CConfigureServer �Ի���

IMPLEMENT_DYNAMIC(CConfigureServer, CDialog)

CConfigureServer::CConfigureServer(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigureServer::IDD, pParent)
{
}

CConfigureServer::~CConfigureServer()
{
}

void CConfigureServer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_PORT, m_nPort);
	DDX_Control(pDX, IDC_EDIT_IP, m_sServer);
	DDX_Control(pDX, IDC_EDIT_USER, m_user);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_passwd);
}


BEGIN_MESSAGE_MAP(CConfigureServer, CDialog)
	ON_BN_CLICKED(IDOK, &CConfigureServer::OnBnClickedOk)
	ON_BN_CLICKED(IDC_RADIO1, &CConfigureServer::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CConfigureServer::OnBnClickedRadio2)
END_MESSAGE_MAP()


BOOL CConfigureServer::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_nPort.SetLimitText(5);
	m_sServer.SetLimitText(15);
	m_passwd.SetLimitText(31);
	m_user.SetLimitText(31);

	char buffer[MAX_PATH];
	char user[MAX_PATH], passwd[MAX_PATH];
	int port;
	int type;
	g_packmanager.ReadConfigure(buffer,&port, user, passwd, &type);

	m_user.SetWindowTextA(user);
	m_passwd.SetWindowTextA(passwd);
	m_sServer.SetWindowTextA(buffer);
	sprintf(buffer,"%d",port);
	m_nPort.SetWindowTextA(buffer);


	if(type == LOG_TO_FILE)
		((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
	else
		((CButton *)GetDlgItem(IDC_RADIO2))->SetCheck(TRUE);


	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


// CConfigureServer ��Ϣ�������

void CConfigureServer::OnBnClickedOk()
{
	//�ж����룬��ȡ����
	char buffer[MAX_PATH];
	int port = 0;
	char user[32], passwd[32];

	m_nPort.GetWindowTextA(buffer,MAX_PATH);
	port = atoi(buffer);
	m_sServer.GetWindowTextA(buffer,MAX_PATH);
	m_user.GetWindowTextA(user, sizeof(user));
	m_passwd.GetWindowTextA(passwd, sizeof(passwd));
	
	g_packmanager.SetConfigure(buffer,port, user, passwd, m_logtype);
	OnOK();
}

void CConfigureServer::OnBnClickedRadio1()
{
	// TODO: Add your control notification handler code here
	m_logtype = LOG_TO_FILE;
}

void CConfigureServer::OnBnClickedRadio2()
{
	// TODO: Add your control notification handler code here
	m_logtype = LOG_TO_SCREEN;
}
