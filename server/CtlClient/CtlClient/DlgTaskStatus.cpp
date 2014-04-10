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
	StopRefresh();
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
	ON_WM_CLOSE()
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
void CDlgTaskStatus::GenExampleListData()
{
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
	m_ListStatus.InsertColumn(2, _T("�㷨"), LVCFMT_LEFT, 60);
	m_ListStatus.InsertColumn(3, _T("����ʱ��"), LVCFMT_LEFT, 70);
	m_ListStatus.InsertColumn(4, _T("ʣ��ʱ��"), LVCFMT_LEFT, 70);
	m_ListStatus.InsertColumn(5, _T("����"), LVCFMT_LEFT, 50);
	m_ListStatus.InsertColumn(6, _T("�и����"), LVCFMT_LEFT, 80);
	m_ListStatus.InsertColumn(7, _T("��ɷ���"), LVCFMT_LEFT, 80);
	m_ListStatus.InsertColumn(8, _T("״̬"), LVCFMT_LEFT, 80);

	
	m_bThreadRefreshRunning = 0;
	StartRefresh();//����

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
	char* buffer = new char[num == 0?1:num*(sizeof(struct crack_hash)+100)];
	buffer[0] = 0;
	for(i=0;i < num;i ++ ){

		memset(temp,0,128);
		p = &pres[i];
		CLog::Log(LOG_LEVEL_WARNING,"Get Task Result : %s ,%d,%s\n",p->john,p->status,p->password);
		
		if(p->password[0])
		{
			sprintf(temp,"Hash : %s, status :%d ,password : %s\n",p->john,p->status,p->password);
			strcat(buffer,temp);
		}
		
	}

	g_packmanager._free(pres);

	if(buffer[0]==0)
		AfxMessageBox("û�н��ܽ��");
	else
		AfxMessageBox(buffer);

	delete []buffer;
	*pResult = 0;
}

BOOL CDlgTaskStatus::RefreshList(void)
{
	OnBnClickedBtnRefresh();
	return 0;
}


const char *crack_algorithm_string[80]={
	"none",
	"md4",//=0x01,        //MD4 plugin
	"md5",//             //MD5 plugin
	"md5md5",//          //md5(md5(pass)) plugin
	"md5unix",//         //MD5(Unix) plugin (shadow files)
	"mediawiki",//       //md5(salt.'-'.md5(password)) plugin (Wikimedia)
	"oscommerce",//      //md5(salt",//password) plugin (osCommerce)
	"ipb2",//            //md5(md5(salt).md5(pass)) plugin (IPB > 2.x)
	"joomla",//          //md5(password",//salt) plugin (joomla)
	"vbulletin",//       //md5(md5(pass).salt) plugin
	"desunix",//         //DES(Unix) plugin (.htpasswd)
	"sha1",//            //SHA1 plugin
	"sha1sha1",//        //sha1(sha1(pass)) plugin
	"sha256",//          //SHA-256 plugin
	"sha256unix",//      //SHA256(Unix) plugin (shadow files)
	"sha512",//          //SHA-512 plugin
	"sha512unix",//      //SHA512(Unix) plugin (shadow files)
	"msoffice_old",//    //MS Office XP/2003 plugin
	"msoffice",//        //MS Office 2007/2010/2013 plugin
	"django256",//       //Django SHA-256 plugin
	"zip",//             //ZIP passwords plugin
	"rar",//             //RAR3 passwords plugin
	"apr1",//            //Apache apr1 plugin
	"bfunix",//          //bfunix plugin (shadow files)
	"dmg",//             //FileVault (v1)  passwords plugin
	"drupal7",//         //Drupal >=7 hashes plugin
	"lm",//              //LM plugin
	"luks",//            //LUKS encrypted block device plugin
	"mscash",//          //Domain cached credentials plugin
	"mscash2",//         //Domain cached credentials v2 plugin
	"mssql_2000",//      //Microsoft SQL Server 2000 plugin
	"mssql_2005",//      //Microsoft SQL Server 2005 plugin
	"mssql_2012",//      //Microsoft SQL Server 2012 plugin
	"mysql5",//          //MySQL > 4.1 plugin
	"nsldap",//          //LDAP SHA plugin
	"nsldaps",//         //LDAP SSHA (salted SHA) plugin
	"ntlm",//            //NTLM plugin
	"o5logon",//         //Oracle TNS O5logon
	"oracle_old",//      //Oracle 7 up to 10r2 plugin
	"oracle11g",//       //Oracle 11g plugin
	"osx_old",//         //MacOSX <= 10.6 system passwords plugin
	"osxlion",//         //MacOSX Lion system passwords plugin
	"phpbb3",//          //phpBB3 hashes plugin
	"pixmd5",//          //Cisco PIX password hashes plugin
	"privkey",//         //SSH/SSL private key passphrase plugin
	"ripemd160",//       //RIPEMD-160 plugin
	"sapg",//            //SAP CODVN G passwords plugin
	"sl3",//             //Nokia SL3 plugin
	"smf",//             //SMF plugin
	"wordpress",//       //Wordpress hashes plugin
	"wpa",
	"max"
};

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

		//�㷨
		if( (p->m_algo>algo_wpa)||(p->m_algo<algo_md4) )
			sprintf(tmpbuf,"%s","�޷�ʶ��");
		else
			sprintf(tmpbuf,"%s",crack_algorithm_string[p->m_algo]);
		m_ListStatus.SetItemText(i,2,tmpbuf);

		unsigned int t_sec = p->m_running_time;
		unsigned int t_sec_s = t_sec%60 ;
		unsigned int t_sec_m = (t_sec/60)%60 ;
		unsigned int t_sec_h = t_sec/(60*60) ;
		if(t_sec_h>0)
			sprintf(tmpbuf,"%dСʱ%d��%d��",t_sec_h,t_sec_m,t_sec_s);
		else if(t_sec_m>0)
			sprintf(tmpbuf,"%d����%d��",t_sec_m,t_sec_s);
		else
			sprintf(tmpbuf,"%d��",t_sec_s);
		m_ListStatus.SetItemText(i,3,tmpbuf);


		 t_sec = p->m_remain_time;
		 t_sec_s = t_sec%60 ;
		 t_sec_m = (t_sec/60)%60 ;
		 t_sec_h = t_sec/(60*60) ;
		if(t_sec_h>0)
			sprintf(tmpbuf,"%dСʱ%d��%d��",t_sec_h,t_sec_m,t_sec_s);
		else if(t_sec_m>0)
			sprintf(tmpbuf,"%d����%d��",t_sec_m,t_sec_s);
		else
			sprintf(tmpbuf,"%d��",t_sec_s);
		m_ListStatus.SetItemText(i,4,tmpbuf);

		sprintf(tmpbuf,"%.0f%%",(p->m_progress)*100.0);
		m_ListStatus.SetItemText(i,5,tmpbuf);
		
		wsprintfA(tmpbuf,"%d",p->m_split_number);
		m_ListStatus.SetItemText(i,6,tmpbuf);
		
		wsprintfA(tmpbuf,"%d",p->m_fini_number);
		m_ListStatus.SetItemText(i,7,tmpbuf);
		
		memset(tmpbuf,0,128);
		GetStatusStrByCmd(p->status,tmpbuf);
		m_ListStatus.SetItemText(i,8,tmpbuf);
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


//�Զ�ˢ�������б�
void *CDlgTaskStatus::ThreadRefresh(void *par)
{
	CDlgTaskStatus *p = (CDlgTaskStatus*)par;
	
	while(p->m_bThreadRefreshStop!=TRUE)
	{
		p->RefreshList();
		g_packmanager.StartSleep(p->m_hStopRefresh,10*1000);//��10��
	}
	return 0;
}
void CDlgTaskStatus::StartRefresh(void)
{	
	if(m_bThreadRefreshRunning==TRUE)
	{
		CLog::Log(LOG_LEVEL_WARNING,"�߳������У�����Ҫ�ٴ���\n");
		return;
	}
	
	m_bThreadRefreshStop = FALSE;
	m_bThreadRefreshRunning = TRUE;

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	pthread_mutex_t running_mutex;
	pthread_cond_t keeprunning_cv;
	pthread_mutex_init(&running_mutex, NULL);
	pthread_cond_init(&keeprunning_cv, NULL);
	
	m_hStopRefresh = g_packmanager.CreateSleep();
	
	int returnValue = pthread_create( &m_ThreadRefresh, &attr, ThreadRefresh, (void *)this);
	if( returnValue != 0 )
	{
		CLog::Log(LOG_LEVEL_ERROR,"�����߳�ʧ�ܣ��������: %d\n", returnValue);
		m_bThreadRefreshStop = TRUE;
		m_bThreadRefreshRunning = FALSE;
		g_packmanager.StopSleep(m_hStopRefresh);
	}
	else
	{
		CLog::Log(LOG_LEVEL_NOMAL,"�����̴߳����ɹ�\n");
		m_bThreadRefreshStop = FALSE;
		m_bThreadRefreshRunning = TRUE;
	}
}

void CDlgTaskStatus::StopRefresh(void)
{
	if(m_bThreadRefreshRunning==FALSE)
	{
		CLog::Log(LOG_LEVEL_NOMAL,"�߳�û��������\n");
		return;
	}

	m_bThreadRefreshStop = TRUE;
	g_packmanager.StopSleep(m_hStopRefresh);
	int returnValue = pthread_join(m_ThreadRefresh, NULL);
	if( returnValue != 0 )
	{
		CLog::Log(LOG_LEVEL_ERROR,"�߳��˳�ʧ�ܣ��������: %d\n", returnValue);
	}
	else{
		CLog::Log(LOG_LEVEL_NOMAL,"�̳߳ɹ��˳�\n");
	}
	m_bThreadRefreshRunning = FALSE;
}
