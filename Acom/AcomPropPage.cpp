// AcomPropPage.cpp : CAcomPropPage ����ҳ���ʵ�֡�

#include "stdafx.h"
#include "Acom.h"
#include "AcomPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CAcomPropPage, COlePropertyPage)



// ��Ϣӳ��

BEGIN_MESSAGE_MAP(CAcomPropPage, COlePropertyPage)
END_MESSAGE_MAP()



// ��ʼ���๤���� guid

IMPLEMENT_OLECREATE_EX(CAcomPropPage, "ACOM.AcomPropPage.1",
	0x9ad654f7, 0x1119, 0x48dc, 0xb1, 0x37, 0x87, 0x68, 0xe6, 0x5d, 0x2c, 0x7a)



// CAcomPropPage::CAcomPropPageFactory::UpdateRegistry -
// ��ӻ��Ƴ� CAcomPropPage ��ϵͳע�����

BOOL CAcomPropPage::CAcomPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_ACOM_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}



// CAcomPropPage::CAcomPropPage - ���캯��

CAcomPropPage::CAcomPropPage() :
	COlePropertyPage(IDD, IDS_ACOM_PPG_CAPTION)
{
}



// CAcomPropPage::DoDataExchange - ��ҳ�����Լ��ƶ�����

void CAcomPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
}



// CAcomPropPage ��Ϣ�������
