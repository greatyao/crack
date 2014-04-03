// DlgUploadTask.cpp : implementation file
//

#include "stdafx.h"
#include "CtlClient.h"
#include "DlgUploadTask.h"

#include "algorithm_types.h"

#include "PackManager.h"
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
	DDX_Control(pDX, IDC_COMBO1, m_comboalgo);
	DDX_Control(pDX, IDC_COMBO2, m_combocharset);
	DDX_Control(pDX, IDC_COMBO3, m_dectype);
	DDX_Text(pDX, IDC_EDIT1, m_startlength);
	DDX_Text(pDX, IDC_EDIT2, m_endlength);
	DDX_Text(pDX, IDC_EDIT3, m_filename);
	DDX_Control(pDX, IDC_RADIO1, m_btndec);
}


BEGIN_MESSAGE_MAP(CDlgUploadTask, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgUploadTask::OnBnClickedButton1)
	ON_BN_CLICKED(IDOK, &CDlgUploadTask::OnBnClickedOk)
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
	m_dectype.InsertString(2,_T("�ʺ��"));

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

	m_btndec.SetCheck(TRUE);
	return ;

}

BOOL CDlgUploadTask::OnInitDialog(){

	CDialog::OnInitDialog();
    m_toolTip.Create(this,TTS_ALWAYSTIP | TTS_BALLOON);
    m_toolTip.SetMaxTipWidth(200);
    m_toolTip.AddTool(GetDlgItem(IDC_COMBO_ALG), "ѡ���ύ�ļ����㷨����");
    m_toolTip.AddTool(GetDlgItem(IDC_COMBO_CHARSET), "ѡ��Ŀ���ƽ���ַ�������");
    m_toolTip.AddTool(GetDlgItem(IDC_COMBO_TYPE), "ѡ��ʹ�����ַ����ƽ�");
    m_toolTip.AddTool(GetDlgItem(IDC_RADIO_IS_FILE), "����ϴ����ļ��Ǽ����ļ����ͣ�\r\n����rar�ļ���word�ļ������ѡ��");
    m_toolTip.AddTool(GetDlgItem(IDC_RADIO_NO_FILE), "����ϴ����ļ��Ǳ���md5��hash��Ϣ���ļ�����ѡ��");
    m_toolTip.AddTool(GetDlgItem(IDC_EDIT_LEN_MIN), "�����ƽ��������С����");
    m_toolTip.AddTool(GetDlgItem(IDC_EDIT_LEN_MAX), "�����ƽ��������󳤶�");
    m_toolTip.AddTool(GetDlgItem(IDC_EDIT_FILE_PATH), "�����ļ�·�������ߵ�������ѡ���ļ���ťѡ���ļ�");
    m_toolTip.AddTool(GetDlgItem(IDC_BUTTON_SEL_FILE), "ѡ���ƽ��Ŀ���ļ�(hash�ļ����߼����ļ�)");
    m_toolTip.AddTool(GetDlgItem(IDOK), "�ϴ�����");

	m_pDlgTaskStatus = NULL;

	ComboInit();

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
            pMsg->hwnd == GetDlgItem(IDC_RADIO_IS_FILE)->m_hWnd  || 
            pMsg->hwnd == GetDlgItem(IDC_RADIO_NO_FILE)->m_hWnd  || 
            pMsg->hwnd == GetDlgItem(IDC_EDIT_LEN_MIN)->m_hWnd  || 
            pMsg->hwnd == GetDlgItem(IDC_EDIT_LEN_MAX)->m_hWnd  || 
            pMsg->hwnd == GetDlgItem(IDC_EDIT_FILE_PATH)->m_hWnd  || 
            pMsg->hwnd == GetDlgItem(IDC_BUTTON_SEL_FILE)->m_hWnd|| 
            pMsg->hwnd == GetDlgItem(IDOK)->m_hWnd )
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

void CDlgUploadTask::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here

	// ��ȡ��ǰ����·��
	CString strAppName;//��ǰ����Ŀ¼
	::GetModuleFileName(NULL, strAppName.GetBuffer(_MAX_PATH), _MAX_PATH);
	strAppName.ReleaseBuffer();
	int nPos = strAppName.ReverseFind('\\');
	strAppName = strAppName.Left(nPos + 1);

	// AfxMessageBox(strAppName);


	// �ļ���չ��������
	//��һ���������FALSE�����Ǳ����ļ�����ʼĿ¼�ǵ�ǰ����Ŀ¼, ��ʼѡ����ļ�����file����ʼ��׺�������� Chart Files (*.xlc)
	CSelectFileDialog dlg(TRUE,NULL ,strAppName,OFN_ENABLESIZING ,NULL,NULL); // ����TRUE, ������ FALSE

	//CFileDialog::SetControlText(IDOK,_T("ѡ��"));

	if(dlg.DoModal() == IDOK)
	{
		CString strFile = dlg.GetPathName(); // ȫ·��
		m_filename = dlg.GetPathName();
		
		UpdateData(FALSE);

	}

}

void CDlgUploadTask::OnBnClickedOk()
{
	
	//��ȡ��������
	//�����㷨
	int loc_algo   = m_comboalgo.GetCurSel()+1;
	//�����ַ���
	int loc_charset= m_combocharset.GetCurSel();
	//��������
	int loc_type   = m_dectype.GetCurSel();
	//�Ƿ��ļ�����
	//��ʼ����
	char *p = (LPSTR)(LPCTSTR)m_startlength.GetBuffer(); 
	int loc_len_start = strtoul(p,NULL,10);
	//��������
	p = (LPSTR)(LPCTSTR)m_endlength.GetBuffer();
	int loc_len_end = strtoul(p,NULL,10);
	//�ļ�·��

	char buf[1024];
	memset(buf,0,1024);
/*
	struct crack_task
{
	unsigned char algo;		//�����㷨
	unsigned char charset;	//�����ַ���
	unsigned char type;		//��������
	unsigned char special;	//�Ƿ����ļ����ܣ�pdf+office+rar+zip��
	unsigned char startLength;//��ʼ����
	unsigned char endLength;	//�ս᳤��
	unsigned char filename[256];	//�û����������ļ���
	char guid[40];			//�û��˵������GUID
	int count;				//��Ҫ���ܵ�Hash������������ļ�=1��
	struct crack_hash hashes[0];			//������Ҫ��̬����
	
};
*/
	
	crack_task newtask;



	newtask.algo = m_comboalgo.GetCurSel()+1;
	newtask.charset = m_combocharset.GetCurSel();
	newtask.type = m_dectype.GetCurSel();

	newtask.special = 0;

	p = (LPSTR)(LPCTSTR)m_endlength.GetBuffer();
	newtask.endLength = strtoul(p,NULL,10);

	p = (LPSTR)(LPCTSTR)m_startlength.GetBuffer(); 
	newtask.startLength = strtoul(p,NULL,10);

	sprintf((char *)newtask.filename,"%s",m_filename);

	

	struct task_upload_res res;

	int ret =0;
	ret = g_packmanager.DoTaskUploadPack(newtask,&res);
	if (ret < 0){

		CString tmpStr("Upload Task Error");

		AfxMessageBox(tmpStr);
		return ;

	}

	file_upload_req uploadreq;
	memset(&uploadreq,0,sizeof(file_upload_req));
	memcpy(uploadreq.guid,res.guid,sizeof(res.guid));
	memset(g_packmanager.m_cur_upload_guid,0,sizeof(g_packmanager.m_cur_upload_guid));
	memcpy(g_packmanager.m_cur_upload_guid,res.guid,40);

	file_upload_res uploadres;
	memset(&uploadres,0,sizeof(file_upload_res));
	
	ret = g_packmanager.GenFileUploadPack(uploadreq,&uploadres);
	if (ret < 0){

		CString tmpStr("Upload File Error");

		AfxMessageBox(tmpStr);
		return ;

	}
	
	file_upload_end_res endres;
	memset(&endres,0,sizeof(file_upload_end_res));

	memset(g_packmanager.m_cur_local_file,0,sizeof(g_packmanager.m_cur_local_file));
	memcpy(g_packmanager.m_cur_local_file,this->m_filename,strlen(m_filename));
	
	
	ret = g_packmanager.GenFileUploadStart(&endres);
	if (ret < 0){

		CString tmpStr("Upload File End Error");

		AfxMessageBox(tmpStr);
		return ;

	}

	CString tmpStr1("Upload File End OK");

	AfxMessageBox(tmpStr1);

	task_start_req startreq;
	task_status_res startres;

	memset(&startreq,0,sizeof(task_start_req));
	memcpy(startreq.guid,g_packmanager.m_cur_upload_guid,40);

	TRACE("guid : %s",g_packmanager.m_cur_upload_guid);
	ret = g_packmanager.GenTaskStartPack(startreq,&startres);
	if (ret < 0){

		TRACE("GenTaskStartPack Error : %d",ret);
		return ;

	}




/*
	TRACE(m_filename);
	sprintf((char *)newtask.filename,"%s",m_filename);



	char *p = (LPSTR)(LPCTSTR)m_startlength.GetBuffer();
	TRACE(m_startlength);

	newtask.startLength =strtoul(p,NULL,10);
	
	char *p1 = (LPSTR)(LPCTSTR)m_endlength.GetBuffer();
	newtask.endLength = strtoul(p1,NULL,10);
	TRACE(m_endlength);

	sprintf(buf,"%d %d %d %d %s %d %d",newtask.algo,newtask.charset,newtask.type,
		newtask.special,newtask.filename,newtask.startLength,newtask.endLength);
	CString tmpStr(buf);
	UpdateData(TRUE);

	AfxMessageBox(tmpStr);
*/


	//����
	//m_pDlgTaskStatus->AddToTaskList(0,0,0,0,2,4,(char*)newtask.filename,"");
	
}