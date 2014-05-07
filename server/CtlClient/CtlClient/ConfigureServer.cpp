// ConfigureServer.cpp : 实现文件
//

#include "stdafx.h"
#include "CtlClient.h"
#include "ConfigureServer.h"


// CConfigureServer 对话框

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
END_MESSAGE_MAP()


BOOL CConfigureServer::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_nPort.SetLimitText(5);
	m_sServer.SetLimitText(15);
	m_passwd.SetLimitText(31);
	m_user.SetLimitText(31);

	char buffer[MAX_PATH];
	char user[MAX_PATH], passwd[MAX_PATH];
	int port;
	g_packmanager.ReadConfigure(buffer,&port, user, passwd);

	m_user.SetWindowTextA(user);
	m_passwd.SetWindowTextA(passwd);
	m_sServer.SetWindowTextA(buffer);
	sprintf(buffer,"%d",port);
	m_nPort.SetWindowTextA(buffer);


	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


// CConfigureServer 消息处理程序

void CConfigureServer::OnBnClickedOk()
{
	//判断输入，读取输入
	char buffer[MAX_PATH];
	int port = 0;
	char user[32], passwd[32];

	m_nPort.GetWindowTextA(buffer,MAX_PATH);
	port = atoi(buffer);
	m_sServer.GetWindowTextA(buffer,MAX_PATH);
	m_user.GetWindowTextA(user, sizeof(user));
	m_passwd.GetWindowTextA(passwd, sizeof(passwd));
	
	g_packmanager.SetConfigure(buffer,port, user, passwd);
	OnOK();
}
