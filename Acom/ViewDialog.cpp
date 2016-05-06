// ViewDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "Acom.h"
#include "ViewDialog.h"
#include "afxdialogex.h"


// CViewDialog 对话框

IMPLEMENT_DYNAMIC(CViewDialog, CDialog)

CViewDialog::CViewDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CViewDialog::IDD, pParent)
{

}

CViewDialog::~CViewDialog()
{
}

void CViewDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CViewDialog, CDialog)
END_MESSAGE_MAP()


// CViewDialog 消息处理程序
