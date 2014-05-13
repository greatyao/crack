// CtlClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CtlClient.h"
#include "CtlClientDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CCtlClientDlg dialog




CCtlClientDlg::CCtlClientDlg(CWnd* pParent /*=NULL*/)
: CDialog(CCtlClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCtlClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TABMAIN, m_tabmain);
}

BEGIN_MESSAGE_MAP(CCtlClientDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_NOTIFY(TCN_SELCHANGE, IDC_TABMAIN, &CCtlClientDlg::OnTcnSelchangeTabmain)
	ON_COMMAND(ID_CFG_SERVER, &CCtlClientDlg::OnCfgServer)
	ON_COMMAND(ID_ABOUT, &CCtlClientDlg::OnAbout)
	ON_COMMAND(ID_EXIT, &CCtlClientDlg::OnExit)
END_MESSAGE_MAP()


// CCtlClientDlg message handlers

BOOL CCtlClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	m_tabmain.InsertItem(0,_T("上传任务"));
	m_tabmain.InsertItem(1,_T("查看任务状态"));
	m_tabmain.InsertItem(2,_T("查看计算节点状态"));


	m_dlgupload.Create(IDD_DLG_UPLOAD,GetDlgItem(IDC_TABMAIN));
	m_dlgtask.Create(IDD_DLG_TASK_STATUS,GetDlgItem(IDC_TABMAIN));

	m_dlgclient.Create(IDD_DLG_CLIENT_STATUS,GetDlgItem(IDC_TABMAIN));

	
	CRect rs;
	m_tabmain.GetClientRect(&rs);
	rs.top+=20;
	rs.bottom -=3;
	rs.left +=2;
	rs.right +=2;

	m_dlgupload.MoveWindow(&rs);
	m_dlgtask.MoveWindow(&rs);
	m_dlgclient.MoveWindow(&rs);

	m_dlgupload.ShowWindow(true);
	//m_dlgclient.ShowWindow(true);
	//m_dlgtask.ShowWindow(true);


	m_tabmain.SetCurSel(0);
	
	m_dlgupload.SetCDlgTaskStatus(&m_dlgtask);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCtlClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCtlClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCtlClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CCtlClientDlg::OnTcnSelchangeTabmain(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here

	int CurSel = m_tabmain.GetCurSel();
	switch(CurSel)
	{
	case 0:
		m_dlgupload.ShowWindow(true);
		m_dlgtask.ShowWindow(false);
		m_dlgclient.ShowWindow(false);

		break;
	case 1:
		m_dlgtask.ShowWindow(true);
		m_dlgclient.ShowWindow(false);
		m_dlgupload.ShowWindow(false);
		break;
	case 2:
		m_dlgclient.ShowWindow(true);
		m_dlgtask.ShowWindow(false);
		m_dlgupload.ShowWindow(false);
		break;
	default:;
	}  
	*pResult = 0;
}

void CCtlClientDlg::OnCfgServer()
{
	CConfigureServer dlg; 
	dlg.DoModal();
}

void CCtlClientDlg::OnAbout()
{
	// TODO: 在此添加命令处理程序代码
	AfxMessageBox("更多信息，请联系开发人员");
}

void CCtlClientDlg::OnExit()
{
	// TODO: 在此添加命令处理程序代码
	AfxMessageBox("点击对话框右上角的 X 即可");
}
