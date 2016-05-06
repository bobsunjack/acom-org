#include "StdAfx.h"
#include "CMyButton3.h"
#include <afxwin.h>
#include "resource.h"
#include "JPublicConfig.h"
#include "AcomCtrl.h"
CMyButton3::CMyButton3(void)
{
	m_DownColor = m_UpColor = RGB(0,0,0);
}
CMyButton3::~CMyButton3(void)
{
}

//CMyButton是CButton派生类，具有CButton的全部成员函数，
//但在创建时需要使用BS_OWNERDRAW风格。
//如果按钮不是动态生成，使用Attach函数使CMyButton代替原来按钮的窗口过程。
BOOL CMyButton3::Attach(const UINT nID, CWnd* pParent)
{
	//GetDlgItem(nID)->ModifyStyle(0,BS_OWNERDRAW,0);
	if (!SubclassDlgItem(nID, pParent))
		return FALSE;
	return TRUE;
}
void CMyButton3::SetDownColor(COLORREF color)
{
	m_DownColor = color;
}
void CMyButton3::SetUpColor(COLORREF color)
{
	m_UpColor = color;
}

void CMyButton3::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: Add your code to draw the specified item
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);//得到绘制的设备环境CDC
	VERIFY(lpDrawItemStruct->CtlType==ODT_BUTTON);

	// 得当Button上文字,这里的步骤是:1,先得到在资源里编辑的按钮的文字,
	//然后将此文字重新绘制到按钮上,
	//同时将此文字的背景色设为透明,这样,按钮上仅会显示文字
	const int bufSize = 512;
	TCHAR buffer[bufSize];
	GetWindowText(buffer, bufSize);
	int size=strlen(buffer);   //得到长度
	DrawText(lpDrawItemStruct->hDC,buffer,size,&lpDrawItemStruct->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_TABSTOP);   //绘制文字
	SetBkMode(lpDrawItemStruct->hDC,TRANSPARENT);   //透明

	if (lpDrawItemStruct->itemState &ODS_SELECTED)  //当按下按钮时的处理
	{//
		//重绘整个控制
		CBrush brush(m_DownColor);  
		dc.FillRect(&(lpDrawItemStruct->rcItem),&brush);//
		//因为这里进行了重绘,所以文字也要重绘
		DrawText(lpDrawItemStruct->hDC,buffer,size,&lpDrawItemStruct->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_TABSTOP);   
		SetBkMode(lpDrawItemStruct->hDC,TRANSPARENT);
	}
	else                     //当按钮不操作或者弹起时
	{
		CBrush brush(m_UpColor);     
		dc.FillRect(&(lpDrawItemStruct->rcItem),&brush);//
		//同上,进行重绘文字
		DrawText(lpDrawItemStruct->hDC,buffer,size,&lpDrawItemStruct->rcItem,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_TABSTOP);    
		SetBkMode(lpDrawItemStruct->hDC,TRANSPARENT);
	}
	if ((lpDrawItemStruct->itemState &ODS_SELECTED)&&(lpDrawItemStruct->itemAction &(ODA_SELECT| ODA_DRAWENTIRE)))
	{   //选中了本控件,高亮边框
		COLORREF fc=RGB(255-GetRValue(m_UpColor),255-GetGValue(m_UpColor), 255-             GetBValue(m_UpColor));//
		CBrush brush(fc);//
		dc.FrameRect(&(lpDrawItemStruct->rcItem),&brush);//
	}
	if (!(lpDrawItemStruct->itemState & ODS_SELECTED) &&(lpDrawItemStruct->itemAction & ODA_SELECT))
	{
		//控制的选中状态结束,去掉边框
		CBrush brush(m_UpColor);
		dc.FrameRect(&lpDrawItemStruct->rcItem,&brush);//
	}
	dc.Detach();//
}

BOOL CMyButton3::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// 捕获鼠标左键按下和松开状态，上报父窗口
	switch(message)
	{
	case WM_LBUTTONDOWN:
		OUTPUT_LOG("downD");
		::PostMessage(m_hande3,WM_DOWN_D,0,0);
		//m_hWnd
		break;
	case WM_LBUTTONUP:
		OUTPUT_LOG("upD");
		::PostMessage(m_hande3,WM_UP_D,0,0);
		break;
	default:
		break;
	}
	return CButton::OnWndMsg(message, wParam, lParam, pResult);
}

BEGIN_MESSAGE_MAP(CMyButton3, CButton)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()


void CMyButton3::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (!_bMouseTrack)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = this->GetSafeHwnd();
		tme.dwFlags = TME_LEAVE;
		tme.dwHoverTime = 1;
		_bMouseTrack = _TrackMouseEvent(&tme);

		::PostMessage(m_hande3,WM_BUTTON_G,0,0);
	}

	CButton::OnMouseMove(nFlags, point);
}


void CMyButton3::OnMouseLeave()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	_bMouseTrack = FALSE;
	::PostMessage(m_hande3,WM_BUTTON_H,0,0);
	CButton::OnMouseLeave();
}
