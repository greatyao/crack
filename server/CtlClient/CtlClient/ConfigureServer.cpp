// ConfigureServer.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CtlClient.h"
#include "ConfigureServer.h"


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
}


BEGIN_MESSAGE_MAP(CConfigureServer, CDialog)
	ON_BN_CLICKED(IDOK, &CConfigureServer::OnBnClickedOk)
END_MESSAGE_MAP()


BOOL CConfigureServer::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_nPort.SetLimitText(5);
	m_sServer.SetLimitText(15);

	char buffer[MAX_PATH];
	int port;
	g_packmanager.ReadConfigure(buffer,&port);

	m_sServer.SetWindowTextA(buffer);
	sprintf(buffer,"%d",port);
	m_nPort.SetWindowTextA(buffer);


	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


// CConfigureServer ��Ϣ�������

void CConfigureServer::OnBnClickedOk()
{
	//�ж����룬��ȡ����
	char buffer[MAX_PATH];
	int port = 0;



	m_nPort.GetWindowTextA(buffer,MAX_PATH);
	port = atoi(buffer);
	m_sServer.GetWindowTextA(buffer,MAX_PATH);
	
	g_packmanager.SetConfigure(buffer,port);
	OnOK();
}
