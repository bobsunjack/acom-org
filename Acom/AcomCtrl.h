#pragma once

//#include "ViewDialog.h"
// AcomCtrl.h : CAcomCtrl ActiveX 控件类的声明。
#include <objsafe.h>
#include "CMyButton.h"
#include "CMyButton1.h"
#include "CMyButton2.h"
#include "CMyButton3.h"
// CAcomCtrl : 有关实现的信息，请参阅 AcomCtrl.cpp。
#define WM_DOWN_A (WM_USER+100)
#define WM_UP_A (WM_USER+101)
#define WM_DOWN_B (WM_USER+102)
#define WM_UP_B (WM_USER+103)
#define WM_DOWN_C (WM_USER+104)
#define WM_UP_C (WM_USER+105)
#define WM_DOWN_D (WM_USER+106)
#define WM_UP_D (WM_USER+107)

#define WM_BUTTON_A (WM_USER+108)
#define WM_BUTTON_B (WM_USER+109)
#define WM_BUTTON_C (WM_USER+110)
#define WM_BUTTON_D (WM_USER+111)
#define WM_BUTTON_E (WM_USER+112)
#define WM_BUTTON_F (WM_USER+113)
#define WM_BUTTON_G (WM_USER+114)
#define WM_BUTTON_H (WM_USER+115)

class CAcomCtrl : public COleControl
{
	DECLARE_DYNCREATE(CAcomCtrl)

// 构造函数
public:
	CAcomCtrl();
	//CViewDialog m_VideoDlg;
// 重写
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
    ~CAcomCtrl();
// 实现
protected:
	

	DECLARE_OLECREATE_EX(CAcomCtrl)    // 类工厂和 guid
	DECLARE_OLETYPELIB(CAcomCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CAcomCtrl)     // 属性页 ID
	DECLARE_OLECTLTYPE(CAcomCtrl)		// 类型名称和杂项状态

	//去掉安全警告 BEGIN
	DECLARE_INTERFACE_MAP()
	BEGIN_INTERFACE_PART(ObjectSafety, IObjectSafety)
		STDMETHOD(GetInterfaceSafetyOptions)(REFIID riid, DWORD __RPC_FAR *pdwSupportedOptions, DWORD __RPC_FAR *pdwEnabledOptions);
		STDMETHOD(SetInterfaceSafetyOptions)(REFIID riid, DWORD dwOptionSetMask, DWORD dwEnabledOptions);
	END_INTERFACE_PART(ObjectSafety)
	//去掉安全警告 END

// 消息映射
	DECLARE_MESSAGE_MAP()

// 调度映射
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// 事件映射
	DECLARE_EVENT_MAP()

// 调度和事件 ID
public:
	enum {
		dispidVoiceTalkingVolume = 47,
		dispidalarmPort = 46,
		dispidalarmHostIp = 45,
		dispidalarmUrl = 44,
		dispidmachineId = 43,
		dispidcanInfrared = 42,
		dispidcanPtz = 41,
		dispidVoiceTalking = 40,
		dispidFaceRecognition = 39L,
		dispidisPlaying = 38,
		dispidConnectionType = 37,
		dispidStreamMediaPort = 36,
		dispidStreamMediaIp = 35,
		dispidStreamMediaId = 34,
		dispidSetVideoEffect = 33L,
		dispidAudioClose = 32L,
		dispidAudioOpen = 31L,
		dispidCapturePicture = 30L,
		dispidVolume = 29,
		dispidSaveFileName = 28,
		dispidSaveFilePath = 27,
		dispidHue = 26,
		dispidSaturation = 25,
		dispidContrast = 24,
		dispidBrightness = 23,
		eventidOnStartPlayError = 5L,
		eventidOnGetVideoEffect = 4L,
		eventidonStartPlay = 3L,
		eventidonclick = 2L,
		eventidondblclick = 1L,
		dispidSendPTZControlCommand = 22L,
		dispidPTZSpeed = 21,
		dispidPTZPresetIndex = 20,
		dispidPTZControlEnabled = 19,
		dispidPTZCommandID = 18,
		dispidGetLastError = 17L,
		dispidStopPlay = 16L,
		dispidStartPlay = 15L,
		dispidDVRID = 14,
		dispidPassword = 13,
		dispidLoginUser = 12,
		dispidDisplayBuffer = 11,
		dispidFluency = 10,
		dispidLinkMode = 9,
		dispidhostWin = 8,
		dispidwriteLog = 7,
		dispidChannelNo = 6,
		dispidConfigPort = 5,
		dispidHostPort = 4,
		dispidHostIP = 3,
		dispidToken = 2,
		dispidDeviceType = 1
	};
//	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
//	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:
	void InitDll();
	void ExitDll();
	CMyButton* NewMyRadio(int nID,CRect rect,int nStyle);
	CMyButton1* NewMyRadio1(int nID,CRect rect,int nStyle);
	CMyButton2* NewMyRadio2(int nID,CRect rect,int nStyle);
	CMyButton3* NewMyRadio3(int nID,CRect rect,int nStyle);

	void OnDeviceTypeChanged(void);
	CString m_DeviceType;
	void OnTokenChanged(void);
	CString m_Token;
	void OnHostIPChanged(void);
	CString m_HostIP;
	void OnHostPortChanged(void);
	LONG m_HostPort;
	void OnConfigPortChanged(void);
	LONG m_ConfigPort;
	void OnChannelNoChanged(void);
	LONG m_ChannelNo;
	void OnwriteLogChanged(void);
	LONG m_writeLog;
	void OnhostWinChanged(void);
	VARIANT m_hostWin;
	void OnLinkModeChanged(void);
	LONG m_LinkMode;
	void OnFluencyChanged(void);
	LONG m_Fluency;
	void OnDisplayBufferChanged(void);
	LONG m_DisplayBuffer;
	void OnLoginUserChanged(void);
	CString m_LoginUser;
	void OnPasswordChanged(void);
	CString m_Password;
	void OnDVRIDChanged(void);
	LONG m_DVRID;

	LONG GetLastError(void);
	void OnPTZCommandIDChanged(void);
	LONG m_PTZCommandID;
	void OnPTZControlEnabledChanged(void);
	LONG m_PTZControlEnabled;
	void OnPTZPresetIndexChanged(void);
	LONG m_PTZPresetIndex;
	void OnPTZSpeedChanged(void);
	LONG m_PTZSpeed;
	VARIANT_BOOL SendPTZControlCommand(void);

	void Fireondblclick(void)
	{
		FireEvent(eventidondblclick, EVENT_PARAM(VTS_NONE));
	}

	void Fireonclick(void)
	{
		FireEvent(eventidonclick, EVENT_PARAM(VTS_NONE));
	}

	void FireonStartPlay(void)
	{
		FireEvent(eventidonStartPlay, EVENT_PARAM(VTS_NONE));
	}

	void FireOnGetVideoEffect(void)
	{
		FireEvent(eventidOnGetVideoEffect, EVENT_PARAM(VTS_NONE));
	}

	void FireOnStartPlayError(void)
	{
		FireEvent(eventidOnStartPlayError, EVENT_PARAM(VTS_NONE));
	}
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	
protected:
	void OnBrightnessChanged(void);
	LONG m_Brightness;
	void OnContrastChanged(void);
	LONG m_Contrast;
	void OnSaturationChanged(void);
	LONG m_Saturation;
	void OnHueChanged(void);
	LONG m_Hue;
public:
	void StartPlay(void);
	void StopPlay(void);
	LONG sessionId;
	LONG sessionId1;
	CMyButton *m_button;
	CMyButton1 *m_button1;
	CMyButton2 *m_button2;
	CMyButton3 *m_button3;
protected:
	void OnSaveFilePathChanged(void);
	CString m_SaveFilePath;
	void OnSaveFileNameChanged(void);
	CString m_SaveFileName;
	void OnVolumeChanged(void);
	LONG m_Volume;
	BSTR CapturePicture(void);
	VARIANT_BOOL AudioOpen(void);
	VARIANT_BOOL AudioClose(void);
	VARIANT_BOOL SetVideoEffect(void);
	void OnCopySrc();
	void OnDraw(CDC* pDC);
	CBitmap ScrBmp;
public:
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	BOOL _bMouseTrack;
	BOOL _bMouseTrack_a;
	afx_msg void OnMouseLeave();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	bool flag1;
	bool flag2;
protected:
	void OnStreamMediaIdChanged(void);
	CString m_StreamMediaId;
	void OnStreamMediaIpChanged(void);
	CString m_StreamMediaIp;
	void OnStreamMediaPortChanged(void);
	LONG m_StreamMediaPort;
	void OnConnectionTypeChanged(void);
	LONG m_ConnectionType;
	void OnisPlayingChanged(void);
	VARIANT_BOOL m_isPlaying;
	void FaceRecognition(void);
	void OnVoiceTalkingChanged(void);
	VARIANT_BOOL m_VoiceTalking;
public:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	afx_msg LRESULT OnDownA(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnUpA(WPARAM wParam,LPARAM lParam);

	afx_msg LRESULT OnDownB(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnUpB(WPARAM wParam,LPARAM lParam);

	afx_msg LRESULT OnDownC(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnUpC(WPARAM wParam,LPARAM lParam);

	afx_msg LRESULT OnDownD(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnUpD(WPARAM wParam,LPARAM lParam);

	afx_msg LRESULT OnButtonA(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnButtonB(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnButtonC(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnButtonD(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnButtonE(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnButtonF(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnButtonG(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnButtonH(WPARAM wParam,LPARAM lParam);
protected:
	void OncanPtzChanged(void);
	LONG m_canPtz;
	void OncanInfraredChanged(void);
	LONG m_canInfrared;
	void OnmachineIdChanged(void);
	LONG m_machineId;
	void OnalarmUrlChanged(void);
	CString m_alarmUrl;
	void OnalarmHostIpChanged(void);
	CString m_alarmHostIp;
	void OnalarmPortChanged(void);
	LONG m_alarmPort;
	void OnVoiceTalkingVolumeChanged(void);
	LONG m_VoiceTalkingVolume;
};
