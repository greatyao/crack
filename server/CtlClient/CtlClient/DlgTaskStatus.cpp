// DlgTaskStatus.cpp : implementation file
//

#include "stdafx.h"
#include "CtlClient.h"
#include "DlgTaskStatus.h"


// CDlgTaskStatus dialog

IMPLEMENT_DYNAMIC(CDlgTaskStatus, CDialog)

CDlgTaskStatus::CDlgTaskStatus(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTaskStatus::IDD, pParent)
{

}

CDlgTaskStatus::~CDlgTaskStatus()
{
}

void CDlgTaskStatus::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TASK_LIST, m_ListStatus);
}


BEGIN_MESSAGE_MAP(CDlgTaskStatus, CDialog)
	ON_BN_CLICKED(IDC_BTN_START, &CDlgTaskStatus::OnBnClickedBtnStart)
END_MESSAGE_MAP()


// CDlgTaskStatus message handlers
void CDlgTaskStatus::GenExampleListData(){

	
	//update the list control
	/*
	unsigned char guid[40];
	unsigned int cputhreads;
	unsigned int gputhreads;
	unsigned char hostname[50];
	unsigned char ip[20];
	unsigned char os[48];
	*/

/*
	 LONG lStyle; 
       lStyle = GetWindowLong(m_clientlist.m_hWnd, GWL_STYLE);// ��ȡ��ǰ����style 
       lStyle &= ~LVS_TYPEMASK; // �����ʾ��ʽλ 
       lStyle |= LVS_REPORT; // ����style 
       SetWindowLong(m_clientlist.m_hWnd, GWL_STYLE, lStyle);

	DWORD dwStyle = m_clientlist.GetExtendedStyle();
      dwStyle |= LVS_EX_FULLROWSELECT;//ѡ��ĳ��ʹ���и�����ֻ������report����listctrl��
      dwStyle |= LVS_EX_GRIDLINES;//�����ߣ�ֻ������report����listctrl��
    //  dwStyle |= LVS_EX_CHECKBOXES;//itemǰ����checkbox�ؼ�
      m_clientlist.SetExtendedStyle(dwStyle); //������չ���

	m_clientlist.InsertColumn( 0, _T("GUID"), LVCFMT_CENTER, 100 ); 
	m_clientlist.InsertColumn( 1, _T("CPU_NUM"), LVCFMT_CENTER, 100 ); 
	m_clientlist.InsertColumn( 2, _T("GPU_NUM"), LVCFMT_CENTER, 100 ); 
	m_clientlist.InsertColumn( 3, _T("HOSTNAME"), LVCFMT_LEFT, 100 ); 
	m_clientlist.InsertColumn( 4, _T("IPADDRESS"), LVCFMT_LEFT, 100 ); 
	m_clientlist.InsertColumn( 5, _T("OS"), LVCFMT_LEFT, 100 ); 

	int nRow = m_clientlist.InsertItem(0,_T("0000001"));

	m_clientlist.SetItemText(0,1,_T("4"));
	m_clientlist.SetItemText(0,2,_T("2"));
	m_clientlist.SetItemText(0,3,_T("GASS_TEST"));
	m_clientlist.SetItemText(0,4,_T("192.168.10.20"));
	m_clientlist.SetItemText(0,5,_T("Window 7"));


	nRow = m_clientlist.InsertItem(1,_T("0000002"));

	m_clientlist.SetItemText(1,1,_T("8"));
	m_clientlist.SetItemText(1,2,_T("4"));
	m_clientlist.SetItemText(1,3,_T("GASS_TEST_2"));
	m_clientlist.SetItemText(1,4,_T("192.168.10.22"));
	m_clientlist.SetItemText(1,5,_T("Ubuntu"));


	*/
}



BOOL CDlgTaskStatus::OnInitDialog()
{
	CDialog::OnInitDialog();

	//��ʼ����Ϣ
    m_toolTip.Create(this,TTS_ALWAYSTIP | TTS_BALLOON);
    m_toolTip.SetMaxTipWidth(200);
    m_toolTip.AddTool(GetDlgItem(IDC_TASK_LIST), "˫����Ŀ�쿴��ϸ��Ϣ");
    m_toolTip.AddTool(GetDlgItem(IDC_BTN_START), "��ʼѡ����Ŀ���ƽ�");
    m_toolTip.AddTool(GetDlgItem(IDC_BTN_PAUSE), "��ͣѡ����Ŀ���ƽ�");
    m_toolTip.AddTool(GetDlgItem(IDC_BTN_DELETE), "ɾ��ѡ�е���Ŀ����");
    m_toolTip.AddTool(GetDlgItem(IDC_BTN_STOP), "ֹͣѡ����Ŀ���ƽ�");
    m_toolTip.AddTool(GetDlgItem(IDC_BTN_REFRESH), "ˢ���б�������Ľ�����Ϣ");

	//��ʼ��CListCtrl
	ListView_SetExtendedListViewStyle(m_ListStatus.m_hWnd, LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES );

	m_ListStatus.InsertColumn(0, _T("ѡ��"), LVCFMT_LEFT, 40);
	m_ListStatus.InsertColumn(1, _T("�����ļ���"), LVCFMT_LEFT, 200);
	m_ListStatus.InsertColumn(2, _T("�㷨"), LVCFMT_LEFT, 50);
	m_ListStatus.InsertColumn(3, _T("����״̬"), LVCFMT_LEFT, 100);
	m_ListStatus.InsertColumn(4, _T("������ɶ�"), LVCFMT_LEFT, 80);
	m_ListStatus.InsertColumn(5, _T("������"), LVCFMT_LEFT, 80);//guid

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

BOOL CDlgTaskStatus::PreTranslateMessage(MSG* pMsg) 
{
    if (pMsg->message == WM_MOUSEMOVE)
    {
        if( pMsg->hwnd == GetDlgItem(IDC_TASK_LIST)->m_hWnd || 
            pMsg->hwnd == GetDlgItem(IDC_BTN_START)->m_hWnd || 
            pMsg->hwnd == GetDlgItem(IDC_BTN_PAUSE)->m_hWnd || 
            pMsg->hwnd == GetDlgItem(IDC_BTN_DELETE)->m_hWnd|| 
            pMsg->hwnd == GetDlgItem(IDC_BTN_STOP)->m_hWnd  || 
            pMsg->hwnd == GetDlgItem(IDC_BTN_REFRESH)->m_hWnd )
        {
            m_toolTip.RelayEvent(pMsg);
        }
        else
        {
            m_toolTip.Pop();
        }
        return TRUE;
    }
    return CDialog::PreTranslateMessage(pMsg);
}


BOOL CDlgTaskStatus::AddToTaskList(int nAlgo,int nCharset,int nType,int nIsFile,int nLenMin,int nLenMax,char *psFile,char *guid)
{
	//��ӵ�list�������浽��������		
	int n = m_ListStatus.GetItemCount();
	
	m_ListStatus.InsertItem(n,"");		
	m_ListStatus.SetItemText (n, 1, psFile);
	m_ListStatus.SetItemText (n, 2, "1");
	m_ListStatus.SetItemText (n, 3, "û�п�ʼ�ƽ�");
	m_ListStatus.SetItemText (n, 4, "%0");
	m_ListStatus.SetItemText (n, 5, "guid");

	return 0;
}

void CDlgTaskStatus::OnBnClickedBtnStart()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}
