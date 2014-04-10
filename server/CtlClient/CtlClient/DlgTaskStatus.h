#pragma once
#include "afxcmn.h"
#include <string>
#include <vector>

//pthread
#include "pthread.h"
#include "sched.h"
#include "semaphore.h"

using namespace std;

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
public:	
	vector<string> m_TaskList;//保存任务
	BOOL RefreshList(void);


    CToolTipCtrl m_toolTip;//提示信息
	CListCtrl m_ListStatus;
	afx_msg void OnNMDblclkListTask(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnStart();
	virtual BOOL OnInitDialog();
	BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedBtnRefresh();
	void GetStatusStrByCmd(char cmd,char *pdes);
	afx_msg void OnBnClickedBtnPause();
	afx_msg void OnBnClickedBtnDelete();
	afx_msg void OnBnClickedBtnStop();

	//自动刷新任务列表	
	HANDLE m_hStopRefresh;	//sleep

	pthread_t m_ThreadRefresh;	//线程句柄
	int m_bThreadRefreshStop;	//停止线程标记
	int m_bThreadRefreshRunning;//线程运行标记

	static void *ThreadRefresh(void *);
	void StartRefresh(void);		
	void StopRefresh(void);
};
