// SelectFileDialog.cpp : implementation file
//

#include "stdafx.h"
#include "CtlClient.h"
#include "SelectFileDialog.h"


// CSelectFileDialog

IMPLEMENT_DYNAMIC(CSelectFileDialog, CFileDialog)

CSelectFileDialog::CSelectFileDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{

}

CSelectFileDialog::~CSelectFileDialog()
{
}


BEGIN_MESSAGE_MAP(CSelectFileDialog, CFileDialog)
END_MESSAGE_MAP()



// CSelectFileDialog message handlers

void CSelectFileDialog::OnInitDone(){


	SetControlText(IDOK,_T("Ñ¡Ôñ"));

	
}