// DlgTaskStatus.cpp : implementation file
//

#include "stdafx.h"
#include "CtlClient.h"
#include "DlgTaskStatus.h"
#include "macros.h"

#include "PackManager.h"
#include "CLog.h"	

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
	DDX_Control(pDX, IDC_TASK_LIST, m_tasklist);
}


BEGIN_MESSAGE_MAP(CDlgTaskStatus, CDialog)
	ON_BN_CLICKED(IDC_BTN_REFRESH, &CDlgTaskStatus::OnBnClickedBtnRefresh)
END_MESSAGE_MAP()



void CDlgTaskStatus::GetStatusStrByCmd(char cmd,char *pdes){

	/*
	CT_STATUS_READY = 1,   //����ľ���״̬
	CT_STATUS_FINISHED,		//�������״̬	
	CT_STATUS_FAILURE,		//�������ʧ��״̬
	CT_STATUS_RUNNING,		//�������ڽ���״̬

//	CT_STATUS_STOPPED,		//����ֹͣ״̬  
	CT_STATUS_PAUSED,		//������ͣ����״̬
	CT_STATUS_DELETED,		//����������Ϊɾ��״̬
	CT_STATUS_MAX
	*/
	
	switch(cmd){


		case CT_STATUS_READY:
			
			strcpy(pdes,"���о���");
			break;
		case CT_STATUS_FINISHED:

			strcpy(pdes,"�ƽ����");
			break;
		case CT_STATUS_FAILURE:
	
			strcpy(pdes,"�ƽ�ʧ��");
			break;
		case CT_STATUS_RUNNING:

			strcpy(pdes,"������");
			break;
		case CT_STATUS_PAUSED:
			
			strcpy(pdes,"��������ͣ");
			break;
		case CT_STATUS_DELETED:
			
			strcpy(pdes,"������ɾ��");
			break;
		default:
	
			strcpy(pdes,"״̬δ֪");
			break;	

	}
	return;

}

// CDlgTaskStatus message handlers
void CDlgTaskStatus::GenExampleListData(){

	
	 LONG lStyle; 
       lStyle = GetWindowLong(m_tasklist.m_hWnd, GWL_STYLE);// ��ȡ��ǰ����style 
       lStyle &= ~LVS_TYPEMASK; // �����ʾ��ʽλ 
       lStyle |= LVS_REPORT; // ����style 
       SetWindowLong(m_tasklist.m_hWnd, GWL_STYLE, lStyle);

	DWORD dwStyle = m_tasklist.GetExtendedStyle();
      dwStyle |= LVS_EX_FULLROWSELECT;//ѡ��ĳ��ʹ���и�����ֻ������report����listctrl��
      dwStyle |= LVS_EX_GRIDLINES;//�����ߣ�ֻ������report����listctrl��
    //  dwStyle |= LVS_EX_CHECKBOXES;//itemǰ����checkbox�ؼ�
      m_tasklist.SetExtendedStyle(dwStyle); //������չ���
	 

	m_tasklist.InsertColumn( 0, _T("GUID"), LVCFMT_CENTER, 100 ); 
	m_tasklist.InsertColumn( 1, _T("����"), LVCFMT_CENTER, 100 ); 
	m_tasklist.InsertColumn( 2, _T("�ֿ�����"), LVCFMT_CENTER, 100 ); 
	m_tasklist.InsertColumn( 3, _T("��ɷֿ���"), LVCFMT_LEFT, 100 ); 
	m_tasklist.InsertColumn( 4, _T("��ǰ״̬"), LVCFMT_LEFT, 100 ); 
	


	  /*
	  unsigned char guid[40];
	float m_progress;
	
	unsigned m_split_number;
	unsigned m_fini_number;
	unsigned int status;
	*/
	
	
	
	

	  
/*
	m_clientlist.InsertColumn( 0, _T("GUID"), LVCFMT_CENTER, 100 ); 
	m_clientlist.InsertColumn( 1, _T("CPU_NUM"), LVCFMT_CENTER, 100 ); 
	m_clientlist.InsertColumn( 2, _T("GPU_NUM"), LVCFMT_CENTER, 100 ); 
	m_clientlist.InsertColumn( 3, _T("HOSTNAME"), LVCFMT_LEFT, 100 ); 
	m_clientlist.InsertColumn( 4, _T("IPADDRESS"), LVCFMT_LEFT, 100 ); 
	m_clientlist.InsertColumn( 5, _T("OS"), LVCFMT_LEFT, 100 ); 
*/
	
	int nRow = m_tasklist.InsertItem(0,_T("0000001"));

	m_tasklist.SetItemText(0,1,_T("4"));
	m_tasklist.SetItemText(0,2,_T("2"));
	m_tasklist.SetItemText(0,3,_T("GASS_TEST"));
	m_tasklist.SetItemText(0,4,_T("192.168.10.20"));

/*
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
	GenExampleListData();

	return TRUE;
}



void CDlgTaskStatus::OnBnClickedBtnRefresh()
{
	// TODO: Add your control notification handler code here

	int ret = 0;
	struct task_status_info *pres = NULL;
	struct task_status_info *p = NULL;
	ret = g_packmanager.GenTaskStatusPack(&pres);
	if (ret < 0){

		CLog::Log(LOG_LEVEL_WARNING,"Gen Task Status Pack Error,Code : %d\n",ret);
		return ;

	}

	int i = 0;
	char tmpbuf[128];
	int count = ret/sizeof(task_status_info);

	m_tasklist.SetRedraw(FALSE);
	m_tasklist.DeleteAllItems();  
  


	for(i = 0;i < count ;i ++ ){

		p = &pres[i];
		m_tasklist.InsertItem(i,(LPCTSTR)(p->guid));
		memset(tmpbuf,0,128);
		sprintf(tmpbuf,"%f",p->m_progress);
		m_tasklist.SetItemText(i,1,_T(tmpbuf));
		memset(tmpbuf,0,128);
		sprintf(tmpbuf,"%d",p->m_split_number);
		m_tasklist.SetItemText(i,2,_T(tmpbuf));
		memset(tmpbuf,0,128);
		sprintf(tmpbuf,"%d",p->m_fini_number);
		m_tasklist.SetItemText(i,3,_T(tmpbuf));

		memset(tmpbuf,0,128);
		GetStatusStrByCmd(pres->status,tmpbuf);
		m_tasklist.SetItemText(i,4,_T(tmpbuf));

	}
	
	m_tasklist.SetRedraw(TRUE);
	//	m_tasklist.UpdateData(FALSE);

}
