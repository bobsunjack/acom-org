#pragma once
#include "afxwin.h"

class CMyButton2 : public CButton
{
	//DECLARE_DYNAMIC(CMyButton)
public:
	CMyButton2();
	virtual ~CMyButton2();
	//设置Button Down的背景颜色
	void SetDownColor(COLORREF color);
	//设置Button Up的背景颜色
	void SetUpColor(COLORREF color);
	BOOL Attach(const UINT nID, CWnd* pParent);
protected:
	//必需重载的函数
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
public:
	//三种颜色分别为文字，Button Down的背景颜色，Button Up的背景颜色
	COLORREF m_TextColor, m_DownColor, m_UpColor;

	BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult); 

	HWND m_hande2; 
	BOOL _bMouseTrack;
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
};