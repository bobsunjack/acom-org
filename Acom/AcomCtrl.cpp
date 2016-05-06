// AcomCtrl.cpp : CAcomCtrl ActiveX 控件类的实现。

#include "stdafx.h"
#include <Windows.h>
#include <WinSock.h>
#include "Acom.h"
#include "AcomCtrl.h"
#include "AcomPropPage.h"
#include "afxdialogex.h"
#include "JPublicConfig.h"
#include "Resource.h"
#include <Mmsystem.h>
#include <windows.h> 
#pragma comment(lib, "Winmm.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static HINSTANCE DLLInst = NULL; //动态库句

#pragma pack(push)
#pragma pack(1)//1字节对齐

typedef struct  
{
	int sessionid;	
	CString devicetype;
	int dvrid;
	int channelno;
	int linkmode;
	int fluency;
	int displaybuffer;
	CString hostip;
	int hostport;
	CString loginuser;
	CString password;
	int winder;
	char *lp;
} TPFileList;

typedef struct  
{
	int sessionid1;	
	CString devicetype1;
	char *lp1;
} TPFileList1;

typedef struct  
{	
	CString devicetype2;
	int dvrid2;
	int channeelno2;
	int ptzpresetindex2;
	int ptzspeed2;
	int sessionid2;
	CString hostip2;
	int hostport2;
	CString loginuser2;
	CString password2;
} TPFileList2;

typedef struct  
{
	int bri;	//预览通道的亮度。（0-127）
	int con;	//预览通道的对比度。（0-127）
	int sat;	//预览通道的饱和度。（0-127）
	int hue;	//预览通道的色调。（0-127）
} SvVideoPara , *PSvVideoPara ;

typedef struct
{
	char filename[512];
	char startTime[23];
	char endTime[23];
	int	 fileLength;
	char fileType[5];
}SvFileList , *PSvFileList;

typedef struct
{
	long long dvrEquipId;
	short channelNo;
	char filename[225];
	char fileType[20];
	char startTime[23];
	char endTime[23];
	long long playLength;     //文件长度（毫秒）
	char lowerHalfUrl[2000];  //url后半部分
}SvCloudFileList , *PSvCloudFileList;

typedef struct  
{
	int HardDecodeCount;		//解码通道
	int DisplayChannelCount;	//输出通道
	int DisplayPara;			//输出通道的画面个数
}SvMatrix , *PsvMatrix ;

//云台控制-3D放大
typedef struct
{
	int param_type;			// 1 - 坐标方式 2 - 角度方式 
	int	iStatus_1;			// 0 - 恢复 1 - 放大
	int iStart_x_1;			// x起始坐标
	int iEnd_x_1;			// x结束坐标
	int iStart_y_1;			// y起始坐标
	int iEnd_y_1;			// y结束坐标
	float H_coordinate_2;	// 水平坐标 0.0度~359.99度
	float V_coordinate_2;	// 垂直坐标 -180.00度~+180度
	float multiplecoord_2;	// 变倍坐标 0.0倍~655.35倍
	int Orientation_2;		// 方位 1-北，2-东北，3-东，4-东南，5-南，6-西南，7-西，8-西北
}SvPTZControl3D , *PSvPTZControl3D;

//解码之后的数据
typedef struct
{
	long nWidth; //画面宽，单位像素，如果是音频数据则为0
	long nHeight;//画面高，如果是音频数据则为0
	long nStamp;//时标信息，单位毫秒
	long nType; //数据类型,T_AUDIO16,T_RGB32,T_YV12
	long nFrameRate;//编码是产生的图像的帧率
	long dwFrameNum;//帧号
}SvFrameInfo,*PSvFrameInfo;

#pragma pack(pop)

//画图回调函数 
typedef void (__stdcall *fDrawFun)(int sessionID,HDC hDc, long dwUser);
//画图回调函数 
typedef void (__stdcall *fIFredFun)(char* buff, int width, int height);

//初始化
VARIANT_BOOL (__stdcall *sv_init)();//定义指针函数、接口。	
//反初始化
VARIANT_BOOL (__stdcall *sv_uninit)();	
//关闭一个对应的子进程
VARIANT_BOOL (__stdcall *sv_close_process)(CHAR* dvr_type); 
//直连预览的开始函数，dvr_type为设备类型、handle为播放窗口句柄。linkmode为码流类型,fluency为画面质量,是否丢帧的判断,暂时不考虑丢帧填写为0就可,displaybuffer为播放缓冲区帧队列个数,默认的话就填写为0
LONG (__stdcall *sv_start_preview_dvr)(CHAR* dvr_type,LONG equipId , LONG channelno , LONG linkmode ,LONG fluency ,LONG displaybuffer,CHAR* ip , LONG port ,CHAR* username , CHAR* password , LONG handle);
//所有类型预览的关闭函数，dvr_type为设备类型
VARIANT_BOOL (__stdcall *sv_stop_preview)(CHAR* dvr_type,LONG sessionId);
//每个sessionId的心跳
VARIANT_BOOL (__stdcall *sv_alive_request)(CHAR* dvr_type , LONG sessionId);
//直连的云台控制 Speed 云台控制速度，Param为预置点值，Start为0时开始云台控制转动，否则停止
VARIANT_BOOL (__stdcall *sv_ptz_control_dvr)(CHAR* dvr_type, LONG equipId , LONG channelno , LONG PtzCmd , LONG Start , LONG Param , LONG Speed , LONG sessionId , CHAR* ip ,LONG port , CHAR* username , CHAR* password);
//获得视频参数函数,videoPara为获得参数结构体指针。
VARIANT_BOOL (__stdcall *sv_get_videopara)(CHAR* dvr_type,LONG sessionId ,SvVideoPara* videoPara);
//抓图命令 ,预览和回放都可以，returnPath 为返回路径
VARIANT_BOOL (__stdcall *sv_capture_picture)(CHAR* dvr_type,LONG sessionId,CHAR* path , CHAR * returnPath);
//通道声音控制，预览和回放都可以,SoundEnabled为声音控制命令：1为开启声音、0为关闭声音,SoundVolume 声音音量(1-10)默认为5
VARIANT_BOOL (__stdcall *sv_sound_control)(CHAR* dvr_type,LONG sessionId, LONG SoundEnabled ,LONG SoundVolume);
//设置视频参数函数
VARIANT_BOOL (__stdcall *sv_set_videopara)(CHAR* dvr_type,LONG sessionId ,LONG bri ,LONG con ,LONG sat , LONG  hue);
//获取错误代码
LONG (__stdcall *sv_get_last_error)(CHAR* dvr_type, CHAR* errorCode);
//过流媒体的预览，dvr_type为设备类型、handle为播放窗口句柄。linkmode为码流类型,fluency为画面质量,是否丢帧的判断,暂时不考虑丢帧填写为0就可,displaybuffer为播放缓冲区帧队列个数,默认的话就填写为0,mediaid为流媒体的设备ID可以为0，clientnetid为设置的网络ID，没有的话可以为0
LONG (__stdcall *sv_start_preview_media)(CHAR* dvr_type,LONG equipId,LONG channelno,LONG linkmode,LONG fluency,LONG displaybuffer,LONG mediaid,CHAR* mediaip,LONG mediaport,LONG clientnetid,CHAR* token,LONG handle);
//设置画图回调
VARIANT_BOOL (__stdcall *sv_set_draw_callback)(CHAR* dvr_type,LONG sessionId ,fDrawFun drawCallBack ,LONG nUser);
//设置红外分析回调
VARIANT_BOOL (__stdcall *sv_set_ifred_callback)(CHAR* dvr_type,LONG sessionId ,fIFredFun ifredCallBack ,LONG nUser);
//开启直连语音对讲
LONG (__stdcall *sv_start_voicecom_dvr)(CHAR* dvr_type , LONG equipId , CHAR* ip , LONG port ,CHAR* username , CHAR* password);
//关闭直连语音对讲
VARIANT_BOOL (__stdcall *sv_stop_voicecom_dvr)(CHAR* dvr_type,LONG sessionId);
//控制语音对讲音量
VARIANT_BOOL (__stdcall *sv_voicecom_volumel_dvr)(CHAR* dvr_type, LONG sessionId, LONG volume);

IMPLEMENT_DYNCREATE(CAcomCtrl, COleControl)

	//去掉安全警告 BEGIN
	BEGIN_INTERFACE_MAP(CAcomCtrl, COleControl)
		INTERFACE_PART(CAcomCtrl, IID_IObjectSafety, ObjectSafety)
	END_INTERFACE_MAP()
	// Implementation of IObjectSafety
	STDMETHODIMP CAcomCtrl::XObjectSafety::GetInterfaceSafetyOptions(
		REFIID riid,
		DWORD __RPC_FAR *pdwSupportedOptions,
		DWORD __RPC_FAR *pdwEnabledOptions)
	{
		METHOD_PROLOGUE_EX(CAcomCtrl, ObjectSafety)
			if (!pdwSupportedOptions || !pdwEnabledOptions)
			{
				return E_POINTER;
			}
			*pdwSupportedOptions = INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA;
			*pdwEnabledOptions = 0;
			if (NULL == pThis->GetInterface(&riid))
			{
				TRACE("Requested interface is not supported.\n");
				return E_NOINTERFACE;
			}
			// What interface is being checked out anyhow?
			OLECHAR szGUID[39];
			int i = StringFromGUID2(riid, szGUID, 39);
			if (riid == IID_IDispatch)
			{
				// Client wants to know if object is safe for scripting
				*pdwEnabledOptions = INTERFACESAFE_FOR_UNTRUSTED_CALLER;
				return S_OK;
			}
			else if (riid == IID_IPersistPropertyBag
				|| riid == IID_IPersistStreamInit
				|| riid == IID_IPersistStorage
				|| riid == IID_IPersistMemory)
			{
				// Those are the persistence interfaces COleControl derived controls support
				// as indicated in AFXCTL.H
				// Client wants to know if object is safe for initializing from persistent data
				*pdwEnabledOptions = INTERFACESAFE_FOR_UNTRUSTED_DATA;
				return S_OK;
			}
			else
			{
				// Find out what interface this is, and decide what options to enable
				TRACE("We didn't account for the safety of this interface, and it's one we support...\n");
				return E_NOINTERFACE;
			}
	}
	STDMETHODIMP CAcomCtrl::XObjectSafety::SetInterfaceSafetyOptions(
		REFIID riid,
		DWORD dwOptionSetMask,
		DWORD dwEnabledOptions)
	{
		METHOD_PROLOGUE_EX(CAcomCtrl, ObjectSafety)
			OLECHAR szGUID[39];
		// What is this interface anyway?
		// We can do a quick lookup in the registry under HKEY_CLASSES_ROOT\Interface
		int i = StringFromGUID2(riid, szGUID, 39);
		if (0 == dwOptionSetMask && 0 == dwEnabledOptions)
		{
			// the control certainly supports NO requests through the specified interface
			// so it"s safe to return S_OK even if the interface isn"t supported.
			return S_OK;
		}
		// Do we support the specified interface?
		if (NULL == pThis->GetInterface(&riid))
		{
			TRACE1("%s is not support.\n", szGUID);
			return E_FAIL;
		}
		if (riid == IID_IDispatch)
		{
			TRACE("Client asking if it's safe to call through IDispatch.\n");
				TRACE("In other words, is the control safe for scripting?\n");
			if (INTERFACESAFE_FOR_UNTRUSTED_CALLER == dwOptionSetMask && INTERFACESAFE_FOR_UNTRUSTED_CALLER == dwEnabledOptions)
			{
				return S_OK;
			}
			else
			{
				return E_FAIL;
			}
		}
		else if (riid == IID_IPersistPropertyBag
			|| riid == IID_IPersistStreamInit
			|| riid == IID_IPersistStorage
			|| riid == IID_IPersistMemory)
		{
			TRACE("Client asking if it's safe to call through IPersist*.\n");
				TRACE("In other words, is the control safe for initializing from persistent data?\n");
			if (INTERFACESAFE_FOR_UNTRUSTED_DATA == dwOptionSetMask && INTERFACESAFE_FOR_UNTRUSTED_DATA == dwEnabledOptions)
			{
				return NOERROR;
			}
			else
			{
				return E_FAIL;
			}
		}
		else
		{
			TRACE1("We didn't account for the safety of %s, and it's one we support...\n", szGUID);
			return E_FAIL;
		}
	}
	STDMETHODIMP_(ULONG) CAcomCtrl::XObjectSafety::AddRef()
	{
		METHOD_PROLOGUE_EX_(CAcomCtrl, ObjectSafety)
			return (ULONG)pThis->ExternalAddRef();
	}
	STDMETHODIMP_(ULONG) CAcomCtrl::XObjectSafety::Release()
	{
		METHOD_PROLOGUE_EX_(CAcomCtrl, ObjectSafety)
			return (ULONG)pThis->ExternalRelease();
	}
	STDMETHODIMP CAcomCtrl::XObjectSafety::QueryInterface(
		REFIID iid, LPVOID* ppvObj)
	{
		METHOD_PROLOGUE_EX_(CAcomCtrl, ObjectSafety)
			return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
	}
	//去掉安全警告 END

// 消息映射

BEGIN_MESSAGE_MAP(CAcomCtrl, COleControl)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
//	ON_WM_ACTIVATE()
//ON_WM_CREATE()
ON_WM_SIZE()
ON_WM_CREATE()
ON_WM_LBUTTONDBLCLK()
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONUP()
ON_WM_MOUSEHOVER()
ON_WM_MOUSELEAVE()
ON_WM_MOUSEMOVE()

ON_MESSAGE(WM_DOWN_A,OnDownA)
ON_MESSAGE(WM_UP_A,OnUpA)

ON_MESSAGE(WM_DOWN_B,OnDownB)
ON_MESSAGE(WM_UP_B,OnUpB)

ON_MESSAGE(WM_DOWN_C,OnDownC)
ON_MESSAGE(WM_UP_C,OnUpC)

ON_MESSAGE(WM_DOWN_D,OnDownD)
ON_MESSAGE(WM_UP_D,OnUpD)

ON_MESSAGE(WM_BUTTON_A,OnButtonA)
ON_MESSAGE(WM_BUTTON_B,OnButtonB)
ON_MESSAGE(WM_BUTTON_C,OnButtonC)
ON_MESSAGE(WM_BUTTON_D,OnButtonD)
ON_MESSAGE(WM_BUTTON_E,OnButtonE)
ON_MESSAGE(WM_BUTTON_F,OnButtonF)
ON_MESSAGE(WM_BUTTON_G,OnButtonG)
ON_MESSAGE(WM_BUTTON_H,OnButtonH)

ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()



// 调度映射

BEGIN_DISPATCH_MAP(CAcomCtrl, COleControl)
	DISP_FUNCTION_ID(CAcomCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
	DISP_PROPERTY_NOTIFY_ID(CAcomCtrl, "DeviceType", dispidDeviceType, m_DeviceType, OnDeviceTypeChanged, VT_BSTR)
	DISP_PROPERTY_NOTIFY_ID(CAcomCtrl, "Token", dispidToken, m_Token, OnTokenChanged, VT_BSTR)
	DISP_PROPERTY_NOTIFY_ID(CAcomCtrl, "HostIP", dispidHostIP, m_HostIP, OnHostIPChanged, VT_BSTR)
	DISP_PROPERTY_NOTIFY_ID(CAcomCtrl, "HostPort", dispidHostPort, m_HostPort, OnHostPortChanged, VT_I4)
	DISP_PROPERTY_NOTIFY_ID(CAcomCtrl, "ConfigPort", dispidConfigPort, m_ConfigPort, OnConfigPortChanged, VT_I4)
	DISP_PROPERTY_NOTIFY_ID(CAcomCtrl, "ChannelNo", dispidChannelNo, m_ChannelNo, OnChannelNoChanged, VT_I4)
	DISP_PROPERTY_NOTIFY_ID(CAcomCtrl, "writeLog", dispidwriteLog, m_writeLog, OnwriteLogChanged, VT_I4)
	DISP_PROPERTY_NOTIFY_ID(CAcomCtrl, "hostWin", dispidhostWin, m_hostWin, OnhostWinChanged, VT_VARIANT)
	DISP_PROPERTY_NOTIFY_ID(CAcomCtrl, "LinkMode", dispidLinkMode, m_LinkMode, OnLinkModeChanged, VT_I4)
	DISP_PROPERTY_NOTIFY_ID(CAcomCtrl, "Fluency", dispidFluency, m_Fluency, OnFluencyChanged, VT_I4)
	DISP_PROPERTY_NOTIFY_ID(CAcomCtrl, "DisplayBuffer", dispidDisplayBuffer, m_DisplayBuffer, OnDisplayBufferChanged, VT_I4)
	DISP_PROPERTY_NOTIFY_ID(CAcomCtrl, "LoginUser", dispidLoginUser, m_LoginUser, OnLoginUserChanged, VT_BSTR)
	DISP_PROPERTY_NOTIFY_ID(CAcomCtrl, "Password", dispidPassword, m_Password, OnPasswordChanged, VT_BSTR)
	DISP_PROPERTY_NOTIFY_ID(CAcomCtrl, "DVRID", dispidDVRID, m_DVRID, OnDVRIDChanged, VT_I4)
	DISP_FUNCTION_ID(CAcomCtrl, "StartPlay", dispidStartPlay, StartPlay, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CAcomCtrl, "StopPlay", dispidStopPlay, StopPlay, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CAcomCtrl, "GetLastError", dispidGetLastError, GetLastError, VT_I4, VTS_NONE)
	DISP_PROPERTY_NOTIFY_ID(CAcomCtrl, "PTZCommandID", dispidPTZCommandID, m_PTZCommandID, OnPTZCommandIDChanged, VT_I4)
	DISP_PROPERTY_NOTIFY_ID(CAcomCtrl, "PTZControlEnabled", dispidPTZControlEnabled, m_PTZControlEnabled, OnPTZControlEnabledChanged, VT_I4)
	DISP_PROPERTY_NOTIFY_ID(CAcomCtrl, "PTZPresetIndex", dispidPTZPresetIndex, m_PTZPresetIndex, OnPTZPresetIndexChanged, VT_I4)
	DISP_PROPERTY_NOTIFY_ID(CAcomCtrl, "PTZSpeed", dispidPTZSpeed, m_PTZSpeed, OnPTZSpeedChanged, VT_I4)
	DISP_FUNCTION_ID(CAcomCtrl, "SendPTZControlCommand", dispidSendPTZControlCommand, SendPTZControlCommand, VT_BOOL, VTS_NONE)
	DISP_PROPERTY_NOTIFY_ID(CAcomCtrl, "Brightness", dispidBrightness, m_Brightness, OnBrightnessChanged, VT_I4)
	DISP_PROPERTY_NOTIFY_ID(CAcomCtrl, "Contrast", dispidContrast, m_Contrast, OnContrastChanged, VT_I4)
	DISP_PROPERTY_NOTIFY_ID(CAcomCtrl, "Saturation", dispidSaturation, m_Saturation, OnSaturationChanged, VT_I4)
	DISP_PROPERTY_NOTIFY_ID(CAcomCtrl, "Hue", dispidHue, m_Hue, OnHueChanged, VT_I4)
	DISP_PROPERTY_NOTIFY_ID(CAcomCtrl, "SaveFilePath", dispidSaveFilePath, m_SaveFilePath, OnSaveFilePathChanged, VT_BSTR)
	DISP_PROPERTY_NOTIFY_ID(CAcomCtrl, "SaveFileName", dispidSaveFileName, m_SaveFileName, OnSaveFileNameChanged, VT_BSTR)
	DISP_PROPERTY_NOTIFY_ID(CAcomCtrl, "Volume", dispidVolume, m_Volume, OnVolumeChanged, VT_I4)
	DISP_FUNCTION_ID(CAcomCtrl, "CapturePicture", dispidCapturePicture, CapturePicture, VT_BSTR, VTS_NONE)
	DISP_FUNCTION_ID(CAcomCtrl, "AudioOpen", dispidAudioOpen, AudioOpen, VT_BOOL, VTS_NONE)
	DISP_FUNCTION_ID(CAcomCtrl, "AudioClose", dispidAudioClose, AudioClose, VT_BOOL, VTS_NONE)
	DISP_FUNCTION_ID(CAcomCtrl, "SetVideoEffect", dispidSetVideoEffect, SetVideoEffect, VT_BOOL, VTS_NONE)
	DISP_PROPERTY_NOTIFY_ID(CAcomCtrl, "StreamMediaId", dispidStreamMediaId, m_StreamMediaId, OnStreamMediaIdChanged, VT_BSTR)
	DISP_PROPERTY_NOTIFY_ID(CAcomCtrl, "StreamMediaIp", dispidStreamMediaIp, m_StreamMediaIp, OnStreamMediaIpChanged, VT_BSTR)
	DISP_PROPERTY_NOTIFY_ID(CAcomCtrl, "StreamMediaPort", dispidStreamMediaPort, m_StreamMediaPort, OnStreamMediaPortChanged, VT_I4)
	DISP_PROPERTY_NOTIFY_ID(CAcomCtrl, "ConnectionType", dispidConnectionType, m_ConnectionType, OnConnectionTypeChanged, VT_I4)
	DISP_PROPERTY_NOTIFY_ID(CAcomCtrl, "isPlaying", dispidisPlaying, m_isPlaying, OnisPlayingChanged, VT_BOOL)
	DISP_FUNCTION_ID(CAcomCtrl, "FaceRecognition", dispidFaceRecognition, FaceRecognition, VT_EMPTY, VTS_NONE)
	DISP_PROPERTY_NOTIFY_ID(CAcomCtrl, "VoiceTalking", dispidVoiceTalking, m_VoiceTalking, OnVoiceTalkingChanged, VT_BOOL)
	DISP_PROPERTY_NOTIFY_ID(CAcomCtrl, "canPtz", dispidcanPtz, m_canPtz, OncanPtzChanged, VT_I4)
	DISP_PROPERTY_NOTIFY_ID(CAcomCtrl, "canInfrared", dispidcanInfrared, m_canInfrared, OncanInfraredChanged, VT_I4)
	DISP_PROPERTY_NOTIFY_ID(CAcomCtrl, "machineId", dispidmachineId, m_machineId, OnmachineIdChanged, VT_I4)
	DISP_PROPERTY_NOTIFY_ID(CAcomCtrl, "alarmUrl", dispidalarmUrl, m_alarmUrl, OnalarmUrlChanged, VT_BSTR)
	DISP_PROPERTY_NOTIFY_ID(CAcomCtrl, "alarmHostIp", dispidalarmHostIp, m_alarmHostIp, OnalarmHostIpChanged, VT_BSTR)
	DISP_PROPERTY_NOTIFY_ID(CAcomCtrl, "alarmPort", dispidalarmPort, m_alarmPort, OnalarmPortChanged, VT_I4)
	DISP_PROPERTY_NOTIFY_ID(CAcomCtrl, "VoiceTalkingVolume", dispidVoiceTalkingVolume, m_VoiceTalkingVolume, OnVoiceTalkingVolumeChanged, VT_I4)
END_DISPATCH_MAP()



// 事件映射

BEGIN_EVENT_MAP(CAcomCtrl, COleControl)
	EVENT_CUSTOM_ID("ondblclick", eventidondblclick, Fireondblclick, VTS_NONE)
	EVENT_CUSTOM_ID("onclick", eventidonclick, Fireonclick, VTS_NONE)
	EVENT_CUSTOM_ID("onStartPlay", eventidonStartPlay, FireonStartPlay, VTS_NONE)
	EVENT_CUSTOM_ID("OnGetVideoEffect", eventidOnGetVideoEffect, FireOnGetVideoEffect, VTS_NONE)
	EVENT_CUSTOM_ID("OnStartPlayError", eventidOnStartPlayError, FireOnStartPlayError, VTS_NONE)
END_EVENT_MAP()



// 属性页

// TODO: 按需要添加更多属性页。请记住增加计数!
BEGIN_PROPPAGEIDS(CAcomCtrl, 1)
	PROPPAGEID(CAcomPropPage::guid)
END_PROPPAGEIDS(CAcomCtrl)



// 初始化类工厂和 guid

IMPLEMENT_OLECREATE_EX(CAcomCtrl, "ACOM.AcomCtrl.1",
	0x6cb6c1b5, 0xe0d, 0x4622, 0x85, 0x10, 0xdf, 0x1e, 0xb6, 0x35, 0, 0x50)



// 键入库 ID 和版本

IMPLEMENT_OLETYPELIB(CAcomCtrl, _tlid, _wVerMajor, _wVerMinor)



// 接口 ID

const IID IID_DAcom = { 0x44FB3261, 0x2DF3, 0x4596, { 0x95, 0x27, 0xFE, 0x96, 0x3F, 0x54, 0xF, 0x25 } };
const IID IID_DAcomEvents = { 0x731A0EF, 0xDAA1, 0x4DDD, { 0xB7, 0xC1, 0xB7, 0x47, 0x3A, 0x41, 0x64, 0xF8 } };


// 控件类型信息

static const DWORD _dwAcomOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CAcomCtrl, IDS_ACOM, _dwAcomOleMisc)



// CAcomCtrl::CAcomCtrlFactory::UpdateRegistry -
// 添加或移除 CAcomCtrl 的系统注册表项

BOOL CAcomCtrl::CAcomCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: 验证您的控件是否符合单元模型线程处理规则。
	// 有关更多信息，请参考 MFC 技术说明 64。
	// 如果您的控件不符合单元模型规则，则
	// 必须修改如下代码，将第六个参数从
	// afxRegApartmentThreading 改为 0。

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_ACOM,
			IDB_ACOM,
			afxRegApartmentThreading,
			_dwAcomOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}



// CAcomCtrl::CAcomCtrl - 构造函数

CAcomCtrl::CAcomCtrl()
{
	InitializeIIDs(&IID_DAcom, &IID_DAcomEvents);
	sessionId = 0;
	m_button = NULL;
	m_button1 = NULL;
	m_button2 = NULL;
	m_button3 = NULL;
	// TODO: 在此初始化控件的实例数据。
	OUTPUT_LOG("初始化");
}



// CAcomCtrl::~CAcomCtrl - 析构函数

CAcomCtrl::~CAcomCtrl()
{
	// TODO: 在此清理控件的实例数据。
	/*delete m_button;
	delete m_button1;
	delete m_button2;
	delete m_button3;*/
	OUTPUT_LOG("销毁");
}



// CAcomCtrl::OnDraw - 绘图函数

void CAcomCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if (!pdc)
		return;

	// TODO: 用您自己的绘图代码替换下面的代码。
	//pdc->FillRect(rcBounds, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
	//pdc->Ellipse(rcBounds);
	this->MoveWindow(rcBounds,TRUE);
	CBrush brBackGnd(TranslateColor(AmbientBackColor()));
	pdc->FillRect(rcBounds, &brBackGnd);
}


// CAcomCtrl::DoPropExchange - 持久性支持

void CAcomCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: 为每个持久的自定义属性调用 PX_ 函数。
}



// CAcomCtrl::OnResetState - 将控件重置为默认状态

void CAcomCtrl::OnResetState()
{
	COleControl::OnResetState();  // 重置 DoPropExchange 中找到的默认值

	// TODO: 在此重置任意其他控件状态。
}



// CAcomCtrl::AboutBox - 向用户显示“关于”框

void CAcomCtrl::AboutBox()
{
	CDialogEx dlgAbout(IDD_ABOUTBOX_ACOM);
	dlgAbout.DoModal();
}



// CAcomCtrl 消息处理程序


//void CAcomCtrl::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
//{
//	COleControl::OnActivate(nState, pWndOther, bMinimized);
//
//	// TODO: 在此处添加消息处理程序代码
//}


//int CAcomCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
//{
//	if (COleControl::OnCreate(lpCreateStruct) == -1)
//		return -1;
//
//	// TODO:  在此添加您专用的创建代码
//
//	return 0;
//}


void CAcomCtrl::OnSize(UINT nType, int cx, int cy)
{
	COleControl::OnSize(nType, cx, cy);

	RECT activeRECT;
	GetClientRect(&activeRECT);
	this->MoveWindow(&activeRECT);

	/*if (!m_button&&!m_button1&&!m_button2&&!m_button3)
	{*/
		delete m_button;
		delete m_button1;
		delete m_button2;
		delete m_button3;
		OUTPUT_LOG("delete");
	//}

	m_button = NewMyRadio( IDC_MYRADIO1, CRect(0,cy/5*2,20,cy/5*3), 0 );
	m_button1 = NewMyRadio1( IDC_MYRADIO2, CRect(cx/5*2,0,cx/5*3,20), 0 );
	m_button2 = NewMyRadio2( IDC_MYRADIO3, CRect(cx-20,cy/5*2,cx,cy/5*3), 0 );
	m_button3 = NewMyRadio3( IDC_MYRADIO4, CRect(cx/5*2,cy-20,cx/5*3,cy), 0 );

	m_button->ShowWindow(SW_HIDE);
	m_button1->ShowWindow(SW_HIDE);
	m_button2->ShowWindow(SW_HIDE);
	m_button3->ShowWindow(SW_HIDE);
}

CMyButton* CAcomCtrl::NewMyRadio(int nID,CRect rect,int nStyle)
{
	CString m_Caption;
	m_Caption.LoadString( nID ); //取按钮标题
	CMyButton *p_Radio = new CMyButton();
	ASSERT_VALID(p_Radio);
	p_Radio->Create( m_Caption, WS_CHILD|WS_VISIBLE|WS_BORDER|BS_OWNERDRAW, rect, this, nID ); //创建按钮
	p_Radio->m_hande = this->GetSafeHwnd();
    p_Radio->SetDownColor(RGB(205,170,125));
	p_Radio->SetUpColor(RGB(220,220,220));
	return p_Radio;
}

CMyButton1* CAcomCtrl::NewMyRadio1(int nID,CRect rect,int nStyle)
{
	CString m_Caption;
	m_Caption.LoadString( nID ); //取按钮标题
	CMyButton1 *p_Radio = new CMyButton1();
	ASSERT_VALID(p_Radio);
	p_Radio->Create( m_Caption, WS_CHILD|WS_VISIBLE|WS_BORDER|BS_OWNERDRAW, rect, this, nID ); //创建按钮
	p_Radio->m_hande1 = this->GetSafeHwnd();
	p_Radio->SetDownColor(RGB(205,170,125));
	p_Radio->SetUpColor(RGB(220,220,220));
	return p_Radio;
}
CMyButton2* CAcomCtrl::NewMyRadio2(int nID,CRect rect,int nStyle)
{
	CString m_Caption;
	m_Caption.LoadString( nID ); //取按钮标题
	CMyButton2 *p_Radio = new CMyButton2();
	ASSERT_VALID(p_Radio);
	p_Radio->Create( m_Caption, WS_CHILD|WS_VISIBLE|WS_BORDER|BS_OWNERDRAW, rect, this, nID ); //创建按钮
	p_Radio->m_hande2 = this->GetSafeHwnd();
	p_Radio->SetDownColor(RGB(205,170,125));
	p_Radio->SetUpColor(RGB(220,220,220));
	return p_Radio;
}
CMyButton3* CAcomCtrl::NewMyRadio3(int nID,CRect rect,int nStyle)
{
	CString m_Caption;
	m_Caption.LoadString( nID ); //取按钮标题
	CMyButton3 *p_Radio = new CMyButton3();
	ASSERT_VALID(p_Radio);
	p_Radio->Create( m_Caption, WS_CHILD|WS_VISIBLE|WS_BORDER|BS_OWNERDRAW, rect, this, nID ); //创建按钮
	p_Radio->m_hande3 = this->GetSafeHwnd();
	p_Radio->SetDownColor(RGB(205,170,125));
	p_Radio->SetUpColor(RGB(220,220,220));
	return p_Radio;
}
int CAcomCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;

	//m_VideoDlg.Create(IDD_MAIN_DIALOG,this);
	return 0;
}

//----------------------------------------------------------------------------// 
// 功能：加载sv_dvr_process.dll动态库和其中的一些功能函数 
// 输入/输出参数：无 
// 版本：1.0 
// 修改： 
//----------------------------------------------------------------------------// 
void CAcomCtrl::InitDll()
{
	if (DLLInst!=NULL)
	{
		return;
	}
	DLLInst=LoadLibrary("sv_dvr_process.dll");
	/*F:\\new\\nvmpclientsdk\\sv_dvr_process\\Debug\\*/
	if(DLLInst!=NULL)
	{
		OUTPUT_LOG("加载sv_dvr_process.dll成功");
		sv_init=(VARIANT_BOOL (__stdcall *)())(GetProcAddress(DLLInst,"sv_init"));
		sv_uninit=(VARIANT_BOOL (__stdcall *)())(GetProcAddress(DLLInst,"sv_uninit"));
		sv_close_process=(VARIANT_BOOL (__stdcall *)(CHAR* dvr_type))(GetProcAddress(DLLInst,"sv_close_process"));
		sv_start_preview_dvr=(LONG (__stdcall *)(CHAR* dvr_type,LONG , LONG , LONG ,LONG ,LONG,CHAR* ip , LONG ,CHAR* username , CHAR* password , LONG))(GetProcAddress(DLLInst,"sv_start_preview_dvr"));
		sv_stop_preview=(VARIANT_BOOL (__stdcall *)(CHAR* dvr_type,LONG ))(GetProcAddress(DLLInst,"sv_stop_preview"));
		sv_alive_request=(VARIANT_BOOL (__stdcall *)(CHAR* dvr_type , LONG ))(GetProcAddress(DLLInst,"sv_alive_request"));
		sv_ptz_control_dvr=(VARIANT_BOOL (__stdcall *)(CHAR* dvr_type, LONG  , LONG  , LONG  , LONG  , LONG  , LONG  , LONG  , CHAR* ip ,LONG  , CHAR* username , CHAR* password))(GetProcAddress(DLLInst,"sv_ptz_control_dvr"));
		sv_get_videopara=(VARIANT_BOOL (__stdcall *)(CHAR* dvr_type,LONG ,SvVideoPara* videoPara))(GetProcAddress(DLLInst,"sv_get_videopara"));
		sv_capture_picture=(VARIANT_BOOL (__stdcall *)(CHAR* dvr_type,LONG ,CHAR* path , CHAR * returnPath))(GetProcAddress(DLLInst,"sv_capture_picture"));
		sv_sound_control=(VARIANT_BOOL (__stdcall *)(CHAR* dvr_type,LONG , LONG  ,LONG ))(GetProcAddress(DLLInst,"sv_sound_control"));
		sv_set_videopara=(VARIANT_BOOL (__stdcall *)(CHAR* dvr_type,LONG sessionId ,LONG bri ,LONG con ,LONG sat , LONG  hue))(GetProcAddress(DLLInst,"sv_set_videopara"));
		sv_get_last_error=(LONG (__stdcall *)(CHAR* dvr_type, CHAR* errorCode))(GetProcAddress(DLLInst,"sv_get_last_error"));
		sv_start_preview_media=(LONG (__stdcall *)(CHAR* dvr_type,LONG,LONG,LONG,LONG,LONG,LONG,CHAR* mediaip,LONG,LONG,CHAR* token,LONG))(GetProcAddress(DLLInst,"sv_start_preview_media"));
		sv_start_voicecom_dvr=(LONG (__stdcall *)(CHAR* dvr_type , LONG , CHAR* ip , LONG ,CHAR* username , CHAR* password))(GetProcAddress(DLLInst,"sv_start_voicecom_dvr"));
		sv_stop_voicecom_dvr=(VARIANT_BOOL (__stdcall *)(CHAR* dvr_type,LONG))(GetProcAddress(DLLInst,"sv_stop_voicecom_dvr"));
		sv_set_draw_callback=(VARIANT_BOOL (__stdcall *)(CHAR* dvr_type,LONG,fDrawFun drawCallBack ,LONG))(GetProcAddress(DLLInst,"sv_set_draw_callback"));
		sv_set_ifred_callback=(VARIANT_BOOL (__stdcall *)(CHAR* dvr_type,LONG,fIFredFun ifredCallBack ,LONG))(GetProcAddress(DLLInst,"sv_set_ifred_callback"));
		sv_voicecom_volumel_dvr=(VARIANT_BOOL (__stdcall *)(CHAR* dvr_type, LONG, LONG))(GetProcAddress(DLLInst,"sv_voicecom_volumel_dvr"));
	}
	else
	{
		::MessageBox(NULL,"加载动态库失败！", "提示信息", MB_OK | MB_ICONINFORMATION);
		OUTPUT_LOG("加载sv_dvr_process.dll失败");
		exit(0);
	}
}
//----------------------------------------------------------------------------// 
// 功能：释放动态库 
// 输入/输出参数：无 
// 版本：1.0 
// 修改： 
//----------------------------------------------------------------------------// 
void CAcomCtrl::ExitDll()
{
	if(DLLInst!=NULL)
		FreeLibrary(DLLInst);
}

void CAcomCtrl::OnDeviceTypeChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


void CAcomCtrl::OnTokenChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


void CAcomCtrl::OnHostIPChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


void CAcomCtrl::OnHostPortChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


void CAcomCtrl::OnConfigPortChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


void CAcomCtrl::OnChannelNoChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


void CAcomCtrl::OnwriteLogChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


void CAcomCtrl::OnhostWinChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码
	CString str = m_hostWin;
	OUTPUT_LOG("%s","hostWin == "+str);
	SetModifiedFlag();
}


void CAcomCtrl::OnLinkModeChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


void CAcomCtrl::OnFluencyChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


void CAcomCtrl::OnDisplayBufferChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


void CAcomCtrl::OnLoginUserChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


void CAcomCtrl::OnPasswordChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


void CAcomCtrl::OnDVRIDChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}

DWORD __stdcall Fun(LPVOID lp)   
{	
	TPFileList *lpParamter = (TPFileList *)lp;
	LONG id1 = lpParamter->sessionid;
	CString devicetype1 = lpParamter->devicetype;

	CAcomCtrl *ctrl;
	ctrl = (CAcomCtrl *)lpParamter->lp;
	char errorCode;
	bool succ1 = true;
	while(succ1)
	{
		//AfxMessageBox("kk");
		sv_alive_request((LPSTR)(LPCTSTR)devicetype1,id1);
		succ1 = ctrl->flag1;
		if (!succ1)
		{
			OUTPUT_LOG("重连退出");
			break;
		}
		int error_ = sv_get_last_error((LPSTR)(LPCTSTR)devicetype1,&errorCode);
		//OUTPUT_LOG("保活");
		//if ( -44== errorCode)goto start11;
		if ( -44 == errorCode)
		{
			OUTPUT_LOG("重连");
			goto start11;
		}
		Sleep(500);
	}

	delete lpParamter;
	return 0;

start11:
	
	ctrl->StopPlay();
	ctrl->StartPlay();
	
	delete lpParamter;
	return 0;
}

void __stdcall DrawFun(int sessionID,HDC hDc, long dwUser)
{
	CAcomCtrl *ctrl;
	ctrl = (CAcomCtrl *)dwUser;

	if ( hDc == NULL )
	{
		return;
	}
	CDC *pDC = CDC::FromHandle(hDc);
	CString str = "□";

	SetBkMode(hDc, TRANSPARENT);
	SetTextColor(hDc, RGB(0, 0, 250));
	SetBkColor(hDc, RGB(0, 0, 255));

	CFont font;
	font.CreateFont( 
		150,                              //   nHeight  
		50,                               //   nWidth  
		0,                                //   nEscapement
		0,                                //   nOrientation  
		FW_NORMAL,                        //   nWeight  
		FALSE,                            //   bItalic  
		FALSE,                            //   bUnderline  
		0,                                //   cStrikeOut  
		ANSI_CHARSET,                     //   nCharSet  
		OUT_DEFAULT_PRECIS,               //   nOutPrecision  
		CLIP_DEFAULT_PRECIS,              //   nClipPrecision  
		DEFAULT_QUALITY,                  //   nQuality  
		DEFAULT_PITCH   |   FF_SWISS,     //   nPitchAndFamily     
		_T("方正兰亭超细黑简体"));

	pDC->SelectObject(&font);
	pDC->TextOut(220, 126, str, str.GetLength());
	//bool bRet = TextOut(hDc, 60, 60, str, str.GetLength());
}
extern int i = 0;
extern int flag3 = 1;

extern int port = 0;
extern int id_A = 0;
extern CString ip = NULL;

int upload_rest(char* ip, unsigned short port, char* url, char* request, unsigned int len, char** ssresponse ,char* httptype = "GET");
int HTTPRest(void);
char* m_cookie;

DWORD __stdcall Fun3() 
{
	while(flag3)
	{
		if (i>0)
		{
			OUTPUT_LOG("Rest");
			HTTPRest();
            i = 0;
			Sleep(2000);
		}
	}
	return 0;
}

void __stdcall IFredFun(char* buff, int width, int height)
{
	OUTPUT_LOG("回调");
	i++;
	CString str;
	str.Format("%d",i);
	OUTPUT_LOG(str);
}

int upload_rest( char* ip, unsigned short port, char* url, char* request, unsigned int len, char** ssresponse, char* httptype)
{
	
	/*	ss << "POST " << url <<" HTTP/1.1\r\nHost: " << host << ":" << port << "\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length:" 
	<< strlen(data) + 5 <<"\r\n\r\n" <<  "data=" << data ;*/

 	static char *post_req = "%s %s HTTP/1.1\r\n"
 		"Host: %s:%d\r\n"
 		"Cookie: csst_token=%s\r\n"
 		"Content-Type: application/x-www-form-urlencoded\r\n"
 		"Content-Length: %d\r\n"
 		"Connection: keep-alive\r\n\r\n";

	//建立socket连接
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2,1);
	err = WSAStartup(wVersionRequested,&wsaData);
	if (err != 0) 
		return -1;


	SOCKET sockSrv = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	SOCKADDR_IN addrSrv;
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_addr.s_addr = inet_addr(ip);
	addrSrv.sin_port = htons(port);

	if(connect(sockSrv,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR_IN)) == SOCKET_ERROR){
		int error = WSAGetLastError();
		closesocket(sockSrv);
		return -2;
	}
	//////////////////////////////////////////////////////////////////////////

	//构造HTTP头
	char* ssUrl = new char[4096];
	::sprintf(ssUrl,
		post_req,
		httptype,
		url,
		ip,
		port,
		m_cookie,
		len);

	//////////////////////////////////////////////////////////////////////////
	int timeOut = 5000;
	::setsockopt(sockSrv, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeOut, sizeof(timeOut));
	::setsockopt(sockSrv, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeOut, sizeof(timeOut));

	//发送http头
	int sent_len = strlen(ssUrl);
	int result = 0;
	do{
		int ret = send(sockSrv,(const char *)ssUrl+result, sent_len-result, 0);//发送请求头
		if(SOCKET_ERROR==ret)
		{
			int error = WSAGetLastError();
			closesocket(sockSrv);
			return -3;
		}
		result += ret;
	}while(result<sent_len);

	//////////////////////////////////////////////////////////////////////////


	//发送http body， request
	sent_len = strlen(request);
	result = 0;
	do{
		int ret = send(sockSrv,(const char *)request+result, sent_len-result, 0);
		if(SOCKET_ERROR==ret)
		{
			int error = WSAGetLastError();
			closesocket(sockSrv);
			return -4;
		}
		result += ret;
	}while(result<sent_len);

	//检查数据长度
	///返回数据的长度,http，搜索空行，\r\n\r\n

	char* response1 = new char[2048];
	memset(response1, 0, 2048);
	int rn = 0;
	int len1 = 0;
	while(1)
	{
		//收一段数据
		int now = recv(sockSrv,(char *)response1+len1, 128, 0);	//接受响应
		len1 += now;
		if (!now || SOCKET_ERROR==len1)
		{
			int error = WSAGetLastError();
			closesocket(sockSrv);
			return -5;
		}

		//如果有200 OK，检查换行符
		char* p = strstr(response1,"\r\n\r\n");
		if(p)
		{
			rn = p - response1 + 4;
			break;
		}
	}

	if(rn)	///body offset
	{
		//获取内容长度content-length
		int bodyLen = 0;
		char* p = strstr(response1,"Content-Length:");
		if(p)
		{
			p += ::strlen("Content-Length:");
			while(' '==*p) p++;
			char* p2 = strstr(p,"\r\n");
			*p2 = 0;
			bodyLen = atoi(p);
			*p2 = '\r';
		}else{
			closesocket(sockSrv);
			return -6;
		}

		//得到长度后，读取目标内容
		int remains = bodyLen+rn-len1;	//len1-rn

		int len2 = 0;
		if(remains)
		{
			char* response2 = new char[remains+100];
			::memset(response2,0,remains+100);
			while(1)
			{
				int now = recv(sockSrv,(char *)response2+len2, remains-len2, 0);
				len2 += now;
				if(len2>=remains)
				{
					break;
				}
				if (!now || SOCKET_ERROR==len2)
				{
					int error = WSAGetLastError();
					closesocket(sockSrv);
					return -7;
				}
			}

			//合并两块内存
			char* pp = new char[bodyLen + 100];
			::memset(pp,0,bodyLen+100);
			::memcpy(pp,response1+rn,len1-rn);
			::memcpy(pp + len1 - rn,response2,remains);
			*ssresponse = pp;
		}
		else{
			*ssresponse = new char[bodyLen + 100];
			memset(*ssresponse,0,bodyLen + 100);
			::memcpy(*ssresponse,response1+rn,len1-rn);
		}

		//检查200 OK
		if (strstr(response1, "200 OK") || strstr(response1, "302 Moved"))
		{
			closesocket(sockSrv);
			return 0;
		}else{
			closesocket(sockSrv);
			return -8;
		}
	}

	closesocket(sockSrv);
	return -9;	
}

int HTTPRest(void)
{
	/*http://10.3.0.95:8080/sc/machine?_type=web&_method=getMachineInfraRedAlarm&scode=1*/
	char* ssresponse;
	std::stringstream url;
	url<<"/sc/machine?_type=web&_method=getMachineInfraRedAlarm&scode="<<id_A;

	std::stringstream request;
	unsigned int requestlen = 0;

	int ret = upload_rest((LPSTR)(LPCTSTR)ip, port, (char*)url.str().c_str(), (char*)request.str().c_str(), requestlen, &ssresponse );
	CString str;
	str.Format("%d",ret);
	OUTPUT_LOG("rest请求返回值："+str);

	OUTPUT_LOG(ip);
	CString str2;
	str2.Format("%d",port);
	OUTPUT_LOG(str2);
	CString str3;
	str3.Format("%d",id_A);
	OUTPUT_LOG(str3);
	return 0;
}

void CAcomCtrl::StartPlay(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	//flag = false;
	m_button->ShowWindow(SW_HIDE);
	m_button1->ShowWindow(SW_HIDE);
	m_button2->ShowWindow(SW_HIDE);
	m_button3->ShowWindow(SW_HIDE);

	port = m_alarmPort;
	id_A = m_machineId;
	ip = m_alarmHostIp;

	InitDll();
	sv_init();
	TPFileList* tp = new TPFileList();
	//int wind = (int)this->m_VideoDlg.GetSafeHwnd();
	int wind = (int)this->GetSafeHwnd();

	if (m_StreamMediaIp.IsEmpty())
	{
		OUTPUT_LOG("直连预览");
		//sessionId = sv_start_preview_dvr("Hik_Embed",196612,0,0,0,0,"192.168.203.149",8000,"admin","12345", wind);
		sessionId = sv_start_preview_dvr( (LPSTR)(LPCTSTR)m_DeviceType,  m_DVRID, m_ChannelNo,  m_LinkMode , m_Fluency , m_DisplayBuffer, (LPSTR)(LPCTSTR)m_HostIP ,  m_HostPort , (LPSTR)(LPCTSTR)m_LoginUser ,  (LPSTR)(LPCTSTR)m_Password , wind);
		if (DLLInst!=NULL)
		{
			flag1 = true;
			tp->sessionid = sessionId;
			tp->devicetype = m_DeviceType;
			tp->lp = (char *)this;
			//CString str;
			//str.Format("%d",sessionId);
			//AfxMessageBox(str);
			HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Fun, tp,0, NULL);   
			CloseHandle(hThread);
			//OUTPUT_LOG("打开成功");
			FireonStartPlay();
			m_isPlaying = VARIANT_TRUE;

			SvVideoPara para;
			VARIANT_BOOL succ = sv_get_videopara((LPSTR)(LPCTSTR)m_DeviceType,sessionId,&para);
			if (succ)
			{
				m_Brightness = para.bri;
				m_Contrast = para.con;
				m_Saturation = para.hue;
				m_Hue = para.sat;

				FireOnGetVideoEffect();
			}
		}

		if (1 == m_canInfrared)
		{
			sv_set_ifred_callback((LPSTR)(LPCTSTR)m_DeviceType,sessionId,IFredFun,(long)this);
			flag3 = 1;
			HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Fun3, NULL,0, NULL);   
			CloseHandle(hThread);
			OUTPUT_LOG("开始红外");
		} 
		else if(0 == m_canInfrared)
		{
			sv_set_ifred_callback((LPSTR)(LPCTSTR)m_DeviceType,sessionId,NULL,(long)this);
			OUTPUT_LOG("停止红外");
		}
	}
	else
	{
		LONG streammediaID;
		streammediaID = atoi(m_StreamMediaId);
		sessionId = sv_start_preview_media((LPSTR)(LPCTSTR)m_DeviceType,m_DVRID,m_ChannelNo,m_LinkMode,m_Fluency,m_DisplayBuffer,streammediaID,(LPSTR)(LPCTSTR)m_StreamMediaIp,m_StreamMediaPort,m_ConnectionType,(LPSTR)(LPCTSTR)m_Token,wind);
		if (DLLInst!=NULL)
		{
			OUTPUT_LOG("流媒体预览");
			flag1 = true;
			tp->sessionid = sessionId;
			tp->devicetype = m_DeviceType;
			tp->lp = (char *)this;
			//CString str;
			//str.Format("%d",sessionId);
			//AfxMessageBox(str);
			HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Fun, tp,0, NULL);   
			CloseHandle(hThread);
			//OUTPUT_LOG("打开成功");
			FireonStartPlay();
			m_isPlaying = VARIANT_TRUE;

			SvVideoPara para;
			VARIANT_BOOL succ = sv_get_videopara((LPSTR)(LPCTSTR)m_DeviceType,sessionId,&para);
			if (succ)
			{
				m_Brightness = para.bri;
				m_Contrast = para.con;
				m_Saturation = para.hue;
				m_Hue = para.sat;

				FireOnGetVideoEffect();
			}
		}
	}
}


void CAcomCtrl::StopPlay(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	flag1 = false;
	flag3 = 0;
	if ( m_DeviceType == "")
	{
		return;
	}

	try
	{
		VARIANT_BOOL succ = sv_stop_preview((LPSTR)(LPCTSTR)m_DeviceType,sessionId);
		//VARIANT_BOOL succ = sv_stop_preview("Hik_Embed",1);
		m_isPlaying = VARIANT_FALSE;
	}
	catch(...)
	{
		//AfxMessageBox("ww");
	}
	OUTPUT_LOG("关闭");
	return;
}


LONG CAcomCtrl::GetLastError(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码

	return 0;
}


void CAcomCtrl::OnPTZCommandIDChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


void CAcomCtrl::OnPTZControlEnabledChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


void CAcomCtrl::OnPTZPresetIndexChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


void CAcomCtrl::OnPTZSpeedChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


VARIANT_BOOL CAcomCtrl::SendPTZControlCommand(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	VARIANT_BOOL succ = sv_ptz_control_dvr((LPSTR)(LPCTSTR)m_DeviceType,  m_DVRID, m_ChannelNo,  m_PTZCommandID , m_PTZControlEnabled , m_PTZPresetIndex,m_PTZSpeed,sessionId, (LPSTR)(LPCTSTR)m_HostIP ,  m_HostPort , (LPSTR)(LPCTSTR)m_LoginUser ,  (LPSTR)(LPCTSTR)m_Password );
	if (succ)
	{
		OUTPUT_LOG("云台控制成功");
		return VARIANT_TRUE;
	}
	else
	{
		OUTPUT_LOG("云台控制失败");
		return VARIANT_FALSE;
	}
}


void CAcomCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
	//OnCopySrc();
	Fireondblclick();
	COleControl::OnLButtonDblClk(nFlags, point);
}


void CAcomCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
	COleControl::OnLButtonDown(nFlags, point);
}


void CAcomCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
	Fireonclick();
	COleControl::OnLButtonUp(nFlags, point);
}


void CAcomCtrl::OnBrightnessChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


void CAcomCtrl::OnContrastChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


void CAcomCtrl::OnSaturationChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


void CAcomCtrl::OnHueChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


void CAcomCtrl::OnSaveFilePathChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


void CAcomCtrl::OnSaveFileNameChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


void CAcomCtrl::OnVolumeChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


BSTR CAcomCtrl::CapturePicture(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString str1 = m_SaveFilePath + m_SaveFileName + ".jpg";
	char path[255];
	memset(path,0,sizeof(path));
	strcpy(path,m_SaveFilePath);
	char file[255];
	memset(file,0,sizeof(file));
	strcpy(file,m_SaveFileName);
	strcat(path,file);

	sv_capture_picture((LPSTR)(LPCTSTR)m_DeviceType,sessionId ,path ,file);

	return str1.AllocSysString();
}


VARIANT_BOOL CAcomCtrl::AudioOpen(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	VARIANT_BOOL succ = sv_sound_control((LPSTR)(LPCTSTR)m_DeviceType,sessionId,1,m_Volume);
	if (succ)
	{
		OUTPUT_LOG("声音打开成功");
		return VARIANT_TRUE;
	}
	else
	{
		OUTPUT_LOG("声音打开失败");
		return VARIANT_FALSE;
	}

}


VARIANT_BOOL CAcomCtrl::AudioClose(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	VARIANT_BOOL succ = sv_sound_control((LPSTR)(LPCTSTR)m_DeviceType,sessionId,0,m_Volume);
	if (succ)
	{
		OUTPUT_LOG("声音关闭成功");
		return VARIANT_TRUE;
	}
	else
	{
		OUTPUT_LOG("声音声音失败");
		return VARIANT_FALSE;
	}

}


VARIANT_BOOL CAcomCtrl::SetVideoEffect(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	VARIANT_BOOL succ = sv_set_videopara((LPSTR)(LPCTSTR)m_DeviceType,sessionId,m_Brightness,m_Contrast,m_Saturation,m_Hue);
	if (succ)
	{
		OUTPUT_LOG("设置参数成功");
		return VARIANT_TRUE;
	}
	else
	{
		OUTPUT_LOG("设置参数失败");
		return VARIANT_FALSE;
	}
}


void CAcomCtrl::OnMouseHover(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//if (!_bMouseTrack)
	//{
	//	TRACKMOUSEEVENT tme;
	//	tme.cbSize = sizeof(tme);
	//	tme.hwndTrack = this->GetSafeHwnd();
	//	tme.dwFlags = TME_LEAVE;
	//	tme.dwHoverTime = 1;
	//	_bMouseTrack = _TrackMouseEvent(&tme);

	//	AfxMessageBox("鼠标在按钮上");
	//}
	COleControl::OnMouseHover(nFlags, point);
}


void CAcomCtrl::OnMouseLeave()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
    _bMouseTrack = FALSE;

	COleControl::OnMouseLeave();
}


void CAcomCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS)); 
	if (!_bMouseTrack && 1 == m_canPtz)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = this->GetSafeHwnd();
		tme.dwFlags = TME_LEAVE;
		tme.dwHoverTime = 1;
		_bMouseTrack = _TrackMouseEvent(&tme);

		OUTPUT_LOG("进入主窗体");
		m_button->ShowWindow(SW_SHOW);
		m_button1->ShowWindow(SW_SHOW);
		m_button2->ShowWindow(SW_SHOW);
		m_button3->ShowWindow(SW_SHOW);
	}
	COleControl::OnMouseMove(nFlags, point);
}

BOOL BitMapToFile(HBITMAP hBitmap, LPSTR FileName)
{
	HDC hdc;
	int ibits;//当前显示模式下每个像素所占字节数
	WORD  wbitsCount;//位图中每个像素所占字节数。
	DWORD dwpalettelsize=0;//调色板大小
	DWORD dwbmdibitsize,dwdibsize,dwwritten;
	BITMAP bitmap;
	BITMAPFILEHEADER bmfhdr;
	BITMAPINFOHEADER bi;
	LPBITMAPINFOHEADER lpbi;
	HANDLE fh,fdib,hpal,holdpal=NULL;
	CString work;
	hdc=CreateDC("display",NULL,NULL,NULL);
	ibits=::GetDeviceCaps(hdc,BITSPIXEL)*GetDeviceCaps(hdc,PLANES);
	if(ibits<=1)
		wbitsCount=1;
	if(ibits<=4)
		wbitsCount=4;
	if(ibits<=8)
		wbitsCount=8;
	if(ibits<=16)
		wbitsCount=16;
	if(ibits<=24)
		wbitsCount=24;
	else 
		wbitsCount=32;
	//以上代码中ibits返回的显示模式下，每个像素所占的位数，wbitsCount是位图上每个像素所占的位数。
	if(wbitsCount<=8)
		dwpalettelsize=(1<<wbitsCount)*sizeof(RGBQUAD);
	GetObject(hBitmap,sizeof(BITMAP),(void *)&bitmap);//得到BITMAP的信息，这是一个BITMAP结构。
	bi.biSize=sizeof(BITMAPINFOHEADER);
	bi.biWidth=bitmap.bmWidth;
	bi.biHeight=bitmap.bmHeight;
	bi.biPlanes=1;
	bi.biBitCount=wbitsCount;
	bi.biClrImportant=0;
	bi.biClrUsed=0;
	bi.biCompression=BI_RGB;
	bi.biSizeImage=0;
	bi.biYPelsPerMeter=0;
	bi.biXPelsPerMeter=0;
	dwbmdibitsize=((bitmap.bmWidth*wbitsCount+31)/8)*bitmap.bmHeight;
	fdib=GlobalAlloc(GHND,dwbmdibitsize+dwpalettelsize+sizeof(BITMAPINFOHEADER));
	lpbi=(LPBITMAPINFOHEADER)::GlobalLock(fdib);
	*lpbi=bi;//将bi中的数据写入分配的内存中。
	//处理调色板
	hpal=::GetStockObject(DEFAULT_PALETTE);
	if(hpal)
	{
		hdc=::GetDC(NULL);
		holdpal=::SelectPalette(hdc,(HPALETTE)hpal,false);
		::RealizePalette(hdc);
	}
	GetDIBits(hdc,hBitmap,0,(UINT)bitmap.bmHeight,(LPSTR)lpbi+sizeof(BITMAPINFOHEADER)+dwpalettelsize,(BITMAPINFO *)lpbi,DIB_RGB_COLORS);
	if(holdpal)
	{
		::SelectPalette(hdc,(HPALETTE)holdpal,true);
		::RealizePalette(hdc);
		::ReleaseDC(NULL,hdc);
	}
	//创建文件
	fh=CreateFile(FileName,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN,NULL);
	if(fh==INVALID_HANDLE_VALUE)
		return FALSE;
	bmfhdr.bfType=0x4d42;
	dwdibsize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwbmdibitsize+dwpalettelsize;
	bmfhdr.bfSize=dwdibsize;
	bmfhdr.bfReserved1=0;
	bmfhdr.bfReserved2=0;
	bmfhdr.bfOffBits=(DWORD)sizeof(BITMAPFILEHEADER)+(DWORD)sizeof(BITMAPINFOHEADER)+dwpalettelsize;
	::WriteFile(fh,(LPSTR)&bmfhdr,sizeof(BITMAPFILEHEADER),&dwwritten,NULL);
	::WriteFile(fh,(LPSTR)lpbi,dwdibsize,&dwwritten,NULL);
	::GlobalUnlock(fdib);
	::GlobalFree(fdib);
	::CloseHandle(fh);
	return TRUE;
}

void CAcomCtrl::OnCopySrc()  
{  
	CDC   MemDc,ScrDc; //   屏幕和内存设备描述表     
	//得到屏幕分辨率     
	int   ScrWidth = GetSystemMetrics(SM_CXSCREEN);     
	int   ScrHeight = GetSystemMetrics(SM_CYSCREEN);     

	//1   创建一个屏幕DC     
	ScrDc.CreateDC("display",NULL,NULL,NULL);     

	//2   创建一个与屏幕DC相兼容的内存DC     
	MemDc.CreateCompatibleDC(&ScrDc);     

	//3   创建与屏幕DC相兼容的位图     
	ScrBmp.Detach();     
	ScrBmp.CreateCompatibleBitmap(&ScrDc,ScrWidth,ScrHeight);     

	//4   将位图指针放入内存DC中,并得到一个pOld位图指针     
	CBitmap*   pOld   =   MemDc.SelectObject(&ScrBmp);     

	//5   将位图由屏幕DC拷贝到内存DC中     
	MemDc.BitBlt(0,0,ScrWidth,ScrHeight,&ScrDc,0,0,SRCCOPY);     

	//6   将位图放入内存DC中     
	MemDc.SelectObject(pOld);     

	CClientDC   pDC(this);
	char filename[30]={0},num[5]={0},num1[5]={0},num2[5]={0},num3[5]={0},num4[5]={0},num5[5]={0};
	CTime t = CTime::GetCurrentTime(); //获取系统日期
	int d=t.GetDay(); //获得几号
	int y=t.GetYear(); //获取年份
	int m=t.GetMonth(); //获取当前月份
	int h=t.GetHour(); //获取当前为几时
	int mm=t.GetMinute(); //获取分钟
	int s=t.GetSecond(); //获取秒
	strcpy(filename,"D:\\picture");
	itoa(y,num,10);
	strcat(filename,num);
	itoa(m,num1,10);
	strcat(filename,num1);
	itoa(d,num2,10);
	strcat(filename,num2);
	itoa(h,num3,10);
	strcat(filename,num3);
	itoa(mm,num4,10);
	strcat(filename,num4);
	itoa(s,num5,10);
	strcat(filename,num5);
	strcat(filename,".bmp");
	//BitMapToFile((HBITMAP)ScrBmp,filename);
	OnDraw(&pDC);     

}  

void CAcomCtrl::OnDraw(CDC* pDC)  
{  
	CRect   trect;     
	GetClientRect(&trect);     
	CDC   MemDC;     
	MemDC.CreateCompatibleDC(pDC);     
	CBitmap*   pOldB=MemDC.SelectObject(&ScrBmp);     
	//显示大小   可自己定     
	pDC->BitBlt(0,0,trect.Width(),trect.Height(),&MemDC,0,0,SRCCOPY);     
}



void CAcomCtrl::OnStreamMediaIdChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


void CAcomCtrl::OnStreamMediaIpChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


void CAcomCtrl::OnStreamMediaPortChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


void CAcomCtrl::OnConnectionTypeChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


void CAcomCtrl::OnisPlayingChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


void CAcomCtrl::FaceRecognition(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加调度处理程序代码
	sv_set_draw_callback((LPSTR)(LPCTSTR)m_DeviceType,sessionId,DrawFun,(long)this);
	//sv_set_ifred_callback((LPSTR)(LPCTSTR)m_DeviceType,sessionId,IFredFun,(long)this);
	//HTTPRest();

}

DWORD __stdcall Fun1(LPVOID lp)   
{	
	TPFileList1 *lpParamter1 = (TPFileList1 *)lp;
	LONG id2 = lpParamter1->sessionid1;
	CString devicetype2 = lpParamter1->devicetype1;

	CAcomCtrl *ctrl1;
	ctrl1 = (CAcomCtrl *)lpParamter1->lp1;
	bool succ2 = true;
	while(succ2)
	{
		OUTPUT_LOG("语音保活");
		sv_alive_request((LPSTR)(LPCTSTR)devicetype2,id2);
		succ2 = ctrl1->flag2;
		if (!succ2)
		{
			OUTPUT_LOG("语音保活退出");
			break;
		}
		Sleep(5000);
	}
	return 0;
}

void CAcomCtrl::OnVoiceTalkingChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	// TODO: 在此添加属性处理程序代码

	TPFileList1* tp1 = new TPFileList1();

	if (m_VoiceTalking)
	{
		sessionId1 = sv_start_voicecom_dvr((LPSTR)(LPCTSTR)m_DeviceType,m_DVRID,(LPSTR)(LPCTSTR)m_HostIP,m_HostPort ,(LPSTR)(LPCTSTR)m_LoginUser,(LPSTR)(LPCTSTR)m_Password);
		flag2 = true;
		tp1->sessionid1 = sessionId1;
		tp1->devicetype1 = m_DeviceType;
		tp1->lp1 = (char *)this;
		HANDLE hThread1 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Fun1, tp1,0, NULL);   
		CloseHandle(hThread1);
	}
	else
	{
		sv_stop_voicecom_dvr((LPSTR)(LPCTSTR)m_DeviceType,sessionId1);
		OUTPUT_LOG("主动关闭");
		flag2 = false;
	}

	SetModifiedFlag();
} 

DWORD __stdcall Fun2 (LPVOID lp)
{
	TPFileList2 *lpParam = (TPFileList2 *)lp;
	CString devicetype2 = lpParam->devicetype2;
	LONG dvrid2 = lpParam->dvrid2;
	LONG channelno2 = lpParam->channeelno2;
	LONG ptzpresetindex2 = lpParam->ptzpresetindex2;
	LONG ptzspeed2 = lpParam->ptzspeed2;
	LONG sessionid2 = lpParam->sessionid2;
	CString hostip2 = lpParam->hostip2;
	LONG hostport2 = lpParam->hostport2;
	CString loginuser2 = lpParam->loginuser2;
	CString password2 = lpParam->password2;

	Sleep(1000);

	VARIANT_BOOL succ = sv_ptz_control_dvr((LPSTR)(LPCTSTR)devicetype2,  dvrid2, channelno2,  11 , 0 , ptzpresetindex2,ptzspeed2,sessionid2, (LPSTR)(LPCTSTR)hostip2 ,  hostport2 , (LPSTR)(LPCTSTR)loginuser2 ,  (LPSTR)(LPCTSTR)password2 );
	if (succ)
	{
		OUTPUT_LOG("放大停止成功");                  
	}
	else
	{
		OUTPUT_LOG("放大停止失败");
	}

	return 0;
}
DWORD __stdcall Fun3 (LPVOID lp)
{
	TPFileList2 *lpParam = (TPFileList2 *)lp;
	CString devicetype2 = lpParam->devicetype2;
	LONG dvrid2 = lpParam->dvrid2;
	LONG channelno2 = lpParam->channeelno2;
	LONG ptzpresetindex2 = lpParam->ptzpresetindex2;
	LONG ptzspeed2 = lpParam->ptzspeed2;
	LONG sessionid2 = lpParam->sessionid2;
	CString hostip2 = lpParam->hostip2;
	LONG hostport2 = lpParam->hostport2;
	CString loginuser2 = lpParam->loginuser2;
	CString password2 = lpParam->password2;

	Sleep(100);

	VARIANT_BOOL succ = sv_ptz_control_dvr((LPSTR)(LPCTSTR)devicetype2,  dvrid2, channelno2,  10 , 0 , ptzpresetindex2,ptzspeed2,sessionid2, (LPSTR)(LPCTSTR)hostip2 ,  hostport2 , (LPSTR)(LPCTSTR)loginuser2 ,  (LPSTR)(LPCTSTR)password2 );
	if (succ)
	{
		OUTPUT_LOG("缩小停止成功");                  
	}
	else
	{
		OUTPUT_LOG("缩小停止失败");
	}

	return 0;
}

BOOL CAcomCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值  
	if (!flag1)
	{
		return FALSE;
	}
	TPFileList2* tp2 = new TPFileList2();

	tp2->devicetype2 = m_DeviceType;
	tp2->dvrid2 = m_DVRID;
	tp2->channeelno2 = m_ChannelNo;
	tp2->ptzpresetindex2 = m_PTZPresetIndex;
	tp2->ptzspeed2 = m_PTZSpeed;
	tp2->sessionid2 = sessionId;
	tp2->hostip2 = m_HostIP;
	tp2->hostport2 = m_HostPort;
	tp2->loginuser2 = m_LoginUser;
	tp2->password2 = m_Password;

	if (zDelta>0)
	{
		CString str;
		str.Format("%d",zDelta);
		OUTPUT_LOG(str);
		VARIANT_BOOL succ = sv_ptz_control_dvr((LPSTR)(LPCTSTR)m_DeviceType,  m_DVRID, m_ChannelNo,  11 , 1 , m_PTZPresetIndex,m_PTZSpeed,sessionId, (LPSTR)(LPCTSTR)m_HostIP ,  m_HostPort , (LPSTR)(LPCTSTR)m_LoginUser ,  (LPSTR)(LPCTSTR)m_Password );
		if (succ)
		{
			OUTPUT_LOG("放大成功");
			
			HANDLE hThread2 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Fun2, tp2,0, NULL);   
			CloseHandle(hThread2);
		}
		else
		{
			OUTPUT_LOG("放大失败");
		}
	}
	else
	{
		CString str;
		str.Format("%d",zDelta);
		OUTPUT_LOG(str);
		VARIANT_BOOL succ = sv_ptz_control_dvr((LPSTR)(LPCTSTR)m_DeviceType,  m_DVRID, m_ChannelNo,  10 , 1 , m_PTZPresetIndex,m_PTZSpeed,sessionId, (LPSTR)(LPCTSTR)m_HostIP ,  m_HostPort , (LPSTR)(LPCTSTR)m_LoginUser ,  (LPSTR)(LPCTSTR)m_Password );
		if (succ)
		{
			OUTPUT_LOG("缩小成功");
			
			HANDLE hThread3 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Fun3, tp2,0, NULL);   
			CloseHandle(hThread3);
		}
		else
		{
			OUTPUT_LOG("缩小失败");
		}
	}


	return COleControl::OnMouseWheel(nFlags, zDelta, pt);
}

LRESULT CAcomCtrl::OnDownA(WPARAM wParam,LPARAM lParam)
{
	if (!flag1)
	{
		return 1;
	}
	//OUTPUT_LOG("downAok");
	VARIANT_BOOL succ = sv_ptz_control_dvr((LPSTR)(LPCTSTR)m_DeviceType,  m_DVRID, m_ChannelNo,  18 , 1 , m_PTZPresetIndex,m_PTZSpeed,sessionId, (LPSTR)(LPCTSTR)m_HostIP ,  m_HostPort , (LPSTR)(LPCTSTR)m_LoginUser ,  (LPSTR)(LPCTSTR)m_Password );
	if (succ)
	{
		OUTPUT_LOG("向左开始成功");
	}
	else
	{
		OUTPUT_LOG("向左开始失败");
	}
	return 1;
}

LRESULT CAcomCtrl::OnUpA(WPARAM wParam,LPARAM lParam)
{
	if (!flag1)
	{
		return 1;
	}
	//OUTPUT_LOG("upAok");
	VARIANT_BOOL succ = sv_ptz_control_dvr((LPSTR)(LPCTSTR)m_DeviceType,  m_DVRID, m_ChannelNo,  18 , 0 , m_PTZPresetIndex,m_PTZSpeed,sessionId, (LPSTR)(LPCTSTR)m_HostIP ,  m_HostPort , (LPSTR)(LPCTSTR)m_LoginUser ,  (LPSTR)(LPCTSTR)m_Password );
	if (succ)
	{
		OUTPUT_LOG("向左停止成功");
	}
	else
	{
		OUTPUT_LOG("向左停止失败");
	}
	return 1;
}

LRESULT CAcomCtrl::OnDownB(WPARAM wParam,LPARAM lParam)
{
	if (!flag1)
	{
		return 1;
	}
	//OUTPUT_LOG("downBok");
	VARIANT_BOOL succ = sv_ptz_control_dvr((LPSTR)(LPCTSTR)m_DeviceType,  m_DVRID, m_ChannelNo,  12 , 1 , m_PTZPresetIndex,m_PTZSpeed,sessionId, (LPSTR)(LPCTSTR)m_HostIP ,  m_HostPort , (LPSTR)(LPCTSTR)m_LoginUser ,  (LPSTR)(LPCTSTR)m_Password );
	if (succ)
	{
		OUTPUT_LOG("向上开始成功");
	}
	else
	{
		OUTPUT_LOG("向上开始失败");
	}
	return 1;
}

LRESULT CAcomCtrl::OnUpB(WPARAM wParam,LPARAM lParam)
{
	if (!flag1)
	{
		return 1;
	}
	//OUTPUT_LOG("upBok");
	VARIANT_BOOL succ = sv_ptz_control_dvr((LPSTR)(LPCTSTR)m_DeviceType,  m_DVRID, m_ChannelNo,  12 , 0 , m_PTZPresetIndex,m_PTZSpeed,sessionId, (LPSTR)(LPCTSTR)m_HostIP ,  m_HostPort , (LPSTR)(LPCTSTR)m_LoginUser ,  (LPSTR)(LPCTSTR)m_Password );
	if (succ)
	{
		OUTPUT_LOG("向上停止成功");
	}
	else
	{
		OUTPUT_LOG("向上停止失败");
	}
	return 1;
}

LRESULT CAcomCtrl::OnDownC(WPARAM wParam,LPARAM lParam)
{
	if (!flag1)
	{
		return 1;
	}
	//OUTPUT_LOG("downCok");
	VARIANT_BOOL succ = sv_ptz_control_dvr((LPSTR)(LPCTSTR)m_DeviceType,  m_DVRID, m_ChannelNo,  14 , 1 , m_PTZPresetIndex,m_PTZSpeed,sessionId, (LPSTR)(LPCTSTR)m_HostIP ,  m_HostPort , (LPSTR)(LPCTSTR)m_LoginUser ,  (LPSTR)(LPCTSTR)m_Password );
	if (succ)
	{
		OUTPUT_LOG("向右开始成功");
	}
	else
	{
		OUTPUT_LOG("向右开始失败");
	}
	return 1;
}

LRESULT CAcomCtrl::OnUpC(WPARAM wParam,LPARAM lParam)
{
	if (!flag1)
	{
		return 1;
	}
	//OUTPUT_LOG("upCok");
	VARIANT_BOOL succ = sv_ptz_control_dvr((LPSTR)(LPCTSTR)m_DeviceType,  m_DVRID, m_ChannelNo,  14 , 0 , m_PTZPresetIndex,m_PTZSpeed,sessionId, (LPSTR)(LPCTSTR)m_HostIP ,  m_HostPort , (LPSTR)(LPCTSTR)m_LoginUser ,  (LPSTR)(LPCTSTR)m_Password );
	if (succ)
	{
		OUTPUT_LOG("向右停止成功");
	}
	else
	{
		OUTPUT_LOG("向右停止失败");
	}
	return 1;
}

LRESULT CAcomCtrl::OnDownD(WPARAM wParam,LPARAM lParam)
{
	if (!flag1)
	{
		return 1;
	}
	//OUTPUT_LOG("downDok");
	VARIANT_BOOL succ = sv_ptz_control_dvr((LPSTR)(LPCTSTR)m_DeviceType,  m_DVRID, m_ChannelNo,  16 , 1 , m_PTZPresetIndex,m_PTZSpeed,sessionId, (LPSTR)(LPCTSTR)m_HostIP ,  m_HostPort , (LPSTR)(LPCTSTR)m_LoginUser ,  (LPSTR)(LPCTSTR)m_Password );
	if (succ)
	{
		OUTPUT_LOG("向下开始成功");
	}
	else
	{
		OUTPUT_LOG("向下开始失败");
	}
	return 1;
}

LRESULT CAcomCtrl::OnUpD(WPARAM wParam,LPARAM lParam)
{
	if (!flag1)
	{
		return 1;
	}
	//OUTPUT_LOG("upDok");
	VARIANT_BOOL succ = sv_ptz_control_dvr((LPSTR)(LPCTSTR)m_DeviceType,  m_DVRID, m_ChannelNo,  16 , 0 , m_PTZPresetIndex,m_PTZSpeed,sessionId, (LPSTR)(LPCTSTR)m_HostIP ,  m_HostPort , (LPSTR)(LPCTSTR)m_LoginUser ,  (LPSTR)(LPCTSTR)m_Password );
	if (succ)
	{
		OUTPUT_LOG("向下停止成功");
	}
	else
	{
		OUTPUT_LOG("向下停止失败");
	}
	return 1;
}

LRESULT CAcomCtrl::OnButtonA(WPARAM wParam,LPARAM lParam)
{
	return 1;
}

LRESULT CAcomCtrl::OnButtonB(WPARAM wParam,LPARAM lParam)
{
	m_button->ShowWindow(SW_HIDE);
	m_button1->ShowWindow(SW_HIDE);
	m_button2->ShowWindow(SW_HIDE);
	m_button3->ShowWindow(SW_HIDE);
	return 1;
}

LRESULT CAcomCtrl::OnButtonC(WPARAM wParam,LPARAM lParam)
{
	return 1;
}

LRESULT CAcomCtrl::OnButtonD(WPARAM wParam,LPARAM lParam)
{
	m_button->ShowWindow(SW_HIDE);
	m_button1->ShowWindow(SW_HIDE);
	m_button2->ShowWindow(SW_HIDE);
	m_button3->ShowWindow(SW_HIDE);
	return 1;
}

LRESULT CAcomCtrl::OnButtonE(WPARAM wParam,LPARAM lParam)
{
	return 1;
}

LRESULT CAcomCtrl::OnButtonF(WPARAM wParam,LPARAM lParam)
{
	m_button->ShowWindow(SW_HIDE);
	m_button1->ShowWindow(SW_HIDE);
	m_button2->ShowWindow(SW_HIDE);
	m_button3->ShowWindow(SW_HIDE);
	return 1;
}

LRESULT CAcomCtrl::OnButtonG(WPARAM wParam,LPARAM lParam)
{
	return 1;
}

LRESULT CAcomCtrl::OnButtonH(WPARAM wParam,LPARAM lParam)
{
	m_button->ShowWindow(SW_HIDE);
	m_button1->ShowWindow(SW_HIDE);
	m_button2->ShowWindow(SW_HIDE);
	m_button3->ShowWindow(SW_HIDE);
	return 1;
}

void CAcomCtrl::OncanPtzChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


void CAcomCtrl::OncanInfraredChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


void CAcomCtrl::OnmachineIdChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


void CAcomCtrl::OnalarmUrlChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	/*http://localhost:8080/sc/machine?_type=web&_method=getMachineInfraRedAlarm&scode=1*/

	SetModifiedFlag();
}


void CAcomCtrl::OnalarmHostIpChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


void CAcomCtrl::OnalarmPortChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	SetModifiedFlag();
}


void CAcomCtrl::OnVoiceTalkingVolumeChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 在此添加属性处理程序代码

	VARIANT_BOOL succ = sv_voicecom_volumel_dvr((LPSTR)(LPCTSTR)m_DeviceType,sessionId1,m_VoiceTalkingVolume);
	if (succ)
	{
		OUTPUT_LOG("对讲音量控制成功");
	}
	else
	{
		OUTPUT_LOG("对讲音量控制失败");
	}

	SetModifiedFlag();
}
