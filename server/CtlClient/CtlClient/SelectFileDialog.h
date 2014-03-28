#pragma once


// CSelectFileDialog

class CSelectFileDialog : public CFileDialog
{
	DECLARE_DYNAMIC(CSelectFileDialog)

public:
	CSelectFileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);
	virtual ~CSelectFileDialog();

protected:
	virtual void OnInitDone();
	DECLARE_MESSAGE_MAP()
};


