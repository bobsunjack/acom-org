#pragma once

// AcomPropPage.h : CAcomPropPage 属性页类的声明。


// CAcomPropPage : 有关实现的信息，请参阅 AcomPropPage.cpp。

class CAcomPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CAcomPropPage)
	DECLARE_OLECREATE_EX(CAcomPropPage)

// 构造函数
public:
	CAcomPropPage();

// 对话框数据
	enum { IDD = IDD_PROPPAGE_ACOM };

// 实现
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 消息映射
protected:
	DECLARE_MESSAGE_MAP()
};

