#pragma once
#include "afxwin.h"
#include "SelectFileDialog.h"
#include "DlgTaskStatus.h"
#include "afxcmn.h"


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
	
	CEdit m_EditLenMin;
	CEdit m_EditLenMax;
	afx_msg void OnNMCustomdrawSlider2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderLenMin(NMHDR *pNMHDR, LRESULT *pResult);
	CSliderCtrl m_SlideLenMin;
	CSliderCtrl m_SlideLenMax;
	CEdit m_EditType;
	CStatic m_StaticLenMin;
	CStatic m_StaticLenMax;
	CComboBox m_CombBoxSel;

	void ErrorMsg(int id);
	//攻击相关控件处理
	void ProcessControl(int id);
	afx_msg void OnCbnSelchangeComboType();
};
