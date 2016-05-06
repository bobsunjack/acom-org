// AcomPropPage.cpp : CAcomPropPage 属性页类的实现。

#include "stdafx.h"
#include "Acom.h"
#include "AcomPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CAcomPropPage, COlePropertyPage)



// 消息映射

BEGIN_MESSAGE_MAP(CAcomPropPage, COlePropertyPage)
END_MESSAGE_MAP()



// 初始化类工厂和 guid

IMPLEMENT_OLECREATE_EX(CAcomPropPage, "ACOM.AcomPropPage.1",
	0x9ad654f7, 0x1119, 0x48dc, 0xb1, 0x37, 0x87, 0x68, 0xe6, 0x5d, 0x2c, 0x7a)



// CAcomPropPage::CAcomPropPageFactory::UpdateRegistry -
// 添加或移除 CAcomPropPage 的系统注册表项

BOOL CAcomPropPage::CAcomPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_ACOM_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}



// CAcomPropPage::CAcomPropPage - 构造函数

CAcomPropPage::CAcomPropPage() :
	COlePropertyPage(IDD, IDS_ACOM_PPG_CAPTION)
{
}



// CAcomPropPage::DoDataExchange - 在页和属性间移动数据

void CAcomPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
}



// CAcomPropPage 消息处理程序
