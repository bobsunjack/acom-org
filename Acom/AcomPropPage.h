#pragma once

// AcomPropPage.h : CAcomPropPage ����ҳ���������


// CAcomPropPage : �й�ʵ�ֵ���Ϣ������� AcomPropPage.cpp��

class CAcomPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CAcomPropPage)
	DECLARE_OLECREATE_EX(CAcomPropPage)

// ���캯��
public:
	CAcomPropPage();

// �Ի�������
	enum { IDD = IDD_PROPPAGE_ACOM };

// ʵ��
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ��Ϣӳ��
protected:
	DECLARE_MESSAGE_MAP()
};

