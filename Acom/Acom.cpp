// Acom.cpp : CAcomApp �� DLL ע���ʵ�֡�

#include "stdafx.h"
#include "Acom.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CAcomApp theApp;

const GUID CDECL _tlid = { 0xFAB9D140, 0xC641, 0x48D1, { 0xB4, 0x9A, 0x3E, 0xB2, 0xBE, 0x9E, 0x9C, 0x6E } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;



// CAcomApp::InitInstance - DLL ��ʼ��

BOOL CAcomApp::InitInstance()
{
	BOOL bInit = COleControlModule::InitInstance();

	if (bInit)
	{
		// TODO: �ڴ�������Լ���ģ���ʼ�����롣
	}

	return bInit;
}



// CAcomApp::ExitInstance - DLL ��ֹ

int CAcomApp::ExitInstance()
{
	// TODO: �ڴ�������Լ���ģ����ֹ���롣

	return COleControlModule::ExitInstance();
}



// DllRegisterServer - ������ӵ�ϵͳע���

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}



// DllUnregisterServer - �����ϵͳע������Ƴ�

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}
