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

	m_combocharset.InsertString(0,_T("����"));
	m_combocharset.InsertString(1,_T("СдӢ����ĸ"));
	m_combocharset.InsertString(2,_T("��дӢ����ĸ"));
	m_combocharset.InsertString(3,_T("Ӣ����ĸ"));
	m_combocharset.InsertString(4,_T("СдӢ��+����"));
	m_combocharset.InsertString(5,_T("��дӢ��+����"));
	m_combocharset.InsertString(6,_T("Ӣ����ĸ+����"));
	m_combocharset.InsertString(7,_T("ascii���ַ�"));

	m_dectype.InsertString(0,_T("�����ƽ�"));
	m_dectype.InsertString(1,_T("�ֵ乥��"));
	m_dectype.InsertString(2,_T("�ض�����"));
	m_dectype.InsertString(3,_T("���빥��"));
	m_dectype.InsertString(4,_T("�ʺ��"));


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
    m_toolTip.AddTool(GetDlgItem(IDC_COMBO_ALG), "ѡ���ύ�ļ����㷨����");
    m_toolTip.AddTool(GetDlgItem(IDC_COMBO_CHARSET), "ѡ��Ŀ���ƽ���ַ�������");
    m_toolTip.AddTool(GetDlgItem(IDC_COMBO_TYPE), "ѡ��ʹ�����ַ����ƽ�");
    m_toolTip.AddTool(GetDlgItem(IDC_EDIT_LEN_MIN), "�����ƽ��������С����");
    m_toolTip.AddTool(GetDlgItem(IDC_EDIT_LEN_MAX), "�����ƽ��������󳤶�");
    m_toolTip.AddTool(GetDlgItem(IDC_EDIT_FILE_PATH), "�����ļ�·�������ߵ�������ѡ���ļ���ťѡ���ļ�");
    m_toolTip.AddTool(GetDlgItem(IDC_BUTTON_SEL_FILE), "ѡ���ƽ��Ŀ���ļ�(hash�ļ����߼����ļ�)");
	m_toolTip.AddTool(GetDlgItem(IDC_EDIT_TYPE_TYPE), "��������˵����\r\n�����ƽ���������֪�ַ���?��ͨ��������� 138???? ��ʾͨ��138��ͷ��7λ����");
	m_toolTip.AddTool(GetDlgItem(IDOK), "�ϴ�����");

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
	static TCHAR szFilter[] = TEXT ("ѡ��hash�ļ� (*.hashes)\0*.hashes\0ѡ��ȫ��\0*.*\0");
	     
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
	ofn.lpstrDefExt       = TEXT ("��") ;
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
	//��ȡ��������	
	/////////////////////////////////////////////////
	//�����㷨
	int loc_algo   = m_comboalgo.GetCurSel()+1;
	//�����ַ���
	int loc_charset= m_combocharset.GetCurSel();
	//��������
	int loc_type   = m_dectype.GetCurSel();
	//�Ƿ��ļ�����
	//��ʼ����
	m_EditLenMin.GetWindowTextA(m_startlength);
	char *p = (LPSTR)(LPCTSTR)m_startlength.GetBuffer(); 
	int loc_len_start = strtoul(p,NULL,10);
	//��������
	m_EditLenMax.GetWindowTextA(m_endlength);
	p = (LPSTR)(LPCTSTR)m_endlength.GetBuffer();
	int loc_len_end = strtoul(p,NULL,10);
	//�ļ�·��
	CString loc_file_name = m_filename;
	//�ֵ���߲ʺ��ѡ�����
	int loc_dic_sel = m_CombBoxSel.GetCurSel();
	//��������
	CString loc_s_mask;
	m_EditType.GetWindowTextA(loc_s_mask);
	//�㷨ѡ��


	/////////////////////////////////////////////////
	//����������
	/////////////////////////////////////////////////
	if(loc_file_name.GetLength()<1)
	{
		AfxMessageBox("�ϴ��ļ�����Ϊ��");
		return;
	}
	if(loc_type==0)//����
	{
		if((m_startlength.GetLength()<1)||(m_endlength.GetLength()<1))
		{
			AfxMessageBox("������������ʼ����");
			return;
		}
		if((loc_len_start<1)||(loc_len_start>loc_len_end))
		{
			AfxMessageBox("������ʼ���ȴ���");
			return;
		}
	}
	else if(loc_type==1)//�ֵ�
	{
	}
	else if(loc_type==2)//�ض�����
	{
	}
	else if(loc_type==3)//����
	{
		if(loc_s_mask.GetLength()<1)
		{
			AfxMessageBox("������������Ϣ");
			return;
		}
		if(-1==loc_s_mask.Find('?') )
		{
			AfxMessageBox("������Ϣ����");
			return;
		}

		char *p_mask = loc_s_mask.GetBuffer();
		for(int i=0; i<loc_s_mask.GetLength(); i++)
		{
			if(p_mask[i]&0x80)
			{
				AfxMessageBox("���ܰ������ĵȿ��ַ�");
				return;
			}
		}
	}
	else if(loc_type==4)//�ʺ��
	{
	}


	
	/////////////////////////////////////////////////
	//�����ύ����
	/////////////////////////////////////////////////
	char buf[1024];
	memset(buf,0,1024);
	
	crack_task newtask={0};

	newtask.algo = m_comboalgo.GetCurSel()+1;
	newtask.charset = m_combocharset.GetCurSel();
	newtask.type = m_dectype.GetCurSel();

	newtask.special = 0;


	//�����������Ϣ
	if(loc_type==0)
	{
		p = (LPSTR)(LPCTSTR)m_endlength.GetBuffer();
		newtask.endLength = strtoul(p,NULL,10);

		p = (LPSTR)(LPCTSTR)m_startlength.GetBuffer(); 
		newtask.startLength = strtoul(p,NULL,10);
	}
	if(loc_type==1)//�ֵ�
		newtask.dict_idx = loc_dic_sel;
	else if(loc_type==3)//����
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
		//ֱ�Ӹ���hashֵ
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
	
	//�ϴ��ļ�
	ret = g_packmanager.GenNewFileUploadPack(uploadreq,&uploadres);
	if (ret < 0){	
		ErrorMsg(ret);
		return ;

	}
	
	//�ϴ��ļ���ʼ
	file_upload_start_res uploadstartres={0};
	ret = g_packmanager.GenNewFileUploadStartPack(&uploadstartres);
	if (ret < 0){	
		ErrorMsg(ret);
		return ;
	}

//�ϴ��ļ�
	ret = g_packmanager.GenNewFileUploadingPack();
	if (ret < 0){			
		ErrorMsg(ret);
		return ;
	}

//�ϴ��ļ�����

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

//��󳤶�
void CDlgUploadTask::OnNMCustomdrawSlider2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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

//��С����
void CDlgUploadTask::OnNMCustomdrawSliderLenMin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
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
		AfxMessageBox("�����Ƿ�");
	}
	else if(id==ERR_COMPRESS)
	{
		AfxMessageBox("ѹ������ʧ��");
	}
	else if(id==ERR_UNCOMPRESS)
	{
		AfxMessageBox("��ѹ������ʧ��");
	}
	else if(id==ERR_CONNECTIONLOST)
	{
		AfxMessageBox("��socket�Ͽ�����");
	}
	else {
		AfxMessageBox("δ֪����");
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

	if(id==0)//����
	{
		m_StaticLenMin.SetWindowTextA("��ʼ����");
		m_StaticLenMax.SetWindowTextA("��������");
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
	else if(id==1)//�ֵ�
	{
		m_StaticLenMin.SetWindowTextA("ѡ���ֵ��ļ�");
		m_StaticLenMin.ShowWindow(1);

		m_CombBoxSel.ShowWindow(1);
		m_CombBoxSel.InsertString(0,"�ֵ�0");
		m_CombBoxSel.InsertString(1,"�ֵ�1");
		m_CombBoxSel.SetCurSel(0);
	}
	else if(id==2)
	{
		m_StaticLenMin.SetWindowTextA("�ݲ�֧��");
		m_StaticLenMin.ShowWindow(1);
	}
	else if(id==3)//����
	{
		m_StaticLenMin.SetWindowTextA("��������");
		m_StaticLenMin.ShowWindow(1);
		m_EditType.ShowWindow(1);
	}
	else if(id==4)//�ʺ� 
	{
		m_StaticLenMin.SetWindowTextA("ѡ��ʺ��");
		m_StaticLenMin.ShowWindow(1);
		
		m_CombBoxSel.ShowWindow(1);
		m_CombBoxSel.InsertString(0,"�ʺ��0");
		m_CombBoxSel.InsertString(1,"�ʺ��1");
		m_CombBoxSel.SetCurSel(0);
	}
	else{
		m_StaticLenMin.SetWindowTextA("��Чѡ��");
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
