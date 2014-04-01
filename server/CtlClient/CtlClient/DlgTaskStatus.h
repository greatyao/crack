#pragma once


// CDlgTaskStatus dialog

class CDlgTaskStatus : public CDialog
{
	DECLARE_DYNAMIC(CDlgTaskStatus)

public:
	CDlgTaskStatus(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTaskStatus();

	void GenExampleListData();

// Dialog Data
	enum { IDD = IDD_DLG_TASK_STATUS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
