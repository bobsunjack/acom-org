#pragma once
#include "afxwin.h"

class CMyButton1 : public CButton
{
	//DECLARE_DYNAMIC(CMyButton)
public:
	CMyButton1();
	virtual ~CMyButton1();
	//����Button Down�ı�����ɫ
	void SetDownColor(COLORREF color);
	//����Button Up�ı�����ɫ
	void SetUpColor(COLORREF color);
	BOOL Attach(const UINT nID, CWnd* pParent);
protected:
	//�������صĺ���
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
public:
	//������ɫ�ֱ�Ϊ���֣�Button Down�ı�����ɫ��Button Up�ı�����ɫ
	COLORREF m_TextColor, m_DownColor, m_UpColor;

	BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult); 

	HWND m_hande1; 
	BOOL _bMouseTrack;
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
};