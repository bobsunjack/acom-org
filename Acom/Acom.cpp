// Acom.cpp : CAcomApp 和 DLL 注册的实现。

#include "stdafx.h"
#include "Acom.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CAcomApp theApp;

const GUID CDECL _tlid = { 0xFAB9D140, 0xC641, 0x48D1, { 0xB4, 0x9A, 0x3E, 0xB2, 0xBE, 0x9E, 0x9C, 0x6E } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;



// CAcomApp::InitInstance - DLL 初始化

BOOL CAcomApp::InitInstance()
{
	BOOL bInit = COleControlModule::InitInstance();

	if (bInit)
	{
		// TODO: 在此添加您自己的模块初始化代码。
	}

	return bInit;
}



// CAcomApp::ExitInstance - DLL 终止

int CAcomApp::ExitInstance()
{
	// TODO: 在此添加您自己的模块终止代码。

	return COleControlModule::ExitInstance();
}



// DllRegisterServer - 将项添加到系统注册表

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}



// DllUnregisterServer - 将项从系统注册表中移除

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}
