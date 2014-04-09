// DlgClientStatus.cpp : implementation file
//

#include "stdafx.h"
#include "CtlClient.h"
#include "DlgClientStatus.h"

#include "ResPacket.h"
#include "PackManager.h"


// CDlgClientStatus dialog

IMPLEMENT_DYNAMIC(CDlgClientStatus, CDialog)

CDlgClientStatus::CDlgClientStatus(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgClientStatus::IDD, pParent)
{

}

CDlgClientStatus::~CDlgClientStatus()
{
}

void CDlgClientStatus::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_clientlist);
}


BEGIN_MESSAGE_MAP(CDlgClientStatus, CDialog)
	ON_BN_CLICKED(IDC_BTN_CLIENT, &CDlgClientStatus::OnBnClickedBtnClient)
END_MESSAGE_MAP()



// CDlgClientStatus message handlers
void CDlgClientStatus::GenExampleListData(){

	
	//update the list control
	/*
	unsigned char guid[40];
	unsigned int cputhreads;
	unsigned int gputhreads;
	unsigned char hostname[50];
	unsigned char ip[20];
	unsigned char os[48];
	*/


	 LONG lStyle; 
       lStyle = GetWindowLong(m_clientlist.m_hWnd, GWL_STYLE);// 获取当前窗口style 
       lStyle &= ~LVS_TYPEMASK; // 清除显示方式位 
       lStyle |= LVS_REPORT; // 设置style 
       SetWindowLong(m_clientlist.m_hWnd, GWL_STYLE, lStyle);

	DWORD dwStyle = m_clientlist.GetExtendedStyle();
      dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮（只适用与report风格的listctrl）
      dwStyle |= LVS_EX_GRIDLINES;//网格线（只适用与report风格的listctrl）
    //  dwStyle |= LVS_EX_CHECKBOXES;//item前生成checkbox控件
      m_clientlist.SetExtendedStyle(dwStyle); //设置扩展风格

	m_clientlist.InsertColumn( 0, _T("GUID"), LVCFMT_CENTER, 100 ); 
	m_clientlist.InsertColumn( 1, _T("CPU_NUM"), LVCFMT_CENTER, 100 ); 
	m_clientlist.InsertColumn( 2, _T("GPU_NUM"), LVCFMT_CENTER, 100 ); 
	m_clientlist.InsertColumn( 3, _T("HOSTNAME"), LVCFMT_LEFT, 100 ); 
	m_clientlist.InsertColumn( 4, _T("IPADDRESS"), LVCFMT_LEFT, 100 ); 
	m_clientlist.InsertColumn( 5, _T("OS"), LVCFMT_LEFT, 100 ); 

	/*int nRow = m_clientlist.InsertItem(0,_T("0000001"));

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

BOOL CDlgClientStatus::OnInitDialog(){

	CDialog::OnInitDialog();


	//socket req get client list
	GenExampleListData();


	


	return TRUE;
}
void CDlgClientStatus::OnBnClickedBtnClient()
{
	// TODO: Add your control notification handler code here
	int ret = 0;
	struct compute_node_info *pres = NULL;
	compute_node_info *p = NULL;

	ret = g_packmanager.GenClientStatusPack(&pres);
	if ( ret < 0){

		TRACE("gen client list error\n");
		return ;
	}

	int num = 0;

	/*
	
	unsigned char guid[40];
	unsigned int cputhreads;
	unsigned int gputhreads;
	unsigned char hostname[50];
	unsigned char ip[20];
	unsigned char os[48];
	*/

	m_clientlist.DeleteAllItems();

	char tmpbuf[128];
	num = ret/sizeof(compute_node_info);
	int i = 0;
	for(i=0 ;i < num;i++){

		p = &pres[i];

		memset(tmpbuf,0,128);

		m_clientlist.InsertItem(i,"");
		m_clientlist.SetItemText(i,1,(char *)p->guid);
		m_clientlist.SetItemText(i,5,(char*)p->ip);
		m_clientlist.SetItemText(i,4,(char *)p->hostname);
		m_clientlist.SetItemText(i,6,(char *)p->os);
	//	wsprintfA(tmpbuf,"%d",p->status);
		memset(tmpbuf,0,128);
		sprintf(tmpbuf,"%d",p->cputhreads);
		m_clientlist.SetItemText(i,2,tmpbuf);


		memset(tmpbuf,0,128);
		sprintf(tmpbuf,"%d",p->gputhreads);
		m_clientlist.SetItemText(i,3,tmpbuf);

	}

}
