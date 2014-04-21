// DlgUploadTask.cpp : implementation file
//

#include "stdafx.h"
#include "CtlClient.h"
#include "DlgUploadTask.h"

#include "algorithm_types.h"

#include "PackManager.h"
#include "err.h"
#include "CLog.h"


// CDlgUploadTask dialog

IMPLEMENT_DYNAMIC(CDlgUploadTask, CDialog)

CDlgUploadTask::CDlgUploadTask(CWnd* pParent /*=NULL*/)
: CDialog(CDlgUploadTask::IDD, pParent)
, m_startlength(_T(""))
, m_endlength(_T(""))
, m_filename(_T(""))

{
	
}

CDlgUploadTask::~CDlgUploadTask()
{

}

void CDlgUploadTask::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ALG, m_comboalgo);
	DDX_Control(pDX, IDC_COMBO_CHARSET, m_combocharset);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_dectype);
	DDX_Control(pDX, IDC_EDIT_LEN_MIN, m_EditLenMin);
	DDX_Control(pDX, IDC_EDIT_LEN_MAX, m_EditLenMax);
	DDX_Control(pDX, IDC_SLIDER_LEN_MIN, m_SlideLenMin);
	DDX_Control(pDX, IDC_SLIDER2, m_SlideLenMax);
	DDX_Control(pDX, IDC_EDIT_TYPE_TYPE, m_EditType);
	DDX_Control(pDX, IDC_STATIC_LEN_MIN, m_StaticLenMin);
	DDX_Control(pDX, IDC_STATIC_LEN_MAX, m_StaticLenMax);
	DDX_Control(pDX, IDC_COMBO_SEL, m_CombBoxSel);
}


BEGIN_MESSAGE_MAP(CDlgUploadTask, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SEL_FILE, &CDlgUploadTask::OnBnClickedButton1)
	ON_BN_CLICKED(IDOK, &CDlgUploadTask::OnBnClickedOk)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER2, &CDlgUploadTask::OnNMCustomdrawSlider2)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_LEN_MIN, &CDlgUploadTask::OnNMCustomdrawSliderLenMin)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, &CDlgUploadTask::OnCbnSelchangeComboType)
END_MESSAGE_MAP()


// CDlgUploadTask message handlers

VOID CDlgUploadTask::ComboInit(){

	m_combocharset.InsertString(0,_T("数字"));
	m_combocharset.InsertString(1,_T("小写英文字母"));
	m_combocharset.InsertString(2,_T("大写英文字母"));
	m_combocharset.InsertString(3,_T("英文字母"));
	m_combocharset.InsertString(4,_T("小写英文+数字"));
	m_combocharset.InsertString(5,_T("大写英文+数字"));
	m_combocharset.InsertString(6,_T("英文字母+数字"));
	m_combocharset.InsertString(7,_T("ascii码字符"));

	m_dectype.InsertString(0,_T("暴力破解"));
	m_dectype.InsertString(1,_T("字典攻击"));
	m_dectype.InsertString(2,_T("特定规则"));
	m_dectype.InsertString(3,_T("掩码攻击"));
	m_dectype.InsertString(4,_T("彩虹表"));


	m_comboalgo.InsertString(0,_T("algo_md4"));
	m_comboalgo.InsertString(1,_T("algo_md5"));
	m_comboalgo.InsertString(2,_T("algo_md5md5"));
	m_comboalgo.InsertString(3,_T("algo_md5unix"));
	m_comboalgo.InsertString(4,_T("algo_mediawiki"));
	m_comboalgo.InsertString(5,_T("algo_malgo_oscommerce"));
	m_comboalgo.InsertString(6,_T("algo_ipb2"));
	m_comboalgo.InsertString(7,_T("algo_joomla"));
	m_comboalgo.InsertString(8,_T("algo_vbulletin"));
	m_comboalgo.InsertString(9,_T("algo_desunix"));
	m_comboalgo.InsertString(10,_T("algo_sha1"));


	m_comboalgo.InsertString(11,_T("algo_sha1sha1"));
	m_comboalgo.InsertString(12,_T("algo_sha256"));
	m_comboalgo.InsertString(13,_T("algo_sha256unix"));
	m_comboalgo.InsertString(14,_T("algo_sha512"));
	m_comboalgo.InsertString(15,_T("algo_sha512unix"));
	m_comboalgo.InsertString(16,_T("algo_msoffice_old"));
	m_comboalgo.InsertString(17,_T("algo_msoffice"));
	m_comboalgo.InsertString(18,_T("algo_django256"));
	m_comboalgo.InsertString(19,_T("algo_zip"));
	m_comboalgo.InsertString(20,_T("algo_rar"));
	m_comboalgo.InsertString(21,_T("algo_apr1"));

	m_comboalgo.InsertString(22,_T("algo_bfunix"));
	m_comboalgo.InsertString(23,_T("algo_dmg"));
	m_comboalgo.InsertString(24,_T("algo_drupal7"));
	m_comboalgo.InsertString(25,_T("algo_lm"));
	m_comboalgo.InsertString(26,_T("algo_luks"));
	m_comboalgo.InsertString(27,_T("algo_mscash"));
	m_comboalgo.InsertString(28,_T("algo_mscash2"));
	m_comboalgo.InsertString(29,_T("algo_mssql_2000"));
	m_comboalgo.InsertString(30,_T("algo_mssql_2005"));
	m_comboalgo.InsertString(31,_T("algo_mssql_2012"));
	m_comboalgo.InsertString(32,_T("algo_mysql5"));

	m_comboalgo.InsertString(33,_T("algo_nsldap"));
	m_comboalgo.InsertString(34,_T("algo_nsldaps"));
	m_comboalgo.InsertString(35,_T("algo_ntlm"));
	m_comboalgo.InsertString(36,_T("algo_o5logon"));
	m_comboalgo.InsertString(37,_T("algo_oracle_old"));
	m_comboalgo.InsertString(38,_T("algo_oracle11g"));
	m_comboalgo.InsertString(39,_T("algo_osx_old"));
	m_comboalgo.InsertString(40,_T("algo_osxlion"));
	m_comboalgo.InsertString(41,_T("algo_phpbb3"));
	m_comboalgo.InsertString(42,_T("algo_pixmd5"));
	m_comboalgo.InsertString(43,_T("algo_privkey"));

	m_comboalgo.InsertString(44,_T("algo_ripemd160"));
	m_comboalgo.InsertString(45,_T("algo_sapg"));
	m_comboalgo.InsertString(46,_T("algo_sl3"));
	m_comboalgo.InsertString(47,_T("algo_smf"));
	m_comboalgo.InsertString(48,_T("algo_wordpress"));
	m_comboalgo.InsertString(49,_T("algo_wpa"));


	m_comboalgo.SetCurSel(0);
	m_combocharset.SetCurSel(0);
	m_dectype.SetCurSel(0);
}

BOOL CDlgUploadTask::OnInitDialog(){

	CDialog::OnInitDialog();
    m_toolTip.Create(this,TTS_ALWAYSTIP | TTS_BALLOON);
    m_toolTip.SetMaxTipWidth(200);
    m_toolTip.AddTool(GetDlgItem(IDC_COMBO_ALG), "选择提交文件的算法类型");
    m_toolTip.AddTool(GetDlgItem(IDC_COMBO_CHARSET), "选择目标破解的字符集类型");
    m_toolTip.AddTool(GetDlgItem(IDC_COMBO_TYPE), "选择使用哪种方法破解");
    m_toolTip.AddTool(GetDlgItem(IDC_EDIT_LEN_MIN), "暴力破解密码的最小长度");
    m_toolTip.AddTool(GetDlgItem(IDC_EDIT_LEN_MAX), "暴力破解密码的最大长度");
    m_toolTip.AddTool(GetDlgItem(IDC_EDIT_FILE_PATH), "输入文件路径，或者点击后面的选择文件按钮选择文件");
    m_toolTip.AddTool(GetDlgItem(IDC_BUTTON_SEL_FILE), "选择破解的目标文件(hash文件或者加密文件)");
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_TYPE_TYPE), "掩码输入说明：\r\n掩码破解必须包括已知字符和?号通配符，比如 138???? 表示通配138开头的7位密码");
	m_toolTip.AddTool(GetDlgItem(IDOK), "上传任务");

	m_SlideLenMin.SetRange(1,20);
	m_SlideLenMax.SetRange(1,20);
	m_SlideLenMin.SetLineSize(1);
	m_SlideLenMax.SetLineSize(1);
	m_EditLenMin.SetWindowTextA("1");
	m_EditLenMax.SetWindowTextA("1");

	m_SlideLenMin.SetPageSize(1);
	m_SlideLenMax.SetPageSize(1);


	m_pDlgTaskStatus = NULL;

	ComboInit();

	ProcessControl(0);
	return TRUE;
}

void CDlgUploadTask::SetCDlgTaskStatus(CDlgTaskStatus *pSlgTS)
{
	m_pDlgTaskStatus = pSlgTS;
}

BOOL CDlgUploadTask::AddToTaskList(int nAlgo,int nCharset,int nType,int nIsFile,int nLenMin,int nLenMax,char *psFile)
{
	return 0;
}

BOOL CDlgUploadTask::PreTranslateMessage(MSG* pMsg) 
{
    if (pMsg->message == WM_MOUSEMOVE)
    {
        if( pMsg->hwnd == GetDlgItem(IDC_COMBO_ALG)->m_hWnd || 
            pMsg->hwnd == GetDlgItem(IDC_COMBO_CHARSET)->m_hWnd  || 
            pMsg->hwnd == GetDlgItem(IDC_COMBO_TYPE)->m_hWnd  || 
            pMsg->hwnd == GetDlgItem(IDC_EDIT_LEN_MIN)->m_hWnd  || 
            pMsg->hwnd == GetDlgItem(IDC_EDIT_LEN_MAX)->m_hWnd  || 
            pMsg->hwnd == GetDlgItem(IDC_EDIT_FILE_PATH)->m_hWnd  || 
            pMsg->hwnd == GetDlgItem(IDC_BUTTON_SEL_FILE)->m_hWnd|| 
            pMsg->hwnd == GetDlgItem(IDC_EDIT_TYPE_TYPE)->m_hWnd|| 
            pMsg->hwnd == GetDlgItem(IDOK)->m_hWnd )			
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

void CDlgUploadTask::OnBnClickedButton1()
{
	OPENFILENAME	ofn ;
	static TCHAR pszFileName[MAX_PATH]={0};
	static TCHAR szFilter[] = TEXT ("选择hash文件 (*.hashes)\0*.hashes\0选择全部\0*.*\0");
	     
	ofn.lStructSize       = sizeof (OPENFILENAME) ;
	ofn.hwndOwner         = this->m_hWnd ;
	ofn.hInstance         = NULL ;
	ofn.lpstrFilter       = szFilter ;
	ofn.lpstrCustomFilter = NULL ;
	ofn.nMaxCustFilter    = 0 ;
	ofn.nFilterIndex      = 0 ;
	ofn.lpstrFile         = pszFileName ;
	ofn.nMaxFile          = MAX_PATH ;
	ofn.lpstrFileTitle    = NULL ;
	ofn.nMaxFileTitle     = MAX_PATH ;
	ofn.lpstrInitialDir   = NULL ;
	ofn.lpstrTitle        = NULL ;
	ofn.Flags             = 0 ;
	ofn.nFileOffset       = 0 ;
	ofn.nFileExtension    = 0 ;
	ofn.lpstrDefExt       = TEXT ("打开") ;
	ofn.lCustData         = 0L ;
	ofn.lpfnHook          = NULL ;
	ofn.lpTemplateName    = NULL ;
	 
	if ( !GetOpenFileName( &ofn))
	{		
		return;
	}
	SetDlgItemText(IDC_EDIT_FILE_PATH,pszFileName);
	m_filename = pszFileName;
}

void CDlgUploadTask::OnBnClickedOk()
{	
	
	/////////////////////////////////////////////////
	//读取输入数据	
	/////////////////////////////////////////////////
	//解密算法
	int loc_algo   = m_comboalgo.GetCurSel()+1;
	//解密字符集
	int loc_charset= m_combocharset.GetCurSel();
	//解密类型
	int loc_type   = m_dectype.GetCurSel();
	//是否文件类型
	//起始长度
	m_EditLenMin.GetWindowTextA(m_startlength);
	char *p = (LPSTR)(LPCTSTR)m_startlength.GetBuffer(); 
	int loc_len_start = strtoul(p,NULL,10);
	//结束长度
	m_EditLenMax.GetWindowTextA(m_endlength);
	p = (LPSTR)(LPCTSTR)m_endlength.GetBuffer();
	int loc_len_end = strtoul(p,NULL,10);
	//文件路径
	CString loc_file_name = m_filename;
	//字典或者彩虹表选择序号
	int loc_dic_sel = m_CombBoxSel.GetCurSel();
	//掩码输入
	CString loc_s_mask;
	m_EditType.GetWindowTextA(loc_s_mask);
	//算法选择


	/////////////////////////////////////////////////
	//检测错误输入
	/////////////////////////////////////////////////
	if(loc_file_name.GetLength()<1)
	{
		AfxMessageBox("上传文件不能为空");
		return;
	}
	if(loc_type==0)//暴力
	{
		if((m_startlength.GetLength()<1)||(m_endlength.GetLength()<1))
		{
			AfxMessageBox("请输入密码起始长度");
			return;
		}
		if((loc_len_start<1)||(loc_len_start>loc_len_end))
		{
			AfxMessageBox("密码起始长度错误");
			return;
		}
	}
	else if(loc_type==1)//字典
	{
	}
	else if(loc_type==2)//特定规则
	{
	}
	else if(loc_type==3)//掩码
	{
		if(loc_s_mask.GetLength()<1)
		{
			AfxMessageBox("请输入掩码信息");
			return;
		}
		if(-1==loc_s_mask.Find('?') )
		{
			AfxMessageBox("掩码信息错误");
			return;
		}

		char *p_mask = loc_s_mask.GetBuffer();
		for(int i=0; i<loc_s_mask.GetLength(); i++)
		{
			if(p_mask[i]&0x80)
			{
				AfxMessageBox("不能包含中文等宽字符");
				return;
			}
		}
	}
	else if(loc_type==4)//彩虹表
	{
	}


	
	/////////////////////////////////////////////////
	//处理提交数据
	/////////////////////////////////////////////////
	char buf[1024];
	memset(buf,0,1024);
	
	crack_task newtask={0};

	newtask.algo = m_comboalgo.GetCurSel()+1;
	newtask.charset = m_combocharset.GetCurSel();
	newtask.type = m_dectype.GetCurSel();

	newtask.special = 0;


	//增加掩码等信息
	if(loc_type==0)
	{
		p = (LPSTR)(LPCTSTR)m_endlength.GetBuffer();
		newtask.endLength = strtoul(p,NULL,10);

		p = (LPSTR)(LPCTSTR)m_startlength.GetBuffer(); 
		newtask.startLength = strtoul(p,NULL,10);
	}
	if(loc_type==1)//字典
		newtask.dict_idx = loc_dic_sel;
	else if(loc_type==3)//掩码
	{
		newtask.maskLength = loc_s_mask.GetLength();
		m_EditType.GetWindowTextA(newtask.masks,18);		
	}	

	struct task_upload_res ures={0};
	int ret =0;
	newtask.single = 1;

	if(newtask.single == 0){
		sprintf((char *)newtask.filename,"%s",m_filename);
		memcpy(g_packmanager.m_cur_local_file,newtask.filename,256);
	}else{
		//直接复制hash值
		strcpy((char*)newtask.filename, "00f13b2bcddb408220dc223e15dc2121");
	}
	
	ret = g_packmanager.DoTaskUploadPack(newtask,&ures);
	if (ret < 0){		
		ErrorMsg(ret);
		return ;
	}else if(newtask.single == 1){
		AfxMessageBox("Upload Task OK");
		return;
	}

	file_upload_req uploadreq;
	memset(&uploadreq,0,sizeof(file_upload_req));
	memcpy(uploadreq.guid,ures.guid,sizeof(ures.guid));
	memset(g_packmanager.m_cur_upload_guid,0,40);
	memcpy(g_packmanager.m_cur_upload_guid,ures.guid,40);

	file_upload_res uploadres;
	memset(&uploadres,0,sizeof(file_upload_res));
	
	//上传文件
	ret = g_packmanager.GenNewFileUploadPack(uploadreq,&uploadres);
	if (ret < 0){	
		ErrorMsg(ret);
		return ;

	}
	
	//上传文件开始
	file_upload_start_res uploadstartres={0};
	ret = g_packmanager.GenNewFileUploadStartPack(&uploadstartres);
	if (ret < 0){	
		ErrorMsg(ret);
		return ;
	}

//上传文件
	ret = g_packmanager.GenNewFileUploadingPack();
	if (ret < 0){			
		ErrorMsg(ret);
		return ;
	}

//上传文件结束

	file_upload_end_res uploadendres={0};

	ret = g_packmanager.GenNewFileUploadEndPack(&uploadendres);
	if (ret < 0){

		char buffer[MAX_PATH];
		g_packmanager.GetErrMsg(uploadendres.offset,buffer);
		AfxMessageBox(buffer);
		return ;

	}
	
	CString tmpStr1("Upload File End OK");

	AfxMessageBox(tmpStr1);
}

//最大长度
void CDlgUploadTask::OnNMCustomdrawSlider2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	char buffer[10];
	int i = m_SlideLenMax.GetPos();
	wsprintfA(buffer,"%d",i);
	m_EditLenMax.SetWindowTextA(buffer);

	if(i<m_SlideLenMin.GetPos())
	{
		m_SlideLenMin.SetPos(i);
		m_EditLenMin.SetWindowTextA(buffer);
	}
	*pResult = 0;
}

//最小长度
void CDlgUploadTask::OnNMCustomdrawSliderLenMin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	
	char buffer[10];
	int i = m_SlideLenMin.GetPos();
	wsprintfA(buffer,"%d",i);
	m_EditLenMin.SetWindowTextA(buffer);

	if(i>m_SlideLenMax.GetPos())
	{
		m_SlideLenMax.SetPos(i);
		m_EditLenMax.SetWindowTextA(buffer);
	}
	*pResult = 0;
}


void CDlgUploadTask::ErrorMsg(int id)
{
	if(id==ERR_INVALID_PARAM)
	{
		AfxMessageBox("参数非法");
	}
	else if(id==ERR_COMPRESS)
	{
		AfxMessageBox("压缩数据失败");
	}
	else if(id==ERR_UNCOMPRESS)
	{
		AfxMessageBox("解压缩数据失败");
	}
	else if(id==ERR_CONNECTIONLOST)
	{
		AfxMessageBox("与socket断开连接");
	}
	else {
		AfxMessageBox("未知错误");
	}
}

//
void CDlgUploadTask::ProcessControl(int id)
{	
	m_EditLenMin.ShowWindow(SW_HIDE);
	m_EditLenMax.ShowWindow(SW_HIDE);

	m_SlideLenMin.ShowWindow(SW_HIDE);
	m_SlideLenMax.ShowWindow(SW_HIDE);
	
	m_EditType.ShowWindow(SW_HIDE);
	m_CombBoxSel.ShowWindow(SW_HIDE);
	int nItem = m_CombBoxSel.GetCount();
	for(int i=0; i<nItem; i++)	m_CombBoxSel.DeleteString(0);

	m_StaticLenMin.ShowWindow(SW_HIDE);
	m_StaticLenMax.ShowWindow(SW_HIDE);

	if(id==0)//暴力
	{
		m_StaticLenMin.SetWindowTextA("起始长度");
		m_StaticLenMax.SetWindowTextA("结束长度");
		m_StaticLenMin.ShowWindow(1);
		m_StaticLenMax.ShowWindow(1);

		m_EditLenMin.ShowWindow(1);
		m_EditLenMax.ShowWindow(1);
		m_EditLenMin.SetWindowTextA("1");
		m_EditLenMax.SetWindowTextA("1");

		m_SlideLenMin.ShowWindow(1);
		m_SlideLenMax.ShowWindow(1);
		m_SlideLenMin.SetPos(1);
		m_SlideLenMax.SetPos(1);
	}
	else if(id==1)//字典
	{
		m_StaticLenMin.SetWindowTextA("选择字典文件");
		m_StaticLenMin.ShowWindow(1);

		m_CombBoxSel.ShowWindow(1);
		m_CombBoxSel.InsertString(0,"字典0");
		m_CombBoxSel.InsertString(1,"字典1");
		m_CombBoxSel.SetCurSel(0);
	}
	else if(id==2)
	{
		m_StaticLenMin.SetWindowTextA("暂不支持");
		m_StaticLenMin.ShowWindow(1);
	}
	else if(id==3)//掩码
	{
		m_StaticLenMin.SetWindowTextA("输入掩码");
		m_StaticLenMin.ShowWindow(1);
		m_EditType.ShowWindow(1);
	}
	else if(id==4)//彩虹 
	{
		m_StaticLenMin.SetWindowTextA("选择彩虹表");
		m_StaticLenMin.ShowWindow(1);
		
		m_CombBoxSel.ShowWindow(1);
		m_CombBoxSel.InsertString(0,"彩虹表0");
		m_CombBoxSel.InsertString(1,"彩虹表1");
		m_CombBoxSel.SetCurSel(0);
	}
	else{
		m_StaticLenMin.SetWindowTextA("无效选择");
		m_StaticLenMin.ShowWindow(1);
	}

}
void CDlgUploadTask::OnCbnSelchangeComboType()
{
	static int sel = 0;
	int k = m_dectype.GetCurSel();
	if(k!=sel)
	{
		sel = k;
		ProcessControl(sel);
	}
}
