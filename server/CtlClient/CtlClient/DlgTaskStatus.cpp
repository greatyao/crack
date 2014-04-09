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
	DDX_Control(pDX, IDC_TASK_LIST, m_ListStatus);
}


BEGIN_MESSAGE_MAP(CDlgTaskStatus, CDialog)
	ON_BN_CLICKED(IDC_BTN_START, &CDlgTaskStatus::OnBnClickedBtnStart)
	ON_NOTIFY(NM_DBLCLK, IDC_TASK_LIST, &CDlgTaskStatus::OnNMDblclkListTask)
	ON_BN_CLICKED(IDC_BTN_REFRESH, &CDlgTaskStatus::OnBnClickedBtnRefresh)
	ON_BN_CLICKED(IDC_BTN_PAUSE, &CDlgTaskStatus::OnBnClickedBtnPause)
	ON_BN_CLICKED(IDC_BTN_DELETE, &CDlgTaskStatus::OnBnClickedBtnDelete)
	ON_BN_CLICKED(IDC_BTN_STOP, &CDlgTaskStatus::OnBnClickedBtnStop)
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
	m_ListStatus.InsertColumn(1, _T("����GUID"), LVCFMT_LEFT, 180);
	m_ListStatus.InsertColumn(2, _T("�㷨"), LVCFMT_LEFT, 50);
	m_ListStatus.InsertColumn(3, _T("����ʱ��"), LVCFMT_LEFT, 70);
	m_ListStatus.InsertColumn(4, _T("ʣ��ʱ��"), LVCFMT_LEFT, 70);
	m_ListStatus.InsertColumn(5, _T("����"), LVCFMT_LEFT, 50);
	m_ListStatus.InsertColumn(6, _T("�и����"), LVCFMT_LEFT, 100);
	m_ListStatus.InsertColumn(7, _T("��ɷ���"), LVCFMT_LEFT, 80);
	m_ListStatus.InsertColumn(8, _T("״̬"), LVCFMT_LEFT, 80);

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

//˫����ȡ��ϸ��Ϣ
void CDlgTaskStatus::OnNMDblclkListTask(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UINT uSel = m_ListStatus.GetSelectionMark();
	if(uSel==0xffffffff) return;//û��ѡ��

	CString str1 = m_ListStatus.GetItemText(uSel,1);
	CString str2 = m_ListStatus.GetItemText(uSel,2);
	CString str3 = m_ListStatus.GetItemText(uSel,3);
	CString str4 = m_ListStatus.GetItemText(uSel,4);

	char buffer[200]={0};
	//wsprintfA(buffer,"������ʾ��ϸ��Ϣ��ѡ����Ŀ %d ����1 %s",uSel,str1.GetBuffer());
	//AfxMessageBox(buffer);


	//��ӻ����ؽ��������
	int ret = 0;
	task_result_req req={0};
	task_result_info *pres = NULL;
	task_result_info *p= NULL;

	memcpy(req.guid,(char *)str1.GetBuffer(),strlen((char*)str1.GetBuffer()));

	ret = g_packmanager.GenTaskResultPack(req,&pres);
	if (ret < 0){

		CLog::Log(LOG_LEVEL_WARNING,"Gen Task Result Pack Error,Code : %d\n",ret);
		return;

	}

	int num = ret/sizeof(task_result_info);
	int i = 0;

	char temp[128];

	memset(buffer,0,200);
	for(i=0;i < num;i ++ ){

		memset(temp,0,128);
		p = &pres[i];
		CLog::Log(LOG_LEVEL_WARNING,"Get Task Result : %s ,%d,%s\n",p->john,p->status,p->password);
		
		sprintf(temp,"Hash : %s, status :%d ,password : %s\n",p->john,p->status,p->password);

		strcat(buffer,temp);
		//strcat(buffer,"\n");5
		
	}
	
	g_packmanager._free(pres);

//	char buffer[200];
//	wsprintfA(buffer,"������ʾ��ϸ��Ϣ��ѡ����Ŀ %d ����1 %s",uSel,str1.GetBuffer());
	if(p->password[0]==0)
	AfxMessageBox("û�н��ܽ��");
	else
	AfxMessageBox(buffer);

	*pResult = 0;
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

void CDlgTaskStatus::OnBnClickedBtnRefresh()
{
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

	//m_tasklist.SetRedraw(FALSE);
	m_ListStatus.DeleteAllItems();  
  


	for(i = 0;i < count ;i ++ ){

		p = &pres[i];
	
		m_ListStatus.InsertItem(i,"");
		m_ListStatus.SetItemText(i,1,(char *)p->guid);
		sprintf(tmpbuf,"%f",p->m_progress);
		m_ListStatus.SetItemText(i,2,tmpbuf);
		wsprintfA(tmpbuf,"%d",p->m_split_number);
		m_ListStatus.SetItemText(i,3,tmpbuf);
		wsprintfA(tmpbuf,"%d",p->m_fini_number);
		m_ListStatus.SetItemText(i,4,tmpbuf);
		memset(tmpbuf,0,128);
		GetStatusStrByCmd(p->status,tmpbuf);
		m_ListStatus.SetItemText(i,5,tmpbuf);
	}

	g_packmanager._free(pres);
}

//��ʼ�ƽ�
void CDlgTaskStatus::OnBnClickedBtnStart()
{
	task_start_req  req={0};
	task_status_res res={0};

	int n = m_ListStatus.GetItemCount();
	
	for(int i = 0; i < n; i++)
	{
		int bFlag = m_ListStatus.GetCheck(i);
		if(bFlag)
		{
			//ѡ�еģ���ʼ�ƽ�
			char buffer[100]={0};
			m_ListStatus.GetItemText(i,1,buffer,100);
			memcpy(req.guid,buffer,40);

			int	ret = g_packmanager.GenTaskStartPack(req,&res);
			if (ret < 0){

				CLog::Log(LOG_LEVEL_WARNING,"��ʼ����  %s  ʧ�ܣ�������� : %d\n",req.guid,ret);
			}

		}
	}
}
//��ͣ
void CDlgTaskStatus::OnBnClickedBtnPause()
{
	task_pause_req  req={0};
	task_status_res res={0};

	int n = m_ListStatus.GetItemCount();
	
	for(int i = 0; i < n; i++)
	{
		int bFlag = m_ListStatus.GetCheck(i);
		if(bFlag)
		{
			//ѡ�еģ���ʼ�ƽ�
			char buffer[100]={0};
			m_ListStatus.GetItemText(i,1,buffer,100);
			memcpy(req.guid,buffer,40);

			int	ret = g_packmanager.GenTaskPausePack(req,&res);
			if (ret < 0){

				CLog::Log(LOG_LEVEL_WARNING,"��ʼ����  %s  ʧ�ܣ�������� : %d\n",req.guid,ret);
			}

		}
	}
}
//ɾ��ѡ��
void CDlgTaskStatus::OnBnClickedBtnDelete()
{
	task_delete_req  req={0};
	task_status_res res={0};

	int n = m_ListStatus.GetItemCount();
	
	for(int i = 0; i < n; i++)
	{
		int bFlag = m_ListStatus.GetCheck(i);
		if(bFlag)
		{
			//ѡ�еģ���ʼ�ƽ�
			char buffer[100]={0};
			m_ListStatus.GetItemText(i,1,buffer,100);
			memcpy(req.guid,buffer,40);

			int	ret = g_packmanager.GenTaskDeletePackt(req,&res);
			if (ret < 0){

				CLog::Log(LOG_LEVEL_WARNING,"��ʼ����  %s  ʧ�ܣ�������� : %d\n",req.guid,ret);
			}

		}
	}
}
//ֹͣ�ƽ�
void CDlgTaskStatus::OnBnClickedBtnStop()
{
	task_stop_req  req={0};
	task_status_res res={0};

	int n = m_ListStatus.GetItemCount();
	
	for(int i = 0; i < n; i++)
	{
		int bFlag = m_ListStatus.GetCheck(i);
		if(bFlag)
		{
			//ѡ�еģ���ʼ�ƽ�
			char buffer[100]={0};
			m_ListStatus.GetItemText(i,1,buffer,100);
			memcpy(req.guid,buffer,40);

			int	ret = g_packmanager.GenTaskStopPack(req,&res);
			if (ret < 0){

				CLog::Log(LOG_LEVEL_WARNING,"��ʼ����  %s  ʧ�ܣ�������� : %d\n",req.guid,ret);
			}

		}
	}
}
