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
	ON_WM_CLOSE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


void CDlgTaskStatus::GetStatusStrByCmd(char cmd,char *pdes){
	switch(cmd){

		case CT_STATUS_READY:
			
			strcpy(pdes,"运行就绪");
			break;
		case CT_STATUS_FINISHED:

			strcpy(pdes,"破解完成");
			break;
		case CT_STATUS_FAILURE:
	
			strcpy(pdes,"破解失败");
			break;
		case CT_STATUS_RUNNING:

			strcpy(pdes,"运行中");
			break;
		case CT_STATUS_PAUSED:
			
			strcpy(pdes,"任务已暂停");
			break;
		case CT_STATUS_DELETED:
			
			strcpy(pdes,"任务已删除");
			break;
		default:
	
			strcpy(pdes,"状态未知");
			break;	

	}
	return;

}

BOOL CDlgTaskStatus::OnInitDialog()
{
	CDialog::OnInitDialog();

	//初始化信息
    m_toolTip.Create(this,TTS_ALWAYSTIP | TTS_BALLOON);
    m_toolTip.SetMaxTipWidth(200);
    m_toolTip.AddTool(GetDlgItem(IDC_TASK_LIST), "双击条目察看详细信息");
    m_toolTip.AddTool(GetDlgItem(IDC_BTN_START), "开始选中项目的破解");
    m_toolTip.AddTool(GetDlgItem(IDC_BTN_PAUSE), "暂停选中项目的破解");
    m_toolTip.AddTool(GetDlgItem(IDC_BTN_DELETE), "删除选中的项目任务");
    m_toolTip.AddTool(GetDlgItem(IDC_BTN_STOP), "停止选中项目的破解");
    m_toolTip.AddTool(GetDlgItem(IDC_BTN_REFRESH), "刷新列表中任务的进度信息");

	//初始化CListCtrl
	ListView_SetExtendedListViewStyle(m_ListStatus.m_hWnd,LVS_EX_GRIDLINES| LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES );


	m_ListStatus.InsertColumn(0, _T("选择"), LVCFMT_LEFT, 40);
	m_ListStatus.InsertColumn(1, _T("状态"), LVCFMT_LEFT, 70);
	m_ListStatus.InsertColumn(2, _T("GUID"), LVCFMT_LEFT, 160);
	m_ListStatus.InsertColumn(3, _T("算法"), LVCFMT_LEFT, 60);
	m_ListStatus.InsertColumn(4, _T("进度"), LVCFMT_LEFT, 60);
	m_ListStatus.InsertColumn(5, _T("速度"), LVCFMT_LEFT, 60);
	m_ListStatus.InsertColumn(6, _T("剩余时间"), LVCFMT_LEFT, 70);
	m_ListStatus.InsertColumn(7, _T("已用时间"), LVCFMT_LEFT, 70);
	m_ListStatus.InsertColumn(8, _T("切割份数"), LVCFMT_LEFT, 70);
	m_ListStatus.InsertColumn(9, _T("完成份数"), LVCFMT_LEFT, 70);

	SetTimer(1,3000,NULL);//启动定时器1,定时时间是10秒

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDlgTaskStatus::OnTimer(UINT nIDEvent) 
{
	OnBnClickedBtnRefresh();
	CDialog::OnTimer(nIDEvent);
}

void CDlgTaskStatus::OnClose()
{
	KillTimer(1);    
	CDialog::OnClose();
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
		return CDialog::PreTranslateMessage(pMsg);
    }
    return CDialog::PreTranslateMessage(pMsg);
}

//双击获取详细信息
void CDlgTaskStatus::OnNMDblclkListTask(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	UINT uSel = m_ListStatus.GetSelectionMark();
	if(uSel==0xffffffff) return;//没有选择

	CString str1 = m_ListStatus.GetItemText(uSel,2);

	//添加获得相关结果的请求
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
	const char* status_msg[] = {"就绪", "完成1", "运行", "完成2", "错误"};

	CString temp, buffer("");
	for(i=0;i < num;i ++ ){
		p = &pres[i];
		
		CString st("未知");
		if(p->status >= HASH_STATUS_READY || p->status <= HASH_STATUS_FAILURE)
			st = status_msg[p->status];

		CLog::Log(LOG_LEVEL_WARNING,"Hash:%s, status:%s, password: %s\n",p->john, st, p->password);
		
		if(p->status == HASH_STATUS_FINISH)
			temp.Format("哈希值：%s, 状态：%s , 密码:%s\n", p->john, st, p->password);
		else if(p->status == HASH_STATUS_NO_PASS)
			temp.Format("哈希值：%s, 状态：%s , 未解出密码\n", p->john, st);
		else
			temp.Format("哈希值：%s, 状态：%s\n", p->john, st);
		buffer += temp;
	}

	g_packmanager._free(pres);

	AfxMessageBox(buffer);

	*pResult = 0;
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

	//先保存标记状态信息
	int n = m_ListStatus.GetItemCount();
	CString sFlags;
	for(int i=0; i<n; i++)
	{
		int bFlag = m_ListStatus.GetCheck(i);
		if(bFlag)
		{
			char buffer[100]={0};
			m_ListStatus.GetItemText(i,2,buffer,100);//guid
			sFlags=sFlags+buffer;
		}
	}

	//m_tasklist.SetRedraw(FALSE);
	m_ListStatus.DeleteAllItems();  
 
	for(i = 0;i < count ;i ++ ){

		p = &pres[i];

		//0选择
		m_ListStatus.InsertItem(i,"");

		//1状态
		memset(tmpbuf,0,128);
		GetStatusStrByCmd(p->status,tmpbuf);
		m_ListStatus.SetItemText(i,1,tmpbuf);

		//2任务GUID
		m_ListStatus.SetItemText(i,2,(char *)p->guid);
		if(sFlags.Find((char*)p->guid,0)>=0)
		{
			m_ListStatus.SetCheck(i,1);
		}

		//3算法
		if( (p->m_algo>algo_wpa)||(p->m_algo<algo_md4) )
			sprintf(tmpbuf,"%s","无法识别");
		else
			sprintf(tmpbuf,"%s",crack_algorithm_string[p->m_algo]);
		m_ListStatus.SetItemText(i,3,tmpbuf);

		
		//4进度
		sprintf(tmpbuf,"%.1f%%",(p->m_progress));
		m_ListStatus.SetItemText(i,4,tmpbuf);

		//5速度
		if(p->m_speed>1000000.0)
			sprintf(tmpbuf,"%.2fM c/s",(p->m_speed/1000000.0));
		else if(p->m_speed>1000.0)
			sprintf(tmpbuf,"%.2fK c/s",(p->m_speed/1000.0));
		else
			sprintf(tmpbuf,"%.0f c/s",(p->m_speed));
		m_ListStatus.SetItemText(i,5,tmpbuf);

		
		//6 剩余时间
		unsigned int t_sec = p->m_remain_time;
		unsigned int t_sec_s = t_sec%60 ;
		unsigned int t_sec_m = (t_sec/60)%60 ;
		unsigned int t_sec_h = t_sec/(60*60) ;
		if(t_sec_h>0)
			sprintf(tmpbuf,"%d时%d分%d秒",t_sec_h,t_sec_m,t_sec_s);
		else if(t_sec_m>0)
			sprintf(tmpbuf,"%d分%d秒",t_sec_m,t_sec_s);
		else
			sprintf(tmpbuf,"%d秒",t_sec_s);
		if(t_sec==-1)
		m_ListStatus.SetItemText(i,6,"∞");
		else
		m_ListStatus.SetItemText(i,6,tmpbuf);

		//7 已用时间
		t_sec = p->m_running_time;
		t_sec_s = t_sec%60 ;
		t_sec_m = (t_sec/60)%60 ;
		t_sec_h = t_sec/(60*60) ;
		if(t_sec_h>0)
			sprintf(tmpbuf,"%d时%d分%d秒",t_sec_h,t_sec_m,t_sec_s);
		else if(t_sec_m>0)
			sprintf(tmpbuf,"%d分%d秒",t_sec_m,t_sec_s);
		else
			sprintf(tmpbuf,"%d秒",t_sec_s);
		if(t_sec==-1)
		m_ListStatus.SetItemText(i,7,"∞");
		else
		m_ListStatus.SetItemText(i,7,tmpbuf);
		
		//8 切割份数
		wsprintfA(tmpbuf,"%d",p->m_split_number);
		m_ListStatus.SetItemText(i,8,tmpbuf);
		
		//9 完成份数
		wsprintfA(tmpbuf,"%d",p->m_fini_number);
		m_ListStatus.SetItemText(i,9,tmpbuf);
	}
	g_packmanager._free(pres);
}

//开始破解
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
			//选中的，开始破解
			char buffer[100]={0};
			m_ListStatus.GetItemText(i,2,buffer,100);
			memcpy(req.guid,buffer,40);

			int	ret = g_packmanager.GenTaskStartPack(req,&res);
			if (ret < 0){

				CLog::Log(LOG_LEVEL_WARNING,"开始任务  %s  失败，错误代码 : %d\n",req.guid,ret);
			}
			m_ListStatus.SetCheck(i,0);
		}
	}
}
//暂停
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
			//选中的，开始破解
			char buffer[100]={0};
			m_ListStatus.GetItemText(i,2,buffer,100);
			memcpy(req.guid,buffer,40);

			int	ret = g_packmanager.GenTaskPausePack(req,&res);
			if (ret < 0){

				CLog::Log(LOG_LEVEL_WARNING,"开始任务  %s  失败，错误代码 : %d\n",req.guid,ret);
			}
			m_ListStatus.SetCheck(i,0);
		}
	}
}
//删除选中
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
			//选中的删除
			char buffer[100]={0};
			m_ListStatus.GetItemText(i,2,buffer,100);
			memcpy(req.guid,buffer,40);

			int	ret = g_packmanager.GenTaskDeletePackt(req,&res);
			if (ret < 0){

				CLog::Log(LOG_LEVEL_WARNING,"开始任务  %s  失败，错误代码 : %d\n",req.guid,ret);
			}

		}
	}
	OnBnClickedBtnRefresh();
}
//停止破解
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
			//选中的，开始破解
			char buffer[100]={0};
			m_ListStatus.GetItemText(i,2,buffer,100);
			memcpy(req.guid,buffer,40);

			int	ret = g_packmanager.GenTaskStopPack(req,&res);
			if (ret < 0){

				CLog::Log(LOG_LEVEL_WARNING,"开始任务  %s  失败，错误代码 : %d\n",req.guid,ret);
			}
			m_ListStatus.SetCheck(i,0);
		}
	}
}
