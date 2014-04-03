#pragma once
#include "afxwin.h"
#include "SelectFileDialog.h"
#include "DlgTaskStatus.h"


// CDlgUploadTask dialog

class CDlgUploadTask : public CDialog
{
	DECLARE_DYNAMIC(CDlgUploadTask)

public:
	CDlgUploadTask(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgUploadTask();
	VOID ComboInit();

// Dialog Data
	enum { IDD = IDD_DLG_UPLOAD };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CDlgTaskStatus *m_pDlgTaskStatus;
	void SetCDlgTaskStatus(CDlgTaskStatus *);
	BOOL AddToTaskList(int nAlgo,int nCharset,int nType,int nIsFile,int nLenMin,int nLenMax,char *psFile);

	BOOL PreTranslateMessage(MSG* pMsg);
    CToolTipCtrl m_toolTip;
	CComboBox m_comboalgo;
	CComboBox m_combocharset;
	CComboBox m_dectype;


	CString m_startlength;
	CString m_endlength;
	CString m_filename;

	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedOk();
	
	CButton m_btndec;
};
