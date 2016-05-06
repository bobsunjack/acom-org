#pragma once


// CViewDialog 对话框

class CViewDialog : public CDialog
{
	DECLARE_DYNAMIC(CViewDialog)

public:
	CViewDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CViewDialog();

// 对话框数据
	enum { IDD = IDD_MAIN_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
