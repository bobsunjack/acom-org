

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Mon Apr 18 17:47:27 2016
 */
/* Compiler settings for Acom.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, LIBID_AcomLib,0xFAB9D140,0xC641,0x48D1,0xB4,0x9A,0x3E,0xB2,0xBE,0x9E,0x9C,0x6E);


MIDL_DEFINE_GUID(IID, DIID__DAcom,0x44FB3261,0x2DF3,0x4596,0x95,0x27,0xFE,0x96,0x3F,0x54,0x0F,0x25);


MIDL_DEFINE_GUID(IID, DIID__DAcomEvents,0x0731A0EF,0xDAA1,0x4DDD,0xB7,0xC1,0xB7,0x47,0x3A,0x41,0x64,0xF8);


MIDL_DEFINE_GUID(CLSID, CLSID_Acom,0x6CB6C1B5,0x0E0D,0x4622,0x85,0x10,0xDF,0x1E,0xB6,0x35,0x00,0x50);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



