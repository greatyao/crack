// DlgUploadTask.cpp : implementation file
//

#include "stdafx.h"
#include "CtlClient.h"
#include "DlgUploadTask.h"

#include "algorithm_types.h"


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


	ComboInit();

	return TRUE;


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
	// TODO: Add your control notification handler code here
	OnOK();

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

	newtask.algo = m_comboalgo.GetCurSel();
	newtask.charset = m_combocharset.GetCurSel();
	newtask.type = m_dectype.GetCurSel();

	newtask.special = m_btndec.GetCheck();

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


	AfxMessageBox(tmpStr);

}
